/*
 * ====================================================================
 * 파일명: 10_app_inventor_sensor_protocol.ino
 * 프로젝트: Smart Factory Arms Project
 * 설명: 앱인벤터 ↔ 아두이노 센서 통신 프로토콜 구현
 * 
 * 기능:
 *   - 색상 센서 데이터 전송 (RGB 값)
 *   - 블루투스 양방향 통신
 *   - 제어 명령 수신 (LED, 모터, 서보)
 *   - 프로토콜: SENSOR:R,G,B_ / CMD:명령_ / REQ:타입_
 * 
 * 작성일: 2026-01-31
 * 버전: 1.0
 * ====================================================================
 */

// ==================== 라이브러리 ====================
#include <SoftwareSerial.h>
#include <Servo.h>

// 색상 센서 사용 시 주석 해제
// #include <Wire.h>
// #include "Adafruit_TCS34725.h"

// ==================== 핀 정의 ====================
// 블루투스 모듈
#define BT_RX 10
#define BT_TX 11

// RGB LED (공통 캐소드 기준)
#define LED_R_PIN 9
#define LED_G_PIN 6
#define LED_B_PIN 5

// DC 모터 (L298N 기준)
#define MOTOR_PIN 3

// 서보 모터
#define SERVO_PIN 4

// 색상 센서 핀 (아날로그 시뮬레이션 - 실제 센서 시 주석 처리)
#define COLOR_R_PIN A0
#define COLOR_G_PIN A1
#define COLOR_B_PIN A2

// ==================== 설정 ====================
#define BAUD_RATE 9600
#define ENABLE_DEBUG 1          // 디버그 모드 (0: 운영, 1: 개발)
#define USE_REAL_SENSOR 0       // 실제 TCS34725 센서 사용 (0: 시뮬레이션, 1: 실제)

// ==================== 객체 생성 ====================
SoftwareSerial BTSerial(BT_RX, BT_TX);
Servo myServo;

// 실제 색상 센서 사용 시 주석 해제
// Adafruit_TCS34725 colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// ==================== 전역 변수 ====================
// 통신 버퍼
String receivedData = "";
bool dataComplete = false;

// 센서 데이터
uint8_t sensorRed = 0;
uint8_t sensorGreen = 0;
uint8_t sensorBlue = 0;

// 시스템 상태
bool motorState = false;
int servoAngle = 90;

// ==================== 초기화 ====================
void setup() {
  // 시리얼 통신 초기화
  Serial.begin(BAUD_RATE);
  BTSerial.begin(BAUD_RATE);
  
  // 핀 모드 설정
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  
  // 서보 초기화
  myServo.attach(SERVO_PIN);
  myServo.write(90);
  
  // 색상 센서 초기화
  #if USE_REAL_SENSOR
    if (colorSensor.begin()) {
      debugPrint("[시스템] 색상 센서 초기화 완료");
    } else {
      debugPrint("[에러] 색상 센서 연결 실패");
    }
  #else
    pinMode(COLOR_R_PIN, INPUT);
    pinMode(COLOR_G_PIN, INPUT);
    pinMode(COLOR_B_PIN, INPUT);
    debugPrint("[시스템] 시뮬레이션 모드");
  #endif
  
  // 초기 LED 테스트 (흰색)
  setRGBLed(50, 50, 50);
  delay(500);
  setRGBLed(0, 0, 0);
  
  debugPrint("[시스템] 준비 완료");
  debugPrint("[대기] 명령 수신 대기...");
  
  // 시작 신호 전송
  BTSerial.print("READY_");
}

// ==================== 메인 루프 ====================
void loop() {
  // 블루투스 데이터 수신 처리
  receiveBluetoothData();
  
  // 명령 처리
  if (dataComplete) {
    processCommand(receivedData);
    
    // 버퍼 초기화
    receivedData = "";
    dataComplete = false;
  }
  
  // 추가 작업 (센서 주기적 읽기 등)
  // 여기에 필요한 경우 센서 값을 주기적으로 읽는 로직 추가 가능
}

// ==================== 핵심 함수 1: 블루투스 데이터 수신 ====================
void receiveBluetoothData() {
  // 블루투스 시리얼에서 데이터 읽기
  while (BTSerial.available()) {
    char inChar = (char)BTSerial.read();
    
    // 종료 문자 확인
    if (inChar == '_') {
      dataComplete = true;
      break;
    }
    
    // 버퍼에 추가
    receivedData += inChar;
  }
}

// ==================== 핵심 함수 2: 명령 처리 ====================
void processCommand(String cmd) {
  // 공백 제거
  cmd.trim();
  
  // 디버그 출력
  debugPrint("[수신] " + cmd);
  
  // 빈 명령 처리
  if (cmd.length() == 0) {
    sendError(100);
    return;
  }
  
  // 명령 타입 분리 및 처리
  if (cmd.startsWith("REQ:")) {
    // 데이터 요청 처리
    handleRequest(cmd.substring(4));
  } 
  else if (cmd.startsWith("CMD:")) {
    // 제어 명령 처리
    handleControlCommand(cmd.substring(4));
  } 
  else {
    // 알 수 없는 명령
    sendError(100);
  }
}

