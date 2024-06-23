// ****************************************************************
// Sketch Esp32 Lokalzeit Modular(Tab)
// created: Jens Fleischer, 2018-07-15
// last mod: Jens Fleischer, 2020-03-27
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
// Diese Version von Lokalzeit sollte als Tab eingebunden werden.
// #include <WebServer.h> oder #include <WiFi.h> muss im Haupttab aufgerufen werden
// Funktion "setupTime();" muss im setup() nach dem Verbindungsaufbau aufgerufen werden.
/**************************************************************************************/

#include "time.h"

struct tm tm;

const char* const PROGMEM ntpServer[] = {"fritz.box", "de.pool.ntp.org", "at.pool.ntp.org", "ch.pool.ntp.org", "ptbtime1.ptb.de", "europe.pool.ntp.org"};
const char* const PROGMEM dayNames[] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
const char* const PROGMEM dayShortNames[] = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
const char* const PROGMEM monthNames[] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
const char* const PROGMEM monthShortNames[] = {"Jan", "Feb", "Mrz", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"};

bool getTime() {                                                   // Zeitzone einstellen https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
  configTzTime("CET-1CEST,M3.5.0/02,M10.5.0/03", ntpServer[4]);    // deinen NTP Server einstellen (von 0 - 5 aus obiger Liste)
  if (!getLocalTime(&tm)) return false;
  return true;
}

void setupTime() {
  if (!getTime()) {
    DEBUG_P("Zeit konnte nicht geholt werden\n");
  } else {
    getLocalTime(&tm);
    DEBUG_P(&tm, "Programmstart: %A, %B %d %Y %H:%M:%S");
    //DEBUG_P(dayNames[tm.tm_wday]);               // druckt den aktuellen Tag
    //DEBUG_P(monthNames[tm.tm_mon]);              // druckt den aktuellen Monat
    //DEBUG_P(dayShortNames[tm.tm_wday]);          // druckt den aktuellen Tag (Abk.)
    //DEBUG_P(monthShortNames[tm.tm_mon]);         // druckt den aktuellen Monat (Abk.)
    //DEBUG_P(tm.tm_isdst ? "Sommerzeit" : "Normalzeit");
  }
  server.on("/zeit", []() {
    server.send(200, "application/json",  "\"" + (String)localTime() + "\"");
  });
}

char* localTime() {
  static char buf[20];                                  // je nach Format von "strftime" eventuell die Größe anpassen
  static time_t lastsec {0};
  getLocalTime(&tm, 50);
  if (tm.tm_sec != lastsec) {
    lastsec = tm.tm_sec;
    strftime (buf, sizeof(buf), "%d.%m.%Y %T ", &tm);   // http://www.cplusplus.com/reference/ctime/strftime/
    time_t now;
    if (!(time(&now) % 86400)) getTime();               // einmal am Tag die Zeit vom NTP Server holen o. jede Stunde "% 3600" aller zwei "% 7200"
  }
  return buf;
}
