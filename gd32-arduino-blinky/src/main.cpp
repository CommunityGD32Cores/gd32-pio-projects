#include <Arduino.h>

//#define LED PC13
#define LED PB13

void setup(){
    pinMode(LED, OUTPUT);
}

void loop(){
    digitalWrite(LED, LOW);
    delay(500);
    digitalWrite(LED, HIGH);
    delay(500);
}