// ==================== 핵심 함수 3: 데이터 요청 처리 ====================
void handleRequest(String reqType) {
  debugPrint("[처리] 요청 타입: " + reqType);
  
  if (reqType == "COLOR") {
    // 색상 센서 데이터 전송
    sendColorData();
  } 
  else if (reqType == "ALL") {
    // 모든 센서 데이터 전송
    sendColorData();
    delay(50);
    sendSystemStatus();
  } 
  else if (reqType == "STATUS") {
    // 시스템 상태 전송
    sendSystemStatus();
  } 
  else {
    // 알 수 없는 요청
    sendError(101);
  }
}

// ==================== 핵심 함수 4: 색상 센서 데이터 읽기 및 전송 ====================
void sendColorData() {
  // 센서 값 읽기
  readColorSensor();
  
  // 프로토콜 형식으로 메시지 생성
  String message = "SENSOR:";
  message += String(sensorRed);
  message += ",";
  message += String(sensorGreen);
  message += ",";
  message += String(sensorBlue);
  message += "_";
  
  // 블루투스로 전송
  BTSerial.print(message);
  
  // 디버그 출력
  debugPrint("[전송] " + message);
}

// ==================== 핵심 함수 5: 색상 센서 값 읽기 ====================
void readColorSensor() {
  #if USE_REAL_SENSOR
    // 실제 TCS34725 센서 사용
    uint16_t red, green, blue, clear;
    colorSensor.getRawData(&red, &green, &blue, &clear);
    
    // 0-255 범위로 정규화
    sensorRed = map(red, 0, 65535, 0, 255);
    sensorGreen = map(green, 0, 65535, 0, 255);
    sensorBlue = map(blue, 0, 65535, 0, 255);
  #else
    // 아날로그 핀 시뮬레이션 (0-1023 → 0-255)
    sensorRed = map(analogRead(COLOR_R_PIN), 0, 1023, 0, 255);
    sensorGreen = map(analogRead(COLOR_G_PIN), 0, 1023, 0, 255);
    sensorBlue = map(analogRead(COLOR_B_PIN), 0, 1023, 0, 255);
  #endif
}

// ==================== 핵심 함수 6: 제어 명령 처리 ====================
void handleControlCommand(String ctrlCmd) {
  debugPrint("[제어] " + ctrlCmd);
  
  // LED 제어
  if (ctrlCmd.startsWith("LED,")) {
    parseLedCommand(ctrlCmd);
  } 
  // 모터 제어
  else if (ctrlCmd == "MOTOR_ON") {
    setMotor(true);
    sendAck();
  } 
  else if (ctrlCmd == "MOTOR_OFF") {
    setMotor(false);
    sendAck();
  } 
  // 서보 제어
  else if (ctrlCmd.startsWith("SERVO,")) {
    parseServoCommand(ctrlCmd);
  } 
  // 시스템 리셋
  else if (ctrlCmd == "RESET") {
    systemReset();
    sendAck();
  } 
  // 알 수 없는 명령
  else {
    sendError(102);
  }
}

// ==================== LED 명령 파싱 ====================
void parseLedCommand(String cmd) {
  // 형식: LED,R,G,B
  // 예: LED,255,0,0
  
  int firstComma = cmd.indexOf(',');
  int secondComma = cmd.indexOf(',', firstComma + 1);
  int thirdComma = cmd.indexOf(',', secondComma + 1);
  
  if (firstComma == -1 || secondComma == -1) {
    sendError(103); // LED 명령 형식 오류
    return;
  }
  
  // RGB 값 추출
  int r = cmd.substring(firstComma + 1, secondComma).toInt();
  int g = cmd.substring(secondComma + 1, thirdComma != -1 ? thirdComma : cmd.length()).toInt();
  int b = thirdComma != -1 ? cmd.substring(thirdComma + 1).toInt() : 0;
  
  // 범위 체크
  r = constrain(r, 0, 255);
  g = constrain(g, 0, 255);
  b = constrain(b, 0, 255);
  
  // LED 제어
  setRGBLed(r, g, b);
  sendAck();
}

// ==================== 서보 명령 파싱 ====================
void parseServoCommand(String cmd) {
  // 형식: SERVO,ANGLE
  // 예: SERVO,90
  
  int commaIndex = cmd.indexOf(',');
  if (commaIndex == -1) {
    sendError(104); // 서보 명령 형식 오류
    return;
  }
  
  // 각도 추출
  int angle = cmd.substring(commaIndex + 1).toInt();
  angle = constrain(angle, 0, 180);
  
  // 서보 제어
  setServoAngle(angle);
  sendAck();
}

// ==================== 액추에이터 제어 함수 ====================

// RGB LED 제어
void setRGBLed(int r, int g, int b) {
  analogWrite(LED_R_PIN, r);
  analogWrite(LED_G_PIN, g);
  analogWrite(LED_B_PIN, b);
  
  debugPrint("[LED] R:" + String(r) + " G:" + String(g) + " B:" + String(b));
}

