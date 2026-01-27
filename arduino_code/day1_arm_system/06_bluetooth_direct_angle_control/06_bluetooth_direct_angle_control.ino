/**
 * 로봇 팔 - 6단계: Serial & Bluetooth 직접 각도 제어 시스템
 * 
 * Serial 또는 Bluetooth로 각도를 직접 전송하여 로봇 팔 제어
 * - 조이스틱으로 로봇 팔 제어
 * - Serial/Bluetooth 명령으로 위치 저장/재생
 * - EEPROM에 최대 12단계 저장
 * - 각도 직접 제어 기능 추가 (예: 90,110,150,5_)
 * 
 * 명령어 (Serial 또는 Bluetooth):
 * - 90,110,150,5_ : 각 서보를 해당 각도로 이동 (베이스,팔꿈치,손목,그립)
 * - save_         : 현재 위치 저장
 * - play_         : 저장된 동작 1회 재생
 * - auto_         : 저장된 동작 계속 반복
 * - stop_         : 자동 반복 중지
 * - clear_        : 모든 위치 삭제
 * - list_         : 저장된 위치 목록
 * 
 * Serial 통신:
 * - USB 케이블로 연결
 * - 보드레이트: 9600
 * - Serial Monitor에서 명령 입력 가능
 * - 디버깅 메시지 출력
 * 
 * Bluetooth 연결:
 * - TX: 2번 핀 (아두이노) ← RX (HC-05/06)
 * - RX: 3번 핀 (아두이노) ← TX (HC-05/06)
 * 
 * 중요:
 * - 외부 5V 전원 필수! (USB만으로는 부족)
 * - 각도는 쉼표(,)로 구분하고 언더스코어(_)로 종료
 * - Serial과 Bluetooth 동시 사용 가능 (Serial 우선)
 */

#include <Servo.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

/* ===== 디버그 모드 설정 ===== */
#define ENABLE_DEBUG      1  // 1: 디버그 메시지 출력, 0: 디버그 메시지 생략
#define ENABLE_BLUETOOTH  1  // 1: Bluetooth 사용, 0: Serial만 사용

/* ===== Bluetooth 설정 ===== */
#define PIN_BT_TX   2
#define PIN_BT_RX   3

#if ENABLE_BLUETOOTH
SoftwareSerial BTSerial(PIN_BT_TX, PIN_BT_RX);
#endif

/* ===== 서보 설정 ===== */
#define SERVO_COUNT 4
Servo servo[SERVO_COUNT];
const int servoPins[SERVO_COUNT] = {4, 5, 6, 7};

// 각도 범위 설정 (PROGMEM으로 메모리 절약)
const int minAngles[SERVO_COUNT] PROGMEM = {0, 80, 60, 5};
const int maxAngles[SERVO_COUNT] PROGMEM = {180, 150, 120, 60};

// 현재 각도 (베이스, 팔꿈치, 손목, 그립)
int currentAngles[SERVO_COUNT] = {90, 110, 150, 5};

/* ===== EEPROM 설정 ===== */
#define EEPROM_ADDR_COUNT   0     // 저장된 위치 개수 주소
#define EEPROM_ADDR_START   4     // 위치 데이터 시작 주소
#define MAX_POSITIONS       12    // 최대 저장 가능 위치

/* ===== 전역 변수 ===== */
int savedPositionCount = 0;       // 저장된 위치 개수
bool isPlaying = false;           // 자동 재생 중 여부
bool autoRepeat = false;          // 자동 반복 모드
bool playOnce = false;            // 1회 재생 플래그
int repeatCount = 0;              // 반복 횟수

char cmdBuffer[50];               // 명령 버퍼
int cmdIndex = 0;
unsigned long lastCharTime = 0;   // 마지막 문자 수신 시간
#define CMD_TIMEOUT 100           // 명령어 타임아웃 (ms)

