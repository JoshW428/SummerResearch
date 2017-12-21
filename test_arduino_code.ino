#include "arduinoFFT.h"
int analogPin = A0;
int val = 0;
arduinoFFT FFT = arduinoFFT();
int fft_input[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  Serial.begin(115200);
  pinMode(analogPin, INPUT);
  int fft_input[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

void loop() {
  for (int i = 0; i < 20; i = i + 2) {
    val = analogRead(analogPin);
    fft_input[i] = val;
  }

  FFT.fft_run();

  fft_mag_log();

  for (int i = 0; i < 10; i++) {
    Serial.println(fft_log_out[i]);
  }
}
