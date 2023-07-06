
//******* Függvények **************************************
// ********************************************************

void PROJECT_INFO (){
// ****** forrásfájl és fordítás ideje (megfelelő formátumban) **********************************
    String date = String (compile_date);
    String comp_ev = date.substring(7,11);
    String comp_hnap = date.substring(0,3);
    String comp_nap = date.substring(4,6);
    String comp_ido = String (compile_time);
    comp_idopont = comp_ev +"-"+ comp_hnap +"-"+ comp_nap +"  "+ comp_ido; // Stringként a fordítás időpontja  
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
    Serial.println();}

uint32_t sntp_update_delay_MS_rfc_not_less_than_15000() {
  return 4 * 60 * 60 * 1000UL;               // 4 hours 
  }                            

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String W_S_rec = "";
    for (unsigned int i=0; i < len; i++) W_S_rec += ((char) data[i]);
    Serial.println("W_S_rec: " + W_S_rec);
    if (W_S_rec == "10") proc_restart = 10;    // proc restart törlése
  }
}    

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
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
      break;}
}

void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace){
    Serial.print(F(">>>>>>>>>>>>>>> _callback " ));
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
    if (ftpOperation == FTP_DISCONNECT) Serial.println(F("DISCONNECTED"));};    
void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize){
    Serial.print(F(">>>>>>>>>>>>>>> _transferCallback " ));
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
    Serial.println(transferredSize);};
    
// ------ IP check easyddns -------------------------------------
void WAN_IP_CHECK_easyddns(byte IP_teszt){    // IP_teszt: 0-NoIp update(ha kell) 1-csak teszt 
        if(S_DEBUG)Serial.println(F("WAN_IP_CHECK_easyddns()"));

        HTTPClient http;
        http.setTimeout(1000);   // http válasz timeout: 1000 ms
        http.begin(client, "http://ifconfig.me/ip");                
        httpCode = http.GET();
        WAN_S = String(httpCode); 
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          answer_OK_byte = 1;
          String incoming = http.getString();          
          WAN_most_IP.fromString(incoming.c_str());
          }}        
      else {
          answer_OK_byte = 0;
          if(S_DEBUG)Serial.print(F("Error code: "));
          if(S_DEBUG)Serial.println(httpCode);}
      http.end();
      client.flush();
      if(S_DEBUG)Serial.print(F("httpCode: "));
      if(S_DEBUG)Serial.println(httpCode);
      if(S_DEBUG)Serial.print(F("válasz, WAN_most_IP: "));
      if(S_DEBUG)Serial.println(WAN_most_IP);
      
      if ((WAN_most_IP != WAN_new_IP)&(answer_OK_byte == 1)){
        if(S_DEBUG)Serial.print(F("WAN_new_IP (start): "));
        if(S_DEBUG)Serial.println(WAN_new_IP);
        
        WAN_old_IP = WAN_new_IP;
        WAN_new_IP = WAN_most_IP; 
        
        if(S_DEBUG)Serial.println(F("IP cím megváltozott!"));
        if(S_DEBUG)Serial.print(F("WAN_most_IP: "));
        if(S_DEBUG)Serial.println(WAN_most_IP);
        if(S_DEBUG)Serial.print(F("WAN_new_IP (end): "));
        if(S_DEBUG)Serial.println(WAN_new_IP);       
        writeString(114, WAN_new_IP.toString());
        answer_OK_byte = 0;
      if(IP_teszt == 0){  
        NO_IP_update();}
        }
      else{
        if(S_DEBUG)Serial.println(F("IP cím nem változott!"));
        if(S_DEBUG)Serial.print(F("WAN_most_IP: "));
        if(S_DEBUG)Serial.println(WAN_most_IP);
        if(S_DEBUG)Serial.print(F("WAN_new_IP: "));
        if(S_DEBUG)Serial.println(WAN_new_IP);}
      WAN_T = DATE_STRING + "  |  " + TIME_STRING;
      ws.textAll("21"+WAN_T);       // IP cím lekérdezés ideje
      ws.textAll("22"+WAN_S);       // IP cím lekérdezés válasza http kódja
      }
// ------ IP check no-ip api -------------------------------------
void WAN_IP_CHECK_noip(){
        if(S_DEBUG)Serial.println(F("WAN_IP_CHECK_noip()"));
        HTTPClient http;
        http.setTimeout(1000);   // http válasz timeout: 1000 ms
        http.begin(client, "http://ip1.dynupdate.no-ip.com");                
        httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          String incoming = http.getString();          
          WAN_new_IP.fromString(incoming.c_str());
          }}        
      else {
          if(S_DEBUG)Serial.print(F("Error code: "));
          if(S_DEBUG)Serial.println(httpCode);}
      http.end();
      client.flush();
      if(S_DEBUG)Serial.print(F("httpCode: "));
      if(S_DEBUG)Serial.println(httpCode);
      if(S_DEBUG)Serial.print(F("válasz, WAN_new_IP: "));
      if(S_DEBUG)Serial.println(WAN_new_IP);}
