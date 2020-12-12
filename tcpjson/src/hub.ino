/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "hub.h"

#ifndef APSSID
#define APSSID "Hub"
#define APPSK  "KissMyHub123"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

bool flagRec = false;

ConnectedClient clientList[4] = {{NULL, 0}, {NULL, 0}, {NULL, 0}, {NULL, 0}};

const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;

int port = 8888;  //Port number
WiFiServer server(port);

//JSON object container.
StaticJsonDocument<256> jdoc;

char str01[100];
char buffer[100];
int idx=0;

int Matrix[100];

void setup() {
  delay(100);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  //TCP/ip server.
  server.begin();
  Serial.print("TCP Server Started:");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(port);

  //example of interpreting json string to object.
  deserializeJson(jdoc, "{\"child\":33,\"addr\":1220}");

  Serial.println((int)jdoc["child"]);

  pinMode(LED_BUILTIN, OUTPUT);
  
}

void saveIntMatrix(int idx, int data)
{
  Matrix[idx] = data;
}


void loop() {

  WiFiClient client = server.available();
  
  if (client) {
    if(client.connected())
    {
      Serial.println("Client Connected");
    }
    
    while(client.connected()){      
      while(client.available()>0){
        // read data from the connected client
        buffer[idx] =  client.read();
        idx++;
        //Serial.write(client.read());
        flagRec = true;
      }

      idx = 0;

      if(flagRec)
      {
        //Serial.print(buffer);
        flagRec = false;
        deserializeJson(jdoc, buffer);

        // {"type":1, "data":210}

        Serial.println((int)jdoc["type"]);

        if( 22 == (int)jdoc["type"])
        {
          saveIntMatrix(0, (int)jdoc["data"]);
        }

        if(Matrix[0] == 0)
        {
          digitalWrite(LED_BUILTIN, 0);
          sprintf(str01, "{\"type\":88, \"data\":%d, \"text\":\"LED_OFF\"}", 102);
          client.write(str01);
        }
        else
        {
          digitalWrite(LED_BUILTIN, 1);
          sprintf(str01, "{\"type\":88, \"data\":%d, \"text\":\"LED_ON\"}", 102);
          client.write(str01);
        }
        

      }

      //Send Data to connected client
      /*
      while(Serial.available()>0)
      {
        client.write(Serial.read());
      }
      */
    }
    client.stop();
    Serial.println("Client disconnected");    
  }
  
}
