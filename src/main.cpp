
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <SimpleFTPServer.h>  // FTP server 
#include <AsyncElegantOTA.h>  // Elegant OTA 
#include <ESP_DoubleResetDetector.h>
#include <FS.h>               // Fájlrendszer kezelés miatt, favicon.ico fájlként feltöltve
#include <LittleFS.h>         // Fájlrendszer kezelés
#include <ArduinoJson.h>
#include <Preferences.h>
#include <time.h>             // time() ctime()
#include <coredecls.h>        // settimeofday_cb() visszahívás ellenőrzéshez (NTP lekérdezés)     

#include "variable.h"          // saját külön fájl a változóknak 
#include "NOfile.h"            // HTML oldal ismeretlen kéréshez
#include "decleration.h"

#include "Led.h"   // próba könyvtár
#define LED_PIN 2  // Use the built-in LED
Led led(LED_PIN);  // led objektum, LED_PIN-t OUTPUT-ra állítja

#define DRD_TIMEOUT  10
#define DRD_ADDRESS   0
DoubleResetDetector* drd;
Preferences pref;
WiFiClient client;  // or WiFiClientSecure for HTTPS
FtpServer ftpSrv;   // set #define FTP_DEBUG in ESP8266FtpServer.h to see ...
AsyncWebServer server(TCP_PORT);       // TCP portszám: 80 (alap)
AsyncWebSocket ws("/ws");

ADC_MODE(ADC_VCC);
      
