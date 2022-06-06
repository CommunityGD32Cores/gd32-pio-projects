#include <Arduino.h>

void setup() {
    Serial.begin(115200);
}
int i=0;
void loop() {
    Serial.println("Test: " + String(i++));
    delay(1000);
}