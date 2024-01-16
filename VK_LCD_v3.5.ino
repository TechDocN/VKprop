/*
    Voight-Kampff Prop - Color Display Firmware v3.5n (Nano)

    LATEST BUILD 06/10/2023
    STATUS: WORKING
    KNOWN BUGS/ISSUES: WaveShare branded displays have a color/shift issue

    DESCRIPTION
    - Displays color BMP images on ST7735 TFT display from SD card
    - Activates the boot sequence on the 2 secondary MCUs
      running the secondary OLED screens for the VK prop
    - Provides gameplay functionality for the Blade Runner RPG
    
    HARDWARE
      - Arduino Nano
      - 1.8" color TFT LCD display (ST7735S) with built-in microSD card reader
      - 4 momentary SPST switches
      - Passive buzzer

    SOFTWARE FLOW
      - Startup image displays with a pause for user input
        before simulated boot sequence begins
      - Button press moves past pause to simulated boot sequence
      - At the initiation of the simulated boot sequence, a digital HIGH signal
        is sent to A0 to simulate a button press on each of the secondary MCUs
      - Core menu functionality complete
        - VK scan simulation with randomized results
        - 10 and 12 integer random number generators with text animation
      - Final menu content TBD

    CHANGE NOTES FOR V3.5n
      - Adjusted spacing for RND number generator output
      - Added sound effects to RND number generators
      - Re-ordered the button pins to simplify wiring the 3D printed enclosure
      - Re-named "X" button to "VK" across the UI

    OTHER NOTES
      - Setting the "BUFFPIXEL" attribute (defined in the BMP subroutines below)
        above 20 interferes with reading images from the SD card. I am assuming
        it was a hardware limitation.
*/

#include <Adafruit_GFX.h>     // include Adafruit graphics library
#include <Adafruit_ST7735.h>  // include Adafruit ST7735 hardware library
#include <SPI.h>              // include Arduino SPI library
#include <SD.h>               // include Arduino SD library

// define ST7735 TFT display connections
#define TFT_RST 9  // reset
#define TFT_CS  8  // chip select
#define TFT_DC  7  // data/command
                   // TFT_CLK = D13 (shared with SD card reader SCK)
                   // TFT_DIN = D11 (shared with SD card reader MOSI)

                   // SD MISO = D12
                   // SD MOSI = D11 (shared with LCD DIN)
                   // SD SCK  = D13 (shared with LCD CLK)
                   // SD CS   = D10
                   
#define button4 2  // button pins
#define button1 3
#define button2 4
#define button3 5

#define buzzer  6  // buzzer pin

// initialize Adafruit ST7735 TFT library
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  randomSeed(analogRead(1));  // resets random number generator to a random start point

  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);

  Serial.begin(9600);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);

  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);

  // initialize ST7735S TFT display
  tft.initR(INITR_BLACKTAB);    // use this setting for the DSD Tech 1.8" ST7735 w/microSD
  //tft.initR(INITR_GREENTAB);  // this is supposed to be the setting for the Waveshare 1.8" ST7735
  tft.invertDisplay(false);
  tft.setTextWrap(true);
  tft.setRotation(1);  // 0,2 are portrait - 1,3 are landscape
  tft.setTextSize(1);  // 1 - 5 integers from small to large
  tft.fillScreen(ST77XX_BLACK);

  // initialize SD card reader
  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("failed!");
    while (1)
      ;  // stay here
  }
  Serial.println("OK!");

  File root = SD.open("/");  // open SD card main root
  printDirectory(root, 0);   // print all files names and sizes
  root.close();              // close the opened root

  startBoot();
  bmpDraw("menu.bmp", 0, 0);
  mainMenu();
}

