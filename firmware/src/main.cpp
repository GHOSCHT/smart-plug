#include <Arduino.h>
#include <WiFi.h>
#include <Credentials.h>
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <ResourceNode.hpp>
#include <ArduinoJson.h>
using namespace httpsserver;

const char *ssid = WIFI_SSID;
const char *password = WIFI_PW;
HTTPServer myServer = HTTPServer();
void handleRoot(HTTPRequest *req, HTTPResponse *res);
void handleWebHook(HTTPRequest *req, HTTPResponse *res);
void handleJson(HTTPRequest *req, HTTPResponse *res);

ResourceNode *nodeRoot = new ResourceNode("/", "GET", &handleRoot);
ResourceNode *nodeWebHook = new ResourceNode("/hook", "POST", &handleWebHook);
ResourceNode *nodeWebHook = new ResourceNode("/json", "POST", &handleJson);

void setup()
{
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  WiFi.hostname("SmartPlug");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  myServer.registerNode(nodeRoot);
  myServer.registerNode(nodeWebHook);
  myServer.start();
  pinMode(25, OUTPUT);
  pinMode(33, OUTPUT);
  digitalWrite(25, LOW);
  digitalWrite(33, HIGH);
}

void handleRoot(HTTPRequest *req, HTTPResponse *res)
{
  res->setHeader("Content-Type", "text/html");
  res->println("<!DOCTYPE html>");
  res->println("<html>");
  res->println("<head><title>Smart Plug</title></head>");
  res->println("<body>");
  res->println("<h1>Smart Plug</h1>");
  res->print("<p>send a PUSH request to /hook with the urlencoded 'status' variable set to 0 or 1</p>");
  res->println("</body>");
  res->println("</html>");
}

void handleWebHook(HTTPRequest *req, HTTPResponse *res)
{
  int capacity = 50;
  char *buffer = new char[capacity + 1];
  memset(buffer, 0, capacity + 1);

  size_t idx = 0;
  while (!req->requestComplete() && idx < capacity)
  {
    idx += req->readChars(buffer + idx, capacity - idx);
  }

  char delim[] = "status=";
  char *status = strtok(buffer, delim);
  while (status != NULL)
  {
    if (status[0] == '0')
    {
      digitalWrite(25, HIGH);
      digitalWrite(33, LOW);
    }
    else if (status[0] == '1')
    {
      digitalWrite(25, LOW);
      digitalWrite(33, HIGH);
    }
    Serial.printf("'%s'\n", status);
    status = strtok(NULL, delim);
  }
}

void handleJson(HTTPRequest *req, HTTPResponse *res)
{
  const size_t capacity = JSON_OBJECT_SIZE(4) + 180;
  DynamicJsonDocument jsonBuffer(capacity);
  char *buffer = new char[capacity + 1];
  memset(buffer, 0, capacity + 1);

  size_t idx = 0;
  while (!req->requestComplete() && idx < capacity)
  {
    idx += req->readChars(buffer + idx, capacity - idx);
  }

  res->setStatusCode(200);

  deserializeJson(jsonBuffer, buffer);
  byte status = jsonBuffer["status"];
  if (status == 0)
  {
    digitalWrite(25, HIGH);
    digitalWrite(33, LOW);
  }
  else if (status == 1)
  {
    digitalWrite(25, LOW);
    digitalWrite(33, HIGH);
  }
  Serial.println(status);
}

void loop()
{
  myServer.loop();
  delay(1);
}
