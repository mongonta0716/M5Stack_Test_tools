#include <M5Stack.h>
#include <M5StackUpdater.h>
#include <SPIFFS.h>
#include <FS.h>

String filepath = "/benchmark";

int benchmarkMode = 1;

volatile void getWriteTime(fs::FS &fs, int size, int loop)
{  
  int buffer = 4096;
  int fileloop = size / buffer;

  uint8_t buf[buffer];
  uint64_t res = 0;
  uint64_t sum = 0;
  File f;
  for (int i = 0; i < buffer; ++i) { buf[i] = uint8_t(0xFF); }
  
  Serial.printf("Write,%u,", size / 1000);
  M5.Lcd.printf("Writing %5u KB :", size / 1000);
  for ( int i = 0; i < loop; ++i) {
    M5.Lcd.print(".");
    f = fs.open(filepath + String(i), FILE_WRITE);
    uint64_t start = micros();
    for (int j = 0; j < fileloop; ++j) {
      f.write(&buf[0], buffer);
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

volatile void getReadTime(fs::FS &fs, int size, int loop)
{  
  int buffer = 4096;
  int fileloop = size / buffer;

  uint8_t buf[buffer];
  uint64_t res = 0;
  uint64_t sum = 0;
  File f;

  Serial.printf("Read,%u,", size / 1000);
  M5.Lcd.printf("Reading %5u KB :", size / 1000);
  for ( int i = 0; i < loop; ++i) {
    M5.Lcd.print(".");
    f = fs.open(filepath + String(i), FILE_READ);
    uint64_t start = micros();
    for (int j = 0; j < fileloop; ++j) {
      f.read(&buf[0], buffer);
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
  int size = 4096;
  int loop = 3;

  int filesize = 0;

  dispTitle();

  M5.Lcd.print(fsstr + " Bench Start!  \n");
  Serial.print(fsstr + " Bench Start---------------------------------\n");

  Serial.print("Read/Write,Filesize(KB),");
  for (int i = 0; i < loop; i++) {
    Serial.printf("loop%u(KB/sec),", i);
  }
  Serial.println("average(KB/sec)");
  
  for (int i = 1; i < 10; i++) {
    if (benchmarkMode == 1) {
      filesize = size * i ;
    } else {
      filesize = size * i * i;
    }
    getWriteTime(fs, filesize, loop);
    getReadTime(fs, filesize, loop);
    removeFile(fs, loop);
  }

  M5.Lcd.print(fsstr + " Bench Finished\n");
  Serial.print(fsstr + " Bench Finished\n");
}

void dispTitle() {
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.clear();
  M5.Lcd.printf("Press Button : mode = %u\n", benchmarkMode);
  M5.Lcd.println("Button A:Mode Change 1:4-36KB 2:4-331KB");
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
  // put your main code here, to run repeatedly:
  M5.update();
  if (M5.BtnA.wasPressed()) {
    if (benchmarkMode == 1) {
      // filesize 4096 * loop * loop
      benchmarkMode = 2;
    } else {
      // fileseize 4096 * loop
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
