#include <Arduino.h>
#include <WiFi.h>
#include <Credentials.h>

const char *ssid = WIFI_SSID;
const char *password = WIFI_PW;

void setup()
{
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
}
