/**
 * 로봇 팔 - 6단계: Bluetooth EEPROM 자동화 시스템 (전력 최적화)
 * 
 * Bluetooth로 위치를 저장하고 자동 재생
 * - 조이스틱으로 로봇 팔 제어
 * - Bluetooth 명령으로 위치 저장/재생
 * - EEPROM에 최대 12단계 저장
 * - Serial은 디버깅용으로만 최소 사용
 * 
 * 명령어 (Bluetooth):
 * - save_  : 현재 위치 저장
 * - play_  : 저장된 동작 1회 재생
 * - auto_  : 저장된 동작 계속 반복
 * - stop_  : 자동 반복 중지
 * - clear_ : 모든 위치 삭제
 * - list_  : 저장된 위치 목록
 * 
 * Bluetooth 연결:
 * - TX: 2번 핀 (아두이노) ← RX (HC-05/06)
 * - RX: 3번 핀 (아두이노) ← TX (HC-05/06)
 * 
 * 전력 최적화:
 * - Bluetooth 출력 최소화 (전력 소모 감소)
 * - Serial 출력 최소화 (디버깅용만)
 * - delay 최적화
 * 
 * 중요:
 * - 외부 5V 전원 필수! (USB만으로는 부족)
 */

#include <Servo.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

/* ===== 디버그 모드 설정 ===== */
#define ENABLE_DEBUG      0  // 1: 디버그 메시지 출력, 0: 디버그 메시지 생략 (메모리 절약)
#define ENABLE_BLUETOOTH  1  // 1: Bluetooth 사용, 0: Serial만 사용 (문제 해결용)

/* ===== Bluetooth 설정 ===== */
#define PIN_BT_TX   2
#define PIN_BT_RX   3

#if ENABLE_BLUETOOTH
SoftwareSerial BTSerial(PIN_BT_TX, PIN_BT_RX);
#endif

/* ===== 서보 설정 ===== */
Servo servo[4];
int pin[4] = {4, 5, 6, 7};

// 각도 범위 설정
int minAngles[4] = {0, 50, 60, 10};
int maxAngles[4] = {180, 110, 120, 60};

// 초기 각도 (각 서보의 중간값으로 안전하게 설정)
int angles[4] = {90, 80, 90, 15};        // 현재 각도: 베이스, 팔꿈치, 손목, 그립(열린 상태)
int prevAngles[4] = {90, 80, 90, 15};    // 이전 각도


/* ===== EEPROM 설정 ===== */
#define EEPROM_ADDR_COUNT   0     // 저장된 위치 개수 주소
#define EEPROM_ADDR_START   4     // 위치 데이터 시작 주소
#define MAX_POSITIONS       12    // 최대 저장 가능 위치

/* ===== 전역 변수 ===== */
int savedCount = 0;               // 저장된 위치 개수
bool isPlaying = false;           // 자동 재생 중 여부
bool autoRepeat = false;          // 자동 반복 모드
bool playOnce = false;            // 1회 재생 플래그
int repeatCount = 0;              // 반복 횟수

char cmdBuffer[20];
int cmdIndex = 0;
unsigned long lastCharTime = 0;   // 마지막 문자 수신 시간
#define CMD_TIMEOUT 100           // 명령어 타임아웃 (ms)

void setup() {
#if ENABLE_DEBUG
  Serial.begin(9600);
  delay(50);
  Serial.println("v1.1");
#endif
  
  // 서보 모터 초기화 (과전류 방지)
  for (int i = 0; i < 4; i++) {
    servo[i].attach(pin[i]);
    delay(150);  // 전력 소모 최소화
    
    int currentPos = servo[i].read();
    if (currentPos < 0 || currentPos > 180) currentPos = 90;
    
    // 목표 각도로 부드럽게 이동
    if (currentPos < angles[i]) {
      for (int pos = currentPos; pos <= angles[i]; pos++) {
        servo[i].write(pos);
        delay(15);
      }
    } else {
      for (int pos = currentPos; pos >= angles[i]; pos--) {
        servo[i].write(pos);
        delay(15);
      }
    }
    delay(200);
  }
  
  // EEPROM 데이터 읽기
  savedCount = EEPROM.read(EEPROM_ADDR_COUNT);
  if (savedCount > MAX_POSITIONS) {
    savedCount = 0;
    EEPROM.write(EEPROM_ADDR_COUNT, 0);
  }
  
#if ENABLE_DEBUG
  Serial.print("Data:");
  Serial.print(savedCount);
  Serial.print("/");
  Serial.println(MAX_POSITIONS);
#endif
  
  // Bluetooth 초기화
#if ENABLE_BLUETOOTH
  BTSerial.begin(9600);
  delay(300);  // 최소 대기 시간
#if ENABLE_DEBUG
  Serial.println("BT OK");
#endif
#endif
}

void loop() {
  handleCommand();  // 명령 처리
  
  // 1회 재생 모드
  if (playOnce && !isPlaying) {
    playOnce = false;  // 플래그 초기화
    autoRepeat = false;
    playAllPositions();
    return;
  }
  
  // 자동 반복 모드
  if (autoRepeat && !isPlaying) {
    playAllPositions();
    return;
  }
  
  if (!isPlaying) {
    handleJoystick();
  }
}

