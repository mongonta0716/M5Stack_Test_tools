#include <M5Stack.h>
#include <M5StackUpdater.h>
#include <SPIFFS.h>
#include <FS.h>

String filepath = "/benchmark";

void getWriteTime(fs::FS &fs, int size, int loop)
{  
  Serial.printf("#FileSize %dB:Write\n", size);
  int buffer = 4096;
  int fileloop = size / buffer;

  uint8_t buf[buffer];
  uint64_t res = 0;
  uint64_t sum = 0;
  File f;
  for (int i = 0; i < buffer; ++i) { buf[i] = uint8_t(0xFF); }
  
  M5.Lcd.printf("Writing %5uB:", size);
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
    Serial.printf("%4u KB/s ", uint32_t((uint64_t)(size * 1000) / res));
  }
  Serial.printf(": Avr %4u KB/s\n\n", uint32_t((uint64_t)(size * loop * 1000) /sum));
  M5.Lcd.printf(": Avr %4u KB/s\n", uint32_t((uint64_t)(size * loop * 1000) /sum));
}

void getReadTime(fs::FS &fs, int size, int loop)
{  
  Serial.printf("#FileSize %dB:Read\n", size);
  int buffer = 4096;
  int fileloop = size / buffer;

  uint8_t buf[buffer];
  uint64_t res = 0;
  uint64_t sum = 0;
  File f;

  M5.Lcd.printf("Reading %5uB:", size);
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
    Serial.printf("%4u KB/s ", uint32_t((uint64_t)(size * 1000) / res));
  }
  Serial.printf(": Avr %4u KB/s\n\n", uint32_t((uint64_t)(size * loop * 1000) /sum));
  M5.Lcd.printf(": Avr %4u KB/s\n", uint32_t((uint64_t)(size * loop * 1000) /sum));
}

void removeFile(fs::FS &fs, int loop) {
  for (int i = 0; i < loop; ++i) {
    fs.remove(filepath + String(i));
  }
}

void benchmarkStart(fs::FS &fs) {
  int size = 4096;
  int loop = 5;
  
  for (int i = 1; i < 9; i++) {
    getWriteTime(fs, size * i, loop);
    getReadTime(fs, size * i, loop);
    removeFile(fs, loop);
  }
}

void dispTitle() {
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.clear();
  M5.Lcd.println("Press Button");
  M5.Lcd.println("Button A:SPIFFS Benchmark");
  M5.Lcd.println("Button B:SD     Benchmark");
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
    dispTitle();
    M5.Lcd.print("SPIFFS Bench Start!  \n");
    Serial.print("SPIFFS Bench \n");
    SPIFFS.begin();
    benchmarkStart(SPIFFS);
    M5.Lcd.print("SPIFFS Bench Finished\n");
  }
  if (M5.BtnB.wasPressed()) {
    dispTitle();
    M5.Lcd.print("SD Bench Start!      \n");
    Serial.print("SD Bench \n");
    benchmarkStart(SD);
    M5.Lcd.print("SD Bench Finished    \n");
  }

}
