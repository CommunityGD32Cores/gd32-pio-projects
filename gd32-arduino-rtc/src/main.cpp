#include <Arduino.h>
#include <HardwareRTC.h>

#ifdef LED_BUILTIN
#define LED LED_BUILTIN
#else 
#define LED PC13
//#define LED PB13
#endif 

/* macros for build-time date and time */
#define BUILDTM_YEAR (\
    __DATE__[7] == '?' ? 1900 \
    : (((__DATE__[7] - '0') * 1000 ) \
    + (__DATE__[8] - '0') * 100 \
    + (__DATE__[9] - '0') * 10 \
    + __DATE__[10] - '0'))

#define BUILDTM_MONTH (\
    __DATE__ [2] == '?' ? 1 \
    : __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
    : __DATE__ [2] == 'b' ? 2 \
    : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
    : __DATE__ [2] == 'y' ? 5 \
    : __DATE__ [2] == 'l' ? 7 \
    : __DATE__ [2] == 'g' ? 8 \
    : __DATE__ [2] == 'p' ? 9 \
    : __DATE__ [2] == 't' ? 10 \
    : __DATE__ [2] == 'v' ? 11 \
    : 12)

#define BUILDTM_DAY (\
    __DATE__[4] == '?' ? 1 \
    : ((__DATE__[4] == ' ' ? 0 : \
    ((__DATE__[4] - '0') * 10)) + __DATE__[5] - '0'))

#define COMPUTE_BUILD_HOUR ((__TIME__[0] - '0') * 10 + __TIME__[1] - '0')
#define COMPUTE_BUILD_MIN  ((__TIME__[3] - '0') * 10 + __TIME__[4] - '0')
#define COMPUTE_BUILD_SEC  ((__TIME__[6] - '0') * 10 + __TIME__[7] - '0')

// set current time: year, month, date, hour, minute, second
//UTCTimeStruct timeNow = { 2022, 1, 2, 20, 40, 0 }; 
UTCTimeStruct timeNow = { BUILDTM_YEAR, BUILDTM_MONTH, BUILDTM_DAY, COMPUTE_BUILD_HOUR, COMPUTE_BUILD_MIN, COMPUTE_BUILD_SEC}; 

void secondIntCallback(void);

#define PREVENT_TIME_SETBACK

void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(115200);

  #ifdef PREVENT_TIME_SETBACK
  //only set time if the to be set time is higher
  //we don't want to travel back in time in case of a reset
  UTCTimeStruct currentRTCTime;
  rtc.getUTCTime(&currentRTCTime);
  uint32_t timeToSet = mkTimtoStamp(&timeNow);
  uint32_t timeInRTC = mkTimtoStamp(&currentRTCTime);
  if(timeInRTC > timeToSet) {
    Serial.println("Prevented the RTC time to be set to a time in the past.");
    Serial.print("Timestamp in RTC: ");
    Serial.println(timeInRTC);
    Serial.print("Timestamp of new wanted date: ");
    Serial.println(timeToSet);
  } else {
    rtc.setUTCTime(&timeNow);
    Serial.print("Previous timestamp in RTC: ");
    Serial.println(timeInRTC);
    Serial.print("Set RTC timestamp to: ");
    Serial.println(timeToSet);
  }
  #else
  //set time unconditionally
  rtc.setUTCTime(&timeNow);
  #endif
  rtc.attachInterrupt(secondIntCallback, INT_SECOND_MODE);
}

// the loop function runs over and over again forever
void loop()
{
  delay(1000);
  //print current time every second
  Serial.print(timeNow.year, DEC);
  Serial.print('-');
  Serial.print(timeNow.month, DEC);
  Serial.print('-');
  Serial.print(timeNow.day, DEC);
  Serial.print(' ');
  Serial.print(timeNow.hour, DEC);
  Serial.print(':');
  Serial.print(timeNow.minutes, DEC);
  Serial.print(':');
  Serial.print(timeNow.seconds, DEC);

  //unix timestamp
  Serial.print(" (unix ");
  Serial.print(mkTimtoStamp(&timeNow));
  Serial.println(")");
}

// second interrupt callback function
void secondIntCallback(void)
{
  rtc.getUTCTime(&timeNow); //update global var that is printed
  digitalToggle(LED);
}
