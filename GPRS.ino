void cekGPRS() {
  for (byte i = 1; i < 4; i++) {
    Serial.print(F("\r\nCEK "));
    Serial.println(i);

    if (gprsComm() == 0) {
      Serial.println(F("\r\n- GPRS ERROR -"));
      delay(1000);
      if (i == 3) {
        karakter = 'E';
      }
    }
    else {
      Serial.println(F("\r\nKomunikasi GPRS OK.")); // Kita Lanjut ke step berikutnya
      gprsShut(300);
      break;
    }
  }
}

boolean gprsComm() {
  //CHECK GPRS SERVICE REGISTRATION
  Serial.println(F("\r\n- CEK REG GPRS -"));

  // Cek GPRS Registration
  SIM7000.println("AT");
  readSerial(200);
  char karakterOutput = '.';
  gprsReg(karakterOutput, 10000);
  if (karakterOutput != '1') return 0;

  Serial.flush();
  Serial1.flush();
  Serial.println(F("\r\n- TUTUP TCP IP -"));
  gprsShut(700);

  // SET SINGLE IP CONNECTION
  Serial.println(F("\r\n\r\n- ATUR CIPMUX -")); //MENJADI SINGLE IP CONNECTION
  gprsMux(500);

  //Attach from GPRS Service
  Serial.println(F("\r\n\r\n- AKTIFKAN GPRS -")); //SERVICE
  gprsAttach(500);

  //CONNECTION TYPE = gprs ; ATUR APN, USER, DAN PASSWORD
  Serial.println(F("\r\n\r\n- ISI APN -")); //, USER, DAN PASSWORD
  gprsCSTT(500);

  //CEK IP STATUS IN INITIAL MODE
  Serial.print(F("\r\n\r\n- CEK IP STATUS-\r\n"));
  gprsStatus(500);

  //Bring Up Wireless Connection with GPRS
  Serial.println(F("\r\n\r\n- BUKA TCP IP -"));
  karakterOutput = '.';
  gprsWirelessConnect(karakterOutput, 85000);
  if ( karakterOutput == 'E') {
    Serial.println(F("- ERROR -"));
    return 0;
  }
  Serial.println(F("- CONNECTED -"));

  //GPRS IP
  Serial.println(F("\r\n- IP ADDRESS -"));
  gprsIP(500);

  return 1;
}

//---------------------------------------
// gprs SOURCE CODE ROOT
//---------------------------------------
char cekSerial(unsigned long times) {
  boolean indeks = 0;
  char karakter = '\0';
  unsigned long mulai = millis();
  while (millis() - mulai < times && indeks == 0) {
    while (SIM7000.available() > 0) {
      karakter = SIM7000.read();

#ifdef debug
      Serial.write(karakter);
#endif

      if (karakter == 'O' || karakter == 'E') {
        indeks = 1;
        break;
      } // exit from if
    } // exit from SIM7000 available
  } // exit from millis()

  readSerial(30);
  return karakter;
}

// ATTACH gprs SERVICE
void gprsReg(char &karakter, unsigned int jeda) {
  unsigned long waktu = millis ();
  while (millis () - waktu <= jeda) {
    char g = '+';
    SIM7000.println(F("AT+CGREG?"));
    unsigned long waktuAwal = millis();
    while (millis() - waktuAwal <= 300 && g != 'K') {
      g = SIM7000.read();
      switch (g) {
        case '0' ... '9':
          karakter = g;
#ifdef debug
          Serial.print(karakter);
#endif
          break;

        default:
#ifdef debug
          if (isAscii(g)) Serial.print(g);
#endif
          break;
      }
    }

#ifdef debug
    Serial.print(F("\r\nCGREG="));
    Serial.println(karakter);
#endif

    if (karakter != '1') {
      SIM7000.println(F("AT+CGREG=1"));
      readSerial(200);
    }
    else break;
  }

}

void gprsAttach(unsigned int waktu) {
  // ATTACH gprs SERVICE
  SIM7000.println(F("AT+CGATT=1"));
  readSerial(waktu);
}

void gprsStatus(unsigned int waktu) {
  // ATTACH gprs SERVICE
  byte i = 0;
  unsigned long mulai = millis();
  SIM7000.println(F("AT+CIPSTATUS"));
  while (millis() - mulai <= waktu && i < 4) {
    while (SIM7000.available() > 0) {
      char g = SIM7000.read();
      Serial.write(g);
      if (g == '\n') i = i + 1;
    }
  }
  readSerial(20);
}

void gprsShut(unsigned int waktu) {
  //DEAKTIVASI gprsPDP CONTEXT DAN TUTUP BEARER
  SIM7000.println(F("AT+CIPSHUT"));
  cekSerial(waktu);
}

//void PDPactivate(char &karakter, unsigned long jeda) {
//  // ATTACH gprs SERVICE
//  unsigned long waktu = millis();
//  while (millis() - waktu <= jeda && karakter != 'O') {
//    SIM7000.println(F("AT+CGACT=1,1"));
//    karakter = cekSerial(200);
//  }
//}

void gprsMux(unsigned int waktu) {
  // ATTACH gprs SERVICE
  SIM7000.println(F("AT+CIPMUX=0"));
  readSerial(waktu);
}

