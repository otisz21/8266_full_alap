
// =======================================================================
  // Read File from LittleFS
String readFile(fs::FS &fs, const char * path){
if (S_DEBUG)Serial.printf("Reading file: %s\r\n", path);
  
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
if (S_DEBUG)Serial.println("- failed to open file for reading");
    return String();}
  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;}
  file.close();
  return fileContent;}

// =======================================================================
  // Write file to LittleFS
void writeFile(fs::FS &fs, const char * path, const char * message){
if (S_DEBUG)Serial.printf("Writing file: %s\r\n", path);
  
  File file = fs.open(path, "w");
  if(!file){
if (S_DEBUG)Serial.println("- nem sikerült megnyitni a directorit, íráshoz!"); 
    return;}
  if(file.print(message)){
if (S_DEBUG)Serial.println("- file írása: sikeres!");
  } else {
if (S_DEBUG)Serial.println("- file írása: sikertelen!");
}
  file.close();}

// =======================================================================
//-----------------------------------------------------------------------
void new_logfile(){    // mai_filename pl.: /log/1673434800.txt  - "/log/"+String(mai_epo)+".txt"

  if(S_DEBUG)Serial.println(F("éves összesítés check()"));

 // file_ev = String(N_YEAR);

  year_sum_file = "/year_log/"+String(N_YEAR)+".txt";
  
  if(LittleFS.exists(year_sum_file) != 1){                    // ha még nincs ilyen fájl
     if(S_DEBUG)Serial.print(F("Nincs még ilyen fájl: "));
     if(S_DEBUG)Serial.println(year_sum_file);           
    
     File fileToAppend = LittleFS.open(year_sum_file, "a");   // létrehozza ...
      if(!fileToAppend){
        if(S_DEBUG)Serial.println(F("Nem sikerült megnyitni a fájlt, éves report-hoz!"));
        } 
      if(fileToAppend.println(F("- FILE LÉTREHOZÁSA:" ))){
        fileToAppend.println("- " + DATE_STRING + " - " + TIME_STRING + " - ");
        fileToAppend.println(F("       |    Hőmérséklet  °C   |"));
        fileToAppend.println(F("dátum  |  min  |  max | átlag |"));      
        fileToAppend.println(F("-------------------------------"));
        //       |    Hőmérséklet  °C   |
        //dátum  |  min  |  max | átlag |
        
        if(S_DEBUG)Serial.println(F("Létrehozott fájl: "));
        if(S_DEBUG)Serial.println(year_sum_file);
        if(S_DEBUG)Serial.println(F("Dátum hozzáírása a fájlhoz, első sorba"));
        }
      else {
        if(S_DEBUG)Serial.println(F("Dátum hozzáírása a fájlhoz NEM SIKERÜLT!"));
            }   
     fileToAppend.close();}
  else{                                                          // ha már van ilyen fájl, pl.: restart
     if(S_DEBUG)Serial.print(F("Ez a fájl már létezik: "));      // a fájlhoz nem ír semmit
     if(S_DEBUG)Serial.println(year_sum_file);}
     
 if(S_DEBUG) {
    Serial.print(F("N_YEAR: "));
    Serial.println(N_YEAR);
    Serial.print(F("N_MONTH: "));
    Serial.println(N_MONTH);
    Serial.print(F("N_DAY: "));
    Serial.println(N_DAY);        
    Serial.print(F("adott nap epo-ja 12:00-kor: "));
    Serial.println(mai_epo);
    Serial.print(F("mai_filename: "));
    Serial.println(mai_filename);      
    Serial.println(); } 

  if(LittleFS.exists(mai_filename) != 1){                    // ha még nincs ilyen fájl, pl:-új nap
     if(S_DEBUG)Serial.print(F("Nincs még ilyen fájl: "));
     if(S_DEBUG)Serial.println(mai_filename);          
    
     File fileToAppend = LittleFS.open(mai_filename, "a");   // létrehozza ...
      if(!fileToAppend){
        if(S_DEBUG)Serial.println(F("Nem sikerült megnyitni a fájlt, dátum fozzáíráshoz!"));
        } 
      if(fileToAppend.println(" - " + DATE_STRING + " - " + TIME_STRING + " - " )){    //...hozzáírja a dátumot, időt
        fileToAppend.println(F("|  idő   | T °C | H %  | P hPa |" ));
        if(S_DEBUG)Serial.println(F("Dátum hozzáírása a fájlhoz, első sorba + 1sor"));
        }
      else {
        if(S_DEBUG)Serial.println(F("Dátum hozzáírása a fájlhoz NEM SIKERÜLT!"));
            }   
     fileToAppend.close();}
  else{                                                        // ha már van ilyen fájl, pl.: restart
     if(S_DEBUG)Serial.print("Ez a fájl már létezik: ");      // a fájlhoz nem ír semmit
     if(S_DEBUG)Serial.println(mai_filename);}
   
    Dir dir = LittleFS.openDir("/temp_log/");                         // log folder megnyitása
        while (dir.next()){                                 // log folder összes fájljának körbefuttatása
            if(S_DEBUG)Serial.println(dir.fileName());     // filename       
          if ((dir.fileName().substring(0,10)).toInt() < mai_epo-518400){   // ha a név epo-ja egy héttel régebbi ..        
             if(S_DEBUG)Serial.print(F("áthelyezendő: "));                    // 6 napnyi epo, így lesz mindig 7 fájl
             if(S_DEBUG)Serial.println(dir.fileName());
             if(S_DEBUG)Serial.println(tmConvert_ep(dir.fileName()));
             copy_file (dir.fileName());}}                                  // külön függvény átmásol, töröl
  }
  
