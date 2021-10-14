// Needed to store stuff in Flash using PROGMEM
#include <avr/pgmspace.h>
// Required to control the addressable leds matrix
#include <Adafruit_NeoPixel.h>
//Required to connect to WIFI
#include <SPI.h>
#include <WiFiNINA.h>
// Required to make http requests
#include <ArduinoHttpClient.h>
// Required to read json data
#include <Arduino_JSON.h>
// Required to check world time
#include <NTPClient.h>
#include <WiFiUdp.h>
// Required to set the timezone
#include <Timezone.h>

// Include utils functions
#include "utils.h"

// Include credentials
#include "secrets.h"

// Define the pins
#define LEDS_PIN 2

// Leds Matrix
#define PIXELS 256
Adafruit_NeoPixel Matrix(PIXELS, LEDS_PIN, NEO_GRB + NEO_KHZ800);

// Image index, Moon = 0 (default), Sun = 1, 99 is nothing
int ImageIndex = 99;
int NewImageIndex = 0;

// WIFI
char WifiSSID[] = WIFI_SSID;
char WifiPasswd[] = WIFI_PASS;
int WifiStatus = WL_IDLE_STATUS;

// WIFI Web Client
WiFiSSLClient WifiCli;
HttpClient HttpsCli = HttpClient(WifiCli, WEB_SERVER_ADDR, 443);

// Wifi UDP requests for NTP
int GTMOffset = 0; // SET TO UTC TIME
WiFiUDP NtpUDP;
NTPClient TimeClient(NtpUDP, "europe.pool.ntp.org", GTMOffset*60*60, 60*60*1000);
 
// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone CE(CEST, CET);

