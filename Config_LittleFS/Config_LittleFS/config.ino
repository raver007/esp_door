bool readConfig() {
  String file_content = readFile(config_filename);

  int config_file_size = file_content.length();
  Serial.println("Config file size: " + String(config_file_size));

  if(config_file_size > 1024) {
    Serial.println("Config file too large");
    return false;
  }
  StaticJsonDocument<1024> doc;
  auto error = deserializeJson(doc, file_content);
  if ( error ) { 
    Serial.println("Error interpreting config file");
    return false;
  }

const String _user_SSID = doc["SSID"];
const String _user_PASSWD = doc["PASSWD"];
const int _user_IP0 = doc["IP"][0];
const int _user_IP1 = doc["IP"][1];
const int _user_IP2 = doc["IP"][2];
const int _user_IP3 = doc["IP"][3];
const int _user_NM0 = doc["NM"][0];
const int _user_NM1 = doc["NM"][1];
const int _user_NM2 = doc["NM"][2];
const int _user_NM3 = doc["NM"][3];
const int _user_GW0 = doc["GW"][0];
const int _user_GW1 = doc["GW"][1];
const int _user_GW2 = doc["GW"][2];
const int _user_GW3 = doc["GW"][3];
const bool _user_DHCP = doc["DHCP"];
const String _user_HOSTNAME = doc["HOSTNAME"];
const String _user_ntpServerName = doc["ntpServerNam"];
const bool _user_Timezone = doc["Timezone"];
const bool _user_SummerTime = doc["ummerTime"];

user_SSID =_user_SSID;
user_PASSWD =_user_PASSWD;
user_IP[0] =_user_IP0;
user_IP[1] =_user_IP1;
user_IP[2] =_user_IP2;
user_IP[3] =_user_IP3;
user_NM[0] =_user_NM0;
user_NM[1] =_user_NM1;
user_NM[2] =_user_NM2;
user_NM[3] =_user_NM3;
user_GW[0] =_user_GW0;
user_GW[1] =_user_GW1;
user_GW[2] =_user_GW2;
user_GW[3] =_user_GW3;
user_DHCP = _user_DHCP;
user_HOSTNAME = _user_HOSTNAME;
user_ntpServerName = _user_ntpServerName;
user_Timezone = _user_Timezone;
user_SummerTime = user_SummerTime;
  return true;
}

bool saveConfig() {
  StaticJsonDocument<1024> doc;

  // write variables to JSON file


doc["SSID"]= user_SSID;
doc["PASSWD"]= user_PASSWD;
doc["IP"][0]= user_IP[0];
doc["IP"][1]= user_IP[1];
doc["IP"][2]= user_IP[2];
doc["IP"][3]= user_IP[3];
doc["NM"][0]= user_NM[0];
doc["NM"][1]= user_NM[1];
doc["NM"][2]= user_NM[2];
doc["NM"][3]= user_NM[3];
doc["GW"][0]= user_GW[0];
doc["GW"][1]= user_GW[1];
doc["GW"][2]= user_GW[2];
doc["GW"][3]= user_GW[3];
doc["DHCP"]= user_DHCP;
doc["HOSTNAME"]= user_HOSTNAME;
doc["ntpServerNam"]= user_ntpServerName;
doc["Timezone"]= user_Timezone;
doc["ummerTime"]= user_SummerTime;

  
  // write config file
  String tmp = "";
  serializeJson(doc, tmp);
  writeFile(config_filename, tmp);
  
  return true;
}

void writeFile(String filename, String message){
    File file = LittleFS.open(filename, "w");
    if(!file){
        Serial.println("writeFile -> failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

String readFile(String filename){
    File file = LittleFS.open(filename);
    if(!file){
        Serial.println("Failed to open file for reading");
        return "";
    }
    
    String fileText = "";
    while(file.available()){
        fileText = file.readString();
    }
    file.close();
    return fileText;
}

  void readconfig(){
  // print initialized variables
//  DEBUG_F("setup -> Before reading config: Value1 = " + String(value1) + ", value2 = " + String(value2));

  // Mount LITTLEFS and read in config file
  if (!LittleFS.begin(false)) {
    Serial.println("LITTLEFS Mount failed");
    Serial.println("Did not find filesystem; starting format");
    // format if begin fails
    if (!LittleFS.begin(true)) {
      Serial.println("LITTLEFS mount failed");
      Serial.println("Formatting not possible");
      return;
    } else {
      Serial.println("Formatting");
    }
  }
  else{
    Serial.println("setup -> SPIFFS mounted successfully");
    if(readConfig() == false) {
      Serial.println("setup -> Could not read Config file -> initializing new file");
      // if not possible -> save new config file
      if (saveConfig()) {
        Serial.println("setup -> Config file saved");
      }   
    }

  //  Serial.println("Value1 = " + String(value1) + ", value2 = " + String(value2) + ", user_string = " + user_string);
  }
  }
