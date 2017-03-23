/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello, world!", that
 * will be processed by The Things Network server.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in g1, 
*  0.1% in g2). 
 *
 * Change DEVADDR to a unique address! 
 * See http://thethingsnetwork.org/wiki/AddressSpace
 *
 * Do not forget to define the radio type correctly in config.h, default is:
 *   #define CFG_sx1272_radio 1
 * for SX1272 and RFM92, but change to:
 *   #define CFG_sx1276_radio 1
 * for SX1276 and RFM95.
 *
 *******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <wiringPi.h>
#include <lmic.h>
#include <hal.h>
#include <local_hal.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>

// for file reading
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib> // for exit()

using namespace std;

// LoRaWAN Application identifier (AppEUI)
// Not used in this example
static const u1_t APPEUI[8]  = { 0x70, 0xB3, 0xD5, 0x7E, 0xF0, 0x00, 0x3F, 0xEE };

// LoRaWAN DevEUI, unique device ID (LSBF)
// Not used in this example
static const u1_t DEVEUI[8]  = { 0x00, 0x82, 0x90, 0x94, 0xF3, 0xB0, 0x45, 0x96 };

// LoRaWAN NwkSKey, network session key 
// Use this key for The Things Network
static const u1_t DEVKEY[16] = { 0x3B, 0xAB, 0xEC, 0x2F, 0xDB, 0x17, 0xAE, 0xA8, 0x3F, 0x56, 0xAD, 0x38, 0x06, 0x5F, 0x5E, 0xFD };

// LoRaWAN AppSKey, application session key
// Use this key to get your data decrypted by The Things Network
static const u1_t ARTKEY[16] = { 0x4F, 0x84, 0xE4, 0x23, 0x88, 0xD5, 0x86, 0x92, 0xD0, 0x6D, 0x4E, 0x4A, 0x84, 0x81, 0x5D, 0xE8 };

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
static const u4_t DEVADDR = 0x2601139E ; // <-- Change this address for every node!

//////////////////////////////////////////////////
// APPLICATION CALLBACKS
//////////////////////////////////////////////////

// provide application router ID (8 bytes, LSBF)
void os_getArtEui (u1_t* buf) {
    memcpy(buf, APPEUI, 8);
}

// provide device ID (8 bytes, LSBF)
void os_getDevEui (u1_t* buf) {
    memcpy(buf, DEVEUI, 8);
}

// provide device key (16 bytes)
void os_getDevKey (u1_t* buf) {
    memcpy(buf, DEVKEY, 16);
}

u4_t cntr=0;
u1_t mydata[] = {"Hello, world!                               "};
static osjob_t sendjob;

// Pin mapping
lmic_pinmap pins = {
  .nss = 6,
  .rxtx = UNUSED_PIN, // Not connected on RFM92/RFM95
  .rst = 0,  // Needed on RFM92/RFM95
  .dio = {7,4,5}
};

void onEvent (ev_t ev) {
    //debug_event(ev);

    switch(ev) {
      // scheduled data sent (optionally data received)
      // note: this includes the receive window!
      case EV_TXCOMPLETE:
          // use this event to keep track of actual transmissions
          fprintf(stdout, "Event EV_TXCOMPLETE, time: %d\n", millis() / 1000);
          if(LMIC.dataLen) { // data received in rx slot after tx
              //debug_buf(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
              fprintf(stdout, "Data Received!\n");
          }
          break;
       default:
          break;
    }
}

static void do_send(osjob_t* j){
      time_t t=time(NULL);
      fprintf(stdout, "[%x] (%ld) %s\n", hal_ticks(), t, ctime(&t));
      // Show TX channel (channel numbers are local to LMIC)
      // Check if there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
      fprintf(stdout, "OP_TXRXPEND, not sending");
    } else {
      // Read gps coord from file, compress and send
      // *******************************************
      // read data from geocordinates file
      float float_lat = 0.0f;
      float float_lon = 0.0f;
      ifstream inf("../../data/geo.dat");
      if (inf) {
          for (int i = 0; i < 2; i++) {
              std::string coord;
              getline(inf, coord);
              if (i == 0) {
                  float_lat = stof(coord);
              }
              if (i == 1) {
                  float_lon = stof(coord);
              }
          }

      } else {
          printf("Failed!");
      }

      printf("Sending: %f,%f\n", float_lat, float_lon);

      uint8_t coords[6]; // 6*8 bits = 48
      int32_t lat = float_lat * 10000;
      int32_t lon = float_lon * 10000;

      // Pad 2 int32_t to 6 8uint_t, skipping the last byte (x >> 24)
      coords[0] = lat;
      coords[1] = lat >> 8;
      coords[2] = lat >> 16;

      coords[3] = lon;
      coords[4] = lon >> 8;
      coords[5] = lon >> 16;

      // Check if there is not a current TX/RX job running
      if (LMIC.opmode & OP_TXRXPEND) {
          printf("OP_TXRXPEND, not sending\n");
      } else {
          //digitalWrite(RF_LED_PIN, HIGH);
          // Prepare upstream data transmission at the next possible time.
          LMIC_setTxData2(1, coords, sizeof(coords), 0);
          printf("Packet queued\n");

        }	
      }
      /***
      // Prepare upstream data transmission at the next possible time.
      char buf[100];
      sprintf(buf, "Hello world! [%d]", cntr++);
      int i=0;
      while(buf[i]) {
        mydata[i]=buf[i];
        i++;
      }
      mydata[i]='\0';
      LMIC_setTxData2(1, mydata, strlen(buf), 0);
    }***/
    // Schedule a timed job to run at the given timestamp (absolute system time)
    os_setTimedCallback(j, os_getTime()+sec2osticks(20), do_send);
         
}

void setup() {
  // LMIC init
  wiringPiSetup();

  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  // Set static session parameters. Instead of dynamically establishing a session 
  // by joining the network, precomputed session parameters are be provided.
  LMIC_setSession (0x1, DEVADDR, (u1_t*)DEVKEY, (u1_t*)ARTKEY);
  // Disable data rate adaptation
  LMIC_setAdrMode(0);
  // Disable link check validation
  LMIC_setLinkCheckMode(0);
  // Disable beacon tracking
  LMIC_disableTracking ();
  // Stop listening for downstream data (periodical reception)
  LMIC_stopPingable();
  // Set data rate and transmit power (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7,14);
  //
}

void loop() {

do_send(&sendjob);

while(1) {
  os_runloop();
//  os_runloop_once();
  }
}


int main() {
  setup();

  while (1) {
    loop();
  }
  return 0;
}

