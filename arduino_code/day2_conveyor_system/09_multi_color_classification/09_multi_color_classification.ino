/**
 * 스마트 팩토리 다중 색상 분류 시스템 (5색상 → 3공간)
 * 
 * 동작 순서:
 * 1. Serial 또는 Bluetooth 명령 대기
 * 2. start_ 명령 시 자동화 시작
 * 3. 제품 감지 및 색상 분석 (5가지 색상)
 * 4. 색상별 분류 및 카운터 증가
 * 5. 카운터 값 전송
 * 6. stop_ 명령 시 자동화 중지
 * 7. init_ 명령 시 카운터 초기화
 * 
 * 색상 인식 (5가지):
 * - 빨간색 (Red)
 * - 초록색 (Green)
 * - 파란색 (Blue)
 * - 노란색 (Yellow) - 빨강+초록
 * - 흰색 (White) - 전체 밝기 높음
 * 
 * 분류 공간 (3개):
 * - 공간 A (30°): 빨간색 + 노란색
 * - 공간 B (57°): 초록색 + 흰색
 * - 공간 C (2°):  파란색
 * 
 * 명령어:
 * - start_ : 자동화 시작
 * - stop_  : 자동화 중지
 * - init_  : 카운터 초기화
 * - count_ : 카운터 조회
 * 
 * Bluetooth: TX=2, RX=3
 */

/* ===== 라이브러리 ===== */
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TCS34725.h>
#include <Servo.h>
#include <Wire.h>
#include <SoftwareSerial.h>

/* ===== 핀 번호 설정 ===== */
#define PIN_MOTOR_DIR       13    // DC 모터 방향
#define PIN_MOTOR_SPEED     11    // DC 모터 속도 (PWM)
#define PIN_SERVO           9     // 서보 모터
#define PIN_LED             5     // RGB LED
#define PIN_IR_SENSOR       A0    // 적외선 센서
#define PIN_BUZZER          4     // 부저
#define PIN_BT_TX           2     // Bluetooth TX
#define PIN_BT_RX           3     // Bluetooth RX

/* ===== 서보 각도 설정 (3개 공간) ===== */
#define ANGLE_SPACE_A       30    // 공간 A: 빨간색, 노란색
#define ANGLE_SPACE_B       57    // 공간 B: 초록색, 흰색
#define ANGLE_SPACE_C       2     // 공간 C: 파란색

/* ===== LED 설정 ===== */
#define NUM_PIXELS          3     // LED 개수
#define LED_BRIGHTNESS      255   // LED 밝기

/* ===== 색상 센서 설정 ===== */
#define RAW_MAX             21504 // 센서 최대값
#define MAPPED_MAX          1000  // 매핑 최대값
#define MIN_SUM             15    // 유효 색상 최소값

/* ===== 색상 인식 임계값 ===== */
#define THRESHOLD_WHITE     700   // 흰색 판별 기준 (전체 밝기)
#define THRESHOLD_YELLOW_R  400   // 노란색 빨강 최소값
#define THRESHOLD_YELLOW_G  400   // 노란색 초록 최소값
#define THRESHOLD_YELLOW_B  200   // 노란색 파랑 최대값

/* ===== 모터 설정 ===== */
#define MOTOR_SPEED         120   // 컨베이어 속도
#define MOTOR_DIR_FORWARD   HIGH  // 전진 방향

/* ===== 타이밍 설정 (ms) ===== */
#define DELAY_IR_DETECT     2000  // 적외선 감지 후 대기
#define DELAY_COLOR_DETECT  1500  // 색상 분석 후 대기
#define DELAY_NEXT_PRODUCT  1000  // 다음 제품 대기

/* ===== 명령 버퍼 설정 ===== */
#define CMD_BUFFER_SIZE     20    // 명령 버퍼 크기

/* ===== 색상 타입 열거형 ===== */
enum ColorType {
  COLOR_RED,      // 빨간색
  COLOR_GREEN,    // 초록색
  COLOR_BLUE,     // 파란색
  COLOR_YELLOW,   // 노란색
  COLOR_WHITE,    // 흰색
  COLOR_UNKNOWN   // 알 수 없음
};

/* ===== 전역 객체 ===== */
Adafruit_TCS34725 colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Servo servo;
Adafruit_NeoPixel led = Adafruit_NeoPixel(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);
SoftwareSerial bluetooth(PIN_BT_RX, PIN_BT_TX);

