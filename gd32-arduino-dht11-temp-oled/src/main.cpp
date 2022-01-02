#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SimpleDHT.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#if defined(PB10) && PIN_WIRE_SCL != PB10
#define DHT11PIN PB10
#else 
#define DHT11PIN PB0
#endif

SimpleDHT11 dht11(DHT11PIN);

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
  display.setTextSize(2);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.clearDisplay();
  display.display();
  display.cp437(true);
}

void drawCentreString(const String& buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(buf.c_str(), x, y, &x1, &y1, &w, &h); //calc width of new string
    display.setCursor(x - w / 2, y);
    display.print(buf);
}

void loop()
{
  float temp = 1.0f;
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    return;
  } else {
    Serial.println("Read OK, temp = " + String(temperature) + " hum = " + String(humidity));
  }
  temp = temperature;

  display.clearDisplay();
  display.setCursor(0, 0);
  drawCentreString("Temp & Hum", SCREEN_WIDTH / 2,0);
  String tempString = String(temp,0) + String((char)0xf8 /* °  sign*/) + "C"; 
  drawCentreString(tempString, SCREEN_WIDTH/2 + 25, 20);
  String humString = String(humidity) + "%"; 
  drawCentreString(humString, SCREEN_WIDTH/2, 40);

  display.display();
  delay(500);
}
