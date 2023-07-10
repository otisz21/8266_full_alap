
//******* Függvények **************************************
// ********************************************************

void PROJECT_INFO() {
  // ****** forrásfájl és fordítás ideje (megfelelő formátumban) **********************************
  String date = String(compile_date);
  String comp_ev = date.substring(7, 11);
  String comp_hnap = date.substring(0, 3);
  String comp_nap = date.substring(4, 6);
  String comp_ido = String(compile_time);
  comp_idopont = comp_ev + "-" + comp_hnap + "-" + comp_nap + "  " + comp_ido; // Stringként a fordítás időpontja  
  // ********************************************************************************************** 
  Serial.println();
  Serial.println(F("*************** PROJECT INFO ****************"));
  Serial.println(F("     project:"));
  Serial.print(F("     "));
  Serial.println(project);
  Serial.println();
  Serial.println(F("     fordítás időpontja:"));
  Serial.print(F("     "));
  Serial.println(comp_idopont);
  Serial.println();
  Serial.println(F("     Alaplap (fordítóban beállított): "));
  Serial.print(F("     "));
  Serial.println(ARDUINO_BOARD);
  Serial.println(F("*********************************************"));
  Serial.println();
  }

// --- NTP frissítések időköze (4 óra) --------------------------
uint32_t sntp_update_delay_MS_rfc_not_less_than_15000() {
  return 4 * 60 * 60 * 1000UL;               // 4 hours 
  }                            

// ** Initialize WiFi **************************************************
bool initWiFi() {
  W_T_OUT = 0;
  
  if (wifi_pref_mode == 0) WIFI_STA_or_AP = 0;   // 0->AP-mód
  else WIFI_STA_or_AP = 1;                       // 1->STA mód(client) 

  Serial.print(F("- initWiFi(), ssid: "));
  Serial.println(ssid);
  Serial.print(F("- initWiFi(), pass: "));
  Serial.println(pass);
  Serial.print(F("- initWiFi(), mode: "));
  Serial.println(WIFI_STA_or_AP);  

  if ((ssid == "") || (pass == "") || (WIFI_STA_or_AP == 0) || (drd_status == 1)) {
    WiFi.disconnect();
    WIFI_SCAN();
    WiFi.disconnect();
    delay(10);    
    WiFi.softAP(AP_ssid, NULL);      // open Access Point, nincs PW
    Serial.print(F("AP-SSID: "));
    Serial.print(AP_ssid);
    Serial.print(F("  AP-IP: "));
    Serial.println(WiFi.softAPIP());
    WIFI_STA_or_AP = 0;
    drd_status = 0;
    return false;
    }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  while ((WiFi.status() != WL_CONNECTED) & (W_T_OUT < 100)) {  // 50mp (100x500=50000)
    delay(500);
    Serial.print(".");
    W_T_OUT++;
    }
  Serial.println();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("- (WiFi.status() != WL_CONNECTED)= FALSE (0), ===> AP (CONFIG) mód!"));
    WiFi.disconnect();
    WIFI_SCAN();
    Serial.println(F("CONFIG (AP) mód!"));
    WiFi.disconnect();
    delay(10);
    WiFi.softAP(AP_ssid, NULL);      // NULL sets an open Access Point, nincs PW
    Serial.print(F("AP-SSID: "));
    Serial.print(AP_ssid);
    Serial.print(F("  AP-IP: "));
    Serial.println(WiFi.softAPIP());    
    WIFI_STA_or_AP = 0;
    return false;
    }
  else {
    WIFI_STA_or_AP = 1;
    Serial.println('\n');
    Serial.print(" * * * WiFi connected:\t");
    Serial.println(WiFi.SSID());          // Melyik hálózathoz csatlakozunk
    Serial.print(" * * * WiFi local IP :\t");
    Serial.println(WiFi.localIP());       // ESP8266 IP-címe    
    return true;
    }
  }
