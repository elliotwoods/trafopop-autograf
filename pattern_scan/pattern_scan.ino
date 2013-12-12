#include <SPI.h>

#define NUM 100

enum Mode {
  off = 0,
  scanning = 1,
  passthrough = 2
} mode = scanning;

typedef struct color {
  byte r, g, b;
} color;

color pixels[NUM];
float brightness = 0.1f;
long frameIndex = 0;

long period = 1000.0f * 60.0f / 130.0f; // this is Berlin, everything is 130bpm

long frameCount() {
  return (long) ceil(log( (float) NUM) / log( (float) 2));
}

void pushPixels()
{
  byte* bytes = (byte*) pixels;
  for (int index=0; index<NUM*3; index++)
  {
    // nur 25% Helligkeit    
    byte c = bytes[index] >> 2;

    for (SPDR = c; !(SPSR & _BV(SPIF)););
  }
}

void presentNone() {
  for(int i=0; i<NUM; i++) {
    pixels[i].r = 0;
    pixels[i].g = 0;
    pixels[i].b = 0;  
  }
  
  pushPixels();
}

void presentScaneFrame(int frame) {
  for(int i=0; i<NUM; i++) {
    bool value = (i >> frame) & 1;
    byte rawValue = brightness * 255.0f * value;
    pixels[i].r = rawValue;
    pixels[i].g = rawValue;
    pixels[i].b = rawValue;
  }
  
  pushPixels();
}

void presetPassthrough() {
  presentNone();
}

void processIncomingData() {
  if (!Serial.available() == 0) {
    return;
  }
}

void setup() {
  Serial.begin(115200);
  
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  // SPI.setClockDivider(SPI_CLOCK_DIV16);  // 1 MHz
  SPI.setClockDivider(SPI_CLOCK_DIV8);  // 2 MHz
  // SPI.setClockDivider(SPI_CLOCK_DIV4);  // 4 MHz 
}

void loop() {
  processIncomingData();
  
  switch(mode) {
    case off:
      presentNone();
      break;
    case scanning:
      presentScaneFrame(frameIndex++);
      break;
    case passthrough:
      presetPassthrough();
      break;
  }
  
  frameIndex %= frameCount();
  Serial.write(frameIndex);
  
  delay(period - (millis() % period)); // heartbeat at 130bpm
}