// Images
const int MoonData[256][3] PROGMEM =
{
  {0, 0, 0},    {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 61, 153},   {0, 61, 153},   {0, 61, 153},   {0, 61, 153},   {0, 61, 153},   {0, 61, 153},   {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},
  {0, 0, 0},    {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 61, 153},   {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},
  {0, 0, 0},    {0, 0, 0},      {0, 0, 0},      {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},
  {0, 0, 0},    {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 0, 0},      {0, 0, 0},
  {0, 0, 0},    {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},
  {0, 0, 0},    {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 61, 153},   {51, 133, 255}, {0, 61, 153},   {51, 133, 255}, {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 0, 0}, 
  {0, 61, 153}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {51, 133, 255}, {0, 61, 153},   {51, 133, 255}, {0, 61, 153},   {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0}, 
  {0, 0, 0},    {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 61, 153},   {51, 133, 255}, {0, 61, 153},   {51, 133, 255}, {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153}, 
  {0, 61, 153}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0}, 
  {0, 0, 0},    {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 61, 153},   {0, 61, 153},   {51, 133, 255}, {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {0, 61, 153}, 
  {0, 0, 0},    {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 61, 153},   {0, 61, 153},   {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 61, 153},   {0, 61, 153},   {0, 0, 0}, 
  {0, 0, 0},    {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 0, 0}, 
  {0, 0, 0},    {0, 0, 0},      {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 0, 0},      {0, 0, 0}, 
  {0, 0, 0},    {0, 0, 0},      {0, 0, 0},      {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 0, 0},      {0, 0, 0},      {0, 0, 0}, 
  {0, 0, 0},    {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 61, 153},   {0, 61, 153},   {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {51, 133, 255}, {0, 61, 153},   {0, 61, 153},   {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0}, 
  {0, 0, 0},    {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 61, 153},   {0, 61, 153},   {0, 61, 153},   {0, 61, 153},   {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0},      {0, 0, 0}
};
const int SunData[256][3] PROGMEM =
{
  {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {0, 0, 0},       {0, 0, 0},       {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0}, 
  {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {51, 51, 0},   {51, 51, 0},     {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {51, 51, 0},     {51, 51, 0},   {0, 0, 0},     {0, 0, 0},     {0, 0, 0}, 
  {0, 0, 0},     {0, 0, 0},     {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {51, 51, 0},   {0, 0, 0},     {0, 0, 0},     {0, 0, 0}, 
  {0, 0, 0},     {0, 0, 0},     {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0}, {255, 255, 0}, {51, 51, 0},   {0, 0, 0},
  {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0}, {255, 255, 0}, {51, 51, 0},   {0, 0, 0},
  {0, 0, 0},     {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {51, 51, 0},   {51, 51, 0},     {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {51, 51, 0},     {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {51, 51, 0}, 
  {255, 255, 0}, {255, 255, 0}, {51, 51, 0},   {51, 51, 0},   {51, 51, 0},   {51, 51, 0},     {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {51, 51, 0},
  {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {51, 51, 0},   {255, 255, 0}, {255, 255, 0},   {51, 51, 0},     {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {51, 51, 0},     {255, 255, 0},   {255, 255, 0}, {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, 
  {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {51, 51, 0},
  {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {255, 255, 0},    
  {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {51, 51, 0},   {51, 51, 0},   {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {51, 51, 0},
  {0, 0, 0},     {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {51, 51, 0},   {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {51, 51, 0}, 
  {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {255, 255, 0}, {51, 51, 0},   {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {51, 51, 0},     {255, 255, 0},   {255, 255, 0}, {255, 255, 0}, {51, 51, 0},   {0, 0, 0},
  {0, 0, 0},     {0, 0, 0},     {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {255, 255, 0},   {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {51, 51, 0},     {255, 255, 0},   {255, 255, 0}, {255, 255, 0}, {51, 51, 0},   {0, 0, 0}, 
  {0, 0, 0},     {0, 0, 0},     {51, 51, 0},   {255, 255, 0}, {255, 255, 0}, {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {51, 51, 0},   {0, 0, 0},     {0, 0, 0},     {0, 0, 0},
  {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},     {51, 51, 0},   {51, 51, 0},     {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {255, 255, 0},   {51, 51, 0},     {51, 51, 0},   {0, 0, 0},     {0, 0, 0},     {0, 0, 0}
};

void setup() {
  // Wait for serial port to connect
  // Needed for native USB port only
  Serial.begin(9600);
  
  // Setup leds matrix
  Matrix.begin();
  Matrix.show();

  // Setup Wifi
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String FirmwareVersion = WiFi.firmwareVersion();
  if (FirmwareVersion < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  while (WifiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WifiSSID);
    // Connect to WPA/WPA2 network:
    WifiStatus = WiFi.begin(WifiSSID, WifiPasswd);

    // Wait 5 seconds for connection:
    delay(5000);
  }

  // Start time client
  TimeClient.begin();
}

void loop() {
  // Code to switch images
  if (ImageIndex != NewImageIndex) {
    ImageIndex = NewImageIndex;
    
    Matrix.clear();
    if (ImageIndex == 0) {
        Matrix.setBrightness(1);
        DisplayMoonPixels();
    } else if (ImageIndex == 1) {
        Matrix.setBrightness(5);
        DisplaySunPixels();
    }
    Matrix.show();
  }

  // If Moon displayed
  // then request the server to know when the sun has to rise
  if (ImageIndex == 0) {
    // Code to request wake up time to Home Assistant
    HttpsCli.beginRequest();
    HttpsCli.get(API_URI);
    HttpsCli.sendHeader(API_AUTH_HEADER);
    HttpsCli.sendHeader("Content-Type: application/json");
    HttpsCli.endRequest();
    
    int ResponseStatusCode = HttpsCli.responseStatusCode();
    String WebServerResponse = HttpsCli.responseBody();
    // Serial.print("Status code: ");
    // Serial.println(ResponseStatusCode);
    // Serial.print("Response: ");
    // Serial.println(WebServerResponse);
  
    JSONVar InputDatetime = JSON.parse(WebServerResponse);
    if (JSON.typeof(InputDatetime) == "undefined") {
      Serial.println("Parsing response JSON failed!");
    }
    int WUHour = (int) InputDatetime["attributes"]["hour"];
    int WUMinute = (int) InputDatetime["attributes"]["minute"];
    // Serial.print("Hour: ");
    // Serial.println(WUHour);
    // Serial.print("Minute: ");
    // Serial.println(WUMinute);
  
    // Update time client
    if (TimeClient.update()) {
       // Serial.println("Adjust local clock");
       unsigned long Epoch = TimeClient.getEpochTime();
       setTime(Epoch);
    } else {
       Serial.println("NTP update did not work");
    }
  
    String WUHourMinute = (String) WUHour + ":" + (String) WUMinute;
    String NowHourMinute = GetEpochStringByParams(CE.toLocal(now()), "%H:%M");
    if (WUHourMinute == NowHourMinute) {
      // Serial.print("WUHourMinute: ");
      // Serial.println(WUHourMinute);
      // Serial.print("NowHourMinute: ");
      // Serial.println(NowHourMinute);
      NewImageIndex = 1;
    }
  }

  // Sleep for 45 seconds
  delay(45000);
}

void DisplayMoonPixels() {
  for (int LedID = 0; LedID < 256; LedID++) {
    Matrix.setPixelColor(
      LedID,
      pgm_read_dword(&(MoonData[LedID][0])),
      pgm_read_dword(&(MoonData[LedID][1])),
      pgm_read_dword(&(MoonData[LedID][2]))
    );
  }
}

void DisplaySunPixels() {
  for (int LedID = 0; LedID < 256; LedID++) {
    Matrix.setPixelColor(
      LedID,
      pgm_read_dword(&(SunData[LedID][0])),
      pgm_read_dword(&(SunData[LedID][1])),
      pgm_read_dword(&(SunData[LedID][2]))
    );
  }
}
