/**
 * 스마트 팩토리 Serial + Bluetooth 제어 시스템
 * 
 * 동작 순서:
 * 1. Serial 또는 Bluetooth 명령 대기
 * 2. start_ 명령 시 자동화 시작
 * 3. 제품 감지 및 색상 분석
 * 4. 색상별 분류 및 카운터 증가
 * 5. 카운터 값 전송 (red1_, green1_, blue1_)
 * 6. stop_ 명령 시 자동화 중지
 * 7. init_ 명령 시 카운터 초기화
 * 
 * 명령어:
 * - start_ : 자동화 시작
 * - stop_  : 자동화 중지
 * - init_  : 카운터 초기화
 * 
 * Bluetooth: TX=2, RX=2
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
#define PIN_BT_RX           2     // Bluetooth RX

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
SoftwareSerial bluetooth(PIN_BT_RX, PIN_BT_TX);

/* ===== 전역 변수 ===== */
uint16_t rawR, rawG, rawB, rawC;
int r, g, b;
int productCount = 0;
int redCount = 0;      // 빨간색 제품 카운터
int greenCount = 0;    // 초록색 제품 카운터
int blueCount = 0;     // 파란색 제품 카운터
bool autoMode = false; // 자동화 모드 (기본: 중지)

// Serial 명령 버퍼
char serialCmdBuffer[CMD_BUFFER_SIZE];
int serialCmdIndex = 0;

// Bluetooth 명령 버퍼
char btCmdBuffer[CMD_BUFFER_SIZE];
int btCmdIndex = 0;

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
    printToBoth("[오류] 컬러 센서를 찾을 수 없습니다!");
    while (1) delay(1000);
  }
  
  // LED 초기화
  led.begin();
  led.setBrightness(LED_BRIGHTNESS);
  led.show();
  
  // 시작 메시지
  printToBoth("\n========================================");
  printToBoth("  Serial + Bluetooth 제어 시스템");
  printToBoth("========================================");
  printToBoth("초기화 완료");
  printToBoth("\n명령어:");
  printToBoth("  start_ : 자동화 시작");
  printToBoth("  stop_  : 자동화 중지");
  printToBoth("  init_  : 카운터 초기화");
  printToBoth("========================================");
  printToBoth("명령 대기중...\n");
  
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
  printToBoth("========================================");
  printToBoth("제품 #" + String(productCount) + " 감지됨!");
  printToBoth("========================================");
  
  // 3. 컨베이어 일시 정지
  analogWrite(PIN_MOTOR_SPEED, 0);
  tone(PIN_BUZZER, 523, 50);
  delay(100);
  tone(PIN_BUZZER, 784, 50);
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
  tone(PIN_BUZZER, 523, 50);
  delay(100);
  tone(PIN_BUZZER, 784, 50);
  delay(100);
  
  // 6. 색상 정보 출력
  printToBoth("--- 색상 분석 결과 ---");
  printToBoth("Raw -> R: " + String(rawR) + ", G: " + String(rawG) + ", B: " + String(rawB));
  printToBoth("RGB -> R: " + String(r) + ", G: " + String(g) + ", B: " + String(b));
  
  // 7. 색상 판별 및 분류
  int ledR = 0, ledG = 0, ledB = 0;
  int servoAngle = ANGLE_BLUE;
  String colorName = "알 수 없음";
  String countMsg = "";
  
  if (r > g && r > b) {
    // 빨간색
    colorName = "빨간색";
    servoAngle = ANGLE_RED;
    ledR = 255; ledG = 0; ledB = 0;
    redCount++;
    countMsg = "red" + String(redCount) + "_";
  } 
  else if (g > r && g > b) {
    // 초록색
    colorName = "초록색";
    servoAngle = ANGLE_GREEN;
    ledR = 0; ledG = 255; ledB = 0;
    greenCount++;
    countMsg = "green" + String(greenCount) + "_";
  } 
  else {
    // 파란색
    colorName = "파란색";
    servoAngle = ANGLE_BLUE;
    ledR = 0; ledG = 0; ledB = 255;
    blueCount++;
    countMsg = "blue" + String(blueCount) + "_";
  }
  
  printToBoth("판별 색상: " + colorName);
  printToBoth("---------------------");
  
  // 8. 서보 모터로 분류 방향 설정
  servo.attach(PIN_SERVO);
  servo.write(servoAngle);
  if (!delayWithCommandCheck(500)) {
    servo.detach();
    return;
  }
  
  // 9. RGB LED 색상 표시
  for (int i = 0; i < NUM_PIXELS; i++) {
    led.setPixelColor(i, led.Color(ledR, ledG, ledB));
  }
  led.show();
  
  // 10. 카운터 정보 전송
  printToBoth("[카운터 전송] " + countMsg);
  
  if (!delayWithCommandCheck(DELAY_COLOR_DETECT)) {
    servo.detach();
    return;
  }
  
  // 11. 서보 분리 및 컨베이어 재가동
  servo.detach();
  analogWrite(PIN_MOTOR_SPEED, MOTOR_SPEED);
  if (!delayWithCommandCheck(DELAY_NEXT_PRODUCT)) return;
  
  printToBoth("제품 처리 완료\n");
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
 * Serial과 Bluetooth 둘 다 출력
 */
