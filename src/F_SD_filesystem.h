
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
  
// --- Directory listázása SD FS-ből (teljes lista) --------
void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
      }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
      }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
      }
    else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      time_t cr = entry.getCreationTime();
      time_t lw = entry.getLastWrite();
      struct tm* tmstruct = localtime(&cr);
      Serial.printf("\tCREATION: %d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) +
        1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour,
        tmstruct->tm_min, tmstruct->tm_sec);
      tmstruct = localtime(&lw);
      Serial.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) +
        1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour,
        tmstruct->tm_min, tmstruct->tm_sec);
      }
    entry.close();
    }
  }