// --- Directory listázása SD FS-ből (teljes lista) --------
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;}
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');}
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);}
    else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      time_t cr = entry.getCreationTime();
      time_t lw = entry.getLastWrite();
      struct tm * tmstruct = localtime(&cr);
      Serial.printf("\tCREATION: %d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) +
                    1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour,
                    tmstruct->tm_min, tmstruct->tm_sec);
      tmstruct = localtime(&lw);
      Serial.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) +
                    1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour,
                    tmstruct->tm_min, tmstruct->tm_sec);}
    entry.close();}
}

// -------------------------------------------------------------------------- 
void copy_file(String f_name_1){
    if(S_DEBUG)Serial.println(F("*****"));
    if(S_DEBUG)Serial.print(F("Copy fájl: "));
    if(S_DEBUG)Serial.print(f_name_1);
    if(S_DEBUG)Serial.print(F("  azaz: "));
    if(S_DEBUG)Serial.println(tmConvert_ep(f_name_1));

    String arc_YEAR = tmConvert_ep(f_name_1).substring(0,4); 

    File file_1 = LittleFS.open("/temp_log/"+f_name_1, "r");
    File file_2 = LittleFS.open("/archiv_"+arc_YEAR+"/"+tmConvert_ep(f_name_1), "w");
 
    if(!file_1){
        if(S_DEBUG)Serial.println(F("PR_ Failed to open file for reading (temp_log)"));
        return;}
    if(!file_2){
        if(S_DEBUG)Serial.println(F("PR_ Failed to open file for append (archív)"));
        return;}
        
    if(S_DEBUG)Serial.println(F("PR_ Read and Copy file: "));
    while(file_1.available()){                        
        file_2.println(file_1.readString());}
    file_1.close();
    file_2.close();

    if(S_DEBUG){
    File file_new = LittleFS.open("/archiv_"+arc_YEAR+"/"+tmConvert_ep(f_name_1), "r");
    if(file_new){
      Serial.println();
      Serial.println(F("file_new (archív) readString: "));
        while(file_new.available()){                        
            Serial.println(file_new.readString());}}
    file_new.close();}   
  
    if(LittleFS.remove("/temp_log/"+f_name_1)){
      if(S_DEBUG) Serial.println(F("auto delet: OK!"));
      if(S_DEBUG) Serial.print(F("fájl törölve: temp_log/"));    
      if(S_DEBUG) Serial.println(tmConvert_ep(f_name_1));}
    else{
      if(S_DEBUG) Serial.println(F("auto delet: ERROR!"));
      if(S_DEBUG) Serial.print(F("fájl nem lett törölve: temp_log/"));    
      if(S_DEBUG) Serial.println(tmConvert_ep(f_name_1));      
      }
      if(S_DEBUG)Serial.println(F("*****"));
      }

// -------------------------------------------------------------------------
// **** Epoch, dátumra konvertálása, (log fájl név miatt) ****************** 
// **** https://github.com/PaulStoffregen/Time *****************************
String tmConvert_ep(String EP_string){       // fájlnevet átalakítja dátummá+.txt
  unsigned long EP_int = EP_string.substring(0,10).toInt();                   // 1672311600.txt ==> 1672311600
  tm t_file;                                                               // deklarálva setup előtt
  time_t now = EP_int;                       
  localtime_r(&now, &t_file);
  int tm_year = t_file.tm_year + 1900;
  int tm_month = t_file.tm_mon + 1;
  int tm_day = t_file.tm_mday;
  return (String(tm_year)+"-"+first_0(tm_month)+"-"+first_0(tm_day)+".txt");}  // pl.: 2022-08-05

//-----------------------------------------------------------------------
String to_hh_mm(int mp) {

     int hh = mp / 3600;             // órák száma
     int mm = (mp % 3600) / 60;      // percek száma
     String mm_0;
     String hh_mm;   
   if (mm < 10){
    mm_0 = "0"+String(mm);}
   else  {
    mm_0 = String(mm);}
   
   if (hh > 0){
    return String(hh)+" óra, "+mm_0+" percet.";}
   else{
    return "0 óra, "+mm_0+" percet.";}}