/* ===== 전역 변수 ===== */
uint16_t rawR, rawG, rawB, rawC;
int r, g, b;
int productCount = 0;

// 색상별 카운터 (5가지)
int redCount = 0;
int greenCount = 0;
int blueCount = 0;
int yellowCount = 0;
int whiteCount = 0;

bool autoMode = false; // 자동화 모드 (기본: 중지)

// Serial 명령 버퍼
char serialCmdBuffer[CMD_BUFFER_SIZE];
int serialCmdIndex = 0;

// Bluetooth 명령 버퍼
char btCmdBuffer[CMD_BUFFER_SIZE];
int btCmdIndex = 0;

/**
 * Serial과 Bluetooth 둘 다 출력 (F() 매크로 사용)
 */
void printToBoth(const __FlashStringHelper* msg) {
  Serial.println(msg);
  bluetooth.println(msg);
}

/**
 * Serial과 Bluetooth 둘 다 출력 (char* 사용)
 */
void printToBoth(const char* msg) {
  Serial.println(msg);
  bluetooth.println(msg);
}

/**
 * 색상 판별 함수 (5가지 색상)
 * @param r 빨강 값 (0~1000)
 * @param g 초록 값 (0~1000)
 * @param b 파랑 값 (0~1000)
 * @return 색상 타입
 */
ColorType detectColor(int r, int g, int b) {
  int sum = r + g + b;
  
  // 1. 흰색 판별 (전체 밝기가 높음)
  if (sum > THRESHOLD_WHITE && r > 200 && g > 200 && b > 200) {
    return COLOR_WHITE;
  }
  
  // 2. 노란색 판별 (빨강+초록 높고, 파랑 낮음)
  if (r > THRESHOLD_YELLOW_R && g > THRESHOLD_YELLOW_G && b < THRESHOLD_YELLOW_B) {
    return COLOR_YELLOW;
  }
  
  // 3. 빨간색 판별
  if (r > g && r > b) {
    return COLOR_RED;
  }
  
  // 4. 초록색 판별
  if (g > r && g > b) {
    return COLOR_GREEN;
  }
  
  // 5. 파란색 판별
  if (b > r && b > g) {
    return COLOR_BLUE;
  }
  
  return COLOR_UNKNOWN;
}

/**
 * 색상 타입에 따른 정보 반환
 * @param color 색상 타입
 * @param name 색상 이름 (출력용)
 * @param angle 서보 각도 (출력용)
 * @param ledR LED 빨강 값 (출력용)
 * @param ledG LED 초록 값 (출력용)
 * @param ledB LED 파랑 값 (출력용)
 */
void getColorInfo(ColorType color, const char** name, int* angle, int* ledR, int* ledG, int* ledB) {
  switch (color) {
    case COLOR_RED:
      *name = "빨간색";
      *angle = ANGLE_SPACE_A;
      *ledR = 255; *ledG = 0; *ledB = 0;
      break;
      
    case COLOR_GREEN:
      *name = "초록색";
      *angle = ANGLE_SPACE_B;
      *ledR = 0; *ledG = 255; *ledB = 0;
      break;
      
    case COLOR_BLUE:
      *name = "파란색";
      *angle = ANGLE_SPACE_C;
      *ledR = 0; *ledG = 0; *ledB = 255;
      break;
      
    case COLOR_YELLOW:
      *name = "노란색";
      *angle = ANGLE_SPACE_A;  // 공간 A (빨간색과 같은 공간)
      *ledR = 255; *ledG = 255; *ledB = 0;
      break;
      
    case COLOR_WHITE:
      *name = "흰색";
      *angle = ANGLE_SPACE_B;  // 공간 B (초록색과 같은 공간)
      *ledR = 255; *ledG = 255; *ledB = 255;
      break;
      
    default:
      *name = "알 수 없음";
      *angle = ANGLE_SPACE_C;
      *ledR = 50; *ledG = 50; *ledB = 50;
      break;
  }
}

/**
 * 초기화
 */
