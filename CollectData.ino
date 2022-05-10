void gpsData(unsigned long ms) {
  unsigned long start = millis();
  do
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}

void mataAngin() {
  if (arah >= 337.5 && arah < 360) network = "NORTH";
  if (arah >= 0 && arah < 22.5) network = "NORTH";
  if (arah >= 22.5 && arah < 67.5 ) network = "NORTH EAST";
  if (arah >= 67.5 && arah < 112.5) network = "EAST";
  if (arah >= 12.5 && arah < 157.5 ) network = "SOUTH EAST";
  if (arah >= 157.5 && arah < 202.5 ) network = "SOUTH";
  if (arah >= 202.5 && arah < 247.5) network = "SOUTH WEST";
  if (arah >= 247.5 && arah < 292.5 ) network = "WEST";
  if (arah >= 292.5 && arah < 337.5 ) network = "NORTH WEST";
}

void ambilData() {
  gpsData(1000);

  //parameter GPS
  longitude = gps.location.lng();
  latitude = gps.location.lat();
  kecepatan = gps.speed.kmph() * 0.277778;
  arah = gps.course.deg();

  if (awal == 1) {
    lintang = latitude;
    bujur = longitude;
    awal = 0;
  }

  if (kecepatan > 2) {
    jarakMeter =
      (unsigned long)TinyGPSPlus::distanceBetween(
        latitude, longitude, lintang, bujur);
  }
  else jarakMeter = 0;

  jarakTotal = jarakTotal + jarakMeter;

  //parameter suhu
  sensors.requestTemperatures();
  tempC = sensors.getTempC(insideThermometer);

  //parameter baterai
  nilai = analogRead(A0);
  voltBaterai = ((4700.00 + 1000.00) / 1000.00) * (float) nilai / 1023.00 * 5.00;
  Serial.println(voltBaterai);
  baterai = (voltBaterai-21.00)/3 * 100.00;
  if (baterai>100) baterai = 100;
  if(baterai <0) baterai = 0;
  Serial.println(baterai);
  
  //parameter throttle
  nilai = analogRead(A1);
  Serial.println(nilai);
  gas();

}

void gas() {
  if (nilai < 32)  throttle = 0;
  else if (nilai >= 32 && nilai <= 35) throttle = 10;
  else if (nilai == 36) throttle = 20;
  else if (nilai == 37) throttle = 30;
  else if (nilai == 38) throttle = 40;
  else if (nilai == 39) throttle = 50;
  else if (nilai == 40) throttle = 60;
  else if (nilai == 41) throttle = 70;
  else if (nilai == 42) throttle = 80;
  else if (nilai == 43) throttle = 90;
  else if (nilai > 43) throttle = 100;
}

String konversi (float koordinat, byte pilihan) {
  double degree, minutes, seconds;
  int derajat, menit, detik, konstanta;
  char satuan;
  String conversion;

  //LINTANG
  if (koordinat < 0 && pilihan == 1) {
    konstanta = -1;
    satuan = 'S';
  }
  if (koordinat >= 0 && pilihan == 1) {
    konstanta = 1;
    satuan = 'N';
  }
  //BUJUR
  if (koordinat < 0 && pilihan == 2) {
    konstanta = -1;
    satuan = 'W';
  }
  if (koordinat >= 0 && pilihan == 2) {
    konstanta = 1;
    satuan = 'E';
  }

  koordinat = konstanta * koordinat;
  //DERAJAT
  degree = abs(koordinat);
  derajat = (int)degree;

  conversion = String(derajat) + char(247);

  //MENIT
  minutes = (degree - float(derajat)) * 60;
  menit = (int) minutes;

  if (menit < 10) {
    conversion = conversion + "0";
  }
  conversion = conversion + String(menit) + "'";

  //DETIK
  seconds = ((minutes - float(menit)) * 60);
  detik = (int)seconds;
  if (detik < 10)
  {
    conversion = conversion + "0";
  }
  conversion = conversion + String(detik) + "\" " + satuan ;
  return conversion;
}

void dataJSON() {
  json = "";
  json = "{\"battery\":" + String(baterai, 2);
  json += ",\"temperature\":" + String(tempC, 1);
  json += ",\"throttle\":" + String(throttle, 1);
  json += ",\"distance\":" + String(jarakTotal);
  json += ",\"speed\":" + String(kecepatan, 1);
  json += ",\"position\":{\"value\" : " + String(tt.day());
  json += ",\"context\":{\"lat\":" + String(latitude, 6) + ",\"lng\":" + String(longitude, 6) + "}}";
  json += ",\"direction\":" + String(arah, 1) + "}";

  Serial.println(json);
}

void kirimData() {
  dataJSON();
  

  //SET HTTP PARAMETERS VALUE
  Serial.flush();
  Serial1.flush();
  tft.fillRect(115, 60, 90, 10, HITAM);
  Serial.println(F("\r\n - KIRIM DATA - "));
  nilai = TCPsend();

  if(nilai==0){
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
  }
}
