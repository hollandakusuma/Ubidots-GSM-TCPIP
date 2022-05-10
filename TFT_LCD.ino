#include <Wire.h>
#include <RTClib.h>
#include <TinyGPS++.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library

#include "variabel.h"

#define LCD_CS 40
#define LCD_CD 38
#define LCD_WR 37
#define LCD_RD 39
#define LCD_RESET 42
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
int16_t  x, y;
uint16_t w, h;


//SD CARD
File file;
char namaFile[40];
String filename;

TinyGPSPlus gps;

RTC_DS3231 rtc;
DateTime tt;

#define ONE_WIRE_BUS 9
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

HardwareSerial & SIM7000 = Serial1;
HardwareSerial & gpsSerial = Serial2;

const uint16_t warna[10] = {0x000F, 0x001F, 0x03E0, 0x07E0, 0xAFE5, 0xFFE0, 0xFD20, 0xFC18, 0xF800, 0x7800};
const byte indeksBaterai = 7;
const byte  indeksGas = 215;
const byte indeksY = 134;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//variabel
double bujur, lintang, kecepatan;
double longitude, latitude;
double jarakTotal;
double jarakMeter;
float arah, tempC, voltBaterai, baterai, throttle;
unsigned int kode;

boolean koneksi = 0;
boolean awal = 1;
boolean kirim = 0;
String operators, network;
char karakter;
byte a, b;
unsigned int nilai, i, j;
byte ukuranTeks = 1;

