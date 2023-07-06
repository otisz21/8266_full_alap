
//   Ez egy függvény prototípus 
// - alapvetően a függvényfej {}-blokk nélkül, pontosvesszővel végződve, 
// - minden egyes használni kívánt függvényhez.

// ****************** F_system.h ****************************************
void PROJECT_INFO();

uint32_t sntp_update_delay_MS_rfc_not_less_than_15000();

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);

void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace);
void _transferCallback(FtpTransferOperation ftpOperation, const char *name, unsigned int transferredSize);
void WAN_IP_CHECK_easyddns(byte IP_teszt);
void WAN_IP_CHECK_noip();
void NO_IP_update();
void tempek(byte mit);
void NTP_time_is_set();
void showTime();
String first_0 (int adat);
uint64_t millis64();
void EE_write_3byte (int a, int b, int c, int value);
int EE_read_3byte (int a, int b, int c);
void EE_write_2byte (int a, int b, int value);
int EE_read_2byte (int a, int b);
void writeString(int add,String data);
String read_String(int add);
void EEPROM_clear ();
void EEPROM_read ();

void WS_send_10mp();

// ****************** F_SD_filesystem.h *********************************
bool Check_SD();
String readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void new_logfile();
void printDirectory(File dir, int numTabs);
void copy_file(String f_name_1);
String tmConvert_ep(String EP_string);
String to_hh_mm(int mp);

// ****************** F_chip_info.h ****************************************
void CHIP_INFO_8266_print();
String CHIP_INFO_8266_json();

String wl_status_to_string(int w_status);
