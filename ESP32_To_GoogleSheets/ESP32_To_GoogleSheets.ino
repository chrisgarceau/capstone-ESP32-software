//Include required libraries
#include <WiFi.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include "time.h"

// NTP Server (Internet Time Servers) - sends request packet and a time stamp packet is returned
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000; // Eastern Standard (UTC-5) offset
const int   daylightOffset_sec = 3600; // Daylight savings offset

// WiFi credentials
#define SSID "Jaspernet"       // change SSID
#define PASSWORD ""   // change password

// Google ScriptID
String GOOGLE_SCRIPT_ID = "AKfycbz9MwfII7MLSGk-jYZ3VK_fnJ_wT_lMjQGZH37mwJGU0mToYE97SpdeUNEcBQtsUfqxBQ"; // Web application deployment ID
int count = 0;

// Init WiFi connection
void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(1000);
  // Connect to WiFi source
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.print(SSID);
  Serial.flush();
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Connected.");
  }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}


void loop() {
   if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    String car_enter = "0";
    String car_exit = "0";

    // Generate current time string
    char timeStringBuff[15];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);
    String timeStr(timeStringBuff);

    // Generate current date string
    char dateStringBuff[50]; //50 chars should be enough
    strftime(dateStringBuff, sizeof(dateStringBuff), "%m/%d/%Y", &timeinfo);
    String dateStr(dateStringBuff);
  

    String urlFinal = "https://script.google.com/macros/s/"+ GOOGLE_SCRIPT_ID + "/exec?" + "car_enter=" + car_enter + "&car_exit=" + car_exit + "&date=" + dateStr + "&time=" + timeStr + "&total_cars=" + String(count);
    Serial.print("URL: ");
    Serial.println(urlFinal);
    Serial.println("POSTED Data to Google Spreadsheet:");
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    // getting response from Google Sheet / Google Script (deployed as a Web Server)
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: ");
        Serial.println(payload);    
    }
    //---------------------------------------------------------------------
    http.end();
  }
  Serial.println();
  count++;
  delay(1000);
} 