void setup() {
  // Serial 통신 초기화 (디버깅 및 명령 입력용)
  Serial.begin(9600);
  delay(50);
  
#if ENABLE_DEBUG
  Serial.println(F("=== Robot Arm Angle Control v1.0 ==="));
  Serial.println(F("Serial & Bluetooth Command Support"));
  Serial.println();
#endif
  
  // 서보 모터 초기화
  initializeServos();
  
  // EEPROM 데이터 읽기
  loadSavedPositionCount();
  
#if ENABLE_DEBUG
  Serial.print(F("Saved Data: "));
  Serial.print(savedPositionCount);
  Serial.print(F("/"));
  Serial.println(MAX_POSITIONS);
  Serial.println();
#endif
  
  // Bluetooth 초기화
#if ENABLE_BLUETOOTH
  BTSerial.begin(9600);
  delay(300);
#if ENABLE_DEBUG
  Serial.println(F("Bluetooth Ready"));
#endif
#endif

#if ENABLE_DEBUG
  Serial.println(F("Commands:"));
  Serial.println(F("  90,110,150,5_  : Move to angles"));
  Serial.println(F("  save_          : Save position"));
  Serial.println(F("  play_          : Play once"));
  Serial.println(F("  auto_          : Auto repeat"));
  Serial.println(F("  stop_          : Stop repeat"));
  Serial.println(F("  clear_         : Clear all"));
  Serial.println(F("  list_          : List positions"));
  Serial.println();
  Serial.println(F("Ready for commands..."));
  Serial.println();
#endif
}

