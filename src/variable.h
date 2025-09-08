
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


// ********* nedvesség érz. *******************************************
const int AirValue = 790;   //you need to replace this value with Value_1
const int WaterValue = 390;  //you need to replace this value with Value_2
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent=0;
byte water_on;
byte water_off;

const int buzzerPin = D7;     //for ESP8266 Microcontroller

int melody_1[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};

int noteDurations_1[] = {4, 8, 8, 4, 4, 4, 4, 4};

// notes in the melody:
int melody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5
};

// note durations: 4 = quarter note, 8 = eighth note, etc, also called tempo:
int noteDurations[] = {
  8, 8, 4, 8, 8, 4,
  8, 8, 8, 8, 2,
  8, 8, 8, 8,
  8, 8, 8, 16, 16,
  8, 8, 8, 8,
  4, 4
};

int beep_2x [] = {NOTE_C7, 0, NOTE_C7};
int beep_2x_durations [] = {50, 50, 50};

// ******* Pushover ****************************
const char* apiToken = "abvmk266n2abborqa1y839424ke2s1";
const char* userToken = "uqqxg1h7wyxxiif7qxeo2zb8dracn6";
//Pushover API endpoint
const char* pushoverApiEndpoint = "http://api.pushover.net/1/messages.json";  
// ******* Pushover ****************************


// ******* e-mail *************************************
/** Az smtp gazdagép neve pl. smtp.gmail.com a Gmailhez
     vagy smtp.office365.com Outlookhoz vagy smtp.mail.yahoo.com */
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "irjleveletmost@gmail.com"  // hitelesítő adatok
     // #define AUTHOR_PASSWORD "kisherceg15"         // jelszó:  ezzel nem működik

#define AUTHOR_PASSWORD "louyzczoljlvmzcr"      // APP-jelszó, 16char:  louyzczoljlvmzcr

#define RECIPIENT_EMAIL "otisz21@gmail.com"     // ahová a mailt küldi 

String email_status;
// ******* e-mail *************************************

String str_pr_1 = "Pushover üzenet";
String str_pr_2 = "echo";
String str_pr_3 = "otisz21@gmail.com";
String str_pr_4 = "E-mail üzenet";
String prior = "0";
int device = 1;

// ------ WIFI -----------------------------------
String ssid;
String pass;
int TCP_PORT = 80;

int W_T_OUT;
int WIFI_drb_int;
int WIFI_drb_int_all;
int RSSI_sort_int [9];
String SSID_sort_string [9];
bool WIFI_STA_or_AP;            // 0->AP-mód, 1->STA mód(client)
bool wifi_pref_mode;            // 0->AP-mód, 1->STA mód(client) (Preference mentés)

// ------ NTP -----------------------------------
const char *NTP_SERVER_1 = "0.europe.pool.ntp.org" ; 
const char *NTP_SERVER_2 = "time.nis.gov" ;           // NTP server neve
const char *MY_TZ = "CET-1CEST,M3.4.0/2,M10.5.0/3";   // időzóna Budapest

time_t now;     // ez a korszak (epoch)
unsigned long prevTime;
int N_HOUR, N_MIN, N_SEC; 
String TIME_STRING, DATE_STRING, DAYNAME, EPO_STRING;
String DATE_STRING_old;
//Hét napjai
String DOW_ARRY[7] = {"vasárnap", "hétfő", "kedd", 
                        "szerda", "csütörtök", "péntek", "szombat" };                        
char AKT_IDO_CHAR_10 [10];
char AKT_IDO_CHAR_10_OLD [10];
int year_x_day;                  // day in the year, range 0 to 365  
int winter_sommer_time;          // daylight saving time
String NTP_LASTSYNC_STRING;
unsigned long N_EPO;
unsigned long mai_epo;
byte mp_x_dik;
byte perc_x_dik;

int allrun_perc_int;
int proc_restart_num;
byte proc_restart = 1;

String filesystem = "FS";
float used_FS_percent;

// **************************************************************
bool S_DEBUG = true;       // hogy legyen soros monitor ellenőrzés
bool make_log;
bool blue_led;
bool led_state;
unsigned long led_delay_ul;
unsigned long led_blink_time_ul;

//*** WIFI - WEB, futásidő ******************
int httpResponseCode;
String payload; 
byte mi;
String E_http;

byte run_save_egyszer;
unsigned long WEB_delay_ul;   // késleltetésekhez loop-ban
byte WEB_action_b = 0;        // webről érkező kérések, loopban végrehajtásához
String W_S_rec_str;
int W_S_rec_int = 99;
byte S_deb_valt = 10;

// ------------------------------------------------------------------
