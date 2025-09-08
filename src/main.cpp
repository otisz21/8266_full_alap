
// 8266_full_alap

// 2023-08-08: + Pushover próba

// 8MB lash (platformio.ini)

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

#include <ESP_Mail_Client.h>

#include "pitches.h"
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

SMTPSession smtp;  // Deklarálja a globális használt SMTPSession objektumot az SMTP szállításhoz

//ADC_MODE(ADC_VCC);
      
// * SETUP ************************************************************** 
void setup() {                  
  Serial.begin(115200);
  // Serial.begin(9600);
  delay(3000);

  Serial.println();
  Serial.println(F("---- SYSTEM START ----"));
  Serial.println();

  pinMode(buzzerPin, OUTPUT);
  pref.begin("my-app", false);   // namespace, nem csak olvasás

  //drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-
  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
  if (drd->detectDoubleReset()) drd_status = true;     // AP mód - 0
  else drd_status = false;
  //drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-drd-

  delay(2000);
  PROJECT_INFO();
  
  if (drd_status) Serial.println(F("Double-Reset észlelve, mode->AP"));
  else Serial.println(F("Nincs érzékelve Double-Reset"));  

//---- mentett adatok kiolvasása--------------------------
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
  else            Serial.println(F("Setup / if (initWiFi())= FALSE(0), ===> AP mód!"));

    //   Állítsa be az mDNS válaszadót:
    // - Az első argumentum ebben a példában a domain név
    //   a teljesen minősített domain név "RTC.local"
    // - A második érv a hirdetni kívánt IP-cím
    //   elküldjük IP címünket a WiFi hálózaton
    if (!MDNS.begin(mdns_name)) {
      Serial.println(F("Error setting up MDNS responder!"));
      }
    else{
    Serial.println(F("mDNS responder: OK"));
    Serial.print(F("mDNS url      :  http://"));
    Serial.print(mdns_name);
    Serial.println(F(".local"));
    MDNS.addService("http", "tcp", 80);
    }

    if (LittleFS.begin()) {
      ftpSrv.setCallback(_callback);
      ftpSrv.setTransferCallback(_transferCallback);
      filesystem = "LittleFS";
      Serial.println(F("LittleFS fájlrendszer elindítva!"));
      Serial.print(" * * * FTP username:\t");
      Serial.println(F("esp8266"));            // username for ftp.
      Serial.print(" * * * FTP password:\t");
      Serial.println(F("8266"));               // password for ftp.
      ftpSrv.begin("esp8266", "8266");         // (default 21, 50009 for PASV)  
      }

    configTime(MY_TZ, NTP_SERVER_1, NTP_SERVER_2); //Mytz - Budapest
    settimeofday_cb(NTP_time_is_set);              // opcionális: visszahívás, ha elküldték az időt


// wifi hálózat, 1->STA mód(client)
  if (WIFI_STA_or_AP == 1) server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  else server.serveStatic("/", LittleFS, "/").setDefaultFile("wifi.html");  
    
//****** Index, (főoldal) ************************************************    
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
    DynamicJsonDocument adatok(512);
    JsonArray setup = adatok.createNestedArray("setup");
    setup.add(project);                    // [0] project (a file name, csak main.cpp!)
    setup.add(comp_idopont);               // [1] fordítás időpontja
    setup.add(ARDUINO_BOARD);              // [2] modul tipus
    setup.add(millis64());                 // [3] futásidő
    setup.add(allrun_perc_int);            // [4] teljes futásidő
    setup.add(make_log);                   // [5] hogy legyen soros monitor ellenőrzés
    setup.add(S_DEBUG);                    // [6] készítsen-e log fájlt
    setup.add(blue_led);                   // [7] villogjon-e a beépített kék led

    setup.add(str_pr_1);                   // [8] 
    setup.add(str_pr_2);                   // [9] 
    setup.add(str_pr_3);                   // [10] 
    setup.add(str_pr_4);                   // [11]
    setup.add(prior);                      // [12]
    setup.add(device);                     // [13] 

    serializeJson(adatok, *response);
    request->send(response);});
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

//--------------------------------------------------------------------------------
  server.on("/setup_save_pr", HTTP_GET, [](AsyncWebServerRequest* request) {        // proba stringek             
    str_pr_1 = (request->getParam("str_1")->value());
    str_pr_2 = (request->getParam("str_2")->value());
    str_pr_3 = (request->getParam("str_3")->value());
    str_pr_4 = (request->getParam("str_4")->value());
    prior = (request->getParam("prior")->value());
    device = (request->getParam("device")->value()).toInt();
    
    Serial.printf("str_pr_1 value: %s\n", str_pr_1.c_str());
    Serial.printf("str_pr_2 value: %s\n", str_pr_2.c_str());
    Serial.printf("str_pr_3 value: %s\n", str_pr_3.c_str());
    Serial.printf("str_pr_4 value: %s\n", str_pr_4.c_str());    
    Serial.printf("prior    value: %s\n", prior.c_str());
    Serial.printf("device   value: %u\n", device);

    request->send_P(200, "text/plain", "setup_SAVE_OK");});