// 모터 제어
void setMotor(bool state) {
  motorState = state;
  digitalWrite(MOTOR_PIN, state ? HIGH : LOW);
  
  debugPrint("[모터] " + String(state ? "켜짐" : "꺼짐"));
}

// 서보 제어
void setServoAngle(int angle) {
  servoAngle = angle;
  myServo.write(angle);
  
  debugPrint("[서보] 각도: " + String(angle) + "도");
}

// 시스템 리셋
void systemReset() {
  // LED 끄기
  setRGBLed(0, 0, 0);
  
  // 모터 끄기
  setMotor(false);
  
  // 서보 홈 포지션
  setServoAngle(90);
  
  debugPrint("[시스템] 리셋 완료");
}

// ==================== 응답 전송 함수 ====================

// ACK 응답 전송
void sendAck() {
  BTSerial.print("ACK:OK_");
  debugPrint("[전송] ACK:OK_");
}

// 에러 전송
void sendError(int errorCode) {
  String message = "ERROR:";
  message += String(errorCode);
  message += "_";
  
  BTSerial.print(message);
  debugPrint("[에러] 코드 " + String(errorCode));
}

// 시스템 상태 전송
void sendSystemStatus() {
  String message = "STATUS:";
  message += String(motorState ? 1 : 0);
  message += ",";
  message += String(servoAngle);
  message += "_";
  
  BTSerial.print(message);
  debugPrint("[전송] " + message);
}

// ==================== 디버그 함수 ====================
void debugPrint(String msg) {
  #if ENABLE_DEBUG
    Serial.println(msg);
  #endif
}

/*
 * ====================================================================
 * 에러 코드 정의
 * ====================================================================
 * 100: 알 수 없는 명령
 * 101: 알 수 없는 요청 타입
 * 102: 알 수 없는 제어 명령
 * 103: LED 명령 형식 오류
 * 104: 서보 명령 형식 오류
 * 200: 센서 읽기 실패
 * 201: 액추에이터 제어 실패
 * ====================================================================
 */

/*
 * ====================================================================
 * 프로토콜 규약
 * ====================================================================
 * 
 * [앱인벤터 → 아두이노]
 * - REQ:COLOR_       : 색상 센서 데이터 요청
 * - REQ:STATUS_      : 시스템 상태 요청
 * - REQ:ALL_         : 모든 데이터 요청
 * - CMD:LED,R,G,B_   : LED 제어 (예: CMD:LED,255,0,0_)
 * - CMD:MOTOR_ON_    : 모터 켜기
 * - CMD:MOTOR_OFF_   : 모터 끄기
 * - CMD:SERVO,ANGLE_ : 서보 제어 (예: CMD:SERVO,90_)
 * - CMD:RESET_       : 시스템 리셋
 * 
 * [아두이노 → 앱인벤터]
 * - SENSOR:R,G,B_       : 센서 데이터 (예: SENSOR:255,128,64_)
 * - STATUS:MOTOR,SERVO_ : 시스템 상태 (예: STATUS:1,90_)
 * - ACK:OK_             : 명령 수신 확인
 * - ERROR:CODE_         : 에러 코드 (예: ERROR:100_)
 * - READY_              : 시스템 준비 완료
 * 
 * ====================================================================
 */

/*
 * ====================================================================
 * 테스트 방법
 * ====================================================================
 * 
 * 1. Serial Monitor 테스트 (9600 baud)
 *    - 입력: REQ:COLOR_
 *    - 출력: SENSOR:255,128,64_
 * 
 * 2. LED 테스트
 *    - 입력: CMD:LED,255,0,0_
 *    - 결과: 빨강 LED 점등
 * 
 * 3. 모터 테스트
 *    - 입력: CMD:MOTOR_ON_
 *    - 출력: ACK:OK_
 *    - 결과: 모터 동작
 * 
 * 4. 서보 테스트
 *    - 입력: CMD:SERVO,90_
 *    - 출력: ACK:OK_
 *    - 결과: 서보 90도 이동
 * 
 * 5. 시스템 리셋
 *    - 입력: CMD:RESET_
 *    - 결과: 모든 액추에이터 초기화
 * 
 * ====================================================================
 */

/*
 * ====================================================================
 * 배선 가이드
 * ====================================================================
 * 
 * [블루투스 HC-05/06]
 * - VCC  → 5V
 * - GND  → GND
 * - TXD  → D10 (아두이노 RX)
 * - RXD  → D11 (아두이노 TX)
 * 
 * [RGB LED (공통 캐소드)]
 * - R    → D9  (PWM)
 * - G    → D6  (PWM)
 * - B    → D5  (PWM)
 * - GND  → GND
 * 
 * [DC 모터 (L298N)]
 * - IN1  → D3
 * - GND  → GND
 * - 전원 → 외부 5V
 * 
 * [서보 모터]
 * - 신호 → D4
 * - VCC  → 5V
 * - GND  → GND
 * 
 * [색상 센서 시뮬레이션]
 * - R    → A0
 * - G    → A1
 * - B    → A2
 * 
 * [실제 TCS34725 센서]
 * - VIN  → 5V
 * - GND  → GND
 * - SCL  → A5
 * - SDA  → A4
 * 
 * ====================================================================
 */
