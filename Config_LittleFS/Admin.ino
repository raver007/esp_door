// ****************************************************************
// Sketch Esp32 Admin Modular(Tab)
// created: Jens Fleischer, 2021-05-01
// last mod: Jens Fleischer, 2023-11-12
// For more information visit: https://fipsok.de
// ****************************************************************
// Hardware: Esp32
// Software: Esp32 Arduino Core 1.0.6 - 2.0.14
// Geprüft: bei 4MB Flash
// Getestet auf: ESP32 NodeMCU-32s
/******************************************************************
  Copyright (c) 2021 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************/
// Diese Version von Admin sollte als Tab eingebunden werden.
// #include "SPIFFS.h" #include <WebServer.h> müssen im Haupttab aufgerufen werden
// Die Funktionalität des ESP32 Webservers ist erforderlich.
// Die Spiffs.ino muss im ESP32 Webserver enthalten sein
// Funktion "admin();" muss im setup() nach spiffs() aber vor dem Verbindungsaufbau aufgerufen werden.
// Die Funktion "runtime();" muss mindestens zweimal innerhalb 49 Tage aufgerufen werden.
// Entweder durch den Client(Webseite) oder zur Sicherheit im "loop();"
/**************************************************************************************/

#include <rom/rtc.h>

const char* const PROGMEM flashChipMode[] = {"QIO", "QOUT", "DIO", "DOUT", "Unbekannt"};
const char* const PROGMEM resetReason[] = {"ERR", "Power on", "Unknown", "Software", "Watch dog", "Deep Sleep", "SLC module", "Timer Group 0", "Timer Group 1",
"RTC Watch dog", "Instrusion", "Time Group CPU", "Software CPU", "RTC Watch dog CPU", "Extern CPU", "Voltage not stable", "RTC Watch dog RTC"};

void admin() {                          // Funktionsaufruf "admin();" muss im Setup eingebunden werden
 /* File file = SPIFFS.open("/config_1.json");
  if (file) {
    String Hostname = file.readStringUntil('\n');
    if (Hostname != "") {
      WiFi.setHostname(Hostname.substring(2, Hostname.length() - 2).c_str());
      ArduinoOTA.setHostname(WiFi.getHostname());
    }
  }
  file.close();*/
  String Hostname=user_HOSTNAME;
  if (Hostname != "") {
      WiFi.setHostname(Hostname.substring(0, Hostname.length() ).c_str());
      ArduinoOTA.setHostname(WiFi.getHostname());
    }
  server.on("/admin/renew", handlerenew);
  server.on("/admin/once", handleonce);
  server.on("/reconnect", []() {
    server.send(304, "message/http");
    WiFi.reconnect();
  });
  server.on("/restart", []() {
    server.send(304, "message/http");
    //save();          //Einkommentieren wenn Werte vor dem Neustart gesichert werden sollen
    ESP.restart();
  });
}

void handlerenew() {
  server.send(200, "application/json", R"([")" + runtime() + R"(",")" + temperatureRead() + R"(",")" + WiFi.RSSI() + R"("])");     // Json als Array
}

void handleonce() {
  if (server.arg(0) != "") {
    WiFi.setHostname(server.arg(0).c_str());
    File file = SPIFFS.open("/config_1.json", FILE_WRITE);
    file.printf("[\"%s\"]", WiFi.getHostname());
    file.close();
  }           
  String fname = String(__FILE__).substring( 3, String(__FILE__).lastIndexOf ('\\'));
  String temp = R"({"File":")" + fname.substring(fname.lastIndexOf ('\\') + 1, fname.length()) + R"(", "Build":")" +  (String)__DATE__ + " " + (String)__TIME__ +
                R"(", "SketchSize":")" + formatBytes(ESP.getSketchSize()) + R"(", "SketchSpace":")" + formatBytes(ESP.getFreeSketchSpace()) +
                R"(", "LocalIP":")" +  WiFi.localIP().toString() + R"(", "Hostname":")" + WiFi.getHostname() + R"(", "SSID":")" + WiFi.SSID() +
                R"(", "GatewayIP":")" +  WiFi.gatewayIP().toString() +  R"(", "Channel":")" +  WiFi.channel() + R"(", "MacAddress":")" +  WiFi.macAddress() +
                R"(", "SubnetMask":")" +  WiFi.subnetMask().toString() + R"(", "BSSID":")" +  WiFi.BSSIDstr() + R"(", "ClientIP":")" + server.client().remoteIP().toString() +
                R"(", "DnsIP":")" + WiFi.dnsIP().toString() + R"(", "ChipModel":")" + ESP.getChipModel() + R"(", "Reset1":")" + resetReason[rtc_get_reset_reason(0)] +
                R"(", "Reset2":")" + resetReason[rtc_get_reset_reason(1)] + R"(", "CpuFreqMHz":")" + ESP.getCpuFreqMHz() + R"(", "HeapSize":")" + formatBytes(ESP.getHeapSize()) +
                R"(", "FreeHeap":")" + formatBytes(ESP.getFreeHeap()) + R"(", "MinFreeHeap":")" + formatBytes(ESP.getMinFreeHeap()) +
                R"(", "ChipSize":")" +  formatBytes(ESP.getFlashChipSize()) + R"(", "ChipSpeed":")" + ESP.getFlashChipSpeed() / 1000000 +
                R"(", "ChipMode":")" + flashChipMode[ESP.getFlashChipMode()] + R"(", "IdeVersion":")" + ARDUINO + R"(", "SdkVersion":")" + ESP.getSdkVersion() + R"("})";
  server.send(200, "application/json", temp);     // Json als Objekt
}

String runtime() {
  static uint8_t rolloverCounter;
  static uint32_t previousMillis;
  uint32_t currentMillis {millis()};
  if (currentMillis < previousMillis) {
    rolloverCounter++;                       // prüft Millis Überlauf
    DEBUG_P("Millis Überlauf");
  }
  previousMillis = currentMillis;
  uint32_t sec {(0xFFFFFFFF / 1000) * rolloverCounter + (currentMillis / 1000)};
  char buf[20];
  snprintf(buf, sizeof(buf), "%d Tag%s %02d:%02d:%02d", sec / 86400, sec < 86400 || sec >= 172800 ? "e" : "", sec / 3600 % 24, sec / 60 % 60, sec % 60);
  return buf;
}
