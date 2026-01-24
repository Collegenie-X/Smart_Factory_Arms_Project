/**
 * 스마트 팩토리 Serial 제어 컨베이어 시스템
 * 
 * 동작 순서:
 * 1. Serial 명령 대기 (stop_, start_)
 * 2. start_ 명령 시 자동화 시작
 * 3. 컨베이어 벨트 가동
 * 4. 적외선 센서로 제품 감지 → 일시 정지
 * 5. 컬러 센서까지 이동
 * 6. 색상 분석 (빨강/초록/파랑)
 * 7. 서보 모터로 분류 방향 설정
 * 8. RGB LED로 색상 표시
 * 9. 컨베이어 벨트 재가동 → 제품 분류
 * 10. stop_ 명령 시 자동화 중지
 * 
 * 명령어:
 * - start_ : 자동화 시작
 * - stop_  : 자동화 중지
 */

/* ===== 라이브러리 ===== */
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TCS34725.h>
#include <Servo.h>
#include <Wire.h>

/* ===== 핀 번호 설정 ===== */
#define PIN_MOTOR_DIR       13    // DC 모터 방향
#define PIN_MOTOR_SPEED     11    // DC 모터 속도 (PWM)
#define PIN_SERVO           9     // 서보 모터
#define PIN_LED             5     // RGB LED
#define PIN_IR_SENSOR       A0    // 적외선 센서
#define PIN_BUZZER          4     // 부저

/* ===== 서보 각도 설정 ===== */
#define ANGLE_RED           30    // 빨간색 제품
#define ANGLE_GREEN         57    // 초록색 제품
#define ANGLE_BLUE          2     // 파란색 제품

/* ===== LED 설정 ===== */
#define NUM_PIXELS          3     // LED 개수
#define LED_BRIGHTNESS      255   // LED 밝기

/* ===== 색상 센서 설정 ===== */
#define RAW_MAX             21504 // 센서 최대값
#define MAPPED_MAX          1000  // 매핑 최대값
#define MIN_SUM             15    // 유효 색상 최소값

/* ===== 모터 설정 ===== */
#define MOTOR_SPEED         120   // 컨베이어 속도
#define MOTOR_DIR_FORWARD   HIGH  // 전진 방향

/* ===== 타이밍 설정 (ms) ===== */
#define DELAY_IR_DETECT     2000  // 적외선 감지 후 대기
#define DELAY_COLOR_DETECT  1500  // 색상 분석 후 대기
#define DELAY_NEXT_PRODUCT  1000  // 다음 제품 대기

/* ===== 명령 버퍼 설정 ===== */
#define CMD_BUFFER_SIZE     20    // 명령 버퍼 크기

/* ===== 전역 객체 ===== */
Adafruit_TCS34725 colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Servo servo;
Adafruit_NeoPixel led = Adafruit_NeoPixel(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);

/* ===== 전역 변수 ===== */
uint16_t rawR, rawG, rawB, rawC;
int r, g, b;
int productCount = 0;
bool autoMode = false;  // 자동화 모드 (기본: 중지)
char cmdBuffer[CMD_BUFFER_SIZE];
int cmdIndex = 0;

/**
 * 초기화
 */
