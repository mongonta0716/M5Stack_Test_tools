#include <M5Stack.h>
#include <M5StackUpdater.h>
#include <SPIFFS.h>
#include <FS.h>

String filepath = "/benchmark";
int benchmarkMode = 1;

// If you want to change the measurement conditions, change the following constants.
// Notice: If the total file size exceeds the size of SPIFFS, it does not work properly.

const int initialSize    = 4096;  // Initial File Size
const int increaseSize   = 4096;  // File increase size
const int increaseCount  = 10;    // increment count
const int loopCount      = 3;     // Number of measurements


volatile void startWriteTest(fs::FS &fs, int size, int loop)
{  
  int buffer = 4096;
  int fileloop = size / buffer;
  int residue = size % buffer;
  
  uint8_t buf[buffer];
  uint64_t res = 0;
  uint64_t sum = 0;
  File f;
  for (int i = 0; i < buffer; ++i) { buf[i] = uint8_t(0xFF); }
  
  Serial.printf("Write,%u,", size );
  M5.Lcd.printf("Writing %5u KB :", size / 1000);
  for ( int i = 0; i < loop; ++i) {
    M5.Lcd.print(".");
    f = fs.open(filepath + String(i), FILE_WRITE);
    uint64_t start = micros();
    for (int j = 0; j < fileloop; ++j) {
      f.write(&buf[0], buffer);
    }
    if (residue != 0) {
      f.write(&buf[0], residue);
    }
    f.flush();
    res = micros() - start;
    sum += res;
    f.close();
    Serial.printf("%u,", uint32_t((uint64_t)(size * 1000) / res));
  }
  Serial.printf("%u\n", uint32_t((uint64_t)(size * loop * 1000) /sum));
  M5.Lcd.printf(": Avr %5u KB/s\n", uint32_t((uint64_t)(size * loop * 1000) /sum));
}

volatile void startReadTest(fs::FS &fs, int size, int loop)
{  
  int buffer = 4096;
  int fileloop = size / buffer;
  int residue = size % buffer;

  uint8_t buf[buffer];
  uint64_t res = 0;
  uint64_t sum = 0;
  File f;
  
  Serial.printf("Read,%u,", size );
  M5.Lcd.printf("Reading %5u KB :", size / 1000);
  for ( int i = 0; i < loop; ++i) {
    M5.Lcd.print(".");
    f = fs.open(filepath + String(i), FILE_READ);
    uint64_t start = micros();
    for (int j = 0; j < fileloop; ++j) {
      f.read(&buf[0], buffer);
    }
    if (residue != 0) {
      f.write(&buf[0], residue);
    }
    f.flush();
    res = micros() - start;
    sum += res;
    f.close();
    Serial.printf("%u,", uint32_t((uint64_t)(size * 1000) / res));
  }
  Serial.printf("%u\n", uint32_t((uint64_t)(size * loop * 1000) /sum));
  M5.Lcd.printf(": Avr %5u KB/s\n", uint32_t((uint64_t)(size * loop * 1000) /sum));
}

void removeFile(fs::FS &fs, int loop) {
  for (int i = 0; i < loop; ++i) {
    fs.remove(filepath + String(i));
  }
}

void benchmarkStart(fs::FS &fs ,String fsstr) {

  int filesize = 0;
  dispTitle();

  M5.Lcd.println(fsstr + " Bench Start!  ");
  Serial.println(fsstr + " Bench Start---------------------------------");

  Serial.print("Read/Write,Filesize(B),");
  for (int i = 0; i < loopCount; i++) {
    Serial.printf("%utime(KB/sec),", i + 1);
  }
  Serial.println("average(KB/sec)");

  for (int i = 0; i < increaseCount; i++) {
    if (benchmarkMode == 1) {
      filesize = initialSize + increaseSize * i ;
    } else {
      filesize = initialSize + increaseSize * i * i;
    }
    startWriteTest(fs, filesize, loopCount);
    startReadTest(fs, filesize, loopCount);
    removeFile(fs, loopCount);
  }

  M5.Lcd.println(fsstr + " Bench Finished");
  Serial.println(fsstr + " Bench Finished");
}

void dispTitle() {

  Serial.printf("Press Button : mode = %u\n", benchmarkMode);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.clear();
  M5.Lcd.printf("Press Button : mode = %u\n", benchmarkMode);
  M5.Lcd.printf("InitSize = %u B:IncreaseSize = %u B\n", initialSize, increaseSize);
  M5.Lcd.println("Button A:Increase Mode Change 1:Normal 2: Multiple");
  M5.Lcd.println("Button B:SPIFFS Benchmark");
  M5.Lcd.println("Button C:SD     Benchmark");

}

void setup() {
  M5.begin();
  Wire.begin();
  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  dispTitle();

}

void loop() {

  M5.update();
  if (M5.BtnA.wasPressed()) {
    if (benchmarkMode == 1) {
      // filesize increase multiple
      benchmarkMode = 2;
    } else {
      // fileseize increase normal
      benchmarkMode = 1;
    }
    dispTitle();
  }
  if (M5.BtnB.wasPressed()) {
    SPIFFS.begin();
    benchmarkStart(SPIFFS, "SPIFFS");
  }
  if (M5.BtnC.wasPressed()) {
    benchmarkStart(SD, "SD");
  }

}
