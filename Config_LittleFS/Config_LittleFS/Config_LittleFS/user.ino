bool readUser() {
  String file_content = readUserFile(user_filename);

  int user_file_size = file_content.length();
  Serial.println("User file size: " + String(user_file_size));

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

const String _user_SSID = doc["SSID"];


user_SSID =_user_SSID;

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