//**** WIFI setup oldal **********************************************************
//--------------------------------------------------------------------------------
  server.on("/wifi_json", HTTP_GET, [](AsyncWebServerRequest* request) {           // WIFI-Setup oldal JSON adatok küldése
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    DynamicJsonDocument WIFI_json(768);

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
    wifi_alap.add(ssid);                                 // [9] elmentett SSID
    wifi_alap.add(pass);                                 // [10] elmentett PASWORD    

    JsonArray w_scan_db = WIFI_json.createNestedArray("w_scan_db");
    w_scan_db.add(WIFI_drb_int);                         // [0] talált wifi db.
    w_scan_db.add(WIFI_drb_int_all);                     // [1] talált wifi db.

    JsonArray wifi_scan = WIFI_json.createNestedArray("wifi_scan");
    //wifi_scan.add(WIFI_drb_int);                         // [0] talált wifi db.
    for (int i = 0; i < WIFI_drb_int; i++) {
      // wifi_scan.add(RSSI_sort_int[i]);                   // [1]-[3]-[5]...stb.
      // wifi_scan.add(SSID_sort_string[i]);                // [2]-[4]-[6]...stb.
      wifi_scan.add(RSSI_sort_int[i]);                   // [0]-[2]-[4]...stb.
      wifi_scan.add(SSID_sort_string[i]);                // [1]-[3]-[5]...stb.      
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
  server.on("/NTP_json", HTTP_GET, [](AsyncWebServerRequest* request) {         //  NTP setup oldal JSON adatok küldése
    AsyncResponseStream* response = request->beginResponseStream("application/json");
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
      Serial.println(F("incomming_COM.txt write OK"));
      }
    else {
      Serial.println(F("incomming_COM.txt write ERROR"));
      }
    }
    
// ***Web Socket beérkező üzenetek *******************************
  if ((W_S_rec_int != 99) & (W_S_rec_int != 0)) {
    if (W_S_rec_int == 10) proc_restart = 10;    // proc restart törlése
    if (W_S_rec_int == 21) WAN_IP_CHECK_easyddns();
    if (W_S_rec_int == 22) URL_GET (W_S_rec_str); 
    W_S_rec_int = 99;
    }


// ****** időkezelés **********************************                             
  if (time(&now) != prevTime) {    // ha az idő megváltozott, mp-enként
    showTime();                    // showTime időadatok
    if (blue_led == 1) {           // LED villogtatás
      if (WiFi.status() != WL_CONNECTED) {  // nincs WiFi -gyors
        led_blink_time_ul = 200;
        }
      else {                              // van WiFi - lassú
        led_blink_time_ul = 1000;
        }
      }
    if (proc_restart == 10) {
      proc_restart = 0;
      ws.textAll("03");
      }

/*    // ********* nedvesség érz. *******************************************
    soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
    //Serial.print("Analog read value: "); Serial.println(soilMoistureValue);
    soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
    //Serial.print("soilmoisturepercent: "); Serial.println(soilmoisturepercent);
    if (soilmoisturepercent > 75) {
      water_on = 1;
      Serial.println("> 75 %");
      }
    else if (soilmoisturepercent <= 75) {
      water_off = 0;
      water_on = 0;
      //Serial.println("<= 75 %");
      }
    else if (soilmoisturepercent >= 0 && soilmoisturepercent <= 100) {
      //Serial.print(soilmoisturepercent);
      //Serial.println("%");
      }
    */

    prevTime = time_t(now);
    }

  if ((water_on == 1) & (water_off == 0)) {
    WEB_action_b = 15;
    WEB_delay_ul = millis();
    water_off = 10;
    water_on = 0;
    }


  if ((blue_led == 1) & (millis() - led_delay_ul > led_blink_time_ul)) {
    led_state = !led_state;             // LED villogtatás
    // Serial.println("led_state vált.");
    if (led_state) led.on();
    else led.off();
    led_delay_ul = millis();
    }

//--- Ha a dátum változik ---------------------------------
  if (DATE_STRING != DATE_STRING_old) {                    // ha változik a dátum
    ws.textAll("08" + DATE_STRING + "*" + DAYNAME);                  
    DATE_STRING_old = DATE_STRING;
    }

// *** minden 10. mp-b0en, char=0 -> byte=48 ***********************
  if (mp_x_dik == 48) {
    ws.textAll("01" + TIME_STRING);
    ws.cleanupClients();
    mp_x_dik = 100;
    }

//******* percenként ******************************************** 
   // AKT_IDO_CHAR_10: [0][1][2][3][4][5][6][7][8][9]  
   //             pl.:  1  4  :  2  5  :  3  2  -  -
   // char.  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, .. 0, 1, ....
   // byte  48,49,50,51,52,53,54,55,56,57,    (mp_x_dik)
  if (AKT_IDO_CHAR_10[4] != AKT_IDO_CHAR_10_OLD[4]) {  // percenként - 5. char. [4] 1percenként-enként igaz    
    allrun_perc_int++;                                 // növeli 1-el futásidő számlálót               
    AKT_IDO_CHAR_10_OLD[4] = AKT_IDO_CHAR_10[4];
    }                   // pl.:  1  4  :  2  5  :  3  2  -  -

// ***** futásidő számlálót mentése ********************************************************************
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
    Serial.print("Push_mobil válasz: ");
    Serial.println(Push_mobil(str_pr_1, str_pr_2, prior, device));    
    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 2) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 2"));
    SEND_email(str_pr_3, str_pr_4);
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
    if (S_DEBUG)Serial.println(F("Press Button: 5"));
    //Serial.println(readFile(LittleFS, "/serial/incomming_serial.txt"));
    // hangok száma
    int size_1 = sizeof(noteDurations_1) / sizeof(int);
    Serial.print("melody size: "); Serial.println(size_1);
    // ismételje át a dallam hangjait:
    for (int thisNote_1 = 0; thisNote_1 < size_1; thisNote_1++) {
    // A hangjegy időtartamának kiszámításához vegyen egy másodpercet osztva a hangjegy típusával.
    // például. negyed hang = 1000 / 4, nyolcad hang = 1000/8 stb.      
      int noteDuration_1 = 1000 / noteDurations_1[thisNote_1];
      tone(buzzerPin, melody_1[thisNote_1], noteDuration_1);
    // a hangjegyek megkülönböztetéséhez állítson be egy minimális időt közöttük.
    // a jegyzet időtartama + 30% jól működik:      
      int pauseBetweenNotes_1 = noteDuration_1 * 1.30;
      delay(pauseBetweenNotes_1);
    // a hangszín lejátszásának leállítása:      
      noTone(buzzerPin);
      }
    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 6) & (millis() - WEB_delay_ul > 200)) {
    if (S_DEBUG)Serial.println(F("Press Button: 6"));
    int size = sizeof(noteDurations) / sizeof(int);
    Serial.print("melody size: "); Serial.println(size);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(buzzerPin, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(buzzerPin);
      }
    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 7) & (millis() - WEB_delay_ul > 200)) {
    if (S_DEBUG)Serial.println(F("Press Button: 7"));
    tone(buzzerPin, NOTE_C7, 100);
    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 8) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 8"));
    int size = sizeof(beep_2x_durations) / sizeof(int);
    Serial.print("melody size: "); Serial.println(size);
    for (int i = 0; i < size; i++) {
      int noteDuration = 1000 / noteDurations[i];
      tone(buzzerPin, beep_2x[i], noteDuration);
      delay(noteDuration);
      noTone(buzzerPin);
      }
    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 9) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 9"));
    listDir("/my-app");     // --- Directory listázása Little FS-ből
    WEB_action_b = 0;
    }