void setup() {
  Serial.begin(9600);
  
  // 핀 모드 설정
  pinMode(PIN_MOTOR_DIR, OUTPUT);
  pinMode(PIN_MOTOR_SPEED, OUTPUT);
  pinMode(PIN_IR_SENSOR, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  // 모터 초기화
  digitalWrite(PIN_MOTOR_DIR, MOTOR_DIR_FORWARD);
  analogWrite(PIN_MOTOR_SPEED, 0);
  
  // 서보 초기화
  servo.attach(PIN_SERVO);
  servo.write(ANGLE_RED);
  delay(500);
  servo.write(ANGLE_GREEN);
  delay(500);
  servo.write(ANGLE_BLUE);
  servo.detach();
  
  // 컬러 센서 초기화
  if (!colorSensor.begin()) {
    Serial.println("[오류] 컬러 센서를 찾을 수 없습니다!");
    while (1) delay(1000);
  }
  
  // LED 초기화
  led.begin();
  led.setBrightness(LED_BRIGHTNESS);
  led.show();
  
  // 시작 메시지
  Serial.println("\n========================================");
  Serial.println("  Serial 제어 컨베이어 시스템");
  Serial.println("========================================");
  Serial.println("초기화 완료");
  Serial.println("\n명령어:");
  Serial.println("  start_ : 자동화 시작");
  Serial.println("  stop_  : 자동화 중지");
  Serial.println("========================================");
  Serial.println("명령 대기중...\n");
  
  // 시작 알림음
  tone(PIN_BUZZER, 523, 100);
  delay(150);
  tone(PIN_BUZZER, 659, 100);
  delay(150);
}

/**
 * 메인 루프
 */
void loop() {
  // Serial 명령 처리
  checkSerialCommand();
  
  // 자동화 모드가 아니면 대기
  if (!autoMode) {
    return;
  }
  
  // 1. 제품 감지 확인 (LOW = 감지됨)
  if (digitalRead(PIN_IR_SENSOR) == HIGH) {
    return;  // 제품 없음 → 루프 재시작
  }
  
  // 2. 제품 감지됨!
  productCount++;
  Serial.println("========================================");
  Serial.print("제품 #");
  Serial.print(productCount);
  Serial.println(" 감지됨!");
  Serial.println("========================================");
  
  // 3. 컨베이어 일시 정지
  analogWrite(PIN_MOTOR_SPEED, 0);
  tone(PIN_BUZZER, 523, 50);
  delay(100);
  tone(PIN_BUZZER, 784, 50);
  delay(100);
  
  // DELAY_IR_DETECT 동안 Serial 체크
  if (!delayWithSerialCheck(DELAY_IR_DETECT)) return;
  
  // 4. 컬러 센서까지 이동
  analogWrite(PIN_MOTOR_SPEED, MOTOR_SPEED);
  
  int sum = 0;
  int attempts = 0;
  do {
    // Serial 명령 체크 (stop 명령 즉시 반응)
    checkSerialCommand();
    if (!autoMode) {
      analogWrite(PIN_MOTOR_SPEED, 0);
      return;
    }
    
    colorSensor.getRawData(&rawR, &rawG, &rawB, &rawC);
    r = map(rawR, 0, RAW_MAX, 0, MAPPED_MAX);
    g = map(rawG, 0, RAW_MAX, 0, MAPPED_MAX);
    b = map(rawB, 0, RAW_MAX, 0, MAPPED_MAX);
    sum = r + g + b;
    
    attempts++;
    if (attempts > 1000) break;  // 타임아웃 방지
    delay(10);
  } while (sum < MIN_SUM);
  
  // 5. 컨베이어 정지 (색상 분석)
  analogWrite(PIN_MOTOR_SPEED, 0);
  tone(PIN_BUZZER, 523, 50);
  delay(100);
  tone(PIN_BUZZER, 784, 50);
  delay(100);
  
  // 6. 색상 정보 출력
  Serial.println("--- 색상 분석 결과 ---");
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
  
  // 7. 색상 판별 및 분류
  int ledR = 0, ledG = 0, ledB = 0;
  int servoAngle = ANGLE_BLUE;
  const char* colorName = "알 수 없음";
  
  if (r > g && r > b) {
    // 빨간색
    colorName = "빨간색";
    servoAngle = ANGLE_RED;
    ledR = 255; ledG = 0; ledB = 0;
  } 
  else if (g > r && g > b) {
    // 초록색
    colorName = "초록색";
    servoAngle = ANGLE_GREEN;
    ledR = 0; ledG = 255; ledB = 0;
  } 
  else {
    // 파란색
    colorName = "파란색";
    servoAngle = ANGLE_BLUE;
    ledR = 0; ledG = 0; ledB = 255;
  }
  
  Serial.print("판별 색상: ");
  Serial.println(colorName);
  Serial.println("---------------------");
  
  // 8. 서보 모터로 분류 방향 설정
  servo.attach(PIN_SERVO);
  servo.write(servoAngle);
  if (!delayWithSerialCheck(500)) {
    servo.detach();
    return;
  }
  
  // 9. RGB LED 색상 표시
  for (int i = 0; i < NUM_PIXELS; i++) {
    led.setPixelColor(i, led.Color(ledR, ledG, ledB));
  }
  led.show();
  
  if (!delayWithSerialCheck(DELAY_COLOR_DETECT)) {
    servo.detach();
    return;
  }
  
  // 10. 서보 분리 및 컨베이어 재가동
  servo.detach();
  analogWrite(PIN_MOTOR_SPEED, MOTOR_SPEED);
  if (!delayWithSerialCheck(DELAY_NEXT_PRODUCT)) return;
  
  Serial.println("제품 처리 완료\n");
}

/**
 * delay 동안 Serial 명령 체크
 * @param ms 대기 시간 (밀리초)
 * @return autoMode가 계속 true면 true, false로 변경되면 false
 */
bool delayWithSerialCheck(unsigned long ms) {
  unsigned long startTime = millis();
  while (millis() - startTime < ms) {
    checkSerialCommand();
    if (!autoMode) {
      return false;  // stop 명령 받음
    }
    delay(50);  // 50ms 간격으로 체크
  }
  return true;  // 정상 완료
}

/**
 * Serial 명령 확인 및 처리
 */
void checkSerialCommand() {
  // Serial 데이터가 없으면 리턴
  if (Serial.available() <= 0) {
    return;
  }
  
  // 문자 읽기
  char ch = Serial.read();
  
  // 개행 문자 무시
  if (ch == '\n' || ch == '\r') {
    return;
  }
  
  // '_' 문자를 만나면 명령 처리
  if (ch == '_') {
    cmdBuffer[cmdIndex] = '\0';  // 문자열 종료
    processCommand();
    cmdIndex = 0;  // 버퍼 초기화
    return;
  }
  
  // 버퍼에 문자 추가
  if (cmdIndex < CMD_BUFFER_SIZE - 1) {
    cmdBuffer[cmdIndex] = ch;
    cmdIndex++;
  }
}

/**
 * 명령 처리
 */
void processCommand() {
  Serial.print("[명령 수신] ");
  Serial.println(cmdBuffer);
  
  // start 명령
  if (strcmp(cmdBuffer, "start") == 0) {
    if (autoMode) {
      Serial.println("[알림] 이미 자동화 모드가 실행 중입니다.");
      return;
    }
    
    autoMode = true;
    analogWrite(PIN_MOTOR_SPEED, MOTOR_SPEED);
    
    Serial.println("========================================");
    Serial.println("  자동화 시작");
    Serial.println("========================================");
    Serial.println("컨베이어 가동 시작\n");
    
    tone(PIN_BUZZER, 523, 100);
    delay(150);
    tone(PIN_BUZZER, 659, 100);
    delay(150);
    return;
  }
  
  // stop 명령
  if (strcmp(cmdBuffer, "stop") == 0) {
    if (!autoMode) {
      Serial.println("[알림] 이미 자동화 모드가 중지되어 있습니다.");
      return;
    }
    
    autoMode = false;
    analogWrite(PIN_MOTOR_SPEED, 0);
    
    // LED 끄기
    for (int i = 0; i < NUM_PIXELS; i++) {
      led.setPixelColor(i, led.Color(0, 0, 0));
    }
    led.show();
    
    Serial.println("========================================");
    Serial.println("  자동화 중지");
    Serial.println("========================================");
    Serial.println("컨베이어 정지\n");
    Serial.println("명령 대기중...\n");
    
    tone(PIN_BUZZER, 659, 100);
    delay(150);
    tone(PIN_BUZZER, 523, 100);
    delay(150);
    return;
  }
  
  // 알 수 없는 명령
  Serial.print("[오류] 알 수 없는 명령: ");
  Serial.println(cmdBuffer);
  Serial.println("사용 가능: start_, stop_\n");
}
