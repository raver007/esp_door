bool initTFT() {

  tft.init();
  tft.setRotation(0);
   // Calibrate the touch screen and retrieve the scaling factors
  //touch_calibrate();
  tft.fillScreen(TFT_WHITE);
  Serial.print("TFT init done !\n");
  return false;
  }
uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void drawBmp(const char *filename, int16_t x, int16_t y) {

  if ((x >= tft.width()) || (y >= tft.height())) return;

 File bmpFS = LittleFS.open(filename);
   

  if (!bmpFS)
  {
    Serial.print("File not found");
    return;
  }else{
    Serial.print("File found");
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);
    Serial.print("w = ");
    Serial.println(w);
    Serial.print("h = ");
    Serial.println(h);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
      Serial.print("Loaded in "); Serial.print(millis() - startTime);
      Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

void build_signal(unsigned long x,unsigned long y,unsigned long w,unsigned long h, unsigned long bgcolor, unsigned long color)
{
    long rssi = WiFi.RSSI();
    img.createSprite(w, h); //20 30
   // build_signal(rssi,TFT_TRANSPARENT,TFT_BLACK);

  
if ((-rssi) <= 80)
  // Draw some graphics, the text will apear to scroll over these
    img.fillRect  (0, 0, 20, 30, bgcolor);
if ((-rssi) <= 70){
    img.drawFastVLine(0, 26,  4, color);
    img.drawFastVLine(1, 26,  4, color);
   }
if ((-rssi) <= 67){
    img.drawFastVLine(4, 22,  8, color);
    img.drawFastVLine(5, 22,  8, color);
   }
if ((-rssi) <= 60){
    img.drawFastVLine(8, 18,  12, color);
    img.drawFastVLine(9, 18,  12, color);
   }
if ((-rssi) <= 50){
    img.drawFastVLine(12, 14,  16, color);
    img.drawFastVLine(13, 14,  16, color);
   }
if ((-rssi) <= 30){
    img.drawFastVLine(16, 10,  20, color);
    img.drawFastVLine(17, 10,  20, color);
  }
    img.pushSprite(x, y); //215 55
    img.deleteSprite();

}

void tft_set_spi(void){
// TFT_CS 16 // TFT select pin
// TFT_MOSI 12
// TFT_CLK 14
// TFT_MISO 13
     //    pins(int8_t sck, int8_t miso, int8_t mosi, int8_t ss);
    SPI.begin(TFT_CLK, TFT_MISO, TFT_MOSI, TFT_CS);
}

void printTime(unsigned long x,unsigned long y,unsigned long fontsize,unsigned long bgcolor,unsigned long color){

    tft.setTextColor(bgcolor,color);
    tft.setCursor(x, y, fontsize);
    tft.print(localTime());
}
