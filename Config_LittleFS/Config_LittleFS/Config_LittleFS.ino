/*********
 fucking cool guy
  Complete project details at https://SuckMyDick.com  

ILI
-------------------------------------------------
PEN   |   2
MISO  |   13
MOSI  |   12
CS2   |   17
SCK   |   14
MISO  |   13
LED   |   18
SCK   |   14
MOSI  |   12
DC    |   4
RESET |   22   
CS1   |   16
---------------------------------------------------

DS18B20
---------------------------------------------------
Data  |   19
---------------------------------------------------

Analog In
---------------------------------------------------
IN 1  |   32
IN 2  |   33
---------------------------------------------------

Relais
---------------------------------------------------
Rel_L1  |   25
Rel_L2  |   26
Rel_L3  |   27
Rel_MV1 |   23
Rel_MV2 |   21
---------------------------------------------------

LED
---------------------------------------------------
Online  |   5
LED Rel1|   OC Rel1
LED Rel2|   OC Rel2
LED Rel3|   OC Rel3
LED Rel4|   OC Rel4
LED Rel5|   OC Rel5
---------------------------------------------------

*********/

#include "FS.h"
#include <LittleFS.h>
#include <ArduinoJson.h>  
#include <SPI.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <TFT_eSPI.h>      
#include <TFT_eWidget.h>           
//#include <TFT_eFX.h>  
//#include <EasyMFRC522.h>
//#include <MFRC522.h>
#include <map>

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>



#define USE_LittleFS              // Einkommentieren um den LittleFS Tab zu verwenden 

#define TFT_CS 16 // TFT select pin
#define TFT_DC  4 // TFT display/command pin
#define TFT_MOSI 12
#define TFT_CLK 14
#define TFT_RST 22
#define TFT_MISO 13

#define Buzzer 33
//#define Alarm 19
//#define Opener 23
#define RFID_RST 26
#define RFID_NCS 27

#define IWIDTH  240
#define IHEIGHT 30

#ifdef USE_LittleFS
  #define SPIFFS LittleFS
  #include <LittleFS.h>
#else
  #include <SPIFFS.h>
#endif

//#define DEBUGGING                     // Einkommentieren für die Serielle Ausgabe

#ifdef DEBUGGING
#define DEBUG_B(...) Serial.begin(__VA_ARGS__)
#define DEBUG_P(...) Serial.println(__VA_ARGS__)
#define DEBUG_F(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_B(...)
#define DEBUG_P(...)
#define DEBUG_F(...)
#endif

#define MAX_STRING_SIZE 100  // size of the char array that will be written to the tag
#define BLOCK 1              // initial block, from where the data will be stored in the tag
#define SIZE_BUFFER 18
#define MAX_SIZE_BLOCK 16
//used in authentication
MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
//EasyMFRC522 rfidReader(RFID_RST, RFID_NCS); //the Mifare sensor, with the SDA and RST pins given
                                //the default (factory) keys A and B are used (or used setKeys to change)

WebServer server(80);

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
TFT_eSprite img = TFT_eSprite(&tft);  // Create Sprite object "img" with pointer to "tft" object

          // Include the extension graphics functions library
//TFT_eFX  fex = TFT_eFX(&tft);    // Create TFT_eFX object "efx" with pointer to "tft" object
MFRC522DriverPinSimple ss_pin(27); // Create pin driver. See typical pin layout above.

//SPIClass &spiClass = SPI; // Alternative SPI e.g. SPI2 or from library e.g. softwarespi.
//SPIClass spiClass(VSPI);
//const SPISettings spiSettings = SPISettings(SPI_CLOCK_DIV4, MSBFIRST, SPI_MODE0); // May have to be set if hardware is not fully compatible to Arduino specifications.

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver.

MFRC522 mfrc522{driver}; // Create MFRC522 instance.

#define LED_BUILTIN 5

/* You only need to format LITTLEFS the first time you run a
   test or else use the LITTLEFS plugin to create a partition
   https://github.com/lorol/arduino-esp32littlefs-plugin */
   
#define FORMAT_LITTLEFS_IF_FAILED true

// define filename to store config file
const String config_filename = "/config.json";
const String user_filename = "/user.json";

