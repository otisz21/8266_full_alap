
// **  előfeldolgozó makró (__DATE__), a fordító biztosítja a fordítási időben. ****
  const char compile_date[] = __DATE__ ;
  const char compile_time[] = __TIME__;
  const char file[] =__FILE__;
// ********************************************************************************  
// --- verziószámok, dátumok,------------------------- 
String comp_idopont;     //ESP Szoftver fordításának ideje (automatikusan kapja meg)

String mai_filename;
String year_sum_file;
String log_file_name;

// ******* FIX adatok ****************************
String project = "8266_full_alap";
const char* mdns_name = "8266_full_alap";

// ------ WIFI -----------------------------------
//WiFi 1
const char* ssid1 = "DIGI-WiFi";
const char* password1 = "Myhome01";
//WiFi 2
const char* ssid2 = "TP-LACI";
const char* password2 = "Myhome02";
//WiFi 3
const char* ssid3 = "HUAWEI20";
const char* password3 = "hu202301";

int TCP_PORT = 80;

String ssid_changed = "";
String ssid_changed_old = "";
// WiFi csatlakozási időtúllépés hozzáférési pontonként. Növelje, ha a csatlakozás hosszabb ideig tart.
const uint32_t connectTimeoutMs = 20000;   // 20mp 

// ------ NTP -----------------------------------
const char *NTP_SERVER_1 = "0.europe.pool.ntp.org" ; 
const char *NTP_SERVER_2 = "time.nis.gov" ;           // NTP server neve
const char *MY_TZ = "CET-1CEST,M3.4.0/2,M10.5.0/3";   // időzóna Budapest

const uint64_t ora_1 = 3600000ULL;                    // nincs használva
time_t now;     // ez a korszak (epoch)
unsigned long prevTime;
int N_YEAR, N_MONTH, N_DAY; 
int N_HOUR, N_MIN, N_SEC; 
String TIME_STRING, DATE_STRING, DAYNAME, EPO_STRING;
String DATE_STRING_old;
String TIME_STRING_old;
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

// ------- DDNS ---------------------------------
String DDNS_USER = "laci2021";
String DDNS_PASSW = "OTTwebddns2021";
byte DDNS_ON;
String DDNS_DOMAIN;
//int WAN_IP_CHECK_TIME;   //  int. update time sec. max.: 65 535, 2 byt-on
unsigned long WAN_IP_CHECK_TIME;   //  int. update time sec. max.: 65 535, 2 byt-on
String LAST_IP_CHANGED;
String WAN_T;
String WAN_S;
byte answer_OK_byte;
IPAddress WAN_new_IP;
IPAddress WAN_old_IP;
IPAddress WAN_most_IP;
int httpCode;
String update_url;
String noip_answer = "WAIT";
unsigned long previousMillis;

int allrun_perc_int;
int proc_restart_num;
int cycle;
byte proc_restart = 1;

String filesystem = "FS";
float used_FS_percent;

// **************************************************************
bool S_DEBUG = true;       // hogy legyen soros monitor ellenőrzés
byte make_log;
byte log_egyszer = 1;
byte blue_led;
bool led_state;

//*** WIFI - WEB, futásidő ******************
int W_T_OUT;
int httpResponseCode;
String payload; 
byte mi;
String E_cim;
String E_http;

byte run_save_egyszer;
unsigned long WEB_delay_ul;   // késleltetésekhez loop-ban
byte WEB_action_b = 0;        // webről érkező kérések, loopban végrehajtásához
                              // 0-semmi, 1-button_1, ... 6-button_6, 10-RESET
byte get_tempek;
bool BME280_OK;
byte S_deb_valt = 10;
byte minmax_false = 1;

// --- SD kártya --------------------------------------
byte SD_status;
String SD_type;
String SD_fat;
uint32_t SD_size;
uint32_t SD_used_size;
uint32_t SD_totalBytes;
float used_SD_percent;

// ------------------------------------------------------------------
