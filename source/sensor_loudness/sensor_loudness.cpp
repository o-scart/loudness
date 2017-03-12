#include "sensor_loudness.h"

unsigned int temp;
unsigned int i;

int getSenVal(int PIN){
  temp = 0;
  for(i = 0;i<HIS_LEN;i++){
      temp += analogRead(PIN);
      delay(1);
  }
  return temp / HIS_LEN;
}

int loudnessValue(int senVal){
  if (senVal < 50){return 60;}
  if (senVal < 100){return 70;}
  if (senVal < 150){return 75;}
  if (senVal < 300){return 80;}
  if (senVal < 450){return 85;}
  if (senVal < 600){return 90;}
  if (senVal < 700){return 95;}
  return 99;
}
