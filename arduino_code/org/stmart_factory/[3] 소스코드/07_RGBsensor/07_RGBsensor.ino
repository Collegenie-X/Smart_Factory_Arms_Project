/**
   RGB 센서를 활용한 기본예제  
   RGB 라이브러리를 이용하여 센서로부터 물체의 빛을 감지, 분석하고
   해당 값을 R, G, B로 분류하여 시리얼 모니터에 각각의 값을 출력
*/

#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 TCS =
Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X); // TCS34725 라이브러리 사용을 위한 객체 생성

void setup() {
  Serial.begin(9600);                                               // 9600bps 속도로 시리얼 통신 시작
  TCS.begin();
}

void loop() {
  uint16_t clear, red, green, blue;                                 // unsigned short int형 변수 선언
  delay(1000);                                                      // 1초마다 측정하고 출력
  TCS.getRawData(&red, &green, &blue, &clear);                      // 색상 감지 센서에서 측정 값 받아오기

  int r = map(red, 0, 21504, 0, 2000);    // 빨간색 데이터 값
  int g = map(green, 0, 21504, 0, 2000);  // 초록색 데이터 값
  int b = map(blue, 0, 21504, 0, 2000);   // 파란색 데이터 값

  Serial.print("   R: ");
  Serial.print(r);                        // 시리얼 모니터에 빨간색 값 출력
  Serial.print("   G: ");
  Serial.print(g);                        // 시리얼 모니터에 초록색 값 출력
  Serial.print("   B: ");
  Serial.println(b);                      // 시리얼 모니터에 파란색 값 출력
}