void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  
  // 핀 모드 설정
  pinMode(PIN_MOTOR_DIR, OUTPUT);
  pinMode(PIN_MOTOR_SPEED, OUTPUT);
  pinMode(PIN_IR_SENSOR, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  // 모터 초기화
  digitalWrite(PIN_MOTOR_DIR, MOTOR_DIR_FORWARD);
  analogWrite(PIN_MOTOR_SPEED, 0);
  
  // 서보 초기화 (3개 공간 확인)
  servo.attach(PIN_SERVO);
  servo.write(ANGLE_SPACE_A);
  delay(500);
  servo.write(ANGLE_SPACE_B);
  delay(500);
  servo.write(ANGLE_SPACE_C);
  servo.detach();
  
  // 컬러 센서 초기화
  if (!colorSensor.begin()) {
    printToBoth(F("[오류] 컬러 센서를 찾을 수 없습니다!"));
    while (1) delay(1000);
  }
  
  // LED 초기화
  led.begin();
  led.setBrightness(LED_BRIGHTNESS);
  led.show();
  
  // 시작 메시지
  printToBoth(F("\n========================================"));
  printToBoth(F("  다중 색상 분류 시스템 (5색→3공간)"));
  printToBoth(F("========================================"));
  printToBoth(F("초기화 완료"));
  printToBoth(F("\n인식 색상: 빨강, 초록, 파랑, 노랑, 흰색"));
  printToBoth(F("분류 공간:"));
  printToBoth(F("  공간 A (30°): 빨강 + 노랑"));
  printToBoth(F("  공간 B (57°): 초록 + 흰색"));
  printToBoth(F("  공간 C (2°):  파랑"));
  printToBoth(F("\n명령어:"));
  printToBoth(F("  start_ : 자동화 시작"));
  printToBoth(F("  stop_  : 자동화 중지"));
  printToBoth(F("  init_  : 카운터 초기화"));
  printToBoth(F("  count_ : 카운터 조회"));
  printToBoth(F("========================================"));
  printToBoth(F("명령 대기중...\n"));
  
  // 시작 알림 (LED 깜빡임)
  for (int i = 0; i < NUM_PIXELS; i++) {
    led.setPixelColor(i, led.Color(0, 255, 0));
  }
  led.show();
  delay(200);
  for (int i = 0; i < NUM_PIXELS; i++) {
    led.setPixelColor(i, 0);
  }
  led.show();
  
  Serial.println(F("[READY]"));
}

/**
 * 메인 루프
 */
