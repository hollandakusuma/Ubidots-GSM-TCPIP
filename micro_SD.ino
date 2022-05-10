void initSD() {
  //INIT MICRO SD
  Serial.println("Init micro SD Card");

  ukuranTeks = 2;
  getTextBounds("Init micro SD Card", ukuranTeks);
  tft.setCursor((320 - w) / 2, 200);
  tft.print("Init micro SD Card");
  delay(900);
  pinMode(SSpin, OUTPUT);
  digitalWrite(SSpin, HIGH);
  delay(100);

  if (!SD.begin(SSpin)) { //SD ERROR
    Serial.println(F("SD init error!!!"));
    Serial.flush();
    Error("micro SD Card Error!");
    while (1);
  }

  getTextBounds("micro SD Card OK!", ukuranTeks);
  tft.setCursor((320 - w) / 2, 220);
  tft.print("micro SD Card OK!");
  Serial.println(F("SD init OK"));
  delay(3000);
}

void configs() {
  nilai = 1;
  while (1) {
    sprintf(namaFile, "EV%03d.txt", nilai);


    if (SD.exists(namaFile)) {
      //      Serial.println(" exists.");
    }
    else {
      Serial.println("\r\n");
      Serial.print(namaFile);
      Serial.println(" doesn't exist.");
      break;
    }
    nilai++;
  }

  filename = String(namaFile);
  file = SD.open(filename, FILE_WRITE);
  file.println("YYYY/MM/DD HH:MM:SS, LONGITUDE, LATITUDE, VOLT BATTERY, THROTTLE, SPEED, TEMPERATURE, DIRECTION, DISTANCE, TOTAL DISTANCE");
  file.close();

}


void simpanData() {
  file = SD.open(namaFile, FILE_WRITE);
  file.print(tt.year(), DEC);
  file.print('/');
  file.print(tt.month(), DEC);
  file.print('/');
  file.print(tt.day(), DEC);
  file.print(" ");
  file.print(tt.hour(), DEC);
  file.print(':');
  file.print(tt.minute(), DEC);
  file.print(':');
  file.print(tt.second(), DEC);
  file.print(',');

  file.print(longitude, 6);
  file.print(',');
  file.print(latitude, 6);
  file.print(',');
  file.print(voltBaterai, 2);
  file.print(',');
  file.print(throttle, 2);
  file.print(',');
  file.print(kecepatan, 2);
  file.print(',');
  file.print(tempC, 1);
  file.print(',');
  file.print(arah, 2);
  file.print(',');
  file.print(jarakMeter);
  file.print(',');
  file.println(jarakTotal);
  file.close();
}

#define BUFFPIXEL 30

void bmpDraw(char *filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  progmemPrint(PSTR("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    progmemPrintln(PSTR("File not found"));
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    progmemPrint(PSTR("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    progmemPrint(PSTR("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    progmemPrint(PSTR("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      progmemPrint(PSTR("Bit Depth: ")); Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        progmemPrint(PSTR("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width())  w = tft.width()  - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

        for (row = 0; row < h; row++) { // For each scanline...
          if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col = 0; col < w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if (lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(r, g, b);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if (lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        }
        progmemPrint(PSTR("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp) progmemPrintln(PSTR("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Copy string from flash to serial port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
  char c;
  while (c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial.println();
}

void sd2digits(int number) {
  if (number >= 0 && number < 10) {
    file.print('0');
  }
  file.print(number);
}