// initialize the variables of the program
String user_SSID ="FRITZ!Box 7490";
String user_PASSWD ="31822084650730466392";

//String user_SSID ="BuzzingBy";
//String user_PASSWD ="dticclmpamdc?";

//String user_SSID ="AndroidAP";
//String user_PASSWD ="ggrn1304";


int user_IP[4] ={192,168,99,38};
int user_NM[4] ={255,255,255,0};
int user_GW[4] ={192,168,99,4};
bool user_DHCP = false;
String user_HOSTNAME = "";
String user_ntpServerName = "";
String user_Timezone = "";
bool user_SummerTime=false;
String user_MYSQLServer="";
String user_MySQLDatabase="";


// Die Initialisierungsliste der "map" kann angepasst bzw. beliebig erweitert werden.
//std::map<String, String>user;
std::map<String, String>usermap;
std::map<String, String>newUser;
 uint16_t t_x = 9999, t_y = 9999; 
 
void setup() {
  // Start serial interface

  DEBUG_B(115200);
  DEBUG_F("\nSketchname: %s\nBuild: %s\t\tIDE: %d.%d.%d\n\n", __FILE__, __TIMESTAMP__, ARDUINO / 10000, ARDUINO % 10000 / 100, ARDUINO % 100 / 10 ? ARDUINO % 100 : ARDUINO % 10);
  Serial.begin(115200);
  pinMode(Buzzer, OUTPUT);
//digitalWrite(Buzzer, HIGH);
  delay(100);
  digitalWrite(Buzzer, LOW);

  pinMode(26, OUTPUT);
  digitalWrite(26, HIGH);
  setupFS();
  readconfig();
  initUser();
  readUser();
  initTFT();
  initTOUCH();
  drawBmp("/kandou.bmp", 0, 0);
  tft.setTextColor(TFT_RED,TFT_WHITE);
  tft.setCursor(10, 100, 1);
  tft.print("Init System");
  tft.setCursor(10, 110, 1);
  tft.print("Wifi connect to");
  Connect();
  tft.setCursor(10, 120, 1);
  tft.print(" SSID: ");
  tft.print(WiFi.SSID());
  tft.setCursor(10, 130, 1);
  tft.print(" IP:  ");
  tft.print(WiFi.localIP().toString());

  admin();
  setupTime();
  //  espboardLed();
  //  onboardLed();
  //  bme280();
  //  dht22();                      // Die Tabs die du nutzen möchtest, musst du Einkommentieren
  //  bh1750();
  //  setupSchaltUhr();
  
   initRFID();  
  // writingData();
  tft.setCursor(10, 150, 1);
  tft.print("Filesystem  ");
  tft.setCursor(10, 160, 1);
  tft.print(" total: ");
  tft.print(LittleFS.totalBytes());
  tft.print(" Bytes");
  tft.setCursor(10, 170, 1);
  tft.print(" free:  ");
  tft.print(LittleFS.totalBytes()-LittleFS.usedBytes());
  tft.print(" Bytes");

  initButtons();
  
  ArduinoOTA.onStart([]() {
    //save();                       //Einkommentieren wenn Werte vor dem Update gesichert werden sollen
  });
  ArduinoOTA.begin();

  server.begin();
  DEBUG_P("HTTP Server gestartet\n\n");
}

void loop() {


 static auto letzteMillis = 0;
 static auto letzteMillis_2 = 0;
 auto aktuelleMillis = millis();

  if (aktuelleMillis - letzteMillis >= 1000) {
    printTime(10,75,2,TFT_BLACK,TFT_WHITE);
    letzteMillis = aktuelleMillis;
  }
  if (aktuelleMillis - letzteMillis_2 >= 5000) {
    letzteMillis_2 = aktuelleMillis;
    build_signal(215,55,20,30,TFT_WHITE,TFT_BLACK); //(unsigned long x,unsigned long y,unsigned long w,unsigned long h, unsigned long bgcolor, unsigned long color)
   }
  checkTouchloop();
  ArduinoOTA.handle();
  server.handleClient();
 
  if (millis() < 0x2FFF || millis() > 0xFFFFF0FF) runtime();  // Auskommentieren falls du den Admin Tab nicht nutzen möchtest
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Bad read (was card removed too quickly?)");
    return;
  }

  readingData();
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();  

 
}