// ************************************************************************** 
//-- SETUP ------------------------------------------------------------------
void setup() {                  
  Serial.begin(115200);
  delay(10);
  pref.begin("my-app", false);
  //drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-
  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
  if (drd->detectDoubleReset()) {
    drd_status = true;
    pref.putBool("wifi_pref_mode", 0); // AP mód - 0
    }
  else {
    drd_status = false;
    }
  //drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-

  delay(2000);
  PROJECT_INFO();

//---- mentett adatok kiolvasása--------------------------
  
  if (drd_status) Serial.println(F("Double-Reset észlelve, mode->AP"));
  else Serial.println(F("Nincs érzékelve Double-Reset"));  

  proc_restart_num = pref.getUInt("counter", 0);
  allrun_perc_int = pref.getUInt("allrun_perc", 0);
  S_DEBUG = pref.getBool("S_DEBUG");
  make_log = pref.getBool("make_log");
  blue_led =  pref.getBool("blue_led");
  wifi_pref_mode = pref.getBool("wifi_pref_mode");
  ssid = pref.getString("ssid");
  pass = pref.getString("pw");

  proc_restart_num++;

  Serial.printf("Current counter value: %u\n", proc_restart_num);
  Serial.printf("allrun_perc_int value: %u\n", allrun_perc_int);
  Serial.printf("S_DEBUG  value       : %u\n", S_DEBUG);
  Serial.printf("make_log value       : %u\n", make_log);
  Serial.printf("blue_led value       : %u\n", blue_led);    

  pref.putUInt("counter", proc_restart_num);

  pref.end();
//----------------------------------------------------------

  WiFi.setHostname(project.c_str());
  
  Serial.println("Connecting ...");
// ***** WiFi csatlakozás ***********************************
  if (initWiFi()) Serial.println(F("Setup / if (initWiFi())= TRUE (1), ===> Client mód!"));
  else Serial.println(F("Setup / if (initWiFi())= FALSE(0), ===> AP mód!"));

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

    if (LittleFS.begin()) {
      ftpSrv.setCallback(_callback);
      ftpSrv.setTransferCallback(_transferCallback);
      filesystem = "LittleFS";
      Serial.println(F("LittleFS fájlrendszer elindítva!"));
      Serial.print(" * * * FTP username:\t ");
      Serial.println(F("esp8266"));            // username for ftp.
      Serial.print(" * * * FTP password:\t");
      Serial.println(F("8266"));               // password for ftp.
      ftpSrv.begin("esp8266", "8266");         // (default 21, 50009 for PASV)  
      }

    configTime(MY_TZ, NTP_SERVER_1, NTP_SERVER_2); //Mytz - Budapest
    settimeofday_cb(NTP_time_is_set);              // opcionális: visszahívás, ha elküldték az időt

  if (WIFI_STA_or_AP == 1) {                       // wifi hálózat, 1->STA mód(client) 
//**** automatikusan teljesülő kérések ******************************************  
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  }
  else{                                            // wifi hálózat, 0->AP mód
//**** automatikusan teljesülő kérések ******************************************  
    server.serveStatic("/", LittleFS, "/").setDefaultFile("wifi.html");  
  }
//--------------------------------------------------------------------------------
    
//*******************************************************************************    
    server.on("/index_json", HTTP_GET, [](AsyncWebServerRequest* request) {        // JSON adatok Főoldalra
      AsyncResponseStream* response = request->beginResponseStream("application/json");
      DynamicJsonDocument IR(512);
      JsonArray index = IR.createNestedArray("index");
      index.add(TIME_STRING);              // [0]  
      index.add(DATE_STRING);              // [1]
      index.add(DAYNAME);                  // [2]
      index.add(ESP.getChipId());          // [3]
      index.add(proc_restart);             // [4]
      serializeJson(IR, *response);
      request->send(response);});

    //**** Info + Setup oldal ********************************************************
    server.on("/setup_json", HTTP_GET, [](AsyncWebServerRequest* request) {
      AsyncResponseStream* response = request->beginResponseStream("application/json");
      DynamicJsonDocument adatok(384);
      JsonArray setup = adatok.createNestedArray("setup");      
      setup.add(project);                    // [0] project (a file name, csak main.cpp!)
      setup.add(comp_idopont);               // [1] fordítás időpontja
      setup.add(ARDUINO_BOARD);              // [2] modul tipus
      setup.add(millis64());                 // [3] futásidő
      setup.add(allrun_perc_int);            // [4] teljes futásidő
      setup.add(make_log);                   // [5] hogy legyen soros monitor ellenőrzés
      setup.add(S_DEBUG);                    // [6] készítsen-e log fájlt
      setup.add(blue_led);                   // [7] villogjon-e a beépített kék led
      serializeJson(adatok, *response);
      request->send(response);
      });
//--------------------------------------------------------------------------------
    server.on("/setup_save", HTTP_GET, [](AsyncWebServerRequest* request) {        // Info + Setup adatanak mentése             
      make_log = (request->getParam("make_log")->value()).toInt();
      S_DEBUG = (request->getParam("s_debug")->value()).toInt();
      blue_led = (request->getParam("blue_led")->value()).toInt();
      pref.begin("my-app", false);
      pref.putBool("S_DEBUG", S_DEBUG);
      pref.putBool("blue_led", blue_led);
      pref.putBool("make_log", make_log);
      pref.end();
      request->send_P(200, "text/plain", "setup_SAVE_OK");});

//**** WIFI setup oldal **********************************************************
//--------------------------------------------------------------------------------
  server.on("/wifi_json", HTTP_GET, [](AsyncWebServerRequest* request) {           // WIFI-Setup oldal JSON adatok küldése
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    DynamicJsonDocument WIFI_json(512);

    JsonArray wifi_alap = WIFI_json.createNestedArray("wifi_alap");
    wifi_alap.add(wifi_mode_to_string(WiFi.getMode()));  // [0] 
  if (WIFI_STA_or_AP == 1) {                      // wifi hálózat, 1->STA mód(client) 
    wifi_alap.add(WiFi.SSID());                          // [1] 
    wifi_alap.add(WiFi.localIP().toString());            // [2]
  }
  else{                                            // wifi hálózat, 0->AP mód
    wifi_alap.add(AP_ssid);                              // [1] 
    wifi_alap.add(WiFi.softAPIP());                      // [2]   
  }
    wifi_alap.add(mdns_name);                            // [3]
    wifi_alap.add(TCP_PORT);                             // [4]
    wifi_alap.add(WiFi.RSSI());                          // [5]
    wifi_alap.add(WiFi.macAddress());                    // [6]
    wifi_alap.add(wifi_status_to_string(WiFi.status())); // [7]
    wifi_alap.add(AP_ssid);                              // [8]

    JsonArray wifi_scan = WIFI_json.createNestedArray("wifi_scan");
    wifi_scan.add(WIFI_drb_int);                            // [0] talált wifi db.
    for (int i = 0; i < WIFI_drb_int; i++) {
      wifi_scan.add(RSSI_sort_int[i]);                      // [1]-[3]-[5]...stb.
      wifi_scan.add(SSID_sort_string[i]);                   // [2]-[4]-[6]...stb.
      }

    serializeJson(WIFI_json, *response);
    request->send(response);});

//-- wifi oldal reset mentés nélkül  ----------------------------------
  server.on("/wifi_reset", HTTP_GET, [](AsyncWebServerRequest* request) {
    WEB_action_b = 11;      //Reset loop-ban
    WEB_delay_ul = millis();
    request->send_P(200, "text/plain", "RESET!");}); 

//--------------------------------------------------------------------------------
  server.on("/wifi_rescan", HTTP_GET, [](AsyncWebServerRequest* request) {
    WEB_action_b = 12;      // WIFI SCAN loop-ban
    WEB_delay_ul = millis();
    request->send_P(200, "text/plain", "WIFI_rescan OK!");});


//-- wifi oldal SAVE data és reset -- <form action="/" method="POST"> HTML method---- 
  server.on("/W_data", HTTP_POST, [](AsyncWebServerRequest* request) {      // SSID, pw adatok mentése
    WIFI_STA_or_AP = (request->getParam("wifi_mode")->value()).toInt(); // wifi hálózat 0->AP-mód, 1->STA mód(client) 
    pref.begin("my-app", false);
    if (WIFI_STA_or_AP == 1) {                        // wifi hálózat, 1->STA mód(client)     
      ssid = (request->getParam("ssid")->value());
      pass = (request->getParam("pasw")->value());
      pref.putString("ssid", ssid);
      pref.putString("pw", pass);
      pref.putBool("wifi_pref_mode", WIFI_STA_or_AP); // STA mód (Client)
      }
    if (WIFI_STA_or_AP == 0) {                        // wifi hálózat, 0->AP-mód  
      pref.putBool("wifi_pref_mode", WIFI_STA_or_AP); // AP mód
      }
    WEB_action_b = 11;                                // Reset loop-ban
    WEB_delay_ul = millis();
    pref.end();
    request->send(200, "text/plain", "Done. ESP will restart, connect to : " + ssid);});


//*********************************************************************************
//**** NTP setup oldal ************************************************************
//---------------------------------------------------------------------------------
  server.on("/NTP_json", HTTP_GET, [](AsyncWebServerRequest *request) {         //  NTP setup oldal JSON adatok küldése
      AsyncResponseStream *response = request->beginResponseStream("application/json"); 
      DynamicJsonDocument NTP_adatok(256);
      JsonArray ntp = NTP_adatok.createNestedArray("ntp");                  
      ntp.add(TIME_STRING);              // [0] Pontos idő
      ntp.add(DATE_STRING);              // [1] Dátum
      ntp.add(DAYNAME);                  // [2] nap neve
      ntp.add(EPO_STRING);               // [3] epoch time
      ntp.add(year_x_day);               // [4] az év x. napja
      ntp.add(winter_sommer_time);       // [5] téli vagy nyári időszámytás
      ntp.add(NTP_LASTSYNC_STRING);      // [6] az utolsó NTP szinkronizáció ideje
      ntp.add(mai_epo);                  // [7] mai nap epoch time-ja 12:00-kor
      serializeJson(NTP_adatok, *response);
      request->send(response);});

//*********************************************************************************
//**** DDNS setup oldal ***********************************************************
  server.on("/ddns_json", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    DynamicJsonDocument DDNS_adatok(512);
    JsonArray ddns = DDNS_adatok.createNestedArray("ddns");
    ddns.add(TIME_STRING);          // [0]  
    ddns.add(DATE_STRING);          // [1]
    ddns.add(DAYNAME);              // [2]
    serializeJson(DDNS_adatok, *response);
    request->send(response);});

