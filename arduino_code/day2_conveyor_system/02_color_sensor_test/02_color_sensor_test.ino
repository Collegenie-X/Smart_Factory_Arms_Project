/**
 * 컬러 센서 테스트 프로그램
 * 
 * 기능: TCS34725 센서로 색상 측정 및 판별
 * - RGB 값 측정 및 출력
 * - 색상 자동 판별 (빨강, 초록, 파랑, 노랑)
 */

/* ===== 라이브러리 ===== */
#include <Wire.h>
#include <Adafruit_TCS34725.h>

/* ===== 핀 번호 설정 ===== */
#define PIN_BUZZER          4     // 부저 핀
#define PIN_LED             13    // LED 핀

/* ===== 센서 설정 ===== */
#define RAW_MAX             21504 // 센서 최대값
#define MAPPED_MAX          1000  // 매핑 최대값
#define MIN_SUM             15    // 유효 색상 최소 합계

/* ===== 타이밍 ===== */
#define MEASURE_DELAY       500   // 측정 간격(ms)

/* ===== 전역 변수 ===== */
Adafruit_TCS34725 colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

uint16_t rawR, rawG, rawB, rawC;  // Raw 값
int r, g, b;                       // 매핑된 값
int lastColor = 0;                 // 이전 색상 (0=없음, 1=빨강, 2=초록, 3=파랑, 4=노랑)

/**
 * 초기화
 */
void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  
  // 센서 초기화
  if (!colorSensor.begin()) {
    Serial.println("========================================");
    Serial.println("  [오류] 컬러 센서를 찾을 수 없습니다!");
    Serial.println("========================================");
    
    // 오류 표시 (LED 깜빡임)
    while (1) {
      digitalWrite(PIN_LED, HIGH);
      delay(200);
      digitalWrite(PIN_LED, LOW);
      delay(200);
    }
  }
  
  digitalWrite(PIN_LED, HIGH);
  
  Serial.println("========================================");
  Serial.println("  컬러 센서 테스트 시작");
  Serial.println("========================================");
  Serial.println("센서 초기화 완료");
  Serial.println("색상 측정을 시작합니다...\n");
}

/**
 * 메인 루프
 */
void loop() {
  // 1. 센서에서 RGB 값 읽기
  colorSensor.getRawData(&rawR, &rawG, &rawB, &rawC);
  
  // 2. 0~1000 범위로 매핑
  r = map(rawR, 0, RAW_MAX, 0, MAPPED_MAX);
  g = map(rawG, 0, RAW_MAX, 0, MAPPED_MAX);
  b = map(rawB, 0, RAW_MAX, 0, MAPPED_MAX);
  
  int sum = r + g + b;
  
  // 3. 결과 출력
  Serial.println("----------------------------------------");
  Serial.print("Raw -> R: ");
  Serial.print(rawR);
  Serial.print(", G: ");
  Serial.print(rawG);
  Serial.print(", B: ");
  Serial.println(rawB);
  
  Serial.print("RGB -> R: ");
  Serial.print(r);
  Serial.print(", G: ");
  Serial.print(g);
  Serial.print(", B: ");
  Serial.println(b);
  
  Serial.print("합계: ");
  Serial.println(sum);
  
  // 4. 색상 판별
  int currentColor = 0;  // 0=없음
  const char* colorName = "없음";
  
  if (sum >= MIN_SUM) {
    // 노란색 판별: R과 G가 모두 높고 비슷하며, B가 낮은 경우
    if (r > b && g > b && abs(r - g) < (r + g) / 4) {
      currentColor = 4;  // 노랑
      colorName = "노란색";
    } else if (r > g && r > b) {
      currentColor = 1;  // 빨강
      colorName = "빨간색";
    } else if (g > r && g > b) {
      currentColor = 2;  // 초록
      colorName = "초록색";
    } else if (b > r && b > g) {
      currentColor = 3;  // 파랑
      colorName = "파란색";
    } else {
      colorName = "알 수 없음";
    }
  }
  
  Serial.print("판별 색상: ");
  Serial.println(colorName);
  
  // 5. 색상 변화 시 부저 울림
  if (currentColor > 0 && currentColor != lastColor) {
    int frequency = 523;  // 기본 도 음
    if (currentColor == 1) frequency = 523;  // 빨강: 도
    if (currentColor == 2) frequency = 659;  // 초록: 미
    if (currentColor == 3) frequency = 784;  // 파랑: 솔
    if (currentColor == 4) frequency = 698;  // 노랑: 파
    
    tone(PIN_BUZZER, frequency, 50);
    Serial.println("\n>>> 색상 변화 감지! <<<\n");
  }
  
  lastColor = currentColor;
  delay(MEASURE_DELAY);
}
