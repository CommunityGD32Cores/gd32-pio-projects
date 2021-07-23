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

#ifdef PB11
#define BUTTON_PIN PB11
#elif defined(KEY0) 
#define BUTTON_PIN KEY0
#else
#error "Please manually select button pin."
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
  Serial.begin(115200);
  attachInterrupt(BUTTON_PIN, &button_isr, FALLING);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
}

void loop()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Presses: " + String(num_presses));
  display.display();
  delay(50);
}
