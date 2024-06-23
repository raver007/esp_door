
  



bool readUser() {
  String file_content = readUserFile(user_filename);

  int user_file_size = file_content.length();
  Serial.println("User file size: " + String(user_file_size));
  Serial.println(file_content);

  if(user_file_size > 1024) {
    Serial.println("User file too large");
    return false;
  }
  StaticJsonDocument<1024> doc;
  auto error = deserializeJson(doc, file_content);
  if ( error ) { 
    Serial.println("Error interpreting user file");
    return false;
  }


JsonArray rfidcard = doc["card"].as<JsonArray>();


for (JsonObject a : rfidcard) {
  Serial.println("*");
    for (JsonPair kv : a) {
        Serial.println(".");
        Serial.println(kv.key().c_str());
        if (kv.value().is<int>()) {
            Serial.println(kv.value().as<int>());
        }
        else {
            Serial.println(kv.value().as<String>());
        }
        usermap.insert(std::make_pair( kv.key().c_str(), kv.value().as<String>()));
    }
}


  return true;
}



bool saveUser() {
  StaticJsonDocument<1024> doc;

  // write variables to JSON file


doc["SSID"]= user_SSID;


  
  // write user file
  String tmp = "";
  serializeJson(doc, tmp);
  writeUserFile(user_filename, tmp);
  
  return true;
}

void writeUserFile(String filename, String message){
    File file = LittleFS.open(filename, "w");
    if(!file){
        Serial.println("writeUserFile -> failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

String readUserFile(String filename){
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

  void readuser(){
  // print initialized variables
//  DEBUG_F("setup -> Before reading user: Value1 = " + String(value1) + ", value2 = " + String(value2));

  // Mount LITTLEFS and read in user file
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
    if(readUser() == false) {
      Serial.println("setup -> Could not read User file -> initializing new file");
      // if not possible -> save new user file
      if (saveUser()) {
        Serial.println("setup -> User file saved");
      }   
    }

  //  Serial.println("Value1 = " + String(value1) + ", value2 = " + String(value2) + ", user_string = " + user_string);
  }
  }
void initUser(void){
   newUser.insert(std::make_pair("00000000","Max Mustermann"));

   server.on("/listUser", HTTP_POST, []() {
    if (server.args()) for (auto i = 0; i < server.args(); i++) usermap.find(server.argName(i))->second = server.arg(i).c_str();
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
   server.on("/newUser", HTTP_POST, []() {
    if (server.args()) for (auto i = 0; i < server.args(); i++) usermap.find(server.argName(i))->second = server.arg(i).c_str();
    String temp = "{\"";
    for (auto& el : newUser) {
      if (temp != "{\"") temp += "\",\"";
      temp += el.first;
      temp += "\":\"";
      temp += el.second;
    }
    temp += "\"}";
    server.send(200, "application/json", temp);
      Serial.println("new user");
   
    // Zugriff mittels input.at("Name");
  });

  }

  