// ** Scan WiFi network ************************************************
void WIFI_SCAN() {
  if (S_DEBUG)Serial.println(F("*** WIFI SCAN () ***"));
  WIFI_drb_int = WiFi.scanNetworks(false, false);
  if (S_DEBUG) {
    if (S_DEBUG)Serial.print(F("Talalt WIFI halozatok szama: "));
    if (S_DEBUG)Serial.println(WIFI_drb_int);
    }

  if (WIFI_drb_int == 0) {
    return;
    }
  else {
    int indices[WIFI_drb_int];      // sort by RSSI
    int skip[WIFI_drb_int];
    for (int i = 0; i < WIFI_drb_int; i++) {
      indices[i] = i;
      }
    for (int i = 0; i < WIFI_drb_int; i++) {
      for (int j = i + 1; j < WIFI_drb_int; j++) {
        if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
          std::swap(indices[i], indices[j]);
          std::swap(skip[i], skip[j]);
          }
        }
      }

    if (WIFI_drb_int > 8){  // max 8 db hálózat (weboldal miatt)
      WIFI_drb_int = 8;
    }  
    for (int i = 0; i < WIFI_drb_int; ++i) {
      if (indices[i] != -1) {
        RSSI_sort_int[i] = WiFi.RSSI(indices[i]);
        if (S_DEBUG)Serial.println(WiFi.RSSI(indices[i]));
        SSID_sort_string[i] = String(WiFi.SSID(indices[i]));
        if (S_DEBUG)Serial.println(WiFi.SSID(indices[i]));
        }
      }
    }
  }

// Web Socket esemény ---------------------------------------------------------------
void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
  void* arg, uint8_t* data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      Serial.print("switch (type) --> WS_EVT_DATA: ");
      Serial.println(type);
      Serial.print("WS_EVT_DATA, len: ");
      Serial.println(len);
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      Serial.println("switch (type) --> WS_EVT_PONG");
      break;
    case WS_EVT_ERROR:
      Serial.println("switch (type) --> WS_EVT_ERROR");
      break;
    }
  }

// Web Socket, ha adat érkrzik Clienstől ---------------------------------------------
void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
  AwsFrameInfo* info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    W_S_rec_str = "";
    for (unsigned int i = 0; i < len; i++) W_S_rec_str += ((char)data[i]);
    if (S_DEBUG) Serial.print(F("W_S_rec_str (str_all): "));
    if (S_DEBUG) Serial.println(W_S_rec_str);    
    W_S_rec_int = W_S_rec_str.substring(0, 2).toInt();
    W_S_rec_str = W_S_rec_str.substring(2);
    if (S_DEBUG) Serial.print(F("W_S_rec_int (str_0-2): "));
    if (S_DEBUG) Serial.println(W_S_rec_int);    
    if (S_DEBUG) Serial.print(F("W_S_rec_str (str_2.-): "));
    if (S_DEBUG) Serial.println(W_S_rec_str);
    }
  }

// ------ Simple FTP server callback -------------------------------------------------------
void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace) {
  Serial.print(F(">>>>>>>>>>>>>>> _callback "));
  Serial.print(ftpOperation);
  // FTP_CONNECT,
  // FTP_DISCONNECT,
  // FTP_FREE_SPACE_CHANGE
  Serial.print(" ");
  Serial.print(freeSpace);
  Serial.print(" ");
  Serial.println(totalSpace);
  // freeSpace : totalSpace = x : 360  
  if (ftpOperation == FTP_CONNECT) Serial.println(F("CONNECTED"));
  if (ftpOperation == FTP_DISCONNECT) Serial.println(F("DISCONNECTED"));
  }

// ------ Simple FTP server callback -------------------------------------------------------
void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize) {
  Serial.print(F(">>>>>>>>>>>>>>> _transferCallback "));
  Serial.print(ftpOperation);
  /* FTP_UPLOAD_START = 0,
   * FTP_UPLOAD = 1,
   *
   * FTP_DOWNLOAD_START = 2,
   * FTP_DOWNLOAD = 3,
   *
   * FTP_TRANSFER_STOP = 4,
   * FTP_DOWNLOAD_STOP = 4,
   * FTP_UPLOAD_STOP = 4,
   *
   * FTP_TRANSFER_ERROR = 5,
   * FTP_DOWNLOAD_ERROR = 5,
   * FTP_UPLOAD_ERROR = 5
   */
  Serial.print(" ");
  Serial.print(name);
  Serial.print(" ");
  Serial.println(transferredSize);
  }
    
