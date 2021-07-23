#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define xstr(s) str(s)
#define str(s) #s

#ifdef PA7
#define ANALOG_IN_PIN PA7
#define ANALOG_IN_PIN_STR "PA7"
#else 
#define ANALOG_IN_PIN A0
#define ANALOG_IN_PIN_STR "Analog pin 0"
#endif

#define NUM_SAMPLES  50
#define SAMPLE_DELAY 5

void setup()
{
  Serial.begin(115200);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.clearDisplay();
  display.display();
  pinMode(ANALOG_IN_PIN, INPUT_ANALOG);
  //default is 10-bit ADC (arduino-compatibility).
  //change to 12-bit ADC
  analogReadResolution(12);
}

void loop()
{
  //uint32_t is large enough so that sum does not overflow
  uint32_t sum = 0;
  for(int i=0; i < NUM_SAMPLES; i++) {
    uint32_t val = analogRead(ANALOG_IN_PIN);
    sum += val;
    delay(SAMPLE_DELAY);
  }
  uint32_t val = sum / NUM_SAMPLES;
  float in_volts = val * 3.3f / 4095; //for 12-bit resolution and V_REF = 3.3V

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(String(ANALOG_IN_PIN_STR) + ": " + String(val) + " (" + String(in_volts, 2) + "V)");
  display.display();
  delay(50);
}
