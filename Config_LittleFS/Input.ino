// ****************************************************************
// Sketch Esp32 Input Modular(Tab)
// created: Jens Fleischer, 2020-12-13
// last mod: Jens Fleischer, 2020-12-13
// ****************************************************************
// Hardware: Esp32
// Software: Esp32 Arduino Core 1.0.0 - 2.0.7
// Getestet auf: ESP32 NodeMCU-32s
/******************************************************************
  Copyright (c) 2020 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************/
// Diese Version von Sollwert kann als Tab eingebunden werden.
// #include <WebServer.h> muss im Haupttab aufgerufen werden.
// Die Funktionalität des ESP32 Webservers ist erforderlich.
// Die Funktion "setupModify();" muss im Setup aufgerufen werden.
/**************************************************************************************/

#include <map>

// Die Initialisierungsliste der "map" kann angepasst bzw. beliebig erweitert werden.
std::map<String, String>input;

void setupModify() {
  input.insert(std::make_pair("GW",  String(user_GW[0])+"."+String(user_GW[1])+"."+String(user_GW[2])+"."+String(user_GW[3])));
  input.insert(std::make_pair("NM",  String(user_NM[0])+"."+String(user_NM[1])+"."+String(user_NM[2])+"."+String(user_NM[3])));
  input.insert(std::make_pair("IP",  String(user_IP[0])+"."+String(user_IP[1])+"."+String(user_IP[2])+"."+String(user_IP[3])));
  input.insert(std::make_pair("PASSWD", "*****"));
  input.insert(std::make_pair("SSID", user_SSID));

  server.on("/modified", HTTP_POST, []() {
    if (server.args()) for (auto i = 0; i < server.args(); i++) input.find(server.argName(i))->second = server.arg(i).c_str();
    String temp = "{\"";
    for (auto& el : usermap) {
      if (temp != "{\"") temp += "\",\"";
      temp += el.first;
      temp += "\":\"";
      temp += el.second;
    }
    temp += "\"}";
    server.send(200, "application/json", temp);
    // Zugriff mittels input.at("Name");
    
  });
}