// --- callback, NTP frissítéskor kerül meghívásra --------------------------
void NTP_time_is_set() {          
  WEB_action_b = 13;
  WEB_delay_ul = millis();
  if(S_DEBUG){
    Serial.println();
    Serial.println(F("NTP_time_is_set()"));       
    Serial.print(F("NTP szerver 1.: ")); 
    Serial.println(NTP_SERVER_1); 
    Serial.print(F("NTP szerver 2.: ")); 
    Serial.println(NTP_SERVER_2);
    Serial.print(F("NTP_LASTSYNC_STRING: "));
    Serial.println(NTP_LASTSYNC_STRING);
    Serial.println(F("**** NTP sync. end *****"));}  
  } 

// --- Idő adatok megszerzése -----------------------------------------------
void showTime() {
  tm timeinfo;
  localtime_r(&now, &timeinfo);
  DATE_STRING = "";
  DATE_STRING = String(timeinfo.tm_year + 1900);
  DATE_STRING += "-";
  DATE_STRING += first_0(timeinfo.tm_mon + 1);
  DATE_STRING += "-";
  DATE_STRING += first_0(timeinfo.tm_mday);
  TIME_STRING = "";
  TIME_STRING = first_0(timeinfo.tm_hour);
  TIME_STRING += ":";
  TIME_STRING += first_0(timeinfo.tm_min);
  TIME_STRING += ":";
  TIME_STRING += first_0(timeinfo.tm_sec);

  N_YEAR = timeinfo.tm_year + 1900;
  N_MONTH = timeinfo.tm_mon + 1;
  N_DAY = timeinfo.tm_mday;
  N_HOUR = timeinfo.tm_hour;
  N_MIN = timeinfo.tm_min;
  N_SEC = timeinfo.tm_sec;

  year_x_day = timeinfo.tm_yday + 1;      // day in the year, range 0 to 365
  winter_sommer_time = timeinfo.tm_isdst; // daylight saving time
  EPO_STRING = String(time_t(now));
  N_EPO = time_t(now);
  DAYNAME = DOW_ARRY[timeinfo.tm_wday];
  sprintf(AKT_IDO_CHAR_10, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  // AKT_IDO_CHAR_10: [0][1][2][3][4][5][6][7][8][9]
  //             pl.:  1  4  :  2  5  :  3  2  -  -
  //-----------------------------------------------------------
  // char.  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, .. 0, 1, ....
  // byte  48, 49, 50, 51, 52, 53, 54, 55, 56, 57,              

  mp_x_dik = AKT_IDO_CHAR_10[7];
  perc_x_dik = AKT_IDO_CHAR_10[4];
  }
// ---------------------------------------------------------------------------
String first_0(int adat) {        // 9:2 ==> 09:02
  if (adat < 10) vissza = "0" + String(adat);
  else vissza = String(adat);
  return vissza;
  }

// ---  futásidő, millis() túlcsordulás miatt 64 biten ------------------------
uint64_t millis64() {
  static uint32_t low32, high32;
  uint32_t new_low32 = millis();
  if (new_low32 < low32) high32++;
  low32 = new_low32;
  return (uint64_t)high32 << 32 | low32;
  }

String wifi_status_to_string(int w_status) {
  String WL_ST_String;
  switch (w_status) {
    case 255:
      WL_ST_String = "WL_NO_SHIELD";
      break;
    case 0:
      WL_ST_String = "WL_IDLE_STATUS";
      break;
    case 1:
      WL_ST_String = "WL_NO_SSID_AVAIL";
      break;
    case 2:
      WL_ST_String = "WL_SCAN_COMPLETED";
      break;
    case 3:
      WL_ST_String = "WL_CONNECTED";
      break;
    case 4:
      WL_ST_String = "WL_CONNECT_FAILED";
      break;
    case 5:
      WL_ST_String = "WL_CONNECTION_LOST";
      break;
    case 6:
      WL_ST_String = "WL_WRONG_PASSWORD";
      break;
    case 7:
      WL_ST_String = "WL_DISCONNECTED";
      break;
    default:
      break;
    }
  return WL_ST_String;
  }

// WiFi.getMode() =  // 0=WIFI_OFF,  1=WIFI_STA,  2=WIFI_AP,  3=WIFI_AP_STA  
String wifi_mode_to_string(int w_mode) {
  String WiFi_mode_String;
  switch (w_mode) {
    case 0:
      WiFi_mode_String = "WIFI_OFF";
      break;
    case 1:
      WiFi_mode_String = "WIFI_STA";
      break;
    case 2:
      WiFi_mode_String = "WIFI_AP";
      break;
    case 3:
      WiFi_mode_String = "WIFI_AP_STA";
      break;
    case 4:
      WiFi_mode_String = "ismeretlen";
      break;
    default:
      break;
    }
  return WiFi_mode_String;
  }

// Read File from LittleFS
String readFile(fs::FS& fs, const char* path) {
  if (S_DEBUG)Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    if (S_DEBUG)Serial.println("- failed to open file for reading");
    return String();
    }
  String fileContent;
  while (file.available()) {
    fileContent = file.readStringUntil('\n');
    break;
    }
  file.close();
  return fileContent;
  }