void loop() {
  handleCommand();  // 명령 처리
  
  // 1회 재생 모드
  if (playOnce && !isPlaying) {
    playOnce = false;
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
 * 서보 모터 초기화 (부드러운 시작)
 */
void initializeServos() {
  for (int i = 0; i < SERVO_COUNT; i++) {
    servo[i].attach(servoPins[i]);
    delay(150);
    
    int currentPos = servo[i].read();
    if (currentPos < 0 || currentPos > 180) {
      currentPos = 90;
    }
    
    // 목표 각도로 부드럽게 이동
    moveSmoothly(i, currentPos, currentAngles[i]);
    delay(200);
  }
}

/**
 * 서보를 부드럽게 이동
 */
void moveSmoothly(int servoIndex, int startAngle, int endAngle) {
  if (startAngle < endAngle) {
    for (int pos = startAngle; pos <= endAngle; pos++) {
      servo[servoIndex].write(pos);
      delay(15);
    }
  } else {
    for (int pos = startAngle; pos >= endAngle; pos--) {
      servo[servoIndex].write(pos);
      delay(15);
    }
  }
}

/**
 * EEPROM에서 저장된 위치 개수 읽기
 */
void loadSavedPositionCount() {
  savedPositionCount = EEPROM.read(EEPROM_ADDR_COUNT);
  if (savedPositionCount > MAX_POSITIONS) {
    savedPositionCount = 0;
    EEPROM.write(EEPROM_ADDR_COUNT, 0);
  }
}

/**
 * 조이스틱 제어
 */
void handleJoystick() {
  // 조이스틱 값 읽기
  int joystickValues[SERVO_COUNT];
  joystickValues[0] = analogRead(A0);              // 베이스
  joystickValues[1] = analogRead(A1);              // 팔꿈치
  joystickValues[2] = 1024 - analogRead(A2);       // 손목 (반대)
  joystickValues[3] = 1024 - analogRead(A3);       // 그립 (반대)
  
  // 각 서보 제어
  for (int i = 0; i < SERVO_COUNT; i++) {
    if (joystickValues[i] > 1000) {
      currentAngles[i]++;
      int maxAngle = pgm_read_word(&maxAngles[i]);
      if (currentAngles[i] > maxAngle) {
        currentAngles[i] = maxAngle;
      }
      servo[i].write(currentAngles[i]);
    }
    else if (joystickValues[i] < 100) {
      currentAngles[i]--;
      int minAngle = pgm_read_word(&minAngles[i]);
      if (currentAngles[i] < minAngle) {
        currentAngles[i] = minAngle;
      }
      servo[i].write(currentAngles[i]);
    }
  }
  
  delay(20);
}

/**
 * Serial 및 Bluetooth 명령 처리
 */
void handleCommand() {
  // 재생 중에는 stop 명령만 처리
  if (isPlaying) {
    handleStopCommand();
    return;
  }
  
  // 타임아웃 체크
  if (cmdIndex > 0 && (millis() - lastCharTime) > CMD_TIMEOUT) {
    cmdBuffer[cmdIndex] = '\0';
    executeCommand();
    cmdIndex = 0;
    return;
  }
  
  // Serial 또는 Bluetooth에서 명령 받기
  char ch = '\0';
  
  // Serial 통신 우선 확인 (디버깅 및 테스트용)
  if (Serial.available() > 0) {
    ch = Serial.read();
    lastCharTime = millis();
  }
  // Bluetooth 통신 확인
#if ENABLE_BLUETOOTH
  else if (BTSerial.available() > 0) {
    ch = BTSerial.read();
    lastCharTime = millis();
  }
#endif
  
  if (ch == '\0') return;
  
  // 명령 종료 문자 확인 (_)
  if (ch == '_') {
    if (cmdIndex > 0) {
      cmdBuffer[cmdIndex] = '\0';
#if ENABLE_DEBUG
      Serial.print(F("Received: ["));
      Serial.print(cmdBuffer);
      Serial.println(F("]"));
#endif
      executeCommand();
      cmdIndex = 0;
    }
    return;
  }
  
  // 개행 문자 무시
  if (ch == '\n' || ch == '\r') {
    return;
  }
  
  // 버퍼에 문자 추가
  if (cmdIndex < sizeof(cmdBuffer) - 1) {
    cmdBuffer[cmdIndex] = ch;
    cmdIndex++;
  }
}

/**
 * 재생 중 정지 명령 처리
 */
void handleStopCommand() {
  char ch = '\0';
  
  // Serial 통신 우선 확인
  if (Serial.available() > 0) {
    ch = Serial.read();
  }
  // Bluetooth 통신 확인
#if ENABLE_BLUETOOTH
  else if (BTSerial.available() > 0) {
    ch = BTSerial.read();
  }
#endif
  
  if (ch != '\0') {
    if (ch == '_') {
      if (cmdIndex > 0) {
        cmdBuffer[cmdIndex] = '\0';
        if (strcmp(cmdBuffer, "stop") == 0) {
          stopAutoMode();
        }
        cmdIndex = 0;
      }
    } else if (ch != ' ' && ch != '\n' && ch != '\r' && cmdIndex < sizeof(cmdBuffer) - 1) {
      cmdBuffer[cmdIndex] = ch;
      cmdIndex++;
      lastCharTime = millis();
    }
  }
}

/**
 * 명령어 실행
 */
void executeCommand() {
  // 각도 명령 확인 (쉼표가 있으면 각도 데이터)
  if (strchr(cmdBuffer, ',') != NULL) {
    parseAndMoveToAngles();
    return;
  }
  
  // 기존 명령어 처리
  if (strcmp(cmdBuffer, "save") == 0) {
    saveCurrentPosition();
  }
  else if (strcmp(cmdBuffer, "play") == 0) {
    startPlayOnce();
  }
  else if (strcmp(cmdBuffer, "auto") == 0) {
    startAutoMode();
  }
  else if (strcmp(cmdBuffer, "stop") == 0) {
    stopAutoMode();
  }
  else if (strcmp(cmdBuffer, "clear") == 0) {
    clearAllPositions();
  }
  else if (strcmp(cmdBuffer, "list") == 0) {
    listAllPositions();
  }
#if ENABLE_DEBUG
  else {
    Serial.print(F("Unknown command: "));
    Serial.println(cmdBuffer);
  }
#endif
}

/**
 * 각도 데이터 파싱 및 이동
 * 형식: 90,80,90,30
 */
void parseAndMoveToAngles() {
  int targetAngles[SERVO_COUNT];
  int angleCount = 0;
  
  // 문자열을 쉼표로 분리
  char* token = strtok(cmdBuffer, ",");
  
  while (token != NULL && angleCount < SERVO_COUNT) {
    // 문자열을 정수로 변환
    int angle = atoi(token);
    
    // 각도 유효성 검사
    int minAngle = pgm_read_word(&minAngles[angleCount]);
    int maxAngle = pgm_read_word(&maxAngles[angleCount]);
    
    if (angle < minAngle) {
      angle = minAngle;
#if ENABLE_DEBUG
      Serial.print(F("Warning: Min angle limit ["));
      Serial.print(angleCount);
      Serial.print(F("] "));
      Serial.println(angle);
#endif
    }
    
    if (angle > maxAngle) {
      angle = maxAngle;
#if ENABLE_DEBUG
      Serial.print(F("Warning: Max angle limit ["));
      Serial.print(angleCount);
      Serial.print(F("] "));
      Serial.println(angle);
#endif
    }
    
    targetAngles[angleCount] = angle;
    angleCount++;
    
    // 다음 토큰
    token = strtok(NULL, ",");
  }
  
  // 4개의 각도가 모두 입력되었는지 확인
  if (angleCount != SERVO_COUNT) {
#if ENABLE_DEBUG
    Serial.print(F("Error: Need "));
    Serial.print(SERVO_COUNT);
    Serial.print(F(" angles (received: "));
    Serial.print(angleCount);
    Serial.println(F(")"));
#endif
    return;
  }
  
#if ENABLE_DEBUG
  // 현재 각도 출력
  Serial.print(F("Current angles: "));
  for (int i = 0; i < SERVO_COUNT; i++) {
    Serial.print(currentAngles[i]);
    if (i < SERVO_COUNT - 1) Serial.print(F(","));
  }
  Serial.println();
  
  // 목표 각도 출력
  Serial.print(F("Target angles: "));
  for (int i = 0; i < SERVO_COUNT; i++) {
    Serial.print(targetAngles[i]);
    if (i < SERVO_COUNT - 1) Serial.print(F(","));
  }
  Serial.println();
#endif
  
  // 목표 위치로 이동
  moveToPosition(targetAngles);
  
#if ENABLE_DEBUG
  Serial.println(F("Move complete"));
#endif
}

/**
 * 목표 위치로 부드럽게 이동
 */
void moveToPosition(int target[]) {
  bool moving = true;
  
  while (moving) {
    moving = false;
    
    for (int i = 0; i < SERVO_COUNT; i++) {
      if (currentAngles[i] < target[i]) {
        currentAngles[i]++;
        moving = true;
      }
      else if (currentAngles[i] > target[i]) {
        currentAngles[i]--;
        moving = true;
      }
      servo[i].write(currentAngles[i]);
    }
    
    delay(15);  // 부드러운 이동
  }
}

/**
 * 현재 위치 저장
 */
void saveCurrentPosition() {
  if (savedPositionCount >= MAX_POSITIONS) {
#if ENABLE_DEBUG
    Serial.println(F("Storage full"));
#endif
    return;
  }
  
  // 서보 일시 중지
  for (int i = 0; i < SERVO_COUNT; i++) {
    servo[i].detach();
  }
  delay(30);
  
  // EEPROM에 저장
  int addr = EEPROM_ADDR_START + (savedPositionCount * SERVO_COUNT);
  for (int i = 0; i < SERVO_COUNT; i++) {
    EEPROM.write(addr + i, currentAngles[i]);
    delay(4);
  }
  
  savedPositionCount++;
  EEPROM.write(EEPROM_ADDR_COUNT, savedPositionCount);
  delay(4);
  
  // 서보 재연결
  for (int i = 0; i < SERVO_COUNT; i++) {
    servo[i].attach(servoPins[i]);
    servo[i].write(currentAngles[i]);
    delay(15);
  }
  
#if ENABLE_DEBUG
  Serial.print(F("Position saved #"));
  Serial.println(savedPositionCount);
#endif
}

/**
 * 1회 재생 시작
 */
void startPlayOnce() {
  // 버퍼 비우기
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
  
#if ENABLE_DEBUG
  Serial.println(F("Play once started"));
#endif
}

/**
 * 자동 반복 모드 시작
 */
void startAutoMode() {
  if (savedPositionCount == 0) {
#if ENABLE_DEBUG
    Serial.println(F("No saved data"));
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
  Serial.print(F("Auto repeat started ("));
  Serial.print(savedPositionCount);
  Serial.println(F(" steps)"));
#endif
}

/**
 * 자동 반복 모드 중지
 */
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
  Serial.print(F("Auto repeat stopped (total "));
  Serial.print(repeatCount);
  Serial.println(F(" times)"));
#endif
}

/**
 * 저장된 모든 위치 재생
 */
void playAllPositions() {
  if (savedPositionCount == 0) {
    autoRepeat = false;
    return;
  }
  
  if (autoRepeat) {
    repeatCount++;
  }
  
  isPlaying = true;
  
  // 모든 저장된 위치 재생
  for (int pos = 0; pos < savedPositionCount; pos++) {
    handleCommand();
    if (!autoRepeat && !isPlaying) return;
    
    // EEPROM에서 각도 읽기
    int addr = EEPROM_ADDR_START + (pos * SERVO_COUNT);
    int targetAngles[SERVO_COUNT];
    bool dataValid = true;
    
    for (int i = 0; i < SERVO_COUNT; i++) {
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
    Serial.print(F("Step #"));
    Serial.print(pos + 1);
    Serial.print(F(": "));
    for (int i = 0; i < SERVO_COUNT; i++) {
      Serial.print(targetAngles[i]);
      if (i < SERVO_COUNT - 1) Serial.print(F(","));
    }
    Serial.println();
#endif
    
    // 이동
    if (!moveToPositionWithStop(targetAngles)) return;
    
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
 * 재생 중 목표 위치로 이동 (stop 명령 체크 포함)
 */
bool moveToPositionWithStop(int target[]) {
  bool moving = true;
  
  while (moving) {
    // stop 명령 체크
    handleCommand();
    if (!autoRepeat && !isPlaying) {
      return false;  // stop 명령으로 중지
    }
    
    moving = false;
    
    for (int i = 0; i < SERVO_COUNT; i++) {
      if (currentAngles[i] < target[i]) {
        currentAngles[i]++;
        moving = true;
      }
      else if (currentAngles[i] > target[i]) {
        currentAngles[i]--;
        moving = true;
      }
      servo[i].write(currentAngles[i]);
    }
    
    delay(15);
  }
  
  return true;  // 이동 완료
}

/**
 * 모든 위치 삭제
 */
void clearAllPositions() {
  autoRepeat = false;
  playOnce = false;
  isPlaying = false;
  repeatCount = 0;
  
  // 서보 일시 중지
  for (int i = 0; i < SERVO_COUNT; i++) {
    servo[i].detach();
  }
  delay(30);
  
  // EEPROM 초기화
  savedPositionCount = 0;
  EEPROM.write(EEPROM_ADDR_COUNT, 0);
  delay(5);
  
  // 서보 재연결
  for (int i = 0; i < SERVO_COUNT; i++) {
    servo[i].attach(servoPins[i]);
    servo[i].write(currentAngles[i]);
    delay(15);
  }
  
#if ENABLE_DEBUG
  Serial.println(F("All positions cleared"));
#endif
}

/**
 * 저장된 모든 위치 출력
 */
void listAllPositions() {
#if ENABLE_DEBUG
  if (savedPositionCount == 0) {
    Serial.println(F("No saved positions"));
    return;
  }
  
  Serial.print(F("Saved positions (total "));
  Serial.print(savedPositionCount);
  Serial.println(F("):"));
  
  // 각 저장된 위치 출력
  for (int pos = 0; pos < savedPositionCount; pos++) {
    int addr = EEPROM_ADDR_START + (pos * SERVO_COUNT);
    
    Serial.print(F("  #"));
    Serial.print(pos + 1);
    Serial.print(F(": "));
    
    for (int i = 0; i < SERVO_COUNT; i++) {
      Serial.print(EEPROM.read(addr + i));
      if (i < SERVO_COUNT - 1) Serial.print(F(","));
    }
    Serial.println();
  }
#endif
}