void printToBoth(String msg) {
  Serial.println(msg);
  bluetooth.println(msg);
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
void processCommand(char* cmd, String source) {
  printToBoth("[명령 수신 from " + source + "] " + String(cmd));
  
  // start 명령
  if (strcmp(cmd, "start") == 0) {
    if (autoMode) {
      printToBoth("[알림] 이미 자동화 모드가 실행 중입니다.");
      return;
    }
    
    autoMode = true;
    analogWrite(PIN_MOTOR_SPEED, MOTOR_SPEED);
    
    printToBoth("========================================");
    printToBoth("  자동화 시작");
    printToBoth("========================================");
    printToBoth("컨베이어 가동 시작\n");
    
    tone(PIN_BUZZER, 523, 100);
    delay(150);
    tone(PIN_BUZZER, 659, 100);
    delay(150);
    return;
  }
  
  // stop 명령
  if (strcmp(cmd, "stop") == 0) {
    if (!autoMode) {
      printToBoth("[알림] 이미 자동화 모드가 중지되어 있습니다.");
      return;
    }
    
    autoMode = false;
    analogWrite(PIN_MOTOR_SPEED, 0);
    
    // LED 끄기
    for (int i = 0; i < NUM_PIXELS; i++) {
      led.setPixelColor(i, led.Color(0, 0, 0));
    }
    led.show();
    
    printToBoth("========================================");
    printToBoth("  자동화 중지");
    printToBoth("========================================");
    printToBoth("컨베이어 정지\n");
    printToBoth("명령 대기중...\n");
    
    tone(PIN_BUZZER, 659, 100);
    delay(150);
    tone(PIN_BUZZER, 523, 100);
    delay(150);
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
      
      printToBoth("[알림] 자동화 모드를 먼저 중지합니다.");
      delay(500);
    }
    
    // 카운터 초기화
    redCount = 0;
    greenCount = 0;
    blueCount = 0;
    productCount = 0;
    
    printToBoth("========================================");
    printToBoth("  카운터 초기화");
    printToBoth("========================================");
    printToBoth("빨간색: 0");
    printToBoth("초록색: 0");
    printToBoth("파란색: 0");
    printToBoth("전체 제품: 0");
    printToBoth("========================================");
    printToBoth("명령 대기중...\n");
    
    tone(PIN_BUZZER, 784, 100);
    delay(150);
    tone(PIN_BUZZER, 523, 100);
    delay(150);
    tone(PIN_BUZZER, 784, 100);
    delay(150);
    return;
  }
  
  // 알 수 없는 명령
  printToBoth("[오류] 알 수 없는 명령: " + String(cmd));
  printToBoth("사용 가능: start_, stop_, init_\n");
}
