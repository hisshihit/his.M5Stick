#include <M5StickC.h>
#include <WiFi.h>
#include "time.h"

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

//My hone WiFi,NTP Server
const char* ssid       = "wifi-SSID";
const char* password   = "wifi-password";
const char* ntpServer =  "ntp.jst.mfeed.ad.jp";

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  M5.Lcd.setCursor(40, 0, 2);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.println("RTC time");
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.Rtc.GetData(&RTC_DateStruct);
  M5.Lcd.setCursor(0, 15);
  M5.Lcd.printf("Data: %04d-%02d-%02d\n", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
  M5.Lcd.printf("Week: %d\n", RTC_DateStruct.WeekDay);
  M5.Lcd.printf("Time: %02d : %02d : %02d\n", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
  delay(500);
  if (M5.BtnA.wasReleased())
  {
    // connect to WiFi
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" CONNECTED");

    // Set ntp time to local
//  configTime(9 * 3600, 0, ntpServer); //JST
    configTime(0 * 3600, 0, ntpServer); //UTC

    // Get local time
    struct tm timeInfo;

    if (getLocalTime(&timeInfo)) {
      Serial.print("NTP : ");
      Serial.println(ntpServer);

      Serial.printf("%d %d %d ", timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday);
      Serial.printf("%d %d %d ", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
      Serial.println(timeInfo.tm_wday);
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setCursor(40, 0, 2);
      M5.Lcd.setTextColor(YELLOW, BLACK);
      M5.Lcd.println("NTP sync");

      RTC_TimeTypeDef TimeStruct;
      TimeStruct.Hours   = timeInfo.tm_hour;
      TimeStruct.Minutes = timeInfo.tm_min;
      TimeStruct.Seconds = timeInfo.tm_sec;
      M5.Rtc.SetTime(&TimeStruct);
      RTC_DateTypeDef DateStruct;
      DateStruct.WeekDay = timeInfo.tm_wday;
      DateStruct.Month = timeInfo.tm_mon + 1;
      DateStruct.Date = timeInfo.tm_mday;
      DateStruct.Year = timeInfo.tm_year + 1900;
      M5.Rtc.SetData(&DateStruct);
    }
    WiFi.disconnect();
    delay(500);
  }
  M5.update();
}
