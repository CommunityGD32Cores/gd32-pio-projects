#include <Arduino.h>

#define PWM_OUT_PIN   PA6
#define LED_PIN       PC13

PWM pwm_output(PWM_OUT_PIN);

void captureCompareCallback() {
  digitalWrite(PC13, !digitalRead(PC13));
}

void setup()
{
  /* starts a PWM wave with 500ms total period, 300ms of which are on (aka 60% duty cycle) */
  /* also uses the capture compare callback to additionally toggle an LED in the same rythm. */
  /* we cannot yet use analogWrite() for this as it is not implemented for PWM. */
  pinMode(LED_PIN, OUTPUT);
  pwm_output.setPeriodCycle(500, 300, FORMAT_MS);
  pwm_output.start();
  pwm_output.attachInterrupt(captureCompareCallback);
}

void loop()
{
}
