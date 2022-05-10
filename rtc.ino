void initRTC() {
  Serial.println("----------------------\r\nInit RTC");
  ukuranTeks = 2;
  tft.setTextSize(ukuranTeks);
  getTextBounds("Initialization Time", ukuranTeks);
  tft.setCursor((320 - w) / 2, 200);
  tft.print("Initialization Time");
  delay(2000);

  if (! rtc.begin()) {
    Serial.println("RTC error");
    Error("RTC error!!!");
    while (1);   // sketch halts in an endless loop
  }

  Serial.println("RTC OK");
  getTextBounds("Time Module OK", ukuranTeks);
  tft.setCursor((320 - w) / 2, 220);
  tft.print("Time Module OK");
  delay(3000);
}

void ambilWaktu() {
  gpsData(100);
  tt = DateTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
  unsigned long waktu = tt.unixtime() + 7UL * 3600UL;

  tt = DateTime(waktu);
 
  Serial.print(tt.year(), DEC);
  Serial.print('/');
  Serial.print(tt.month(), DEC);
  Serial.print('/');
  Serial.print(tt.day(), DEC);
  Serial.print(" ");
  Serial.print(tt.hour(), DEC);
  Serial.print(':');
  Serial.print(tt.minute(), DEC);
  Serial.print(':');
  Serial.println(tt.second(), DEC);
  
  
}

void adjustWaktu() {
  gpsData(100);
  tt = DateTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
  unsigned long waktu = tt.unixtime() + 7UL * 3600UL;

  tt = DateTime(waktu);

  Serial.print(tt.year());
  Serial.print('/');
  Serial.print(tt.month());
  Serial.print('/');
  Serial.print(tt.day());
  Serial.print(" ");
  Serial.print(tt.hour());
  Serial.print(':');
  Serial.print(tt.minute());
  Serial.print(':');
  Serial.println(tt.second());
  Serial.println("Adjust time");
  rtc.adjust(DateTime(tt));
}