// --- Setup oldalon gombok 1-10 ---------------
  if ((WEB_action_b == 10) & (millis() - WEB_delay_ul > 200)) {
    if(S_DEBUG)Serial.println(F("Press Button: 10"));
    listDir("/serial/New_directory");     // --- Directory listázása Little FS-ből
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
    JsonArray w_scan_db = adatok.createNestedArray("w_scan_db");
    w_scan_db.add(WIFI_drb_int);                         // [0] talált wifi db.
    w_scan_db.add(WIFI_drb_int_all);                     // [1] talált wifi db.

    JsonArray wifi_scan = adatok.createNestedArray("wifi_scan");
    //wifi_scan.add(WIFI_drb_int);                         // [0] talált wifi db.
    for (int i = 0; i < WIFI_drb_int; i++) {
      // wifi_scan.add(RSSI_sort_int[i]);                   // [1]-[3]-[5]...stb.
      // wifi_scan.add(SSID_sort_string[i]);                // [2]-[4]-[6]...stb.
      wifi_scan.add(RSSI_sort_int[i]);                   // [0]-[2]-[4]...stb.
      wifi_scan.add(SSID_sort_string[i]);                // [1]-[3]-[5]...stb.      
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
  if ((WEB_action_b == 15) & (millis() - WEB_delay_ul > 300)) {
    Serial.print("Push_mobil válasz: ");
    Serial.println(Push_mobil("VÍZ!!!", "tugboat", "1", 1));
    WEB_action_b = 0;
    }
      
// ********************************************************************************************************        
    } // LOOP vége
// ***** LOOP vége **********************************************************

#include "F_system.h"
   
// END -----    
