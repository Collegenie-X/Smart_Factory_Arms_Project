/*
 * ========================================
 * RFID + 버튼 + 릴레이 + LED 통합 예제
 * ========================================
 * 
 * 기능:
 * - RFID 카드 인식 시 릴레이 ON, LED 점등
 * - 버튼 누르면 릴레이 OFF, LED 소등
 * - 3개의 LED로 상태 표시 (대기/인식/에러)
 * 
 * 하드웨어:
 * - RFID-RC522 모듈 (SPI 통신)
 * - 버튼 1개 (풀업 저항)
 * - 릴레이 모듈 1개
 * - LED 3개 (빨강, 초록, 파랑)
 * 
 * 작성일: 2026-01-30
 */

#include <SPI.h>
#include <MFRC522.h>

// ========================================
// 핀 정의
// ========================================
#define RFID_RST_PIN    9     // RFID 리셋 핀
#define RFID_SS_PIN     10    // RFID SS 핀 (SPI)

#define BUTTON_PIN      2     // 버튼 입력 핀 (풀업)
#define RELAY_PIN       3     // 릴레이 제어 핀

#define LED_RED_PIN     4     // 빨강 LED (대기 상태)
#define LED_GREEN_PIN   5     // 초록 LED (인식 성공)
#define LED_BLUE_PIN    6     // 파랑 LED (에러 상태)

// ========================================
// 상수 정의
// ========================================
#define DEBOUNCE_DELAY  50    // 버튼 디바운싱 시간 (ms)
#define RELAY_ON_TIME   5000  // 릴레이 자동 꺼짐 시간 (5초)

// ========================================
// 전역 변수
// ========================================
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);  // RFID 객체 생성

bool relayState = false;                   // 릴레이 상태
unsigned long relayOnTime = 0;             // 릴레이 켜진 시간

int lastButtonState = HIGH;                // 이전 버튼 상태
int buttonState = HIGH;                    // 현재 버튼 상태
unsigned long lastDebounceTime = 0;        // 마지막 디바운싱 시간

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  RFID + 버튼 + 릴레이 + LED 시스템");
  Serial.println("========================================");
  
  // 핀 모드 설정
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  
  // 초기 상태 설정
  digitalWrite(RELAY_PIN, LOW);
  setLED_State("standby");
  
  // RFID 초기화
  SPI.begin();
  rfid.PCD_Init();
  
  Serial.println("[초기화 완료]");
  Serial.println("- RFID 카드를 태그하세요");
  Serial.println("- 버튼을 누르면 릴레이 OFF");
  Serial.println("========================================");
}

// ========================================
// 메인 루프
// ========================================
void loop() {
  // 1. 버튼 입력 처리 (디바운싱 적용)
  handleButton();
  
  // 2. RFID 카드 감지
  handleRFID();
  
  // 3. 릴레이 자동 꺼짐 처리 (5초 후)
  handleRelayAutoOff();
  
  delay(10);  // CPU 부하 감소
}

// ========================================
// 버튼 입력 처리 함수 (디바운싱)
// ========================================
void handleButton() {
  int reading = digitalRead(BUTTON_PIN);
  
  // 버튼 상태가 변경되었을 때
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  // 디바운싱 시간이 지났을 때
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // 버튼 상태가 실제로 변경되었을 때
    if (reading != buttonState) {
      buttonState = reading;
      
      // 버튼이 눌렸을 때 (LOW)
      if (buttonState == LOW) {
        turnOffRelay();
      }
    }
  }
  
  lastButtonState = reading;
}

// ========================================
// RFID 카드 감지 함수
// ========================================
void handleRFID() {
  // 카드가 감지되지 않으면 리턴
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }
  
  // 카드 UID를 읽지 못하면 리턴
  if (!rfid.PICC_ReadCardSerial()) {
    setLED_State("error");
    Serial.println("[에러] 카드 읽기 실패");
    delay(1000);
    setLED_State("standby");
    return;
  }
  
  // 카드 UID 출력
  Serial.println("========================================");
  Serial.print("[RFID 감지] UID: ");
  
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) {
      Serial.print(" ");
    }
  }
  Serial.println();
  
  // 릴레이 ON
  turnOnRelay();
  
  Serial.println("========================================");
  
  // RFID 카드 읽기 종료
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// ========================================
// 릴레이 ON 함수
// ========================================
void turnOnRelay() {
  if (relayState) {
    Serial.println("[알림] 릴레이 이미 ON 상태");
    return;
  }
  
  relayState = true;
  digitalWrite(RELAY_PIN, HIGH);
  relayOnTime = millis();
  
  setLED_State("success");
  
  Serial.println("[릴레이 ON] 5초 후 자동 OFF");
}

// ========================================
// 릴레이 OFF 함수
// ========================================
void turnOffRelay() {
  if (!relayState) {
    Serial.println("[알림] 릴레이 이미 OFF 상태");
    return;
  }
  
  relayState = false;
  digitalWrite(RELAY_PIN, LOW);
  
  setLED_State("standby");
  
  Serial.println("[릴레이 OFF] 버튼으로 수동 종료");
}

// ========================================
// 릴레이 자동 꺼짐 처리 함수
// ========================================
void handleRelayAutoOff() {
  // 릴레이가 켜져 있지 않으면 리턴
  if (!relayState) {
    return;
  }
  
  // 5초가 지났는지 확인
  if (millis() - relayOnTime >= RELAY_ON_TIME) {
    relayState = false;
    digitalWrite(RELAY_PIN, LOW);
    
    setLED_State("standby");
    
    Serial.println("[릴레이 OFF] 자동 타이머 종료");
  }
}

// ========================================
// LED 상태 표시 함수
// ========================================
void setLED_State(String state) {
  if (state == "standby") {
    // 대기 상태: 빨강 LED만 점등
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_BLUE_PIN, LOW);
    
  } else if (state == "success") {
    // 인식 성공: 초록 LED만 점등
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, HIGH);
    digitalWrite(LED_BLUE_PIN, LOW);
    
  } else if (state == "error") {
    // 에러 상태: 파랑 LED만 점등
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_BLUE_PIN, HIGH);
  }
}