// Write file to LittleFS
void writeFile(fs::FS& fs, const char* path, const char* message) {
  if (S_DEBUG)Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
  if (!file) {
    if (S_DEBUG)Serial.println("- nem sikerült megnyitni a directorit, íráshoz!");
    return;
    }
  if (file.print(message)) {
    if (S_DEBUG)Serial.println("- file írása: sikeres!");
    }
  else {
    if (S_DEBUG)Serial.println("- file írása: sikertelen!");
    }
  file.close();
  }
  
// --- Directory listázása Little FS-ből -------------------------------------
void listDir(const char * dirname) {
  Serial.printf("Print Directory: %s\n", dirname);
  Serial.println();
  Dir dir = LittleFS.openDir(dirname);
  while (dir.next()) {
    if (dir.isFile()) {
      Serial.println(F("- FILE:"));
      Serial.print(F("  "));
      Serial.print(dir.fileName());
      Serial.print(F("  SIZE: "));
      Serial.println(dir.fileSize());
      time_t cr = dir.fileCreationTime();
      time_t lw = dir.fileTime();
      struct tm * tmstruct = localtime(&cr);
      Serial.printf("  CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 
                      1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday,
                      tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
      tmstruct = localtime(&lw);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 
                      1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, 
                      tmstruct->tm_min, tmstruct->tm_sec);
      }
    if (dir.isDirectory()) {
      Serial.println(F("- DIRECTORY:"));
      Serial.print(F("  "));
      Serial.print(dirname);
      Serial.print(F("/"));
      Serial.println(dir.fileName());
      }
    Serial.println(F(" * * * "));
    Serial.println();
    }
  }
  