void gprsCSTT(unsigned int waktu) {
  byte indeks;
  //APN
  SIM7000.print(F("AT+CSTT=\""));
  SIM7000.print(apn);

  //USER - ISI DI ANTARA \" DAN \"
  SIM7000.print(F("\",\"\","));

  //PASSWORD - ISI DI ANTARA \" DAN \"
  SIM7000.println(F("\"\""));
  readSerial(waktu);
}

void gprsWirelessConnect(char &karakter, unsigned long waktu) {
  SIM7000.println(F("AT+CIICR"));
  char g = cekSerial(waktu);
  // debug
#if defined debug
  Serial.print(F("\r\nKarakter dari CIICR : "));
  Serial.print(g);
  if (g == 'O')  Serial.println(F("K"));
  else {
    Serial.println(F("RROR"));
  }
#endif

  //PARSING OPEN BEARER UNTUK CEK OK ATAU ERROR
  return g;
}

void gprsIP(unsigned int waktu) {
  SIM7000.println(F("AT+CIFSR"));
  readSerial(waktu);
}

// LOW CODE FOR TCP IP
// --------------------------------------------------
// FUNCTION FOR TCP IP
boolean TCPstart(unsigned long jeda, byte ulangan) {
  char g;
  boolean hasilTCP = 0;
  for (byte angka = 1; angka <= ulangan; angka++) {
    Serial.print(F(" - TCP ke - "));
    Serial.println(angka);
    Serial.flush();
    Serial1.flush();

    SIM7000.print("AT+CIPSTART=");
    SIM7000.print("\"TCP\",\"");
    SIM7000.print(F("things.ubidots.com"));
    SIM7000.println(F("\",\"80\""));

    unsigned long waktu = millis();
    while (waktu + jeda > millis() ) {
      g = SIM7000.read();
      if (isAscii(g))  Serial.print(g);
      if (g == 'S' || g == 'C' || g == 'A' || g == 'E') break;
    }

    readSerial(150);

    if (g == 'C' || g == 'A') {
      hasilTCP = 1;
      break;
    }
    if (g == 'E' || g == 'S') {
      hasilTCP = 0;
      break;
    }
  }
#ifdef debug
  Serial.print(F("\r\nHasil TCP Start="));
  Serial.println(hasilTCP);
#endif

  if (hasilTCP == 0) readSerial(2000);
  return hasilTCP;
}

boolean TCPsend() {
  //siapkan variabel kode HTTP
  kode = 0;
  char karakter = '.';
  SIM7000.println(F("AT+CIPSEND"));
  unsigned long waktu = millis();
  while (millis() - waktu < 1000 && karakter != '>') {
    while (SIM7000.available() > 0 && karakter != '>') {
      karakter = SIM7000.read();
      Serial.write(karakter);
    }
  }
  if (karakter != '>') return 0;
delay(100);
  SIM7000.print("POST /api/v1.6/devices/");
  SIM7000.print(DEVICE_LABEL);
  SIM7000.print(F(" HTTP/1.1\r\n"));
  SIM7000.print(F("Host: "));
  SIM7000.print(HTTPSERVER);
  SIM7000.print(F("\r\n"));
  readSerial(500);

  SIM7000.print(F("User-Agent: "));
  SIM7000.print(USER);
  SIM7000.print(F("/"));
  SIM7000.print(VERSION);
  SIM7000.print(F("\r\n"));

  SIM7000.print(F("X-Auth-Token: "));
  SIM7000.print(TOKEN);
  SIM7000.print(F("\r\n"));
  SIM7000.print(F("Connection: close\r\n"));
  readSerial(500);

  SIM7000.print(F("Content-Type: application/json\r\n"));
  SIM7000.print(F("Content-Length: "));
  SIM7000.print(json.length());
  SIM7000.print(F("\r\n\r\n"));
  readSerial(500);

  SIM7000.print(json);
  SIM7000.print(F("\r\n"));
  readSerial(500);
  Serial.write(13);
  Serial.write(10);
  SIM7000.println((char)26);

  char character = '\0';
  unsigned long waktuAwal = millis();
  // SEND OK
  waktuAwal = millis();
  boolean nilai = 0;
  while (millis() - waktuAwal < 15000 && nilai == 0) {
    nilai = SIM7000.find("SEND OK");
  }
  if (nilai == 0) return 0;


  Serial.println(F("\r\n- TUNGGU KODE HTTP -"));
  //HTTP 1.1
  waktuAwal = millis();
  while (millis() - waktuAwal < 10000 && character != ' ') { // && SIM7000.available()>0
    character = SIM7000.read();
#ifdef debug
    if (isAscii(character)) Serial.write(character);
#endif
  }

  if (character != ' ') return 0;

  //HTTP RESPONSE
  character == '0';

  waktuAwal = millis();
  while (millis() - waktuAwal < 10000 && character != 'O') {
    character = SIM7000.read();

    switch (character) {
      case '0' ... '9':
#ifdef debug
        Serial.print(character);
#endif

        kode *= 10;
        kode += character - '0';
        break;
    }

  } // while serial available
  if (character != 'O') return 0;

  Serial.print(" O");
  readSerial(750);


#ifdef debug
  Serial.print(F("\r\n\r\nKODE="));
  Serial.println(kode);
#endif
  return 1;
}

void TCPclose(unsigned int waktu) {
  SIM7000.println("AT");
  readSerial(100);
  SIM7000.println(F("AT+CIPCLOSE"));
  readSerial(waktu);
}
