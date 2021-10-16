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

// Include logging lib
#include "SerialLogging.h"

/////////////////////////////////////////////////////////////////////////////////////////:
/////////////////////////////////////////////////////////////////////////////////////////:
/////////////////////////////////////////////////////////////////////////////////////////:

// Logging, change this to enable or disable logging
// Possible values: true, false
bool LoggingEnabled = true;
SerialLogging Logging(LoggingEnabled);

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

// Time variable
unsigned long Epoch;

// Variable to retrieve time from server
int WUHour = 0;
int WUMinute = 0;

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
  Logging.SetSpeed(9600);
  
  // Setup leds matrix
  Matrix.begin();
  Matrix.show();

  // Setup Wifi
  if (WiFi.status() == WL_NO_MODULE) {
    Logging.Print("Communication with WiFi module failed!");
    while (true);
  }

  String FirmwareVersion = WiFi.firmwareVersion();
  if (FirmwareVersion < WIFI_FIRMWARE_LATEST_VERSION) {
    Logging.Print("Please upgrade the firmware");
  }

  while (WifiStatus != WL_CONNECTED) {
    Logging.Print("Attempting to connect to WPA SSID: ", false);
    Logging.Print((String) WifiSSID);
    
    // Connect to WPA/WPA2 network:
    WifiStatus = WiFi.begin(WifiSSID, WifiPasswd);

    // Wait 5 seconds for connection:
    delay(5000);
  }
  Logging.Print("Connected !");

  Logging.Print("Start time client... ", false);
  TimeClient.begin();
  Logging.Print("OK");
  
  Logging.Print("Update time client... ", false);
  while (!TimeClient.update()) {
     Logging.Print("NTP update did not work");
  }
  Logging.Print("OK");
  Logging.Print("Adjust local clock... ", false);
  setTime(TimeClient.getEpochTime());
  Logging.Print("OK");
}

void loop() {
  // Code to switch images
  if (ImageIndex != NewImageIndex) {
    ImageIndex = NewImageIndex;
    
    Matrix.clear();
    if (ImageIndex == 0) {
      Logging.Print("Display moon at brightness 1");
      Matrix.setBrightness(1);
      DisplayMoonPixels();
    } else if (ImageIndex == 1) {
      Logging.Print("Display sun at brightness 5");
      Matrix.setBrightness(5);
      DisplaySunPixels();
    }
    Matrix.show();
  }

  Logging.Print("Sleep for 45 seconds...");
  delay(45000);

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
    Logging.Print("Status code: ", false);
    Logging.Print((String) ResponseStatusCode);
    Logging.Print("Response: ", false);
    Logging.Print((String) WebServerResponse);
  
    JSONVar InputDatetime = JSON.parse(WebServerResponse);
    if (JSON.typeof(InputDatetime) != "undefined") {
      WUHour = (int) InputDatetime["attributes"]["hour"];
      WUMinute = (int) InputDatetime["attributes"]["minute"];
      Logging.Print("Hour: ", false);
      Logging.Print((String) WUHour);
      Logging.Print("Minute: ", false);
      Logging.Print((String) WUMinute);
    } else {
      Logging.Print("Parsing response JSON failed!");
    }

    String WUHourMinute = "";
    if (WUHour < 10) {
        WUHourMinute = "0";
    }
    WUHourMinute += (String) WUHour + ":";
    if (WUMinute < 10) {
      WUHourMinute += "0";
    }
    WUHourMinute += (String) WUMinute;
    
    String NowHourMinute = GetEpochStringByParams(CE.toLocal(now()), "%H:%M");
    Logging.Print("WUHourMinute: ", false);
    Logging.Print((String) WUHourMinute);
    Logging.Print("NowHourMinute: ", false);
    Logging.Print((String) NowHourMinute);
    if (WUHourMinute == NowHourMinute) {
      NewImageIndex = 1;
    }
  }
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