//---------- NO-IP frissítés ---------------------------
void NO_IP_update(){
        if(S_DEBUG)Serial.println(F("NO-IP frissítés()"));
        update_url = "";
        update_url = "http://"; 
        update_url += DDNS_USER;
        update_url += ":"; 
        update_url += DDNS_PASSW; 
        update_url += "@dynupdate.no-ip.com/nic/update?hostname=";
        update_url += DDNS_DOMAIN; 
        update_url += "&myip="; 
        update_url += WAN_new_IP.toString();

        if(S_DEBUG)Serial.println(F("update_url: "));
        if(S_DEBUG)Serial.println(update_url);

        HTTPClient http;
        http.setTimeout(8000);         // http válasz timeout: 8000 ms
        http.begin(client, update_url);                 
        httpCode = http.GET();         // Send HTTP GET request     

        if (httpCode == 200) {
          if(S_DEBUG)Serial.print(F("HTTP Response code: "));
          if(S_DEBUG)Serial.println(httpCode);
          noip_answer = http.getString();}
        else{
          if(S_DEBUG)Serial.print(F("Error code: "));
          if(S_DEBUG)Serial.println(httpCode);
          noip_answer = "ERROR: "+String(httpCode);}          
        http.end();
        client.flush();
        if(S_DEBUG)Serial.print(F("httpCode: "));
        if(S_DEBUG)Serial.println(httpCode);
        if(S_DEBUG)Serial.println(F("válasz: "));
        if(S_DEBUG)Serial.println(noip_answer);
        LAST_IP_CHANGED = DATE_STRING + "  |  " + TIME_STRING;
        ws.textAll("23"+LAST_IP_CHANGED);       // NO-IP lekérdezés ideje
        ws.textAll("24"+noip_answer);           // NO-IP válasz 
        }  