/**
 * 조이스틱 제어 (출력 최소화)
 */
void handleJoystick() {
  // 조이스틱 값 읽기
  int val[4];
  val[0] = analogRead(14);              // A0 - 베이스
  val[1] = analogRead(15);              // A1 - 팔꿈치
  val[2] = 1024 - analogRead(16);       // A2 - 손목 (반대)
  val[3] = 1024 - analogRead(17);       // A3 - 그립 (반대)
  
  // 각 서보 제어
  for (int i = 0; i < 4; i++) {
    if (val[i] > 1000) {
      angles[i] += 1;
      if (angles[i] > maxAngles[i]) angles[i] = maxAngles[i];
      servo[i].write(angles[i]);
    }
    else if (val[i] < 100) {
      angles[i] -= 1;
      if (angles[i] < minAngles[i]) angles[i] = minAngles[i];
      servo[i].write(angles[i]);
    }
  }
  
  delay(20);
}

/**
 * Bluetooth 명령 처리 (Serial 출력 최소화)
 */
void handleCommand() {
  // 재생 중에는 stop 명령만 처리
  if (isPlaying) {
    char ch = '\0';
#if ENABLE_BLUETOOTH
    if (BTSerial.available() > 0) {
      ch = BTSerial.read();
    }
#endif
    
    if (ch != '\0') {
      if (ch == '\n' || ch == '\r' || ch == '_') {
        if (cmdIndex > 0) {
          cmdBuffer[cmdIndex] = '\0';
          if (strcmp(cmdBuffer, "stop") == 0) {
            stopAutoMode();
          }
          cmdIndex = 0;
        }
      } else if (ch != ' ' && cmdIndex < 19) {
        cmdBuffer[cmdIndex] = ch;
        cmdIndex++;
        lastCharTime = millis();
      }
    }
    return;
  }
  
  // 타임아웃 체크
  if (cmdIndex > 0 && (millis() - lastCharTime) > CMD_TIMEOUT) {
    cmdBuffer[cmdIndex] = '\0';
    processCommand();
    cmdIndex = 0;
    return;
  }
  
  // Bluetooth에서 명령 받기
  char ch = '\0';
  
#if ENABLE_BLUETOOTH
  if (BTSerial.available() > 0) {
    ch = BTSerial.read();
    lastCharTime = millis();
  }
#endif
  
  if (ch == '\0') return;
  
  // 명령 종료 문자 확인
  if (ch == '\n' || ch == '\r' || ch == '_') {
    if (cmdIndex > 0) {
      cmdBuffer[cmdIndex] = '\0';
#if ENABLE_DEBUG
      Serial.print("[");
      Serial.print(cmdBuffer);
      Serial.println("]");
#endif
      processCommand();
      cmdIndex = 0;
    }
    return;
  }
  
  // 공백 무시
  if (ch == ' ') return;
  
  // 버퍼에 문자 추가
  if (cmdIndex < 19) {
    cmdBuffer[cmdIndex] = ch;
    cmdIndex++;
  }
}

/**
 * 명령 처리 (최소 출력)
 */
void processCommand() {
  // save 명령
  if (strcmp(cmdBuffer, "save") == 0) {
    saveCurrentPosition();
  }
  // play 명령 (1회 재생)
  else if (strcmp(cmdBuffer, "play") == 0) {
    // 블루투스 버퍼 비우기
#if ENABLE_BLUETOOTH
    while (BTSerial.available() > 0) {
      BTSerial.read();
    }
#endif
    cmdIndex = 0;
    cmdBuffer[0] = '\0';
    playOnce = true;
    autoRepeat = false;
    isPlaying = false;
  }
  // auto 명령 (계속 반복)
  else if (strcmp(cmdBuffer, "auto") == 0) {
    startAutoMode();
  }
  // stop 명령 (자동 반복 중지)
  else if (strcmp(cmdBuffer, "stop") == 0) {
    stopAutoMode();
  }
  // clear 명령
  else if (strcmp(cmdBuffer, "clear") == 0) {
    clearAllPositions();
  }
  // list 명령
  else if (strcmp(cmdBuffer, "list") == 0) {
    listAllPositions();
  }
#if ENABLE_DEBUG
  else {
    Serial.print("ERR:");
    Serial.println(cmdBuffer);
  }
#endif
}

/**
 * 현재 위치 저장 (전력 최적화)
 */
void saveCurrentPosition() {
  if (savedCount >= MAX_POSITIONS) {
#if ENABLE_DEBUG
    Serial.println("Full");
#endif
    return;
  }
  
  // 서보 일시 중지
  for (int i = 0; i < 4; i++) {
    servo[i].detach();
  }
  delay(30);
  
  // EEPROM에 저장
  int addr = EEPROM_ADDR_START + (savedCount * 4);
  for (int i = 0; i < 4; i++) {
    EEPROM.write(addr + i, angles[i]);
    delay(4);
  }
  
  savedCount++;
  EEPROM.write(EEPROM_ADDR_COUNT, savedCount);
  delay(4);
  
  // 서보 재연결
  for (int i = 0; i < 4; i++) {
    servo[i].attach(pin[i]);
    servo[i].write(angles[i]);
    delay(15);
  }
  
#if ENABLE_DEBUG
  Serial.print("Save#");
  Serial.println(savedCount);
#endif
}

