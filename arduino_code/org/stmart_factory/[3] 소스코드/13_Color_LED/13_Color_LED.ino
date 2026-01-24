/**
 * RGB센서, LED모듈을 한번에 사용하는 예제
 *
 * 1. RGB 센서에서 RGB값을 분석
 * 2. RGB값 분석 후, 물체의 색과 RGB 중 가장 가까운 색상을 LED모듈에 출력
 */

#include <Adafruit_NeoPixel.h>                                          // LED 모듈 라이브러리
#include <Adafruit_TCS34725.h>                                          // RGB 센서 사용을 위한 라이브러리
#include <Wire.h>                                                       // I2C 통신에 사용되는 Wire 라이브러리

#define PIN_LED 5                                                       // LED 연결 핀
#define NUM_PIXELS 3                                                    // 네오픽셀의 픽셀 수: 3 

uint16_t clear, red, green, blue;                                       // unsigned short int형 변수 선언
char currentColor;                                                      // Red, blue, green, none(물체 감지X) 네 개로 구분

Adafruit_TCS34725 TCS(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(9600);                                                 // 시리얼 통신 시작
    TCS.begin();                                                        // RGB센서 측정 시작(초기화)
    pixels.begin();                                                     // LED 모듈 작동 시작
    pixels.setBrightness(0);                                            // 밝기(0~255) 설정. 최대 밝기로 출력
    currentColor = 'n';
}
void loop() {
    TCS.getRawData(&red, &green, &blue, &clear);                        // 색상 감지 센서에서 측정 값 받아오기
    delay(100);
    int r = map(red, 0, 21504, 0, 1025);                                // 빨간색 데이터 값
    int g = map(green, 0, 21504, 0, 1025);                              // 초록색 데이터 값
    int b = map(blue, 0, 21504, 0, 1025);                               // 파란색 데이터 값
    
    if (r + g + b > 100) {
        if (currentColor != 'n') {
            currentColor = 'n';
            pixels.setBrightness(0);
        }
        return;
    }
    if (r > g && r > b) {                                               // 빨간 색 제품
        pixels.setPixelColor(0, pixels.Color(255, 0, 0));
        pixels.setPixelColor(1, pixels.Color(255, 0, 0));
        pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    }
    else if (g > r && g > b) {                                          // 초록 색 제품
        pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        pixels.setPixelColor(1, pixels.Color(0, 255, 0));
        pixels.setPixelColor(2, pixels.Color(0, 255, 0));
    }
    else {                                                              // 파란 색 제품
        pixels.setPixelColor(0, pixels.Color(0, 0, 255));
        pixels.setPixelColor(1, pixels.Color(0, 0, 255));
        pixels.setPixelColor(2, pixels.Color(0, 0, 255));
    }
    pixels.setBrightness(255);                                          // 밝기를 255로 지정
    pixels.show();                                                      // 입력한 색상 값을 LED 모듈에 출력
}
