#include <Arduino.h>

#ifdef LED_BUILTIN
#define LED LED_BUILTIN
#else 
#define LED PC13
//#define LED PB13
#endif

// Uncomment this macro to do a Serial1 <--> Serial2 bridge test
//#define DO_BRIDGE_TEST

static int i=0;
void setup(){
    // Serial is a macro for Serial1 only if no USB CDC is used
    // otherwise it's the USB serial
#if USBCON
    Serial.begin(115200);
    Serial.println("Start on Serial(USB)!");
#endif
#ifdef HAVE_HWSERIAL1
    Serial1.begin(115200);
    Serial1.println("Start on Serial1!");
#endif
#ifdef HAVE_HWSERIAL2
    Serial2.begin(115200);
    Serial2.println("Start on Serial2!");
#endif
#ifdef HAVE_HWSERIAL3
    Serial3.begin(115200);
    Serial3.println("Start on Serial3!");
#endif
    pinMode(LED, OUTPUT);
}

void loop(){
#if !defined(DO_BRIDGE_TEST)
    digitalWrite(LED, LOW);
    delay(500);
    digitalWrite(LED, HIGH);
    delay(500);
#if USBCON
    Serial.println("SerialUSB Blinky nr. " + String(i));
#endif
#ifdef HAVE_HWSERIAL1
    Serial1.println("Serial1 Blinky nr. " + String(i));
#endif
#ifdef HAVE_HWSERIAL2
    Serial2.println("Serial2 Blinky nr. " + String(i));
#endif
#ifdef HAVE_HWSERIAL3
    Serial3.println("Serial3 Blinky nr. " + String(i));
#endif
    i++;
    /* Forward everything from Serial1 to Serial2 */
    /* Forward everything from Serial2 to Serial1 */
    /* Uses readString() for a timed read. */
    /* Use .setTimeout() to change this behavior. */
#elif defined(HAVE_HWSERIAL1) && defined(HAVE_HWSERIAL2)
    if(Serial1.available() > 0) {
        Serial2.print("From Serial1: " + Serial1.readString());
    }
    if(Serial2.available() > 0) {
        Serial1.print("From Serial2: " + Serial2.readString());
    }
#else
#error "Can't do bridge test because we either don't have HWSERIAL1 or 2."
#endif
}