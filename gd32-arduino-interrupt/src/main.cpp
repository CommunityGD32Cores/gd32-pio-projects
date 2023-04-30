#include <Arduino.h>

#if defined(PB11) && !defined(KEY0)
#define BUTTON_PIN PB11
#elif defined(KEY0) 
#define BUTTON_PIN KEY0
#else
#error "Please manually select button pin."
#endif

#ifdef LED_BUILTIN
#define LED LED_BUILTIN
#else 
#define LED PC13
//#define LED PB13
#endif

static volatile int num_presses = 0;
static unsigned last_press = 0;
#define DEBOUNCE_TIME 300
void button_isr() {
  unsigned now = millis();
  if(now - last_press >= DEBOUNCE_TIME) {
      last_press = now;
      num_presses++;
  }
}

void setup()
{
  digitalPinToPinName(LED);
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), &button_isr, FALLING);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
  // LED will be on for every second button press
  digitalWrite(LED, num_presses % 2);
}
