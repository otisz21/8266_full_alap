
//   Ez egy függvény prototípus 
// - alapvetően a függvényfej {}-blokk nélkül, pontosvesszővel végződve, 
// - minden egyes használni kívánt függvényhez.

// ****************** F_system.h ****************************************
void PROJECT_INFO();

uint32_t sntp_update_delay_MS_rfc_not_less_than_15000();

bool initWiFi();
void WIFI_SCAN(); 

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace);
void _transferCallback(FtpTransferOperation ftpOperation, const char *name, unsigned int transferredSize);

void NTP_time_is_set();
void showTime();
String first_0 (int adat);
uint64_t millis64();
String wifi_status_to_string(int w_status);
String wifi_mode_to_string(int w_mode);
String Push_mobil(String message, String sound, String priority, int device);
void SEND_email(String cel_email, String textMsg);

// ****************** F_SD_filesystem.h *********************************
String readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void listDir(const char * dirname);

void tempek(byte mit);

void WAN_IP_CHECK_easyddns();

void URL_GET (String utvonal);