void setup() {
  Serial.begin(9600);
  SIM7000.begin(9600);
  gpsSerial.begin(9600);

  Serial.println("Electrical Vehicle IoT Monitoring System V 2020");
  Serial.println("Universitas Maritim Raja Ali Haji");

  Serial.print("\nInit TFT");
  tft.reset();
  tft.begin(0x9341); // SDFP5408

  tft.setRotation(3);
  tft.fillScreen(PUTIH);

  pinMode(SSpin, OUTPUT);
  digitalWrite(SSpin, HIGH);
  delay(10);

  if (!SD.begin(SSpin)) {
    Serial.println("SD Error");
    ukuranTeks = 2;
    tft.setTextColor(MERAH);
    tft.setTextSize(ukuranTeks);
    getTextBounds("PROGRAM ERROR!!!", ukuranTeks);
    tft.setCursor((320 - w) / 2, 130);
    tft.print("PROGRAM ERROR!!!");
    while (1);
  }

  bmpDraw("BMP/LOGO.BMP", (320 - 150) / 2, 5);
  delay(10);
  umrah();
  Serial.println("##### Finish #####\n\n");
  delay(30);

  tft.setTextSize(2);
  tft.setTextColor(BIRU);
  tft.fillRect(0, 160, 320, 100, PUTIH); //clear
  tft.drawRect(19, 169, 282, 22, HITAM); //kotak progress
  tft.drawRect(18, 168, 284, 24, HITAM); //kotak progress
  delay(10);

  //INIT RTC
  tft.fillRect(20, 170, 35, 20, MERAH);
  initRTC();

  tft.fillRect(0, 200, 320, 100, PUTIH); //clear
  tft.fillRect(55, 170, 35, 20, PINK);
  initSD();

  // INIT SENSOR SUHU DS18B20
  tft.fillRect(0, 200, 320, 100, PUTIH); //clear
  tft.fillRect(90, 170, 35, 20, ORANGE);

  getTextBounds("Init Temperature Sensor", ukuranTeks);
  Serial.println("Init Temperature Sensor");
  tft.setCursor((320 - w) / 2, 200);
  tft.print("Init Temperature Sensor");
  sensors.begin();
  sensors.getAddress(insideThermometer, 0);
  sensors.setResolution(insideThermometer, 11);
  delay(2000);

  getTextBounds("Temperature Sensor OK", ukuranTeks);
  tft.setCursor((320 - w) / 2, 220);
  tft.print("Temperature Sensor OK");
  delay(2000);

  //INIT GSM
  tft.fillRect(0, 200, 320, 100, PUTIH); //clear
  tft.fillRect(125, 170, 35, 20, KUNING);
  tft.setCursor((320 - 216) / 2, 200);
  Serial.println("Initialization GSM");
  tft.print("Initialization GSM");
  delay(2000);

  Serial.println("\n> Check GSM");
  for (i = 1; i < 5; i++) {
#ifdef debug
    Serial.print(i);
    Serial.print(" ");
#endif
    j = ConnectAT(200);
    if (j == 8) break;
  }
  if ( j == 6) {
    Error("GSM Error!!!");
    while (1);
  }

  getTextBounds("GSM OK!", ukuranTeks);
  tft.setCursor((320 - w) / 2, 220);
  Serial.println("GSM OK!");
  tft.print("GSM OK!");
  delay(2000);


  tft.fillRect(0, 220, 320, 100, PUTIH); //clear
  tft.fillRect(160, 170, 35, 20, HIJAU);
  initGSM();
  getTextBounds("GSM Ready to Use!", ukuranTeks);
  tft.setCursor((320 - w) / 2, 220);
  Serial.println("GSM Ready to Use!");
  tft.print("GSM Ready to Use!");
  delay(2000);

  //operator
  tft.fillRect(0, 200, 320, 100, PUTIH); //clear
  tft.fillRect(195, 170, 35, 20, CYAN);
  getTextBounds("Check Operator", ukuranTeks);
  tft.setCursor((320 - 168) / 2, 200);
  tft.print("Check Operator");
  Serial.println("Check Operator");
  delay(2000);
  gsmOperator();
  delay(3000);

  //kualitas sinyal
  tft.fillRect(0, 200, 320, 100, PUTIH); //clear
  tft.fillRect(230, 170, 35, 20, BIRU);
  tft.setTextSize(2);
  tft.setTextColor(BIRU);
  getTextBounds("Check Signal Quality", ukuranTeks);
  tft.setCursor((320 - 240) / 2, 200);
  tft.print("Check Signal Quality");
  Serial.println("Check Signal Quality");
  delay(2000);
  gsmSinyal();
  getTextBounds(network.c_str(), ukuranTeks);
  tft.setCursor((320 - w) / 2, 220);
  tft.print(network);
  delay(2000);

  tft.fillRect(0, 200, 320, 100, PUTIH); //clear
  tft.fillRect(230, 170, 35, 20, BIRU);
  tft.setTextSize(2);
  tft.setTextColor(BIRU);
  Serial.println("Check Internet Connection");
  getTextBounds("Check Internet", ukuranTeks);
  tft.setCursor((320 - w) / 2, 200);
  tft.print("Check Internet");
  tft.setCursor((320 - w) / 2, 220);
  tft.print("Connection");

  //Cek GPRS
  cekGPRS();
  tft.fillRect(0, 220, 320, 100, PUTIH); //clear
  getTextBounds("done", ukuranTeks);
  tft.setCursor((320 - w) / 2, 220);
  tft.print("done");
  delay(2000);

  //GPS
  tft.fillRect(0, 200, 320, 100, PUTIH); //clear
  tft.fillRect(265, 170, 35, 20, NAVY);
  getTextBounds("Wait GPS Data", ukuranTeks);
  tft.setCursor((320 - w) / 2, 200);
  tft.print("Wait GPS Data");
  Serial.println("\r\n\r\n> Wait GPS Data");
  delay(2000);

  while (gps.time.isValid() == 0) {
    gpsData(100);
  }

  longitude = 0;
  while (longitude < 95) {
    gpsData(100);
    longitude = gps.location.lng();
  }

  tft.fillRect(0, 200, 320, 100, PUTIH); //clear
  Serial.println("GPS Coordinate received.");
  getTextBounds("GPS Coordinate", ukuranTeks);
  tft.setCursor((320 - w) / 2, 200);
  tft.print("GPS Coordinate");
  
  getTextBounds("received", ukuranTeks);
  tft.setCursor((320 - w) / 2, 220);
  tft.print("received");
  delay(2000);




  tft.fillRect(0, 200, 320, 100, PUTIH); //clear
  getTextBounds("Initialization finish", ukuranTeks);
  tft.setCursor((320 - 252) / 2, 200);
  tft.print("Initialization finish");
  Serial.println("Initialization finish");
  delay(2000);
  getTextBounds("Ready to use.", ukuranTeks);
  tft.setCursor((320 - 156) / 2, 220);
  Serial.println("Ready to use.");
  tft.print("Ready to use.");

  gprsComm();
  configs();

  adjustWaktu();

  //BACKROUND SCREEN
  jarakTotal = 0;
  Serial.flush();
  Serial1.flush();
  tft.fillScreen(HITAM);
  bmpDraw("BMP/UI.BMP", 0, 0);
  a = 0;

  //HTTP TERMINATE
  Serial.flush();
  Serial1.flush();
  Serial.println(F("\r\n - TUTUP TCP IP - "));
  TCPclose(500);
  koneksi = 1;
  Serial.flush();
  Serial1.flush();

  //HTTP initilization
  Serial.println(F("\r\n - MULAI TCP IP - "));
  ConnectAT(1000);
  boolean nilai = TCPstart(1000, 1);
  if (nilai == 0) {
    koneksi = 0;
    return 0;
  }
}

void loop(void) {
  ambilWaktu();
  ambilData();
  tampilData();
  simpanData();
  Serial.flush();
  Serial1.flush();
  kirimData();
  /*
  if (kirim == 1) {
    Serial.println("GAk usah Kirim datanya dong");
    kirim = 0;
  }
  else {
    Serial.println("Kirim datanya dong\r\n\r\n");
    kirimData();
    kirim = 1;
  }

  //CONNECTION
  switch (koneksi) {
    case 0:
      network = "NOT CONNECTED";
      break;
    case 1:
      network = "CONNECTED";
      break;
  }
  */
  tft.fillRect(115, 60, 90, 10, HITAM);
  ukuranTeks = 1;
  tft.setTextSize(ukuranTeks);
  getTextBounds(network.c_str(), ukuranTeks);
  tft.setCursor(110 + (100 - w) / 2, 60);
  tft.print(network);
}


void getTextBounds(String Str, byte sizes) {
  int widths = 6;
  int heights = 8;

  unsigned int jumlahKarakter = Str.length();
  w = jumlahKarakter * widths * sizes;
  h = heights * sizes;
}
