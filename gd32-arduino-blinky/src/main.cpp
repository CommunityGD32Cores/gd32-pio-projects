#include <Arduino.h>

#ifdef LED_BUILTIN
#define LED LED_BUILTIN
#else 
#define LED PC13
//#define LED PB13
#endif 

void setup(){
    pinMode(LED, OUTPUT);
}

void loop(){
    digitalWrite(LED, LOW);
    delay(500);
    digitalWrite(LED, HIGH);
    delay(500);
}