void startAutoMode() {
  if (savedCount == 0) {
#if ENABLE_DEBUG
    Serial.println("NoData");
#endif
    return;
  }
  
  // 버퍼 비우기
#if ENABLE_BLUETOOTH
  while (BTSerial.available() > 0) {
    BTSerial.read();
  }
#endif
  
  cmdIndex = 0;
  cmdBuffer[0] = '\0';
  autoRepeat = true;
  playOnce = false;
  isPlaying = false;
  repeatCount = 0;
  
#if ENABLE_DEBUG
  Serial.print("Auto:");
  Serial.println(savedCount);
#endif
}

void stopAutoMode() {
  autoRepeat = false;
  playOnce = false;
  isPlaying = false;
  
  // 버퍼 비우기
#if ENABLE_BLUETOOTH
  while (BTSerial.available() > 0) {
    BTSerial.read();
  }
#endif
  
  cmdIndex = 0;
  cmdBuffer[0] = '\0';
  
#if ENABLE_DEBUG
  Serial.print("Stop:");
  Serial.println(repeatCount);
#endif
}

void playAllPositions() {
  if (savedCount == 0) {
    autoRepeat = false;
    return;
  }
  
  if (autoRepeat) {
    repeatCount++;
  }
  
  isPlaying = true;
  
  // 모든 저장된 위치 재생
  for (int pos = 0; pos < savedCount; pos++) {
    handleCommand();
    if (!autoRepeat && !isPlaying) return;
    
    // EEPROM에서 각도 읽기
    int addr = EEPROM_ADDR_START + (pos * 4);
    int targetAngles[4];
    bool dataValid = true;
    
    for (int i = 0; i < 4; i++) {
      targetAngles[i] = EEPROM.read(addr + i);
      
      // 유효성 검사
      if (targetAngles[i] < 0 || targetAngles[i] > 180) {
        dataValid = false;
        break;
      }
    }
    
    // 유효하지 않으면 건너뛰기
    if (!dataValid) {
      continue;
    }
    
#if ENABLE_DEBUG
    // 디버그 모드에서만 각도 출력
    Serial.print("#");
    Serial.print(pos + 1);
    Serial.print(":");
    Serial.print(targetAngles[0]);
    Serial.print(",");
    Serial.print(targetAngles[1]);
    Serial.print(",");
    Serial.print(targetAngles[2]);
    Serial.print(",");
    Serial.println(targetAngles[3]);
#endif
    
    // 이동
    if (!moveToPosition(targetAngles)) return;
    
    // 대기
    for (int i = 0; i < 100; i++) {
      handleCommand();
      if (!autoRepeat && !isPlaying) return;
      delay(10);
    }
  }
  
  isPlaying = false;
}

/**
 * 목표 위치로 부드럽게 이동
 * @return true: 이동 완료, false: stop 명령으로 중지됨
 */
bool moveToPosition(int target[]) {
  bool moving = true;
  
  while (moving) {
    // stop 명령 체크
    handleCommand();
    if (!autoRepeat && !isPlaying) {
      return false;  // stop 명령으로 중지
    }
    
    moving = false;
    
    for (int i = 0; i < 4; i++) {
      if (angles[i] < target[i]) {
        angles[i]++;
        moving = true;
      }
      else if (angles[i] > target[i]) {
        angles[i]--;
        moving = true;
      }
      servo[i].write(angles[i]);
    }
    
    delay(15);  // 부드러운 이동
  }
  
  return true;  // 이동 완료
}

void clearAllPositions() {
  autoRepeat = false;
  playOnce = false;
  isPlaying = false;
  repeatCount = 0;
  
  // 서보 일시 중지
  for (int i = 0; i < 4; i++) {
    servo[i].detach();
  }
  delay(30);
  
  // EEPROM 초기화
  savedCount = 0;
  EEPROM.write(EEPROM_ADDR_COUNT, 0);
  delay(5);
  
  // 서보 재연결
  for (int i = 0; i < 4; i++) {
    servo[i].attach(pin[i]);
    servo[i].write(angles[i]);
    delay(15);
  }
  
#if ENABLE_DEBUG
  Serial.println("Clear");
#endif
}

void listAllPositions() {
#if ENABLE_DEBUG
  if (savedCount == 0) {
    Serial.println("Empty");
    return;
  }
  
  Serial.print("List:");
  Serial.println(savedCount);
  
  // 각 저장된 위치 출력
  for (int pos = 0; pos < savedCount; pos++) {
    int addr = EEPROM_ADDR_START + (pos * 4);
    
    Serial.print("#");
    Serial.print(pos + 1);
    Serial.print(":");
    
    for (int i = 0; i < 4; i++) {
      Serial.print(EEPROM.read(addr + i));
      if (i < 3) Serial.print(",");
    }
    Serial.println();
  }
#endif
}

