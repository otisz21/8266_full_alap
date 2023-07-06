
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <SimpleFTPServer.h>  // FTP server 
#include <AsyncElegantOTA.h>  // Elegant OTA 
#include <FS.h>               // Fájlrendszer kezelés miatt, favicon.ico fájlként feltöltve
#include <LittleFS.h>         // Fájlrendszer kezelés
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <time.h>             // time() ctime()
#include <coredecls.h>        // settimeofday_cb() visszahívás ellenőrzéshez (NTP lekérdezés)     

#include "variable.h"          // saját külön fájl a változóknak 
#include "NOfile.h"            // HTML oldal ismeretlen kéréshez
#include "decleration.h"

#include "Led.h"   // próba könyvtár
#define LED_PIN 2  // Use the built-in LED
Led led(LED_PIN);  // led objektum, LED_PIN-t OUTPUT-ra állítja

ESP8266WiFiMulti wifi_multi;

WiFiClient client;  // or WiFiClientSecure for HTTPS
FtpServer ftpSrv;   // set #define FTP_DEBUG in ESP8266FtpServer.h to see ...
AsyncWebServer server(TCP_PORT);       // TCP portszám: 80 (alap)
AsyncWebSocket ws("/ws");

ADC_MODE(ADC_VCC);
      
// ************************************************************************** 
//-- SETUP ------------------------------------------------------------------
void setup() {                  
  Serial.begin(115200);
  delay(2000);
  PROJECT_INFO();
  //Adding the WiFi networks to the MultiWiFi instance
  wifi_multi.addAP(ssid1, password1);
  wifi_multi.addAP(ssid2, password2);
  wifi_multi.addAP(ssid3, password3);

  //WiFi.persistent(false);
  //WiFi.mode(WIFI_STA);
  WiFi.setHostname(project.c_str());
  
  Serial.println("Connecting ...");
  int k = 0;                                 // nem működik, wifi_multi.run() "tartja" a programot
  while (wifi_multi.run(connectTimeoutMs) != WL_CONNECTED) { // Várja meg, amíg a Wi-Fi csatlakozik: 
    Serial.print(k);                         // keressen Wi-Fi hálózatokat, és csatlakozzon
    Serial.print(" ");                       // a fenti hálózatok közül a legerősebbhez
    delay(1000);
    k++;
  }
  Serial.println('\n');
  Serial.print("Connected to:\t ");
  Serial.println(WiFi.SSID());          // Melyik hálózathoz csatlakozunk
  Serial.print("IP address  :\t");
  Serial.println(WiFi.localIP());       // ESP8266 IP-címe

    //   Állítsa be az mDNS válaszadót:
    // - Az első argumentum ebben a példában a domain név
    //   a teljesen minősített domain név "RTC.local"
    // - A második érv a hirdetni kívánt IP-cím
    //   elküldjük IP címünket a WiFi hálózaton
    if (!MDNS.begin(mdns_name)) {
      Serial.println(F("Error setting up MDNS responder!"));
      }
    else{
    Serial.println(F("mDNS responder started"));
    Serial.print(F("mDNS name: http://"));
    Serial.print(mdns_name);
    Serial.println(F(".local"));
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
    }

  EEPROM.begin(256);
  //EE_write_3byte (243, 244, 245, 80);

  if (LittleFS.begin()) {
    ftpSrv.setCallback(_callback);
    ftpSrv.setTransferCallback(_transferCallback);
    filesystem = "LittleFS";
    Serial.println(F("LittleFS fájlrendszer elindítva!"));
    Serial.print(F("FTP username: "));
    Serial.println(F("esp8266"));
    Serial.print(F("FTP pw.: "));
    Serial.println(F("8266"));
    Serial.println();
    ftpSrv.begin("esp8266", "8266");     // username, password for ftp.   
    }                                   // (default 21, 50009 for PASV) 

  if (EEPROM.read(239) == 255) {
    EEPROM_clear();
    }
  else {
    EEPROM_read();
    }

    configTime(MY_TZ, NTP_SERVER_1, NTP_SERVER_2);  //Mytz - Budapest
    settimeofday_cb(NTP_time_is_set);          // opcionális: visszahívás, ha elküldték az időt

    delay(100);

//**** automatikusan teljesülő kérések ******************************************  
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");                // Root/ xy  ==> fájl / xy automatikus teljesítése  
//--------------------------------------------------------------------------------
    
//*******************************************************************************    
    server.on("/index_json", HTTP_GET, [](AsyncWebServerRequest* request) {        // JSON adatok Főoldalra
      AsyncResponseStream* response = request->beginResponseStream("application/json");
      DynamicJsonDocument IR(512);
      JsonArray data = IR.createNestedArray("data");
      data.add(TIME_STRING);              // [0]  
      data.add(DATE_STRING);              // [1]
      data.add(DAYNAME);                  // [2]
      data.add(ESP.getChipId());          // [3]
      data.add(proc_restart);             // [4]
      serializeJson(IR, *response);
      request->send(response);});

    //******************************************************************************** 
    //**** Info + Setup oldal ********************************************************
    //------------------------------------------------------------------------------  
    server.on("/setup_json", HTTP_GET, [](AsyncWebServerRequest* request) {
      AsyncResponseStream* response = request->beginResponseStream("application/json");
      DynamicJsonDocument adatok(384);
      JsonArray data = adatok.createNestedArray("data");
      data.add("BME280");                   // [0] Senzor indítása OK, vagy ERROR
      data.add(BME280_OK);                  // [1] Senzor indítása OK, vagy ERROR        
      data.add(project);                    // [2] project (a file name, csak main.cpp!)
      data.add(comp_idopont);               // [3] fordítás időpontja
      data.add(ARDUINO_BOARD);              // [4] modul tipus
      data.add(millis64());                 // [5] futásidő
      data.add(allrun_perc_int);            // [6] teljes futásidő
      data.add(make_log);                   // [7] hogy legyen soros monitor ellenőrzés
      data.add(S_DEBUG);                    // [8] készítsen-e log fájlt
      data.add(blue_led);                   // [9] villogjon-e a beépített kék led
      serializeJson(adatok, *response);
      request->send(response);
      });
//--------------------------------------------------------------------------------
  server.on("/setup_save", HTTP_GET, [](AsyncWebServerRequest *request){        // Info + Setup adatanak mentése             
    make_log = (request->getParam("make_log")->value()).toInt(); 
    S_DEBUG = (request->getParam("s_debug")->value()).toInt();
    blue_led = (request->getParam("blue_led")->value()).toInt();
    EEPROM.write (1, make_log);  
    EEPROM.write (2, S_DEBUG);
    EEPROM.write (3, blue_led);
    EEPROM.commit();
    request->send_P (200, "text/plain", "setup_SAVE_OK");});  

//********************************************************************************
//**** WIFI setup oldal **********************************************************
//--------------------------------------------------------------------------------
  server.on("/wifi_json", HTTP_GET, [](AsyncWebServerRequest *request) {           // WIFI-Setup oldal JSON adatok küldése
      AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument WIFI_json(384);    
        WIFI_json["ssid"] = String (WiFi.SSID());
        WIFI_json["ip"] = WiFi.localIP().toString();
        WIFI_json["port"] = String (TCP_PORT);
        WIFI_json["rssi"] = String (WiFi.RSSI());
        WIFI_json["wifi_mac"] = String(WiFi.macAddress());
        //WIFI_json["ap_ssid"] = String(TEMP_LOG_ssid);
 
     if (WiFi.status() == 0){
        WIFI_json["W_status"] = "WL_IDLE_STATUS";}
     if (WiFi.status() == 1){
        WIFI_json["W_status"] = "WL_NO_SSID_AVAIL";}
     if (WiFi.status() == 2){
        WIFI_json["W_status"] = "WL_SCAN_COMPLETED";}
     if (WiFi.status() == 3){
        WIFI_json["W_status"] = "WL_CONNECTED";}
     if (WiFi.status() == 4){
        WIFI_json["W_status"] = "WL_CONNECT_FAILED";}
     if (WiFi.status() == 5){
        WIFI_json["W_status"] = "WL_CONNECTION_LOST";}
     if (WiFi.status() == 6){
        WIFI_json["W_status"] = "WL_DISCONNECTED";}
     if (WiFi.status() == 7){
        WIFI_json["W_status"] = " 7 - ??? - ";} 
     if (WiFi.status() == 255){
        WIFI_json["W_status"] = "WL_NO_SHIELD";}                       

      serializeJson(WIFI_json, *response);
      request->send(response);});
//--------------------------------------------------------------------------------
  server.on("/WIFI_save", HTTP_GET, [](AsyncWebServerRequest *request){        // Ha a kérés: WIFI_action 
    WEB_action_b = (request->getParam("x")->value()).toInt();  // WIFI_action 10-restart, 13-restart+CONFIG
    WEB_delay_ul = millis();                                 
    request->send_P(200, "text/plain", "WIFI_action OK!");}); 

//*********************************************************************************
//**** NTP setup oldal ************************************************************
//---------------------------------------------------------------------------------
  server.on("/NTP_json", HTTP_GET, [](AsyncWebServerRequest *request) {         //  NTP setup oldal JSON adatok küldése
      AsyncResponseStream *response = request->beginResponseStream("application/json"); 
      DynamicJsonDocument NTP_adatok(256);
      JsonArray data = NTP_adatok.createNestedArray("data");                  
      data.add(TIME_STRING);              // [0] Pontos idő
      data.add(DATE_STRING);              // [1] Dátum
      data.add(DAYNAME);                  // [2] nap neve
      data.add(EPO_STRING);               // [3] epoch time
      data.add(year_x_day);               // [4] az év x. napja
      data.add(winter_sommer_time);       // [5] téli vagy nyári időszámytás
      data.add(NTP_LASTSYNC_STRING);      // [6] az utolsó NTP szinkronizáció ideje
      data.add(mai_epo);                  // [7] mai nap epoch time-ja 12:00-kor
      serializeJson(NTP_adatok, *response);
      request->send(response);});

//*********************************************************************************
//**** DDNS setup oldal ***********************************************************
  server.on("/ddns_json", HTTP_GET, [](AsyncWebServerRequest *request) {  
      AsyncResponseStream *response = request->beginResponseStream("application/json"); 
      DynamicJsonDocument DDNS_adatok(512);
      JsonArray data = DDNS_adatok.createNestedArray("data");        
      data.add(DDNS_ON);                 // [0] bájt 
      data.add(DDNS_DOMAIN);             // [1] String 
      data.add(WAN_IP_CHECK_TIME);       // [2] int. update time sec. max.: 65 535 2 byt-on 
      data.add(WAN_old_IP);              // [3] String
      data.add(WAN_new_IP);              // [4] String
      data.add(LAST_IP_CHANGED);         // [5] String
      data.add(noip_answer);             // [6] String
      data.add(WAN_T);                   // [7] WAN IP check time
      data.add(WAN_S);                   // [8] WAN IP check status
      serializeJson(DDNS_adatok, *response);
      request->send(response);});
// --------------------------------------------------------------------------
  server.on("/ddns_save", HTTP_GET, [](AsyncWebServerRequest *request){

    DDNS_ON = (request->getParam("D_1")->value()).toInt();
    WAN_IP_CHECK_TIME = (request->getParam("D_2")->value()).toInt();
    DDNS_DOMAIN = (request->getParam("D_3")->value());    
                   
    EEPROM.write(134, DDNS_ON); 
    EE_write_2byte (132, 133, WAN_IP_CHECK_TIME);
    writeString (84, DDNS_DOMAIN);
    request->send_P(200, "text/plain", "DDNS adatok: OK");});  

//-------------------------------------------------------------------------------
  server.on("/check_WAN_IP", HTTP_GET, [](AsyncWebServerRequest *request){     
      WEB_delay_ul = millis();
      WEB_action_b = 18;   
    request->send_P (200, "text/plain", "check_WAN_IP: OK");}); 

//-------------------------------------------------------------------------------
  server.on("/NO_IP_update", HTTP_GET, [](AsyncWebServerRequest *request){     
      WEB_delay_ul = millis();
      WEB_action_b = 19;   
    request->send_P (200, "text/plain", "NO_IP_update: OK");});     

//*********************************************************************************
//**** EEPROM PAGE ******************************************************
//-------------------------------------------------------------------------------
  server.on("/proc_json", HTTP_GET, [](AsyncWebServerRequest *request) {     // JSON adatok EEPROM és mem.
      request->send(200, "text/plain", CHIP_INFO_8266_json()); });
//-------------------------------------------------------------------------------
  server.on("/EE_clear", HTTP_GET, [](AsyncWebServerRequest *request){     
      WEB_delay_ul = millis();
      WEB_action_b = 12;   
    request->send_P (200, "text/plain", "INFO adatok: OK");}); 
//-------------------------------------------------------------------------------
  server.on("/ALL_RUN", HTTP_GET, [](AsyncWebServerRequest *request){        //  EEPROM értékek beírása             
      allrun_perc_int = (request->getParam("data")->value()).toInt();
      EE_write_3byte  (240, 241, 242, allrun_perc_int);
      if(S_DEBUG)Serial.print(F("EE_write_3byte (allrun_perc_int): "));    
      if(S_DEBUG)Serial.println(allrun_perc_int);
    request->send_P (200, "text/plain", "INFO adatok: OK");}); 

//-------------------------------------------------------------------------------
  server.on("/X_SZER", HTTP_GET, [](AsyncWebServerRequest *request){        //  EEPROM értékek beírása             
      proc_restart_num = (request->getParam("data")->value()).toInt();
      EE_write_3byte (243, 244, 245, proc_restart_num);
      if(S_DEBUG)Serial.print(F("EE_write_3byte (proc_restart_num): "));    
      if(S_DEBUG)Serial.println(proc_restart_num);
    request->send_P (200, "text/plain", "X_szer: OK");}); 
           
//--------------------------------------------------------------------------------  
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {              // Info + Setup oldalról RESET!
      Serial.println(F("RESET!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 10; 
      request->send_P (200, "text/plain", "RESET!");});


//-- Info & Setup oldal 1.gomb  --------------------------------------------------  
  server.on("/B_1", HTTP_GET, [](AsyncWebServerRequest *request) { 
      if(S_DEBUG)Serial.println(F("Press B-1 buttun!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 1; 
      request->send_P (200, "text/plain", "Button B-1 OK");});
//-- Info & Setup oldal 2.gomb  --------------------------------------------------    
  server.on("/B_2", HTTP_GET, [](AsyncWebServerRequest *request) { 
      if(S_DEBUG)Serial.println(F("Press B-2 buttun!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 2; 
      request->send_P (200, "text/plain", "Button B-2 OK");});
//-- Info & Setup oldal 3.gomb  --------------------------------------------------   
  server.on("/B_3", HTTP_GET, [](AsyncWebServerRequest *request) { 
      if(S_DEBUG)Serial.println(F("Press B-3 buttun!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 3; 
      request->send_P (200, "text/plain", "Button B-3 OK");});
//-- Info & Setup oldal 4.gomb  --------------------------------------------------     
  server.on("/B_4", HTTP_GET, [](AsyncWebServerRequest *request) { 
      if(S_DEBUG)Serial.println(F("Press B-4 buttun!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 4; 
      request->send_P (200, "text/plain", "Button B-4 OK");});
//-- Info & Setup oldal 5.gomb  --------------------------------------------------    
  server.on("/B_5", HTTP_GET, [](AsyncWebServerRequest *request) { 
      if(S_DEBUG)Serial.println(F("Press B-5 buttun!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 5; 
      request->send_P (200, "text/plain", "Button B-5 OK");});
//-- Info & Setup oldal 6.gomb  --------------------------------------------------     
  server.on("/B_6", HTTP_GET, [](AsyncWebServerRequest *request) { 
      if(S_DEBUG)Serial.println(F("Press B-6 buttun!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 6; 
      request->send_P (200, "text/plain", "Button B-6 OK");}); 
//-- Info & Setup oldal 7.gomb  --------------------------------------------------     
  server.on("/B_7", HTTP_GET, [](AsyncWebServerRequest *request) { 
      if(S_DEBUG)Serial.println(F("Press B-7 buttun!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 7; 
      request->send_P (200, "text/plain", "Button B-6 OK");});
//-- Info & Setup oldal 8.gomb  --------------------------------------------------     
  server.on("/B_8", HTTP_GET, [](AsyncWebServerRequest *request) { 
      if(S_DEBUG)Serial.println(F("Press B-8 buttun!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 8; 
      request->send_P (200, "text/plain", "Button B-6 OK");});
//-- Info & Setup oldal 9.gomb  --------------------------------------------------     
  server.on("/B_9", HTTP_GET, [](AsyncWebServerRequest *request) { 
      if(S_DEBUG)Serial.println(F("Press B-9 buttun!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 9; 
      request->send_P (200, "text/plain", "Button B-6 OK");});
//-- Info & Setup oldal 10.gomb  --------------------------------------------------     
  server.on("/B_10", HTTP_GET, [](AsyncWebServerRequest *request) { 
      if(S_DEBUG)Serial.println(F("Press B-10 buttun!")); 
      WEB_delay_ul = millis();
      WEB_action_b = 10; 
      request->send_P (200, "text/plain", "Button B-6 OK");});


//-------------------------------------------------------------------------------- 
  server.onNotFound([](AsyncWebServerRequest *request){      // ha ismeretlen kérés érkezik
    request->send(404, "text/html", NOfile_html);});         // elküldi a 404-es kódot és elküldi a NOfile_html oldalt
          
//-----------------------------------------------------------------------------
  AsyncElegantOTA.setID(project.c_str()); // OTA frissítés azonosító: "project"
  AsyncElegantOTA.begin(&server);         // Start ElegantOTA (server port: 43125)
  ws.onEvent(onEvent);
  server.addHandler(&ws);  
  server.begin();                         // Server indítása

  CHIP_INFO_8266_print();

  Serial.println();
  Serial.println(F(" * * * * * SETUP VÉGE * * * * "));
  Serial.println();
  Serial.println();
  
  }   // SETUP vége
// ***** SETUP vége *********************************************************


//-- LOOP -------------------------------------------------------------------
void loop() { 
  ftpSrv.handleFTP();
  MDNS.update(); 

  if (S_DEBUG != S_deb_valt) {
    if (S_DEBUG) {
      Serial.println(F("SERIAL vonalon, az információk megjelennek."));
      Serial.println(F("----- ********** -----"));
      }
    else {
      Serial.println(F("SERIAL vonalon, nem jelennek meg az információk!"));
      Serial.println(F("----- ********** -----"));
      }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println();
      Serial.println(F("WiFi ERROR!"));
      Serial.println();
      }
    else {
      Serial.println();
      Serial.println(F("WiFi OK!"));
      Serial.print("Connected  : "); Serial.println(WiFi.SSID());
      Serial.print("WiFi RSSI  : "); Serial.println(WiFi.RSSI());
      Serial.print("Local-IP   : "); Serial.println(WiFi.localIP());
      Serial.print("TCP-PORT   : "); Serial.println(TCP_PORT);
      Serial.print("Hostname   : "); Serial.println(WiFi.getHostname());
      Serial.print("WiFi MAC   : "); Serial.println(WiFi.macAddress());
      //wl_status_t w_status = WiFi.status();
      Serial.print("WiFi status: "); Serial.println(wl_status_to_string(WiFi.status()));


      Serial.print("gateway-IP : "); Serial.println(WiFi.gatewayIP());
      Serial.print("wifi-Mode  : "); Serial.println(WiFi.getMode());
      Serial.print("SleepMode  : "); Serial.println(WiFi.getSleepMode());
      Serial.print("AutoConnect: "); Serial.println(WiFi.getAutoReconnect());
      Serial.println();
      }
      S_deb_valt = S_DEBUG;
    }
// ****************************************************************

// **** WAN IP ellenőrzése beállított időközönként !  *************************
    if ((DDNS_ON == 1)&(millis() - previousMillis >= WAN_IP_CHECK_TIME*1000)) {
      previousMillis = millis();
      WAN_IP_CHECK_easyddns(0);}

// Soros vonalon kapott üzenetet hozzáírja az aktuális SD card log fájlhoz
// PRÓBA!!! 
  if (Serial.available()) {
    String incoming = Serial.readString();
    
    Serial.print(F("Serial text: "));
    Serial.println(incoming);
    
    File dataFile = LittleFS.open("/serial/incomming_serial.txt", "w");
  if (dataFile) {
    dataFile.println(incoming);
    dataFile.close();
    Serial.println(F("incomming_COM.txt write OK"));}
  else {
    Serial.println(F("incomming_COM.txt write ERROR"));}}
    

// ****************************************************************   
/* **** WEB-ről érkező parancsok késleltetéssel!  *****************
   // WEB_action_b = 0  --> nem csinál semmit 
   // WEB_action_b = 1-10  --> Setup oldalon gombok 1-10 
   // WEB_action_b = 10  --> reset
   // WEB_action_b = 12  --> EEPROM Clear
   // WEB_action_b = 13  --> RESTART + CONFIG, webről, vagy hosszú gombnyomás
   // WEB_action_b = 14  --> SD DIR listázása
   // WEB_action_b = 15  --> SD card újracsatlakoztatás
   // WEB_action_b = 16  --> SD card újracsatlakoztatás eredménye kis késéssel
   // WEB_action_b = 17  --> GET tempek hő és pára lekérése másik eszközről
   // WEB_action_b = 18  --> check_WAN_IP
   // WEB_action_b = 19  --> NO_IP_update
*/

// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 1) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.print(F("Button: 1"));
    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 2) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.print(F("Button: 2"));
    WEB_action_b = 0;
    }
  // --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 3) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.print(F("Button: 3"));
    proc_restart = 1;
    WEB_action_b = 0;
    }
  // --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 4) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.print(F("Button: 4"));
    proc_restart = 10;
    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 5) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.print(F("Button: 5"));

    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 6) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.print(F("Button: 6"));

    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 7) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.print(F("Button: 7"));

    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 8) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.print(F("Button: 8"));

    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 9) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.print(F("Button: 9"));

    WEB_action_b = 0;
    }
                           

// --- 10  --> (reset és WifiManager save + reset)---------------
  if ((WEB_action_b == 10)&(millis()-WEB_delay_ul > 500)){ 
      EE_write_3byte  (240, 241, 242, allrun_perc_int);
      if(S_DEBUG)Serial.print(F("EE_write_3byte (allrun_perc_int): "));
      if(S_DEBUG)Serial.println(allrun_perc_int);
      WEB_action_b = 0;
      ESP.restart();}   
      
// --- 12  --> (EEPROM Clear)---------------
  if ((WEB_action_b == 12)&(millis()-WEB_delay_ul > 500)){ 
      EEPROM_clear ();                    
      WEB_action_b = 0;}                

  // --- 14  --> Soros monitor SD/dir/fájl listázás -------
  if ((WEB_action_b == 14) & (millis() - WEB_delay_ul > 500)) {
    // File root;
    // root = LittleFS.open("/");
    // printDirectory(root, 0);
    // WEB_action_b = 0;
    }

// --- 17  --> GET tempek hő és pára lekérése másik eszközről -------
  if ((WEB_action_b == 17)&(millis()-WEB_delay_ul > 500)){ 
      tempek(mi);     // mi=0-semmi, mi=10-összes, mi=2-kert, mi=3-konyha... stb.,
      WEB_action_b = 0;}

// --- 18  --> GET tempek hő és pára lekérése másik eszközről -------
  if ((WEB_action_b == 18)&(millis()-WEB_delay_ul > 500)){ 
      WAN_new_IP.fromString("100.100.100.100");
      WAN_IP_CHECK_easyddns(1);
      previousMillis = 0;      
      WEB_action_b = 0;}

// --- 19  --> GET tempek hő és pára lekérése másik eszközről -------
  if ((WEB_action_b == 19)&(millis()-WEB_delay_ul > 500)){ 
      NO_IP_update();
      WEB_action_b = 0;}
      
// --- 20 NTP sync + 300ms késleltetéssel ------------------------------
  if ((WEB_action_b == 11) & (WEB_delay_ul + 300 < millis())){
      if (S_DEBUG)
      Serial.println(F("11. NTP sync +  300ms késleltetéssel"));
      NTP_LASTSYNC_STRING = "";
      NTP_LASTSYNC_STRING = DATE_STRING;
      NTP_LASTSYNC_STRING += " - ";
      NTP_LASTSYNC_STRING += TIME_STRING;
      WEB_delay_ul = millis();
      WEB_action_b = 0;
  }


// **************************************************************** 

// ****** időkezelés **********************************              
  // ------------------ idő -------------------------------------------                 
  if (time(&now) != prevTime) {    // ha az idő megváltozott, mp-enként
    showTime();                   // showTime időadatok
    // Serial.print(F("Cycle/mp: "));
    // Serial.println(cycle);
    // cycle = 0;
    if (wifi_multi.run(connectTimeoutMs) == WL_CONNECTED) {
      ssid_changed = WiFi.SSID();
      }
    else {
      if (S_DEBUG) Serial.println("WiFi not connected!");
      }
    if (ssid_changed != ssid_changed_old) {
      if (S_DEBUG) Serial.print("WiFi connected: ");
      if (S_DEBUG) Serial.print(WiFi.SSID());
      if (S_DEBUG) Serial.print("  ");
      if (S_DEBUG) Serial.print(WiFi.RSSI());
      if (S_DEBUG) Serial.println(" dBm");
      ssid_changed_old = ssid_changed;
      }
    if (blue_led == 1) {
      led_state = !led_state;             // LED villogtatás
      if (led_state) led.on();
      else led.off();
      }
    if (proc_restart == 10) {
      proc_restart = 0;
      ws.textAll("03");
      }
    prevTime = time_t(now);
    }

/*      cycle++;
//10:35:57.488 -> Cycle/mp: 6018
//10:35:57.488 -> * * * AKT_IDO_CHAR_10 [4] (percenként)* * *
//10:35:57.488 -> 2023-01-13 - 10:36:00 - 1673602560 - péntek
//10:35:57.488 -> allrun_perc_int: 3852
//10:35:57.488 -> 10:36:00
//10:35:58.467 -> Cycle/mp: 5834
//10:35:59.482 -> Cycle/mp: 6000
*/
   
  
//--- Ha a dátum változik ---------------------------------
  if (DATE_STRING != DATE_STRING_old) {                    // ha változik a dátum
    ws.textAll("08" + DATE_STRING + "*" + DAYNAME);
    setenv("TZ", "GMT0", 1);   // GMT
    tzset();
    tm x_time;                    // időstruktúra
    x_time.tm_year = N_YEAR - 1900;
    x_time.tm_mon = N_MONTH - 1;
    x_time.tm_mday = N_DAY;
    x_time.tm_hour = 12;
    x_time.tm_min = 0;
    x_time.tm_sec = 0;
    mai_epo = mktime(&x_time);
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);    // budapest
    tzset();
    mai_filename = "/temp_log/" + String(mai_epo) + ".txt";
    if (S_DEBUG)Serial.print(F("mai_filename: "));
    if (S_DEBUG)Serial.println(mai_filename);
    if ((make_log == 1) & (N_EPO > 1577833200)) {  // ha van loggolás, és 2020.01.01. után vagyunk...
      new_logfile();
      }                         // ... hogy rosz szinkron (1970.01.01)esetén ne zavarja meg                    
    DATE_STRING_old = DATE_STRING;
    }


    // *** minden 5. mp-ben, char=5 -> byte=53 ************************
    if (mp_x_dik == 53){      
      //if(S_DEBUG)Serial.println(F("* * xx5. mp * *"));
      mp_x_dik = 100;}

    // *** minden 10. mp-b0en, char=0 -> byte=48 ***********************
    if (mp_x_dik == 48) {
      //if (S_DEBUG)Serial.println(F("* * xx0. mp * *"));
      WS_send_10mp();
      mp_x_dik = 100;
      }

//******* percenként ******************************************** 
   // AKT_IDO_CHAR_10: [0][1][2][3][4][5][6][7][8][9]  
   //             pl.:  1  4  :  2  5  :  3  2  -  -
   // char.  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, .. 0, 1, ....
   // byte  48,49,50,51,52,53,54,55,56,57,    (mp_x_dik)
    if (AKT_IDO_CHAR_10[4] != AKT_IDO_CHAR_10_OLD[4]) {  // percenként - 5. char. [4] 1percenként-enként igaz    
      allrun_perc_int++;                                 // növeli 1-el futásidő számlálót               
      /*       if (S_DEBUG) {
              Serial.println(F("* * * AKT_IDO_CHAR_10 [4] (percenként)* * *"));
              Serial.print(DATE_STRING);
              Serial.print(F(" - "));
              Serial.print(TIME_STRING);
              Serial.print(F(" - "));
              Serial.print(EPO_STRING);
              Serial.print(F(" - "));
              Serial.println(DAYNAME);
              Serial.print(F("allrun_perc_int: "));
              Serial.println(allrun_perc_int);
              Serial.println();
              } */
      AKT_IDO_CHAR_10_OLD[4] = AKT_IDO_CHAR_10[4];
      }                   // pl.:  1  4  :  2  5  :  3  2  -  -

//-------------------------------------------------------------------------------
//----- minden óra, 00, 09, 19, 29, 39, 49, 54, és 59. perc :50mp-kor ---------------
    if (((N_MIN == 9) || (N_MIN == 19) || (N_MIN == 29) || (N_MIN == 39) ||
      (N_MIN == 49) || (N_MIN == 54) || (N_MIN == 59)) & (N_SEC == 50)) {
      log_egyszer = 1;
      }

//******* loggolás, 10 percenként ********************************************    
// https://techtutorialsx.com/2019/06/13/esp8266-spiffs-appending-content-to-file/ 
//----- minden óra, 00, 10, 20, 30, 40, 50 percnél, egyszer --------------------------
   if (((N_MIN == 0) || (N_MIN == 10) || (N_MIN == 20) || (N_MIN == 30) || (N_MIN == 40)
     || (N_MIN == 50)) & (log_egyszer == 1) & (N_EPO > 1577833200)) {          //2020.01.01. után
     if (make_log == 1) {           // Loggolás SD kártyára        
       File fileToAppend = LittleFS.open(mai_filename, "a");
       if (!fileToAppend) {
         if (S_DEBUG) { Serial.println(F("Nem sikerült megnyitni a fájlt, fozzáíráshoz!")); }
         }
       if (fileToAppend.print(TIME_STRING)) {         // "09:00:00"

         Serial.println(F("Tartalom hozzáírása a fájlhoz."));
         }
       else {
         if (S_DEBUG) { Serial.println(F("Tartalom hozzáírása a fájlhoz NEM SIKERÜLT!")); }
         }
       fileToAppend.close();
       }
     log_egyszer = 0;
     }

//---- minden nap 23:55:00-kor, egyszer, napi átlag, és összegzés -----------------------
   if ((N_HOUR == 23) & (N_MIN == 55) & (N_SEC == 0) & (log_egyszer == 1)) {
     if (make_log == 1) {           // Loggolás SD kártyára 
       File fileToAppend = LittleFS.open(mai_filename, "a");
       if (!fileToAppend) {
         if (S_DEBUG) { Serial.println(F("Nem sikerült megnyitni a fájlt, fozzáíráshoz!")); }
         }
       fileToAppend.println(F("------------------------------"));
       fileToAppend.println(F("---- napi min, max, átlag ----"));
       fileToAppend.println(F("--------- hőfok (°C) ---------"));
       fileToAppend.close();

       File Append_year = LittleFS.open(year_sum_file, "a");   // létrehozza ...
       if (!Append_year) {
         if (S_DEBUG)Serial.println(F("Nem sikerült megnyitni a fájlt, éves report-hoz!"));
         }
       if (Append_year.print(MONTH_ARRY[DATE_STRING.substring(5, 7).toInt()])) { // 2023-01-22 ==> "jan."
         Append_year.print(DATE_STRING.substring(8));                        // 2023-01-22 ==> "jan.22" 
         //       |    Hőmérséklet  °C   |                                              
         //dátum  |  min  |  max | átlag |
         //------------------------------
         //01-15,  -22.2,  -22.3,   -22.5,

         if (S_DEBUG)Serial.println(F("Napi átlagok hozzáírása az éves fájlhoz"));
         }
       else {
         if (S_DEBUG)Serial.println(F("Napi átlagok hozzáírása a fájlhoz NEM SIKERÜLT!"));
         }
       Append_year.close();
       }
     log_egyszer = 0;
     }

// ***** futásidő számlálót EEPROM-ba ********************************************************************
// ** minden nap 11:01:01, és 23:01:01-kor, csak egyszer, elmenti a teljes futásidő számlálót EEPROM-ba **
   if (((N_HOUR == 11) || (N_HOUR == 23)) & (N_MIN == 1) & (N_SEC == 1) & (run_save_egyszer == 0)) {
     EE_write_3byte(240, 241, 242, allrun_perc_int);        // teljes futásidő, percben      
     if (S_DEBUG)Serial.println(F("*   *  *  *  *  *  *  *  *  *"));
     if (S_DEBUG)Serial.print(F("futásidő mentés:  "));
     if (S_DEBUG)Serial.println(DATE_STRING + " - " + TIME_STRING);
     if (S_DEBUG)Serial.print(F("EE_write_3byte (allrun_perc_int): "));
     if (S_DEBUG)Serial.println(allrun_perc_int);
     if (S_DEBUG)Serial.println(F("*   *  *  *  *  *  *  *  *  *"));
     millis64();                 // naponta kétszer meghívja a függvényt (futásidő, ms-okban)
     run_save_egyszer = 1;
     }

// ** minden nap 11:31:01, és 23:31:01-kor, visszaállítja "run_save_egyszer" bytot "0"-ra ******************
   if (((N_HOUR == 11) || (N_HOUR == 23)) & (N_MIN == 31) & (N_SEC == 1)) {
     run_save_egyszer = 0;
     }
// ********************************************************************************************************     
//------------------------------------     
    } // LOOP vége

#include "F_system.h"
#include "F_chip_info.h"
#include "F_SD_filesystem.h"
   
// ***** LOOP vége **********************************************************

// http://ip1.dynupdate.no-ip.com/    // NO-IP API-ja        
// http://ifconfig.me/ip              // easyDDNS ezt használja     
// http://api.ipify.org/              // ez is működik
// END -----    