//*********************************************************************************
  server.on("/proc_json", HTTP_GET, [](AsyncWebServerRequest* request) {     // JSON adatok EEPROM és mem.
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument PROC_json(512);
    JsonArray proc = PROC_json.createNestedArray("proc");
    proc.add(ESP.getVcc() * 1.12 / 1023.0);         // [0] Proc Vcc  (D1 mininél)
    proc.add(ESP.getChipId());                  // [1] ESP8266 chip ID 
    proc.add(ESP.getCoreVersion());             // [2] alapverzió
    proc.add(ESP.getSdkVersion());              // [3] SDK-verzió
    proc.add(ESP.getCpuFreqMHz());              // [4] CPU frekvenciája MHz-ben
    //-------------------------------------
    proc.add(ESP.getSketchSize());              // [5] SketchSize, byte
    proc.add(ESP.getFreeSketchSpace());         // [6] FreeSketchSpace, byte
    //-------------------------------------
    proc.add(ESP.getFlashChipId());             // [7] flash chip azonosító
    proc.add(ESP.getFlashChipRealSize());       // [8] Flash chip size
    proc.add(ESP.getFlashChipSpeed() / 1000000);  // [9] Flash chip frekvenciája MHz-ben
    //-------------------------------------
    proc.add(ESP.getFreeHeap());                // [10] szabad kupac méreté
    proc.add(ESP.getMaxFreeBlockSize());        // [11] legnagyobb összefüggő szabad RAM
    proc.add(ESP.getHeapFragmentation());       // [12] HEAP töredezettség %-ban
    //-------------------------------------
    proc.add(proc_restart_num);                 // [13] Proc. újraindult x-szer     
    proc.add(ESP.getResetReason());             // [14] utolsó visszaállítás oka 
    //-------------------------------------
    FSInfo fs_info;
    LittleFS.info(fs_info);
    used_FS_percent = ((float(fs_info.usedBytes) * 100.0) / float(fs_info.totalBytes));
    proc.add(filesystem);                       // [15] Filesysten tipusa
    proc.add(fs_info.totalBytes);               // [16] Total space (byte)
    proc.add(fs_info.usedBytes);                // [17] Total space used (byte)
    proc.add(used_FS_percent);                  // [18] használatban lévő tárhely 
    //-------------------------------------            
    proc.add(allrun_perc_int);                  // [19] teljes futásidő     
    //-------------------------------------
    serializeJson(PROC_json, *response);
    request->send(response);});