void loop() {

  if (digitalRead(button1) == LOW) {
    vkScan();
  }

    if (digitalRead(button2) == LOW) {
    tft.fillScreen(ST77XX_BLACK);
    bmpDraw("randomB.bmp", 0, 0);
    tft.setTextColor(ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(63, 9);
    tft.print("RND 10");
    for (int count = 1; count < 15; count = count + 1) {
      tft.setTextColor(ST77XX_BLUE);
      tft.setTextSize(8);
      tft.setCursor(60, 37);
      int ranDo = random(1, 11);
      tft.print(ranDo);
      tone(buzzer, 2000); delay(15); noTone(buzzer); delay(60);
      tft.setTextColor(ST77XX_BLACK);
      tft.setCursor(60, 37);
      tft.print(ranDo);
      delay(10);
    }
    tft.setTextColor(ST77XX_BLUE);
    tft.setCursor(60, 37);
    int ranDoFNL = random(1, 11);
    if (ranDoFNL == 10) {
      tft.setCursor(35, 37);
    }
    tft.print(ranDoFNL);
    tone(buzzer, 2000); delay(40); noTone(buzzer); delay(20);
    tone(buzzer, 3000); delay(40); noTone(buzzer); delay(20);
    tone(buzzer, 4000); delay(60); noTone(buzzer);
    delay(500);
    tft.setTextSize(1);
    while (digitalRead(button4)) {
      tft.setTextColor(ST77XX_BLACK);
      tft.setCursor(30, 112);
      tft.print("Press VK for Menu");
    }
    bmpDraw("menu.bmp", 0, 0);
  }

  if (digitalRead(button3) == LOW) {
    tft.fillScreen(ST77XX_BLACK);
    bmpDraw("randomB.bmp", 0, 0);
    tft.setTextColor(ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(63, 9);
    tft.print("RND 12");
    for (int count = 1; count < 16; count = count + 1) {
      tft.setTextColor(ST77XX_BLUE);
      tft.setTextSize(8);
      tft.setCursor(60, 37);
      int ranDo = random(1, 13);
      tft.print(ranDo);
      tone(buzzer, 2000); delay(15); noTone(buzzer); delay(60);
      tft.setTextColor(ST77XX_BLACK);
      tft.setCursor(60, 37);
      tft.print(ranDo);
      delay(10);
    }
    tft.setTextColor(ST77XX_BLUE);
    tft.setCursor(60, 37);
    int ranDoFNL = random(1, 13);
    if (ranDoFNL > 9) {
      tft.setCursor(35, 37);
    }    
    tft.print(ranDoFNL);
    tone(buzzer, 2000); delay(40); noTone(buzzer); delay(20);
    tone(buzzer, 3000); delay(40); noTone(buzzer); delay(20);
    tone(buzzer, 4000); delay(60); noTone(buzzer);
    delay(500);
    tft.setTextSize(1);
    while (digitalRead(button4)) {
      tft.setTextColor(ST77XX_BLACK);
      tft.setCursor(30, 112);
      tft.print("Press VK for Menu");
    }
    bmpDraw("menu.bmp", 0, 0);
  }
  mainMenu();
}


// Startup/boot sequence
void startBoot() {
  // logo/splash screen
  bmpDraw("VKlogoGr.bmp", 0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(16, 115);
  tft.print("VOIGHT-KAMPFF  (c)2019");
  tone(buzzer, 3000); delay(70); noTone(buzzer); delay(20);
  tone(buzzer, 4000); delay(70); noTone(buzzer); delay(20);
  tone(buzzer, 4500); delay(100); noTone(buzzer);
  delay(1000);
  tft.setTextColor(ST77XX_BLACK);
  tft.setCursor(16, 115);
  tft.print("VOIGHT-KAMPFF  (c)2019");
  tft.setTextColor(ST77XX_WHITE);
  // waiting for button press
  while (digitalRead(button4)) {
    tft.setCursor(23, 115);
    tft.print("Press VK to Continue");
  }
// boot sequence
  // power up/red screen
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_RED);
  tft.setCursor(15, 110);
  tft.print("SENSORS ON");
  digitalWrite(A0, HIGH);
  delay(500);
  bmpDraw("sensorR.bmp", 0, 0);
  delay(1500);
  // calibrating/blue screen
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 110);
  tft.print("CALIBRATING");
  delay(500);
  bmpDraw("sensorB.bmp", 0, 0);
  digitalWrite(A0, LOW);
  delay(1500);
  // nominal/green screen
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_GREEN);
  bmpDraw("sensorG.bmp", 0, 0);
  tft.setCursor(15, 45);
  tft.print("All Sensors");
  tft.setCursor(40, 70);
  tft.print("Online");
  tone(buzzer, 3000); delay(70); noTone(buzzer); delay(20);
  tone(buzzer, 4000); delay(70); noTone(buzzer); delay(20);
  tone(buzzer, 4500); delay(100); noTone(buzzer);
  delay(1500);
}


void mainMenu() {
  // main menu text
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(15, 18);
  tft.print("Main Menu");
  tft.setTextSize(1);
  tft.setCursor(20, 48);
  tft.print(" 1 - VK Scan");
  tft.setCursor(20, 68);
  tft.print(" 2 - RND 10");
  tft.setCursor(20, 88);
  tft.print(" 3 - RND 12");
}


