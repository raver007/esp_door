// ****************************************************************
// Sketch Esp32 Connect Modular(Tab) mit optischer Anzeige
// created: Jens Fleischer, 2018-07-06
// last mod: Jens Fleischer, 2020-03-26
// For more information visit: https://fipsok.de
// ****************************************************************
// Hardware: Esp32
// Software: Esp32 Arduino Core 1.0.0 - 2.0.7
// Getestet auf: ESP32 NodeMCU-32s
/******************************************************************
  Copyright (c) 2018 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************/
// Diese Version von Connect sollte als Tab eingebunden werden.
// #include <WebServer.h> muss im Haupttab aufgerufen werden
// Die Funktionalität des ESP32 Webservers ist erforderlich.
// Die Funktion "Connect();" muss im Setup eingebunden werden.
/**************************************************************************************/

const char* ssid = "FRITZ!Box 7490";               // << kann bis zu 32 Zeichen haben
const char* password = "31822084650730466392";    // << mindestens 8 Zeichen jedoch nicht länger als 64 Zeichen

//const char* ssid ="AndroidAP";
//const char* password ="ggrn1304";

//const char* ssid ="BuzzingBy";
//const char* password ="dticclmpamdc?";

void Connect() {                                 // Funktionsaufruf "Connect();" muss im Setup nach "spiffs();" eingebunden werden
  pinMode(LED_BUILTIN, OUTPUT);                  // OnBoardLed ESP32 Dev Module
  WiFi.mode(WIFI_STA);
    if (!user_DHCP)
  { //WiFi.config(ip, dns, gateway, subnet);
    Serial.printf("Static IP: %d.%d.%d.%d\n",user_IP[0],user_IP[1],user_IP[2],user_IP[3] );
    WiFi.config(IPAddress(user_IP[0],user_IP[1],user_IP[2],user_IP[3] ),
                IPAddress(user_GW[0],user_GW[1],user_GW[2],user_GW[3] ),
                IPAddress(user_NM[0],user_NM[1],user_NM[2],user_NM[3] ),
                (8,8,8,8)
                ) ;
  }else{
    Serial.println("Dynamic IP:");
  }
  if(!ssid){
    WiFi.begin(user_SSID.c_str(), user_PASSWD.c_str());
    }else{
    WiFi.begin(ssid, password);
  }
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, 1);
    delay(250);
    digitalWrite(LED_BUILTIN, 0);
    delay(250);
    DEBUG_F(".");
    if (millis() > 10000) {
      DEBUG_P("\nVerbindung zum AP fehlgeschlagen\n\n");
      ESP.restart();
    }
  }
  DEBUG_P("\nVerbunden mit: " + WiFi.SSID());
  DEBUG_P("Esp32 IP: " + WiFi.localIP().toString() + "\n");
}