//-------------------------------------------------------------------------------
  server.on("/EE_clear", HTTP_GET, [](AsyncWebServerRequest *request){     
      WEB_delay_ul = millis();
      WEB_action_b = 14;        //EE_clear
    request->send_P (200, "text/plain", "INFO adatok: OK");}); 
//-------------------------------------------------------------------------------
  server.on("/ALL_RUN", HTTP_GET, [](AsyncWebServerRequest* request) {        //  EEPROM értékek beírása             
    allrun_perc_int = (request->getParam("data")->value()).toInt();
    pref.begin("my-app", false);
    pref.putUInt("allrun_perc", allrun_perc_int);
    pref.end();
    if (S_DEBUG)Serial.print(F("pref.putUInt(allrun_perc_int): "));
    if (S_DEBUG)Serial.println(allrun_perc_int);
    request->send_P(200, "text/plain", "INFO adatok: OK");});

//-------------------------------------------------------------------------------
  server.on("/X_SZER", HTTP_GET, [](AsyncWebServerRequest* request) {        //  EEPROM értékek beírása             
    proc_restart_num = (request->getParam("data")->value()).toInt();
    pref.begin("my-app", false);
    pref.putUInt("counter", proc_restart_num);
    pref.end();
    if (S_DEBUG)Serial.print(F("putUInt(proc_restart_num): "));
    if (S_DEBUG)Serial.println(proc_restart_num);
    request->send_P(200, "text/plain", "X_szer: OK");});


//-- Info & Setup oldal 1.gomb  --------------------------------------------------  
  server.on("/B_1", HTTP_GET, [](AsyncWebServerRequest *request) { 
      WEB_delay_ul = millis();
      WEB_action_b = 1; 
      request->send_P (200, "text/plain", "Button B-1 OK");});
//-- Info & Setup oldal 2.gomb  --------------------------------------------------    
  server.on("/B_2", HTTP_GET, [](AsyncWebServerRequest *request) { 
      WEB_delay_ul = millis();
      WEB_action_b = 2; 
      request->send_P (200, "text/plain", "Button B-2 OK");});
//-- Info & Setup oldal 3.gomb  --------------------------------------------------   
  server.on("/B_3", HTTP_GET, [](AsyncWebServerRequest *request) { 
      WEB_delay_ul = millis();
      WEB_action_b = 3; 
      request->send_P (200, "text/plain", "Button B-3 OK");});
//-- Info & Setup oldal 4.gomb  --------------------------------------------------     
  server.on("/B_4", HTTP_GET, [](AsyncWebServerRequest *request) { 
      WEB_delay_ul = millis();
      WEB_action_b = 4; 
      request->send_P (200, "text/plain", "Button B-4 OK");});