void vkScan() {
  // Eye scan simulation with random results
  bmpDraw("static.bmp", 0, 0);
  bmpDraw("eye_f0.bmp", 0, 0);
  bmpDraw("eye_f1.bmp", 0, 0);
  bmpDraw("eye_f2.bmp", 0, 0);
  bmpDraw("eye_f3.bmp", 0, 0);
  //bmpDraw("eye_f2.bmp", 0, 0);
  //bmpDraw("eye_f0.bmp", 0, 0);

  int vkRND = random(100);

  // adjust here to change odds of VK scan outcome
  if (vkRND > 75) { vkResult1(); }  // > 75 means ~25% of outcomes = result1 = inconclusive
  else if (vkRND < 45) {
    vkResult2();
  }                      // < 45 means ~45% of outcomes = result2 = human
  else { vkResult3(); }  // that means ~30% of outcomes = result3 = replicant

  bmpDraw("static.bmp", 0, 0);
  bmpDraw("menu.bmp", 0, 0);
}

void vkResult1() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_YELLOW);
  bmpDraw("alertY.bmp", 0, 0);
  tft.setCursor(10, 16);
  tft.print("INCONCLUSIVE");
  tft.setCursor(45, 45);
  tft.print("Repeat");
  tft.setCursor(35, 67);
  tft.print("V-K Scan");
  tone(buzzer, 3000); delay(75); noTone(buzzer); delay(25);
  tone(buzzer, 2000); delay(75); noTone(buzzer); delay(25);
  tone(buzzer, 1000); delay(75); noTone(buzzer);
  tft.setTextSize(1);
  while (digitalRead(button4)) {
    tft.setCursor(10, 102);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("    Press VK for Menu");
  }
}

void vkResult2() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_GREEN);
  bmpDraw("sensorG.bmp", 0, 0);
  tft.setCursor(20, 52);
  tft.print("VKID Human");
  tone(buzzer, 3000); delay(75); noTone(buzzer); delay(25);
  tone(buzzer, 4000); delay(75); noTone(buzzer); delay(25);
  tone(buzzer, 4500); delay(75); noTone(buzzer);
  tft.setTextSize(1);
  while (digitalRead(button4)) {
    tft.setCursor(10, 95);
    tft.setTextColor(ST77XX_GREEN);
    tft.print("   Press VK for Menu");
  }
}

void vkResult3() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_RED);
  bmpDraw("alertR.bmp", 0, 0);
  tft.setCursor(39, 39);
  tft.print("WARNING");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(21, 60);
  tft.print("REP-DETECT");
  tone(buzzer, 1000); delay(75); noTone(buzzer); delay(25);
  tone(buzzer, 1000); delay(75); noTone(buzzer); delay(25);
  tone(buzzer, 1000); delay(75); noTone(buzzer); delay(25);
  tone(buzzer, 1000); delay(75); noTone(buzzer);
  tft.setTextSize(1);
  while (digitalRead(button4)) {
    tft.setCursor(10, 85);
    tft.setTextColor(ST77XX_RED);
    tft.print("   Press VK for Menu");
  }
}


// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

void bmpDraw(char *filename, uint8_t x, uint16_t y) {

  File bmpFile;
  int bmpWidth, bmpHeight;             // W+H in pixels
  uint8_t bmpDepth;                    // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;             // Start of image data in file
  uint32_t rowSize;                    // Not always = bmpWidth; may have padding
  uint8_t sdbuffer[3 * BUFFPIXEL];     // pixel buffer (R+G+B per pixel)
  uint8_t buffidx = sizeof(sdbuffer);  // Current position in sdbuffer
  boolean goodBmp = false;             // Set to true on valid header parse
  boolean flip = true;                 // BMP is stored bottom-to-top
  int w, h, row, col;
  uint8_t r, g, b;
  uint32_t pos = 0, startTime = millis();

  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) {  // BMP signature
    Serial.print(F("File size: "));
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile);             // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile);  // Start of image data
    Serial.print(F("Image Offset: "));
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: "));
    Serial.println(read32(bmpFile));
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) {    // # planes -- must be '1'
      bmpDepth = read16(bmpFile);  // bits per pixel
      Serial.print(F("Bit Depth: "));
      Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) {  // 0 = uncompressed

        goodBmp = true;  // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width()) w = tft.width() - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.startWrite();
        tft.setAddrWindow(x, y, w, h);

        for (row = 0; row < h; row++) {  // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if (flip)  // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else  // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos) {  // Need seek?
            tft.endWrite();
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer);  // Force buffer reload
          }

          for (col = 0; col < w; col++) {  // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) {  // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0;  // Set index to beginning
              tft.startWrite();
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r, g, b));
          }  // end pixel
        }    // end scanline
        tft.endWrite();
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      }  // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp) Serial.println(F("BMP format not recognized."));
}


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();  // LSB
  ((uint8_t *)&result)[1] = f.read();  // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();  // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();  // MSB
  return result;
}


void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
