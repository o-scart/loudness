#include "application.h"
#include "ntp_client/ntp_client.h"
#include "sensor_loudness/sensor_loudness.h"
#include "secret.h"

SYSTEM_MODE(MANUAL);

// PINS
const int blueLED = D7;
const int LOUD = A0;
const int HALT = D1;

// Settings
const int ITERATIONS = 50;
const int FAILURES = 10;
const int RESYNC_EV_SEC = 60*60;

// Networking
TCPClient client;
static UDP udp;
unsigned int HTTPCode = 0;
const char horst[] = "example.com";
const int PORT = 80;

// Other Variables
boolean ledOn = false;
uint8_t numFail = 0;
uint32_t sensorRawValue;
uint32_t timepoint;
uint32_t nextTimeSync;
String postData;

/**
 * increment Number in the event of failure, will reboot after multiple consecutive failures
 */
void checkSuccess(boolean success){
  if(success){
    numFail = 0;
  } else {
    numFail++;
    if(numFail == FAILURES){
      System.reset();
    }
  }
}

/**
 * toggle the D7 LED
 */
void toggleBLED(){
  if (ledOn){
    // LED is lit, turn off
    pinResetFast(blueLED);
    ledOn = false;
  } else {
    // LED is dark, turn on
    pinSetFast(blueLED);
    ledOn = true;
  }
}

/**
 * try to sync Time, untill successful or reboot
 */
void syncTime(){
  do{
    Time.setTime(ntpUnixTime(udp));
    checkSuccess(false);
    delay(1000);
  } while(Time.now() < (1<<30));
  checkSuccess(true);
  // Set next Sync
  nextTimeSync = Time.now() + RESYNC_EV_SEC;
}

/**
 * get the HTTP response code
 * taken from https://github.com/llad/spark-restclient
 */
int getStatusCode(){
  // an http request ends with a blank line
  boolean currentLineIsBlank = true;
  boolean httpBody = false;
  boolean inStatus = false;

  char statusCode[4];
  int i = 0;
  int code = 0;

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if(c == ' ' && !inStatus){
        inStatus = true;
      }
      if(inStatus && i < 3 && c != ' '){
        statusCode[i] = c;
        i++;
      }
      if(i == 3){
        statusCode[i] = '\0';
        code = atoi(statusCode);
		    // edit: if all digits present, we are finished
		    return code;
      }
      if (c == '\n' && httpBody){
        return code;
      }
      if (c == '\n' && currentLineIsBlank) {
        httpBody = true;
      }
      if (c == '\n') {
        // you're starting a new lineu
        currentLineIsBlank = true;
      }
      else if (c != '\r') {
        // you've gotten a character on the current line
        currentLineIsBlank = false;
      }
    }
  }
  return code;
}

void setup() {
  // initialize PINs
  pinMode(blueLED,OUTPUT);
  pinMode(LOUD,AN_INPUT);
  pinMode(HALT,INPUT_PULLUP);
  Serial.begin(9600);

  // connect to Wifi
  WiFi.on();
  WiFi.setCredentials(SSID,PW,WPA2,WLAN_CIPHER_AES);
  WiFi.connect();

  // Sync Time with NTP
  toggleBLED();
  syncTime();
}

void loop() {
  // If HALT not connected, procede normally
  if (digitalRead(HALT)==HIGH){
    // erase sensor value, toggle LED and remember time
    sensorRawValue = 0;
    toggleBLED();
    timepoint = Time.now();

    // connect to EvA
    if(client.connect(horst,PORT)){
      // get Senvor Value and calculate average
      for(int i = 0;i<ITERATIONS;i++){
        sensorRawValue += getSenVal(LOUD);
      }
      sensorRawValue = sensorRawValue / ITERATIONS;

      // prepare POST data and execute
      postData = String::format("[{\"timestamp\":%d000,\"channels\":{\"sensor_raw\":{\"value\":%d}}}]",timepoint,sensorRawValue);
      client.print(String::format("POST /packets HTTP/1.1\r\nHost: example.com\r\nConnnection: close\r\nContent-Length: %d\r\nContent-Type: application/json\r\n\r\n",strlen(postData)));
      client.print(postData);
      client.print("\r\n\r\n");

      // get response and terminate connection
      HTTPCode = getStatusCode();
      client.stop();
    } else {
      // connection failed
      HTTPCode = 0;
    }

    // check successful response from server
    if (HTTPCode == 200){
      // Status OK
      checkSuccess(true);
    } else {
      // Status not OK
      checkSuccess(false);
    }

    // Sync the time if necessary
    if(Time.now() >= nextTimeSync){
      syncTime();
    }

    // wait until second passes for next iteration
    while(timepoint==Time.now()){
      delay(100);
    }
  } else {
    // HALT connected to GND, do nothing except toggling LED
    toggleBLED();
    delay(250);
  }
}