//-------- egyéb hőmérsékletek -----------------------------             
void tempek(byte mit) {
  // mit=0-semmi, 
  // mit=2-szekrény- http://192.168.0.97:80/skaf
  // mit=3-konyha  - http://192.168.0.67:43101/ora
  // mit=4-nappali - http://192.168.0.102:8080/temp_hum
  // mit=5-háló    - http://192.168.0.61:43130/bedroom
  // mit=6-egyéni lekérdezés - pl.: SSR relay: http://192.168.0.90:43125/ssr
                          // - pl.: DCF77 gen.: http://192.168.0.80/skaf  
  // mit=10        - összes lekérdezése

  if (S_DEBUG)Serial.print(F("Sync tempek, mit?: "));
  if (S_DEBUG)Serial.println(mit);

  HTTPClient http;

  //-------- DCF77 (szekrény) hőmérséklet ------------------------------
  if ((mit == 2) || (mit == 10)) {
    http.begin(client, "http://192.168.0.80/skaf");
    httpResponseCode = http.GET();    // Send HTTP GET request       
    payload = "--";
    if (httpResponseCode == 200) {
      if (S_DEBUG)Serial.print(F("HTTP Response code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      payload = http.getString();
      ws.textAll("11" + payload);
      }
    else {
      if (S_DEBUG)Serial.print(F("Error code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      ws.textAll("11" + String(httpResponseCode));
      }
    http.end();
    if (S_DEBUG)Serial.println(payload);
    }

  //-------- étkezó hőmérséklet ------------------------------
  if ((mit == 3) || (mit == 10)) {
    http.begin(client, "http://192.168.0.67:43101/ora");
    httpResponseCode = http.GET();    // Send HTTP GET request       
    payload = "--";
    if (httpResponseCode == 200) {
      if (S_DEBUG)Serial.print(F("HTTP Response code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      payload = http.getString();
      ws.textAll("12" + payload);
      }
    else {
      if (S_DEBUG)Serial.print(F("Error code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      ws.textAll("12" + String(httpResponseCode));
      }
    http.end();
    if (S_DEBUG)Serial.println(payload);
    }

  //------- nappali hőmérséklet (LED-MATRIX clock) -------------
  if ((mit == 4) || (mit == 10)) {
    http.begin(client, "http://192.168.0.102:8080/temp_hum");
    httpResponseCode = http.GET();    // Send HTTP POST request       
    payload = "--";
    if (httpResponseCode == 200) {
      if (S_DEBUG)Serial.print(F("HTTP Response code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      payload = http.getString();
      ws.textAll("13" + payload);
      }
    else {
      if (S_DEBUG)Serial.print(F("Error code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      ws.textAll("13" + String(httpResponseCode));
      }
    http.end();
    if (S_DEBUG)Serial.println(payload);
    }

  //---------- hálószoba hőmérséklet ---------------------------
  if ((mit == 5) || (mit == 10)) {
    http.begin(client, "http://192.168.0.61:43130/bedroom");
    httpResponseCode = http.GET();    // Send HTTP POST request       
    payload = "--";
    if (httpResponseCode == 200) {
      if (S_DEBUG)Serial.print(F("HTTP Response code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      payload = http.getString();
      ws.textAll("14" + payload);
      }
    else {
      if (S_DEBUG)Serial.print(F("Error code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      ws.textAll("14" + String(httpResponseCode));
      }
    http.end();
    if (S_DEBUG)Serial.println(payload);
    }

  //---------- egyéni lekérdezés ---------------------------
  if ((mit == 6) || (mit == 10)) {
    http.begin(client, "http://" + E_http);
    int httpResponseCode = http.GET();    // Send HTTP POST request       
    String payload = "--";
    if (httpResponseCode == 200) {
      if (S_DEBUG)Serial.print(F("HTTP Response code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      payload = http.getString();
      ws.textAll("15" + payload);
      }
    else {
      if (S_DEBUG)Serial.print(F("Error code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      ws.textAll("15" + String(httpResponseCode));
      }
    http.end();
    if (S_DEBUG)Serial.println(payload);
    }
  }

// ------ IP check easyddns -------------------------------------
void WAN_IP_CHECK_easyddns() {
  if (S_DEBUG)Serial.println(F("WAN_IP_CHECK_easyddns()"));
  HTTPClient http;
  http.setTimeout(800);
  http.begin(client, "http://ifconfig.me/ip");
  httpResponseCode = http.GET();    // Send HTTP GET request       
  payload = "--";
  if (httpResponseCode == 200) {
    if (S_DEBUG)Serial.print(F("HTTP Response code: "));
    if (S_DEBUG)Serial.println(httpResponseCode);
    payload = http.getString();
    ws.textAll("21" + payload);
    }
  else {
    if (S_DEBUG)Serial.print(F("Error code: "));
    if (S_DEBUG)Serial.println(httpResponseCode);
    ws.textAll("21" + String(httpResponseCode));
    }
  http.end();
  if (S_DEBUG)Serial.println(payload);
  }

//-----------------------------------------
  void URL_GET (String utvonal){
  if (S_DEBUG)Serial.println(F("URL_GET (String utvonal)"));
  String keres;
  keres = "http://";
  keres += utvonal;
  HTTPClient http;
  http.setTimeout(800);
  http.begin(client, keres);
  httpResponseCode = http.GET();    // Send HTTP GET request       
  payload = "--";
  if (httpResponseCode == 200) {
    if (S_DEBUG)Serial.print(F("HTTP Response code: "));
    if (S_DEBUG)Serial.println(httpResponseCode);
    payload = http.getString();
    ws.textAll("22" + payload);
    }
  else {
    if (S_DEBUG)Serial.print(F("Error code: "));
    if (S_DEBUG)Serial.println(httpResponseCode);
    ws.textAll("22" + String(httpResponseCode));
    }
  http.end();
  if (S_DEBUG)Serial.println(payload);
  }