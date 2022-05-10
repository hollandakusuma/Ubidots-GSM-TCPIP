void umrah() {
  //logo umrah
  tft.setTextColor(BIRU);
  tft.setTextSize(3);
  tft.setCursor((320 - 198) / 2, 160);
  tft.print("UNIVERSITAS");
  tft.setCursor((320 - 126) / 2, 188);
  tft.print("MARITIM");
  tft.setCursor((320 - 234) / 2, 216);
  tft.print("RAJA ALI HAJI");
}

void Error(String teksError) {
  Serial.println(teksError);
  tft.fillRect(0, 220, 320, 30, PUTIH);
  tft.setTextColor(MERAH);
  ukuranTeks = 2;
  tft.setTextSize(ukuranTeks);
  getTextBounds(teksError, ukuranTeks);
  tft.setCursor((320 - w) / 2, 220);
  tft.print(teksError);
  delay(100);
}

void tampilData() {
  tft.setTextColor(CYAN);

  //NAMA HARI
  tft.fillRect(0, 0, 320, 14, HITAM);
  ukuranTeks = 1;
  tft.setTextSize(ukuranTeks);
  getTextBounds(daysOfTheWeek[tt.dayOfTheWeek()], ukuranTeks );
  tft.setCursor( 50 + (60 - w) / 2, 5);
  tft.print(daysOfTheWeek[tt.dayOfTheWeek()]);

  //WAKTU
  tft.fillRect((320 - 30) / 2, 15, 40, 10, HITAM);
  tft.setCursor((320 - 30) / 2, 15);
  str2digit(tt.hour());
  tft.print(":");
  str2digit(tt.minute());

  //TANGGAL
  tft.setCursor(203, 5);
  str2digit(tt.month());
  tft.print("/");
  str2digit(tt.day());
  tft.print("/");
  tft.print(tt.year());

  //LONGITUDE
  operators = konversi(longitude, 2);
  tft.fillRect(12, 78, 80, 10, HITAM);
  ukuranTeks = 1;
  getTextBounds(operators.c_str(), ukuranTeks);
  tft.setCursor(10 + (100 - w) / 2, 78);
  tft.print(operators);
  Serial.print("Longitude = ");
  Serial.println(longitude, 6);

  //LATITUDE
  operators = konversi(latitude, 1);
  tft.fillRect(215, 78, 80, 10, HITAM);
  getTextBounds(operators.c_str(), ukuranTeks );
  tft.setCursor(210 + (100 - w) / 2, 78);
  tft.print(operators);
  Serial.print("Latitude = ");
  Serial.println(latitude, 6);
  
  //display BATERAI
  barDisplay(0, baterai);
  Serial.print("Voltage Battery = ");
  Serial.println(baterai);
  
  //display THROTTLE
  barDisplay(1, throttle);
  Serial.print("Throttle (%) = ");
  Serial.println(throttle);
  
  //KECEPATAN
  ukuranTeks = 3;
  tft.setTextSize(ukuranTeks);
  tft.fillRect(123, 120, 72, 25, HITAM);
  operators = String(kecepatan, 1);
  getTextBounds(operators.c_str(), ukuranTeks );
  tft.setCursor((320 - w) / 2, 120);
  tft.print(operators);
  Serial.print("Speed (m/s) = ");
  Serial.println(operators);
  
  //SUHU
  ukuranTeks = 1;
  if (tempC < -120) operators = "Error";
  else operators = String(tempC, 2);
  tft.setTextSize(ukuranTeks);
  tft.fillRect(12, 215, 40, 10, HITAM);
  tft.setCursor(20, 215);
  tft.print(operators); // suhu
  Serial.print("TempC = ");
  Serial.println(operators);
  
  //ARAH
  tft.fillRect(120, 207, 85, 28, HITAM);
  ukuranTeks = 1;
  tft.setTextSize(ukuranTeks);
  operators = String(arah, 2);
  getTextBounds(operators.c_str(), ukuranTeks);
  tft.setCursor((320 - w) / 2, 209);
  tft.print(operators);
  Serial.print("Direction = ");
  Serial.println(operators);
  
  mataAngin();
  getTextBounds(network.c_str(), ukuranTeks);
  tft.setCursor((320 - w) / 2, 225);
  tft.print(network);

  //JARAK
  tft.fillRect(235, 210, 60, 15, HITAM);
  ukuranTeks = 1;
  tft.setTextSize(ukuranTeks);
  operators = String(jarakTotal,0); // "999.999";
  getTextBounds(operators.c_str(), ukuranTeks);
  tft.setCursor(230 + (84 - w) / 2, 212);
  tft.print(operators); // jarak tempuh total
  Serial.print("Jarak (m) = ");
  Serial.println(jarakTotal);
}

void barDisplay(boolean pilihan, float persentase) {
  byte posisi = 0;

  String teks = String(persentase) + " %";

  if (pilihan == 0) posisi = indeksBaterai;
  else posisi = indeksGas;

  //clear Bar
  tft.fillRect(posisi, indeksY, 96, 17, HITAM);
  tft.fillRect(posisi + 20, indeksY + 20, 60, 10, HITAM);

  ukuranTeks = 1;
  tft.setTextSize(ukuranTeks);
  getTextBounds(teks.c_str(), ukuranTeks);
  tft.setCursor(posisi + (100 - w) / 2, 155);
  tft.print(teks);

  persentase = (persentase / 10 + 1);

  for (byte i = 0; i < persentase; i++) {
    tft.fillRect(posisi + i * 10, indeksY, 8, 17, warna[i]);
  }
}

void str2digit(byte nomor) {
  if (nomor < 10) tft.print('0');
  tft.print(nomor);
}
