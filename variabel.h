#define SSpin           53

#define HITAM   0x0000
#define BIRU    0x001F
#define MERAH     0xF800
#define HIJAU   0x07E0
#define CYAN    0x3619
#define KUNING 0xFFE0
#define PUTIH 0xFFFF
#define NAVY    0x000F
#define ORANGE 0xFD20 

#define debug

const char *TOKEN = "BBFF-ighe1Ydlbm85x7SVp70wbTbPerugyD";
const char *DEVICE_LABEL = "ev-iot";
char const * HTTPSERVER = "things.ubidots.com";
const int HTTPPORT = 80;
char const * USER = "Arduino_GSM";
char const * VERSION = "1.0";
char apn[] = "Telkomsel"; // Access point name. Leave empty if it is not needed.
char gprsLogin[] = ""; // GPRS username. Leave empty if it is not needed.
char gprsPassword[] = ""; // GPRS password. Leave empty if it is not needed.

String json;
