
// **  előfeldolgozó makró (__DATE__), a fordító biztosítja a fordítási időben. ****
  const char compile_date[] = __DATE__ ;
  const char compile_time[] = __TIME__;
  const char file[] =__FILE__;
// ********************************************************************************  
// --- verziószámok, dátumok,------------------------- 
String comp_idopont;     //ESP Szoftver fordításának ideje (automatikusan kapja meg)
bool drd_status;

// ******* FIX adatok ****************************
String project = "8266_full_alap";
const char* mdns_name = "8266_full_alap";
const char* AP_ssid = "full_alap_CONFIG";

// ------ WIFI -----------------------------------
String ssid;
String pass;

int TCP_PORT = 80;

int W_T_OUT;
int WIFI_drb_int;
int RSSI_sort_int [9];
String SSID_sort_string [9];
bool WIFI_STA_or_AP;            // 0->AP-mód, 1->STA mód(client)
bool wifi_pref_mode;            // 0->AP-mód, 1->STA mód(client) (Preference mentés)
// WiFi csatlakozási időtúllépés hozzáférési pontonként. Növelje, ha a csatlakozás hosszabb ideig tart.
const uint32_t connectTimeoutMs = 20000;   // 20mp 

// ------ NTP -----------------------------------
const char *NTP_SERVER_1 = "0.europe.pool.ntp.org" ; 
const char *NTP_SERVER_2 = "time.nis.gov" ;           // NTP server neve
const char *MY_TZ = "CET-1CEST,M3.4.0/2,M10.5.0/3";   // időzóna Budapest

time_t now;     // ez a korszak (epoch)
unsigned long prevTime;
int N_YEAR, N_MONTH, N_DAY; 
int N_HOUR, N_MIN, N_SEC; 
String TIME_STRING, DATE_STRING, DAYNAME, EPO_STRING;
String DATE_STRING_old;
String vissza;
//Hét napjai
String DOW_ARRY[7] = {"vasárnap", "hétfő", "kedd", 
                        "szerda", "csütörtök", "péntek", "szombat" };                        
String MONTH_ARRY[13] = {"000.", "Jan.", "Feb.", "Mar.", "Apr.", "May.", "Jun.",  //Jan.22.
                                 "Jul.", "Aug.", "Sep.", "Oct.", "Nov.", "Dec." };
char AKT_IDO_CHAR_10 [10];
char AKT_IDO_CHAR_10_OLD [10];   //SSE
int year_x_day;            // day in the year, range 0 to 365  
int winter_sommer_time;    // daylight saving time
String NTP_LASTSYNC_STRING;
unsigned long N_EPO;
unsigned long mai_epo;
byte mp_x_dik;
byte perc_x_dik;

int allrun_perc_int;
int proc_restart_num;
int cycle;
byte proc_restart = 1;

String filesystem = "FS";
float used_FS_percent;

// **************************************************************
bool S_DEBUG = true;       // hogy legyen soros monitor ellenőrzés
bool make_log;
bool blue_led;
bool led_state;
bool led_on_off;
unsigned long led_delay_ul;
unsigned long led_blink_time_ul;
byte log_egyszer = 1;

String mai_filename;
String year_sum_file;
String log_file_name;

//*** WIFI - WEB, futásidő ******************
int httpResponseCode;
String payload; 
byte mi;
String E_cim;
String E_http;

byte run_save_egyszer;
unsigned long WEB_delay_ul;   // késleltetésekhez loop-ban
byte WEB_action_b = 0;        // webről érkező kérések, loopban végrehajtásához
String W_S_rec_str;
int W_S_rec_int = 99;
byte S_deb_valt = 10;

// --- SD kártya --------------------------------------
byte SD_status;
String SD_type;
uint32_t SD_size;
uint32_t SD_used_size;
uint32_t SD_totalBytes;
float used_SD_percent;

// ------------------------------------------------------------------