//-- Info & Setup oldal 5.gomb  --------------------------------------------------    
  server.on("/B_5", HTTP_GET, [](AsyncWebServerRequest *request) { 
      WEB_delay_ul = millis();
      WEB_action_b = 5; 
      request->send_P (200, "text/plain", "Button B-5 OK");});
//-- Info & Setup oldal 6.gomb  --------------------------------------------------     
  server.on("/B_6", HTTP_GET, [](AsyncWebServerRequest *request) { 
      WEB_delay_ul = millis();
      WEB_action_b = 6; 
      request->send_P (200, "text/plain", "Button B-6 OK");}); 
//-- Info & Setup oldal 7.gomb  --------------------------------------------------     
  server.on("/B_7", HTTP_GET, [](AsyncWebServerRequest *request) { 
      WEB_delay_ul = millis();
      WEB_action_b = 7; 
      request->send_P (200, "text/plain", "Button B-7 OK");});
//-- Info & Setup oldal 8.gomb  --------------------------------------------------     
  server.on("/B_8", HTTP_GET, [](AsyncWebServerRequest *request) { 
      WEB_delay_ul = millis();
      WEB_action_b = 8; 
      request->send_P (200, "text/plain", "Button B-8 OK");});
//-- Info & Setup oldal 9.gomb  --------------------------------------------------     
  server.on("/B_9", HTTP_GET, [](AsyncWebServerRequest *request) { 
      WEB_delay_ul = millis();
      WEB_action_b = 9; 
      request->send_P (200, "text/plain", "Button B-9 OK");});
//-- Info & Setup oldal 10.gomb  --------------------------------------------------     
  server.on("/B_10", HTTP_GET, [](AsyncWebServerRequest *request) { 
      WEB_delay_ul = millis();
      WEB_action_b = 10; 
      request->send_P (200, "text/plain", "Button B-10 OK");});


//-------------------------------------------------------------------------------- 
  server.onNotFound([](AsyncWebServerRequest *request){      // ha ismeretlen kérés érkezik
    request->send(404, "text/html", NOfile_html);});         // elküldi a 404-es kódot és elküldi a NOfile_html oldalt
          
//-----------------------------------------------------------------------------
  AsyncElegantOTA.setID(project.c_str()); // OTA frissítés azonosító: "project"
  AsyncElegantOTA.begin(&server);         // Start ElegantOTA (server port: 43125)
  ws.onEvent(onEvent);
  server.addHandler(&ws);  
  server.begin();                         // Server indítása

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
  drd->loop(); 

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
      Serial.println(F("WiFi ERROR, vagy AP mód!"));
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
      Serial.print("WiFi status: "); Serial.println(wifi_status_to_string(WiFi.status()));

      Serial.print("gateway-IP : "); Serial.println(WiFi.gatewayIP());
      Serial.print("wifi-Mode  : "); Serial.println(wifi_mode_to_string(WiFi.getMode()));
      // WiFi.getMode() =  // 0=WIFI_OFF,  1=WIFI_STA,  2=WIFI_AP,  3=WIFI_AP_STA

      Serial.print("SleepMode  : "); Serial.println(WiFi.getSleepMode());
      Serial.print("AutoConnect: "); Serial.println(WiFi.getAutoReconnect());
      Serial.println();
      }
      S_deb_valt = S_DEBUG;
    }
// ****************************************************************

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
    
// ***Web Socket beérkező üzenetek *******************************
  if ((W_S_rec_int != 500) & (W_S_rec_int != 0)) {
    if (W_S_rec_int == 10) proc_restart = 10;    // proc restart törlése
    W_S_rec_int = 500;
    }

/* **** WEB-ről érkező parancsok késleltetéssel!  *****************
   // WEB_action_b = 0  --> nem csinál semmit 
   // WEB_action_b = 1-10  --> Setup oldalon gombok 1-10 
   // WEB_action_b = 11  --> ESP reset
   // WEB_action_b = 12  --> WiFi Scan
   // WEB_action_b = 13  --> NTP sync -> last_sinc_string
   // WEB_action_b = 14  --> EEPROM CLEAR -semmi- delay send WS
   // WEB_action_b = 15  -->
*/
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 1) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 1"));
    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 2) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 2"));
    WEB_action_b = 0;
    }
  // --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 3) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 3"));
    WEB_action_b = 0;
    }
  // --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 4) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 4"));
    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 5) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 5"));

    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 6) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 6"));

    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 7) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 7"));

    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 8) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 8"));

    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 9) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 9"));

    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 10) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 10"));

    WEB_action_b = 0;
    }                            
      
