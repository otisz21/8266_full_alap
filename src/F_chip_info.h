
void CHIP_INFO_8266_print(){
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, CHIP_INFO_8266_json());
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
    }
  JsonArray data = doc["data"];

  // [0] Proc Vcc  (D1 mininél)
  double data_0 = data[0];              // 3.21827957
  Serial.println("* * * POCESSOR INFO (json) * * *");
  Serial.print(F("getVcc()              : "));
  Serial.println(data_0);
  // [1] ESP8266 chip ID
  long data_1 = data[1];               // 16058434
  Serial.print(F("getChipId             : "));
  Serial.println(data_1);
  // [2] alapverzió
  const char* data_2 = data[2];        // "3.1.2"
  Serial.print(F("getCoreVersion        : "));
  Serial.println(data_2);
  // [3] SDK-verzió
  const char* data_3 = data[3];         // "2.2.2-dev(38a443e)"
  Serial.print(F("getSdkVersion         : "));
  Serial.println(data_3);
  // [4] CPU frekvenciája MHz-
  int data_4 = data[4];                // 80
  Serial.print(F("getCpuFreqMHz         : "));
  Serial.println(data_4);
  // [5] SketchSize, byte
  long data_5 = data[5];                // 532752
  Serial.print(F("SketchSize (byte)     : "));
  Serial.println(data_5);
  // [6] FreeSketchSpace, byte
  long data_6 = data[6];                // 1560576
  Serial.print(F("Free Sk. Space (byte) : "));
  Serial.println(data_6);
  // [5] flash chip azonosító
  long data_7 = data[7];                // 1458187
  Serial.print(F("getFlashChipID        : "));
  Serial.println(data_7);
  // [6] Flash chip size
  long data_8 = data[8];                // 4194304
  Serial.print(F("getFlashChipRealSize  : "));
  Serial.println(data_8);
  // [7] Flash chip frekvenciája MHz-
  int data_9 = data[9];                 // 40
  Serial.print(F("getFlashChipSpeed     : "));
  Serial.println(data_9);
  // [8] szabad kupac méreté
  long data_10 = data[10];                // 33872
  Serial.print(F("getFreeHeap           : "));
  Serial.println(data_10);
  // [9] legnagyobb összefüggő szabad RAM
  long data_11 = data[11];               // 33600
  Serial.print(F("getMaxFreeBlockSize   : "));
  Serial.println(data_11);
  // [10] HEAP töredezettség %-
  int data_12 = data[12];               // 1
  Serial.print(F("getHeapFragmentation  : "));
  Serial.println(data_12);
  // [11] Proc. újraindult x-szer
  int data_13 = data[13];              // 82
  Serial.print(F("Processor újraindult  : "));
  Serial.println(data_13);
  // [12] utolsó visszaállítás oka  
  const char* data_14 = data[14];       // "Software/System restart"
  Serial.print(F("getResetReason        : "));
  Serial.println(data_14);
  // [13] Filesysten tipusa
  const char* data_15 = data[15];       // "LittleFS"
  Serial.print(F("Filesystem type       : "));
  Serial.println(data_15);
  // [14] Total space (byte)
  long data_16 = data[16];              // 2072576
  Serial.print(F("FS total byte         : "));
  Serial.println(data_16);
  // [15] Total space used (byte)
  long data_17 = data[17];              // 409600
  Serial.print(F("FS használt byte      : "));
  Serial.println(data_17);
  // [16] használatban lévő tárhely
  double data_18 = data[18];            // 19.76284599
  Serial.print(F("FS használatban (%)   : "));
  Serial.println(data_18);
  // [17] teljes futásidő   
  int data_19 = data[19];               // 2
  Serial.print(F("teljes futásidő (perc): "));
  Serial.println(data_19);
  Serial.println(F("* * * POCESSOR INFO END * * *"));
}

String CHIP_INFO_8266_json() {
      //DynamicJsonDocument PROC_json(512);
      StaticJsonDocument<512> PROC_json;    
      JsonArray data = PROC_json.createNestedArray("data");  
      String PROC_data;
      data.add(ESP.getVcc()*1.12/1023.0);         // [0] Proc Vcc  (D1 mininél)
      data.add(ESP.getChipId());                  // [1] ESP8266 chip ID 
      data.add(ESP.getCoreVersion());             // [2] alapverzió
      data.add(ESP.getSdkVersion());              // [3] SDK-verzió
      data.add(ESP.getCpuFreqMHz());              // [4] CPU frekvenciája MHz-ben
      //-------------------------------------
      data.add(ESP.getSketchSize());              // [5] SketchSize, byte
      data.add(ESP.getFreeSketchSpace());         // [6] FreeSketchSpace, byte
      //-------------------------------------
      data.add(ESP.getFlashChipId());             // [7] flash chip azonosító
      data.add(ESP.getFlashChipRealSize());       // [8] Flash chip size
      data.add(ESP.getFlashChipSpeed()/1000000);  // [9] Flash chip frekvenciája MHz-ben
      //-------------------------------------
      data.add(ESP.getFreeHeap());                // [10] szabad kupac méreté
      data.add(ESP.getMaxFreeBlockSize());        // [11] legnagyobb összefüggő szabad RAM
      data.add(ESP.getHeapFragmentation());       // [12] HEAP töredezettség %-ban
      //-------------------------------------
      data.add(proc_restart_num);                 // [13] Proc. újraindult x-szer     
      data.add(ESP.getResetReason());             // [14] utolsó visszaállítás oka 
      //-------------------------------------
      FSInfo fs_info;
      LittleFS.info(fs_info);
      used_FS_percent = ((float(fs_info.usedBytes)*100.0) / float(fs_info.totalBytes));                      
      data.add(filesystem);                       // [15] Filesysten tipusa
      data.add(fs_info.totalBytes);               // [16] Total space (byte)
      data.add(fs_info.usedBytes);                // [17] Total space used (byte)
      data.add(used_FS_percent);                  // [18] használatban lévő tárhely 
      //-------------------------------------            
      data.add(allrun_perc_int);                  // [19] teljes futásidő     
      //-------------------------------------
      serializeJson(PROC_json, PROC_data);
      return PROC_data;
      }


String wl_status_to_string(int w_status) {
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
  