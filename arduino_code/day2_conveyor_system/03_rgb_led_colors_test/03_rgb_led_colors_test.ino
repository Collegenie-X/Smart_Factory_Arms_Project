/**
 * RGB LED 색상 테스트 프로그램
 * 
 * 기능: NeoPixel LED로 다양한 색상 표시
 * - 15가지 색상 순차 표시
 * - 무지개 효과
 */

/* ===== 라이브러리 ===== */
#include <Adafruit_NeoPixel.h>

/* ===== 핀 번호 설정 ===== */
#define PIN_LED             6    // LED 데이터 핀

/* ===== LED 설정 ===== */
#define NUM_PIXELS          3     // LED 개수
#define BRIGHTNESS          255   // 밝기 (0~255)

/* ===== 타이밍 ===== */
#define COLOR_DELAY         1000  // 색상 표시 시간(ms)

/* ===== 전역 변수 ===== */
Adafruit_NeoPixel led = Adafruit_NeoPixel(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);

/**
 * 초기화
 */
void setup() {
  Serial.begin(9600);
  
  led.begin();
  led.setBrightness(BRIGHTNESS);
  led.show();  // 모든 LED 끄기
  
  Serial.println("========================================");
  Serial.println("  RGB LED 색상 테스트 시작");
  Serial.println("========================================");
  Serial.print("LED 개수: ");
  Serial.println(NUM_PIXELS);
  Serial.println("========================================\n");
  
  delay(1000);
}

/**
 * 메인 루프
 */
void loop() {
  Serial.println("========================================");
  Serial.println("[테스트 1] 기본 색상 표시 (15가지)");
  Serial.println("========================================");
  
  // 15가지 색상 배열 (R, G, B)
  const int colors[15][3] = {
    {255, 0,   0},    // 1. 빨간색
    {0,   255, 0},    // 2. 초록색
    {0,   0,   255},  // 3. 파란색
    {255, 255, 0},    // 4. 노란색
    {255, 0,   255},  // 5. 자홍색
    {0,   255, 255},  // 6. 청록색
    {255, 255, 255},  // 7. 흰색
    {255, 128, 0},    // 8. 주황색
    {128, 0,   128},  // 9. 보라색
    {255, 192, 203},  // 10. 분홍색
    {0,   128, 0},    // 11. 진한 초록
    {0,   0,   128},  // 12. 남색
    {128, 128, 0},    // 13. 올리브
    {255, 215, 0},    // 14. 금색
    {192, 192, 192}   // 15. 은색
  };
  
  const char* names[15] = {
    "빨간색", "초록색", "파란색", "노란색", "자홍색",
    "청록색", "흰색", "주황색", "보라색", "분홍색",
    "진한 초록", "남색", "올리브", "금색", "은색"
  };
  
  // 색상 순차 표시
  for (int i = 0; i < 15; i++) {
    Serial.print("색상 ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(names[i]);
    
    // 모든 LED에 같은 색상 표시
    for (int j = 0; j < NUM_PIXELS; j++) {
      led.setPixelColor(j, led.Color(colors[i][0], colors[i][1], colors[i][2]));
    }
    led.show();
    delay(COLOR_DELAY);
  }
  
  Serial.println();
  delay(2000);
  
  // 무지개 효과
  // Serial.println("========================================");
  // Serial.println("[테스트 2] 무지개 효과");
  // Serial.println("========================================");
  
  // for (int cycle = 0; cycle < 3; cycle++) {
  //   for (int hue = 0; hue < 65536; hue += 256) {
  //     for (int i = 0; i < NUM_PIXELS; i++) {
  //       int pixelHue = hue + (i * 65536L / NUM_PIXELS);
  //       led.setPixelColor(i, led.gamma32(led.ColorHSV(pixelHue)));
  //     }
  //     led.show();
  //     delay(20);
  //   }
  // }
  
  // // LED 끄기
  // for (int i = 0; i < NUM_PIXELS; i++) {
  //   led.setPixelColor(i, led.Color(0, 0, 0));
  // }
  // led.show();
  
  // Serial.println("무지개 효과 완료\n");
  // Serial.println("모든 테스트 완료. 5초 후 재시작...\n");
  // delay(5000);
}
