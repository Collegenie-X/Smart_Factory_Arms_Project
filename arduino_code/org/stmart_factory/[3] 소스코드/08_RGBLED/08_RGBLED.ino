#include <Adafruit_NeoPixel.h>
#define PIN 5                                                    // LED와 연결 될 핀번호 설정
#define NUMPIXELS 3                                              // LED 개수(3개)

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);  // LED 제어를 위한 변수 생성

void setup() {
  pixels.begin();                                                // LED 시작
  pixels.setBrightness(50);
}
void loop() {
  for (int i = 0; i < NUMPIXELS; i++) {                          // 0~2번째 픽셀까지 순서대로(총 3개) 조작
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));            // ex) 0번째 픽셀의(i) RGB색상을 각각 255, 0, 0으로 부여
    pixels.show();                                               // LED 모듈에 나타내기                                                   
  }
  delay(1000);                                                   // 다음 픽셀은 1초 뒤 변화
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));
    pixels.show();
    delay(10);
  }
  delay(1000);
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));
    pixels.show();
    delay(10);
  }
  delay(1000);
}
