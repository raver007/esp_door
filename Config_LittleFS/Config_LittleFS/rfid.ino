bool initRFID() {
  bool result;
  mfrc522.PCD_Init();   // Init MFRC522 board.
  delay(10);
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);  // Show details of PCD - MFRC522 Card Reader details.
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  tft.setCursor(10, 140, 1);
  tft.print("RFID:  ");
  int rfid_version=mfrc522.PCD_GetVersion(); 
  switch(rfid_version) {
    case 0xb2:
       tft.print(" FM17522_1");
      break;
    case 0x88:
      tft.print(" FM17522");
      break;
    case 0x89:
      tft.print(" FM17522E");
      break;
    case 0x90:
      tft.print(" v0.0");
      break;
    case 0x91:
      tft.print(" v1.0");
      break;
    case 0x92:
      tft.print(" v2.0");
      break;
    case 0x12:
      tft.print(" counterfeit chip");
      break;
    default:
      tft.print(" (unknown)");
  }
  return result;
  }

//reads data from card/tag
void readingData()
{
  //prints the technical details of the card/tag
  //mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); 
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);
  //prepare the key - all keys are set to FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
      char tag[sizeof(mfrc522.uid.uidByte) * 4] = { 0 };
    for (int i = 0; i < mfrc522.uid.size; i++) {
      char buff[5]; // 3 digits, dash and \0.
      snprintf(buff, sizeof(buff), "%s%02x", i ? "-" : "", mfrc522.uid.uidByte[i]);
      strncat(tag, buff, sizeof(tag));
    };
    Serial.println("Good scan: ");
    Serial.println(tag);
  //buffer for read data
  byte buffer[SIZE_BUFFER] = {0};
 
  //the block to operate
  byte block = 1;
  byte size = SIZE_BUFFER;  //authenticates the block to operate
  status = mfrc522.PCD_Authenticate(MFRC522Constants::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522Constants::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(MFRC522Debug::GetStatusCodeName(status));
    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
    return;
  }

  //read data from block
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522Constants::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(MFRC522Debug::GetStatusCodeName(status));
    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
    return;
  }
  else{
      digitalWrite(Buzzer, HIGH);
      delay(100);
      digitalWrite(Buzzer, LOW);
  }

  Serial.print(F("\nData from block ["));
  Serial.print(block);Serial.print(F("]: "));

 //prints read data
  for (uint8_t i = 0; i < MAX_SIZE_BLOCK; i++)
  {
      Serial.write(buffer[i]);
  }
  Serial.println(" ");
}


void writingData(){

//prints thecnical details from of the card/tag

 if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Bad read (was card removed too quickly?)");
    return;
  }
 MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);
 
 for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
      char tag[sizeof(mfrc522.uid.uidByte) * 4] = { 0 };
    for (int i = 0; i < mfrc522.uid.size; i++) {
      char buff[5]; // 3 digits, dash and \0.
      snprintf(buff, sizeof(buff), "%s%02x", i ? "-" : "", mfrc522.uid.uidByte[i]);
      strncat(tag, buff, sizeof(tag));
    };
Serial.println(tag);

//prepare the key - all keys are set to FFFFFFFFFFFFh
for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

//buffer para armazenamento dos dados que iremos gravar
//buffer for storing data to write
byte buffer[MAX_SIZE_BLOCK] = "";
byte block; //the block to operate
byte dataSize; //size of data (bytes)

//recover on buffer the data from Serial
//all characters before chacactere '#'
dataSize = Serial.readBytesUntil('#', (char*)buffer, MAX_SIZE_BLOCK);
//void positions that are left in the buffer will be filled with whitespace
for(byte i=dataSize; i < MAX_SIZE_BLOCK; i++)
{
buffer[i] = ' ';
}

block = 1; //the block to operate
//String str = (char*)buffer; //transforms the buffer data in String
String str;
 for (std::pair<String, String> element : newUser) {

 str =element.second;
 Serial.println(str);
}


//authenticates the block to operate
//Authenticate is a command to hability a secure communication
status = mfrc522.PCD_Authenticate(MFRC522Constants::PICC_CMD_MF_AUTH_KEY_A,
block, &key, &(mfrc522.uid));

if (status != MFRC522Constants::STATUS_OK) {
Serial.print(F("PCD_Authenticate() failed: "));
Serial.println(MFRC522Debug::GetStatusCodeName(status));
digitalWrite(Buzzer, HIGH);
delay(50);
digitalWrite(Buzzer, LOW);
return;
}
//else Serial.println(F("PCD_Authenticate() success: "));

//Writes in the block
status = mfrc522.MIFARE_Write(block, buffer, MAX_SIZE_BLOCK);
if (status != MFRC522Constants::STATUS_OK) {
Serial.print(F("MIFARE_Write() failed: "));
Serial.println(MFRC522Debug::GetStatusCodeName(status));
digitalWrite(Buzzer, HIGH);
delay(100);
digitalWrite(Buzzer, LOW);
return;
}
else{
Serial.println(F("MIFARE_Write() success: "));
digitalWrite(Buzzer, HIGH);
delay(500);
digitalWrite(Buzzer, LOW);
}

}
