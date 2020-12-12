#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

typedef struct __ConnectedClient
{
    WiFiClient *client;
    int clientID;
}ConnectedClient;