void loop() {
  // Serial 및 Bluetooth 명령 처리
  checkSerialCommand();
  checkBluetoothCommand();
  
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
  printToBoth(F("========================================"));
  Serial.print(F("제품 #"));
  Serial.print(productCount);
  Serial.println(F(" 감지됨!"));
  bluetooth.print(F("제품 #"));
  bluetooth.print(productCount);
  bluetooth.println(F(" 감지됨!"));
  printToBoth(F("========================================"));
  
  // 3. 컨베이어 일시 정지
  analogWrite(PIN_MOTOR_SPEED, 0);
  delay(100);
  
  // DELAY_IR_DETECT 동안 명령 체크
  if (!delayWithCommandCheck(DELAY_IR_DETECT)) return;
  
  // 4. 컬러 센서까지 이동
  analogWrite(PIN_MOTOR_SPEED, MOTOR_SPEED);
  
  int sum = 0;
  int attempts = 0;
  do {
    // 명령 체크 (stop 명령 즉시 반응)
    checkSerialCommand();
    checkBluetoothCommand();
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
  delay(100);
  
  // 6. 색상 정보 출력
  printToBoth(F("--- 색상 분석 결과 ---"));
  
  Serial.print(F("Raw -> R: "));
  Serial.print(rawR);
  Serial.print(F(", G: "));
  Serial.print(rawG);
  Serial.print(F(", B: "));
  Serial.println(rawB);
  
  bluetooth.print(F("Raw -> R: "));
  bluetooth.print(rawR);
  bluetooth.print(F(", G: "));
  bluetooth.print(rawG);
  bluetooth.print(F(", B: "));
  bluetooth.println(rawB);
  
  Serial.print(F("RGB -> R: "));
  Serial.print(r);
  Serial.print(F(", G: "));
  Serial.print(g);
  Serial.print(F(", B: "));
  Serial.println(b);
  
  bluetooth.print(F("RGB -> R: "));
  bluetooth.print(r);
  bluetooth.print(F(", G: "));
  bluetooth.print(g);
  bluetooth.print(F(", B: "));
  bluetooth.println(b);
  
  // 7. 색상 판별 (5가지 색상)
  ColorType detectedColor = detectColor(r, g, b);
  
  // 8. 색상별 카운터 증가
  switch (detectedColor) {
    case COLOR_RED:
      redCount++;
      bluetooth.print(F("red"));
      bluetooth.print(redCount);
      bluetooth.println(F("_"));
      break;
      
    case COLOR_GREEN:
      greenCount++;
      bluetooth.print(F("green"));
      bluetooth.print(greenCount);
      bluetooth.println(F("_"));
      break;
      
    case COLOR_BLUE:
      blueCount++;
      bluetooth.print(F("blue"));
      bluetooth.print(blueCount);
      bluetooth.println(F("_"));
      break;
      
    case COLOR_YELLOW:
      yellowCount++;
      bluetooth.print(F("yellow"));
      bluetooth.print(yellowCount);
      bluetooth.println(F("_"));
      break;
      
    case COLOR_WHITE:
      whiteCount++;
      bluetooth.print(F("white"));
      bluetooth.print(whiteCount);
      bluetooth.println(F("_"));
      break;
      
    default:
      break;
  }
  
  // 9. 색상 정보 가져오기
  const char* colorName;
  int servoAngle, ledR, ledG, ledB;
  getColorInfo(detectedColor, &colorName, &servoAngle, &ledR, &ledG, &ledB);
  
  Serial.print(F("판별 색상: "));
  Serial.println(colorName);
  bluetooth.print(F("판별 색상: "));
  bluetooth.println(colorName);
  
  Serial.print(F("분류 공간: "));
  if (servoAngle == ANGLE_SPACE_A) {
    Serial.println(F("공간 A (30°)"));
    bluetooth.println(F("공간 A (30°)"));
  } else if (servoAngle == ANGLE_SPACE_B) {
    Serial.println(F("공간 B (57°)"));
    bluetooth.println(F("공간 B (57°)"));
  } else {
    Serial.println(F("공간 C (2°)"));
    bluetooth.println(F("공간 C (2°)"));
  }
  
  printToBoth(F("---------------------"));
  
  // 10. 서보 모터로 분류 방향 설정
  servo.attach(PIN_SERVO);
  servo.write(servoAngle);
  if (!delayWithCommandCheck(500)) {
    servo.detach();
    return;
  }
  
  // 11. RGB LED 색상 표시
  for (int i = 0; i < NUM_PIXELS; i++) {
    led.setPixelColor(i, led.Color(ledR, ledG, ledB));
  }
  led.show();
  
  if (!delayWithCommandCheck(DELAY_COLOR_DETECT)) {
    servo.detach();
    return;
  }
  
  // 12. 서보 분리 및 컨베이어 재가동
  servo.detach();
  analogWrite(PIN_MOTOR_SPEED, MOTOR_SPEED);
  if (!delayWithCommandCheck(DELAY_NEXT_PRODUCT)) return;
  
  printToBoth(F("제품 처리 완료\n"));
}

/**
 * delay 동안 명령 체크
 * @param ms 대기 시간 (밀리초)
 * @return autoMode가 계속 true면 true, false로 변경되면 false
 */
bool delayWithCommandCheck(unsigned long ms) {
  unsigned long startTime = millis();
  while (millis() - startTime < ms) {
    checkSerialCommand();
    checkBluetoothCommand();
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
  if (Serial.available() <= 0) {
    return;
  }
  
  char ch = Serial.read();
  
  // 개행 문자 무시
  if (ch == '\n' || ch == '\r') {
    return;
  }
  
  // '_' 문자를 만나면 명령 처리
  if (ch == '_') {
    serialCmdBuffer[serialCmdIndex] = '\0';  // 문자열 종료
    processCommand(serialCmdBuffer, "Serial");
    serialCmdIndex = 0;  // 버퍼 초기화
    return;
  }
  
  // 버퍼에 문자 추가
  if (serialCmdIndex < CMD_BUFFER_SIZE - 1) {
    serialCmdBuffer[serialCmdIndex] = ch;
    serialCmdIndex++;
  }
}

/**
 * Bluetooth 명령 확인 및 처리
 */
void checkBluetoothCommand() {
  if (bluetooth.available() <= 0) {
    return;
  }
  
  char ch = bluetooth.read();
  
  // 개행 문자 무시
  if (ch == '\n' || ch == '\r') {
    return;
  }
  
  // '_' 문자를 만나면 명령 처리
  if (ch == '_') {
    btCmdBuffer[btCmdIndex] = '\0';  // 문자열 종료
    processCommand(btCmdBuffer, "Bluetooth");
    btCmdIndex = 0;  // 버퍼 초기화
    return;
  }
  
  // 버퍼에 문자 추가
  if (btCmdIndex < CMD_BUFFER_SIZE - 1) {
    btCmdBuffer[btCmdIndex] = ch;
    btCmdIndex++;
  }
}

/**
 * 명령 처리
 * @param cmd 명령 문자열
 * @param source 명령 출처 ("Serial" 또는 "Bluetooth")
 */
void processCommand(char* cmd, const char* source) {
  // 시리얼에만 간단히 출력
  Serial.print(F("["));
  Serial.print(cmd);
  Serial.println(F("]"));
  
  // start 명령
  if (strcmp(cmd, "start") == 0) {
    if (autoMode) {
      printToBoth(F("[알림] 이미 자동화 모드가 실행 중입니다."));
      return;
    }
    
    autoMode = true;
    analogWrite(PIN_MOTOR_SPEED, MOTOR_SPEED);
    
    printToBoth(F("========================================"));
    printToBoth(F("  자동화 시작"));
    printToBoth(F("========================================"));
    printToBoth(F("컨베이어 가동 시작\n"));
    return;
  }
  
  // stop 명령
  if (strcmp(cmd, "stop") == 0) {
    if (!autoMode) {
      printToBoth(F("[알림] 이미 자동화 모드가 중지되어 있습니다."));
      return;
    }
    
    autoMode = false;
    analogWrite(PIN_MOTOR_SPEED, 0);
    
    // LED 끄기
    for (int i = 0; i < NUM_PIXELS; i++) {
      led.setPixelColor(i, led.Color(0, 0, 0));
    }
    led.show();
    
    printToBoth(F("========================================"));
    printToBoth(F("  자동화 중지"));
    printToBoth(F("========================================"));
    printToBoth(F("컨베이어 정지\n"));
    printToBoth(F("명령 대기중...\n"));
    return;
  }
  
  // init 명령 (카운터 초기화 + 시스템 중지)
  if (strcmp(cmd, "init") == 0) {
    // 시스템이 가동 중이면 먼저 중지
    if (autoMode) {
      autoMode = false;
      analogWrite(PIN_MOTOR_SPEED, 0);
      
      // LED 끄기
      for (int i = 0; i < NUM_PIXELS; i++) {
        led.setPixelColor(i, led.Color(0, 0, 0));
      }
      led.show();
      
      printToBoth(F("[알림] 자동화 모드를 먼저 중지합니다."));
      delay(500);
    }
    
    // 카운터 초기화 (5가지 색상)
    redCount = 0;
    greenCount = 0;
    blueCount = 0;
    yellowCount = 0;
    whiteCount = 0;
    productCount = 0;
    
    printToBoth(F("========================================"));
    printToBoth(F("  카운터 초기화"));
    printToBoth(F("========================================"));
    printToBoth(F("빨간색: 0"));
    printToBoth(F("초록색: 0"));
    printToBoth(F("파란색: 0"));
    printToBoth(F("노란색: 0"));
    printToBoth(F("흰색: 0"));
    printToBoth(F("전체 제품: 0"));
    printToBoth(F("========================================"));
    printToBoth(F("명령 대기중...\n"));
    return;
  }
  
  // count 명령 (카운터 조회) - 실행/정지 상태 관계없이 작동
  if (strcmp(cmd, "count") == 0) {
    // 시리얼 출력 (5가지 색상)
    Serial.print(F("R:"));
    Serial.print(redCount);
    Serial.print(F(" G:"));
    Serial.print(greenCount);
    Serial.print(F(" B:"));
    Serial.print(blueCount);
    Serial.print(F(" Y:"));
    Serial.print(yellowCount);
    Serial.print(F(" W:"));
    Serial.print(whiteCount);
    Serial.print(F(" ["));
    Serial.print(autoMode ? F("RUN") : F("STOP"));
    Serial.println(F("]"));
    Serial.flush();
    
    delay(20);
    
    // 블루투스 전송 (쉼표 구분, 5가지 색상)
    bluetooth.print(redCount);
    bluetooth.print(',');
    bluetooth.print(greenCount);
    bluetooth.print(',');
    bluetooth.print(blueCount);
    bluetooth.print(',');
    bluetooth.print(yellowCount);
    bluetooth.print(',');
    bluetooth.println(whiteCount);
    bluetooth.flush();
    
    delay(20);
    
    return;
  }
  
  // 알 수 없는 명령
  printToBoth(F("[오류] 알 수 없는 명령"));
  printToBoth(F("사용 가능: start_, stop_, init_, count_\n"));
}

