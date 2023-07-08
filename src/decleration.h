
//   Ez egy függvény prototípus 
// - alapvetően a függvényfej {}-blokk nélkül, pontosvesszővel végződve, 
// - minden egyes használni kívánt függvényhez.

// ****************** F_system.h ****************************************
void PROJECT_INFO();

uint32_t sntp_update_delay_MS_rfc_not_less_than_15000();

bool initWiFi();
void WIFI_SCAN(); 

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);

void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace);
void _transferCallback(FtpTransferOperation ftpOperation, const char *name, unsigned int transferredSize);

void tempek(byte mit);
void NTP_time_is_set();
void showTime();
String first_0 (int adat);
uint64_t millis64();

// ****************** F_SD_filesystem.h *********************************
bool Check_SD();
String readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void new_logfile();
void printDirectory(File dir, int numTabs);
void copy_file(String f_name_1);
String tmConvert_ep(String EP_string);
String to_hh_mm(int mp);

String wifi_status_to_string(int w_status);
String wifi_mode_to_string(int w_mode);