//-------- egyéb hőmérsékletek -----------------------------             
void tempek(byte mit){                      
  // mit=0-semmi, 
  // mit=2-szekrény- http://192.168.0.97:80/skaf
  // mit=3-konyha  - http://192.168.0.67:43101/ora
  // mit=4-nappali - http://192.168.0.102:8080/temp_hum
  // mit=5-háló    - http://192.168.0.61:43130/bedroom
  // mit=6-egyéni lekérdezés - pl.: SSR relay: http://192.168.0.90:43125/ssr
                          // - pl.: DCF77 gen.: http://192.168.0.80/skaf  
  // mit=10        - összes lekérdezése
                                                               
  if(S_DEBUG)Serial.print(F("Sync tempek, mit?: "));
  if(S_DEBUG)Serial.println(mit);

  HTTPClient http;
  
//-------- DCF77 (szekrény) hőmérséklet ------------------------------
    if((mit == 2)||(mit == 10)){      
        http.begin(client, "http://192.168.0.80/skaf");                
        httpResponseCode = http.GET();    // Send HTTP GET request       
        payload = "--";        
    if (httpResponseCode == 200) {
      if (S_DEBUG)Serial.print(F("HTTP Response code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      payload = http.getString();
      ws.textAll("11"+payload);
      }
    else {
      if (S_DEBUG)Serial.print(F("Error code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      ws.textAll("11"+String(httpResponseCode));
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
      ws.textAll("12"+payload);
      }
    else {
      if (S_DEBUG)Serial.print(F("Error code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      ws.textAll("12"+String(httpResponseCode));
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
      ws.textAll("13"+payload);
      }
    else {
      if (S_DEBUG)Serial.print(F("Error code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      ws.textAll("13"+String(httpResponseCode));
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
      ws.textAll("14"+payload);
      }
    else {
      if (S_DEBUG)Serial.print(F("Error code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      ws.textAll("14"+String(httpResponseCode));
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
      ws.textAll("15"+payload);
      }
    else {
      if (S_DEBUG)Serial.print(F("Error code: "));
      if (S_DEBUG)Serial.println(httpResponseCode);
      ws.textAll("15"+String(httpResponseCode));
      }
    http.end();
    if (S_DEBUG)Serial.println(payload);
    }
  }


// --- callback, NTP frissítéskor kerül meghívásra --------------------------
void NTP_time_is_set() {          
  WEB_action_b = 11;
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

  mp_x_dik = AKT_IDO_CHAR_10[7];    // char.  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, .. 0, 1, ....
                                    // byte  48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
  perc_x_dik = AKT_IDO_CHAR_10[4];  // char.  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, .. 0, 1, ....
                                    // byte  48, 49, 50, 51, 52, 53, 54, 55, 56, 57,                                
  }
// ---------------------------------------------------------------------------
String first_0 (int adat){        // 9:2 ==> 09:02
    if (adat<10){
     vissza = "0" + String(adat);}
    else{
      vissza = String(adat);} 
    return vissza;}
// ---  futásidő, millis() túlcsordulás miatt 64 biten ------------------------
uint64_t millis64() {             
    static uint32_t low32, high32;
    uint32_t new_low32 = millis();
    if (new_low32 < low32) high32++;
    low32 = new_low32;          
    return (uint64_t) high32 << 32 | low32;} 
// --- 3 bájt írása EEPROM-ba a, b, c, címekre -------------------------------
void EE_write_3byte (int a, int b, int c, int value){      // 3 bájt max: 16.777.215 perc ==> 11.650 nap, kb.:31 év
      EEPROM.write(c,(value >> 16) & 0xFF);
      EEPROM.write(b,(value >> 8) & 0xFF);
      EEPROM.write(a, value & 0xFF);
      EEPROM.commit();}         
// --- 3 bájt olvasása EEPROM-ból a, b, c, címekről --------------------------
int EE_read_3byte (int a, int b, int c){
    int value = (EEPROM.read(c) <<  16) +
                (EEPROM.read(b) <<  8) +
                (EEPROM.read(a));
                 return value;} 
// --- 2 bájt írása EEPROM-ba a, b, címekre ----------------------------------
void EE_write_2byte (int a, int b, int value){
      EEPROM.write(b,(value >> 8) & 0xFF);
      EEPROM.write(a, value & 0xFF);                      
      EEPROM.commit();
      }      
// --- 2 bájt olvasása EEPROM-ból a, b, címekről -----------------------------
int EE_read_2byte (int a, int b){
    int value = (EEPROM.read(b) <<  8) +
                (EEPROM.read(a));
                 return value;} 
// ---------------------------------------------------------------------------
void writeString(int add,String data) {  // karakterlánc beírása EEPROM-ba "add" címre
    int _size = data.length();
    int i;
  for(i=0;i<_size;i++)           {
    EEPROM.write(add+i,data[i]); }       // bájtok feltöltése a karakterekkel, kezdő (add) címtől karakterlánc hossz végéig
    EEPROM.write(add+_size,'\0');        // záró '\0' karakter
    EEPROM.commit();               }
// ---------------------------------------------------------------------------
String read_String(int add) {           // karakterlánc kiolvasása EEPROM-ból "add" címtől
    char data[50];                      // Max 50 Bytes
    int len=0;
    int k;
    k=EEPROM.read(add);
  while(k != '\0' && len<30) {          // kiolvasás null character-ig    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;                    }
  data[len]='\0';
  return String(data);        }  
// ----------------------------------------------------------------------------- 
void EEPROM_clear (){
    for (int i = 1; i < 240; i++){     // EEPROM terület törlése, 1-től, 239-ig
          EEPROM.write(i, 0);          // összes cím feltöltése 0-val
          delay (20);}                            
    EEPROM.write(239, 100);   // Teszt bájt      
    EEPROM.commit();
    ws.textAll("30");        
    //events.send(String ("1").c_str(), "EE_STATUS", 1000);
    EEPROM_read ();}      
// -----------------------------------------------------------------------------
void EEPROM_read (){             // Induláskor az összes EEPROM adat kiolvasása             
    make_log = EEPROM.read(1);       
    S_DEBUG = EEPROM.read(2); 
    blue_led = EEPROM.read(3);
    E_cim = read_String(4);
    E_http = read_String(44);

    DDNS_DOMAIN = read_String(84);
    WAN_new_IP.fromString(read_String (114));
    WAN_IP_CHECK_TIME = EE_read_2byte (132, 133);
    DDNS_ON = EEPROM.read(134);
        
    allrun_perc_int = EE_read_3byte (240, 241, 242);  // teljes futásidő beolvasása EEPROM-ból
    proc_restart_num = EE_read_3byte (243, 244, 245);
    proc_restart_num++;
    EE_write_3byte (243, 244, 245, proc_restart_num);

    Serial.print(F("allrun_perc_int, kiolvasása EEPROM-ból: "));
    Serial.println(allrun_perc_int);     
    if(S_DEBUG)Serial.println();
    if(S_DEBUG)Serial.println(F("EEPROM READ OK!")); 
    }                                        
// ------------------------------------------------------------------------------

void WS_send_10mp() {
  // DynamicJsonDocument WS_send(384);
  // JsonArray data = WS_send.createNestedArray("data");
  // data.add(TIME_STRING);              // [0]  
  // data.add(DATE_STRING);              // [1]
  // data.add(DAYNAME);                  // [2]
  // data.add(ESP.getChipId());          // [3]
  // data.add(proc_restart);             // [4] 
  // char buffer[256];
  // //size_t len = serializeJson(WS_send, buffer);
  // //ws.textAll(buffer, len);
  // ws.textAll("01"+String(buffer));
  ws.textAll("01"+TIME_STRING);
  }
