/**
 * NTP-Client for Arduino - works also with Photon
 *
 * © Francesco Potortì 2013 - GPLv3 - Revision: 1.13
 *
 * Send an NTP packet and wait for the response, return the Unix time
 *
 * To lower the memory footprint, no buffers are allocated for sending
 * and receiving the NTP packets.  Four bytes of memory are allocated
 * for transmision, the rest is random garbage collected from the data
 * memory segment, and the received packet is read one byte at a time.
 * The Unix time is returned, that is, seconds from 1970-01-01T00:00.
 *
 * playground.arduino.cc/Code/NTPclient
 */

#include "application.h"

unsigned long ntpUnixTime(UDP &udp);
