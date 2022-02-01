#include <Arduino.h>
/*
  ,------.          ,--.                       ,-----.       ,--.         
  |  .-.  \ ,--.,--.`--',--,--,  ,---. ,-----.'  .--./ ,---. `--',--,--,  
  |  |  \  :|  ||  |,--.|      \| .-. |'-----'|  |    | .-. |,--.|      \ 
  |  '--'  /'  ''  '|  ||  ||  |' '-' '       '  '--'\' '-' '|  ||  ||  | 
  `-------'  `----' `--'`--''--' `---'         `-----' `---' `--'`--''--' 
  Official code for Arduino boards                          version 2.7.4
  
  Duino-Coin Team & Community 2019-2021 © MIT Licensed
  https://duinocoin.com
  https://github.com/revoxhere/duino-coin
  If you don't know where to start, visit official website and navigate to
  the Getting Started page. Have fun mining!
*/

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif
/* For 8-bit microcontrollers we should use 16 bit variables since the
difficulty is low, for all the other cases should be 32 bits. */
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
typedef uint16_t uintDiff;
#else
typedef uint32_t uintDiff;
#endif
// Arduino identifier library - https://github.com/ricaun
#include "uniqueID.h"
#include "sha1.h"

// Create globals
String lastblockhash = "";
String newblockhash = "";
String DUCOID = "";
uintDiff difficulty = 0;
uintDiff ducos1result = 0;
// 40+40+20+3 is the maximum size of a job
const uint16_t job_maxsize = 104;  
uint8_t job[job_maxsize];

String get_DUCOID();

void setup() {
  // Prepare built-in led pin as output
  pinMode(LED_BUILTIN, OUTPUT);
  DUCOID = get_DUCOID();
  // Open serial port
  Serial.begin(115200);
  Serial.setTimeout(10000);
  while (!Serial)
    ;  // For Arduino Leonardo or any board with the ATmega32U4
  Serial.println("Firmware started!!");
  Serial.flush();
}

// DUCO-S1A hasher
uintDiff ducos1a(String lastblockhash, String newblockhash,
                 uintDiff difficulty) {
  newblockhash.toUpperCase();
  const char *c = newblockhash.c_str();
  uint8_t final_len = newblockhash.length() >> 1;
  for (uint8_t i = 0, j = 0; j < final_len; i += 2, j++)
    job[j] = ((((c[i] & 0x1F) + 9) % 25) << 4) + ((c[i + 1] & 0x1F) + 9) % 25;

    // Difficulty loop
  #if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
    // If the difficulty is too high for AVR architecture then return 0
    if (difficulty > 655) return 0;
  #endif
  for (uintDiff ducos1res = 0; ducos1res < difficulty * 100 + 1; ducos1res++) {
    //Serial.println("Iteration " + String(ducos1res));
    Sha1.init();
    Sha1.print(lastblockhash + String(ducos1res));
    // Get SHA1 result
    uint8_t *hash_bytes = Sha1.result();
    if (memcmp(hash_bytes, job, SHA1_HASH_LEN * sizeof(char)) == 0) {
      // If expected hash is equal to the found hash, return the result
      return ducos1res;
    }
  }
  return 0;
}

String get_DUCOID() {
  String ID = "DUCOID";
  char buff[4];
  for (size_t i = 0; i < 8; i++) {
    sprintf(buff, "%02X", (uint8_t)UniqueID8[i]);
    ID += buff;
  }
  return ID;
}

void loop() {
  // Wait for serial data
  if (Serial.available() > 0) {
    //Serial.println("Got data: "+ String(Serial.available()));
    // Turn on the built-in led
    #if defined(ARDUINO_ARCH_AVR)
        PORTB = PORTB & B11011111;
    #else
        digitalWrite(LED_BUILTIN, HIGH);
    #endif
    memset(job, 0, job_maxsize);
    // Read last block hash
    //Serial.println("Reading lastblockhash");
    lastblockhash = Serial.readStringUntil(',');
    // Read expected hash
    //Serial.println("Reading newblockhash");
    newblockhash = Serial.readStringUntil(',');
    // Read difficulty
    //Serial.println("Reading difficulty");
    difficulty = strtoul(Serial.readStringUntil(',').c_str(), NULL, 10);
    // Clearing the receive buffer reading one job.
    //Serial.println("Clearing read buf");
    while (Serial.available()) Serial.read();
    // Start time measurement
    uint32_t startTime = micros();
    // Call DUCO-S1A hasher
    //Serial.println("Doing the hashing.. Difficulty = " + String(difficulty) + " last: " + lastblockhash + " new: " + newblockhash);
    ducos1result = ducos1a(lastblockhash, newblockhash, difficulty);
    // Calculate elapsed time
    uint32_t elapsedTime = micros() - startTime;
    // Clearing the receive buffer before sending the result.
    //Serial.println("Flushing read buf");
    while (Serial.available()) Serial.read();
    // Send result back to the program with share time
    Serial.print(String(ducos1result, 2) 
                 + "," 
                 + String(elapsedTime, 2) 
                 + "," 
                 + String(DUCOID) 
                 + "\n");
    // Turn off the built-in led
    #if defined(ARDUINO_ARCH_AVR)
        PORTB = PORTB | B00100000;
    #else
        digitalWrite(LED_BUILTIN, LOW);
    #endif
  }
}