// --- 11  --> ESP Restart ---------------
  if ((WEB_action_b == 11) & (millis() - WEB_delay_ul > 300)) {
    pref.begin("my-app", false);
    pref.putUInt("allrun_perc", allrun_perc_int);
    pref.end();
    if (S_DEBUG)Serial.println(F("pref.putUInt(allrun_perc, allrun_perc_int)"));
    if (S_DEBUG)Serial.println(F("RESTART"));
    WEB_action_b = 0;
    ESP.restart();
    }

  // --- 12  --> WIFI-scan  -------
  if ((WEB_action_b == 12) & (millis() - WEB_delay_ul > 300)) {
    if (S_DEBUG)Serial.println(F("- - WIFI RESCAN! - -"));
    WIFI_SCAN();
    DynamicJsonDocument adatok(512);
    JsonArray wifi_scan = adatok.createNestedArray("wifi_scan");
    wifi_scan.add(WIFI_drb_int);                            // [0] talált wifi db.
    for (int i = 0; i < WIFI_drb_int; i++) {
      wifi_scan.add(RSSI_sort_int[i]);                      // [1]-[3]-[5]...stb.
      wifi_scan.add(SSID_sort_string[i]);                   // [2]-[4]-[6]...stb.
      }
    String buf;
    serializeJson(adatok, buf);
    ws.textAll("05" + buf);
    WEB_action_b = 0;
    }

// --- 13  -->  NTP sync -> last_sinc_string -------
  if ((WEB_action_b == 13)&(millis()-WEB_delay_ul > 300)){ 
      if (S_DEBUG) Serial.println(F("13. NTP sync +  300ms késleltetéssel"));
      NTP_LASTSYNC_STRING = "";
      NTP_LASTSYNC_STRING = DATE_STRING;
      NTP_LASTSYNC_STRING += " - ";
      NTP_LASTSYNC_STRING += TIME_STRING;
      WEB_action_b = 0;
      }

// --- 14  -->  EEPROM CLEAR -semmi, send WS -------
  if ((WEB_action_b == 14)&(millis()-WEB_delay_ul > 3000)){   // 3 mp!
      ws.textAll("30");
      WEB_action_b = 0;}

// --- 15  -->  -------
  if ((WEB_action_b == 15)&(millis()-WEB_delay_ul > 300)){ 

      WEB_action_b = 0;}
      

// ****** időkezelés **********************************              
  // ------------------ idő -------------------------------------------                 
  if (time(&now) != prevTime) {    // ha az idő megváltozott, mp-enként
    showTime();                   // showTime időadatok
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


//--- Ha a dátum változik ---------------------------------
  if (DATE_STRING != DATE_STRING_old) {                    // ha változik a dátum
    ws.textAll("08" + DATE_STRING + "*" + DAYNAME);                  
    DATE_STRING_old = DATE_STRING;
    }

    // *** minden 5. mp-ben, char=5 -> byte=53 ************************
    if (mp_x_dik == 53){      
      //if(S_DEBUG)Serial.println(F("* * xx5. mp * *"));
      mp_x_dik = 100;}

    // *** minden 10. mp-b0en, char=0 -> byte=48 ***********************
    if (mp_x_dik == 48) {
      //if (S_DEBUG)Serial.println(F("* * xx0. mp * *"));
      ws.textAll("01"+TIME_STRING);
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
     || (N_MIN == 50)) & (log_egyszer == 1) & (N_EPO > 1577833200)) {    //2020.01.01. után
     if (make_log == 1) {                                                // Loggolás SD kártyára        
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
     pref.begin("my-app", false);
     pref.putUInt("allrun_perc", allrun_perc_int);   // teljes futásidő, percben 
     pref.end();
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
#include "F_SD_filesystem.h"
   
// ***** LOOP vége **********************************************************

// http://ip1.dynupdate.no-ip.com/    // NO-IP API-ja        
// http://ifconfig.me/ip              // easyDDNS ezt használja     
// http://api.ipify.org/              // ez is működik
// END -----    
