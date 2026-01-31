/**
 * ====================================================================
 * 로봇 팔 07: 앱인벤터 색상 구역 제어 시스템
 * ====================================================================
 * 
 * 기능:
 * - 조이스틱으로 로봇 팔 제어
 * - 앱인벤터에서 색상 구역별 위치 저장/이동
 * - 간단한 명령어 프로토콜 (헤더 없음)
 * - EEPROM에 구역별 위치 저장
 * 
 * 명령어 (Bluetooth):
 * - start_         : 자동 실행 시작
 * - stop_          : 자동 실행 중지
 * - reset_         : 초기 위치로 이동
 * - save_          : 현재 위치 저장 (현재 모드에 따라)
 * - redCheck_      : 빨강 구역으로 이동
 * - blueCheck_     : 파랑 구역으로 이동
 * - greenCheck_    : 초록 구역으로 이동
 * - 12,7,1_        : RED=12개, GREEN=7개, BLUE=1개 (카운트 정보 앱으로 전달)
 * 
 * Bluetooth 연결:
 * - TX: 2번 핀 (아두이노 TX) → RX (HC-05/06)
 * - RX: 3번 핀 (아두이노 RX) → TX (HC-05/06)
 * 
 * 작성일: 2026-01-31
 * 버전: 1.0
 * ====================================================================
 */

#include <Servo.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

/* ===== 디버그 모드 설정 ===== */
#define ENABLE_DEBUG      1  // 1: 디버그 출력, 0: 운영 모드
#define ENABLE_BLUETOOTH  1  // 1: Bluetooth 사용

/* ===== Bluetooth 설정 ===== */
#define PIN_BT_TX   2
#define PIN_BT_RX   3

#if ENABLE_BLUETOOTH
SoftwareSerial BTSerial(PIN_BT_TX, PIN_BT_RX);
#endif

/* ===== 서보 설정 ===== */
Servo servo[4];
int pin[4] = {4, 5, 6, 7};

// 각도 범위
int minAngles[4] = {0, 50, 60, 10};
int maxAngles[4] = {180, 110, 120, 60};

// 현재 각도 (베이스, 팔꿈치, 손목, 그립)
int angles[4] = {90, 80, 90, 15};

/* ===== EEPROM 주소 정의 ===== */
#define EEPROM_ADDR_HOME    0      // 초기 위치 (4바이트)
#define EEPROM_ADDR_RED     4      // 빨강 구역 (4바이트)
#define EEPROM_ADDR_GREEN   8      // 초록 구역 (4바이트)
#define EEPROM_ADDR_BLUE    12     // 파랑 구역 (4바이트)
#define EEPROM_ADDR_RED_CNT 16     // 빨강 카운트 (1바이트)
#define EEPROM_ADDR_GREEN_CNT 17   // 초록 카운트 (1바이트)
#define EEPROM_ADDR_BLUE_CNT 18    // 파랑 카운트 (1바이트)

/* ===== 전역 변수 ===== */
bool isRunning = false;            // 자동 실행 중
int redCount = 0;                  // 빨강 물체 개수
int greenCount = 0;                // 초록 물체 개수
int blueCount = 0;                 // 파랑 물체 개수

// 명령어 버퍼
char cmdBuffer[30];
int cmdIndex = 0;
unsigned long lastCharTime = 0;
#define CMD_TIMEOUT 100            // 명령어 타임아웃 (ms)

// 현재 모드 (위치 저장용)
enum SaveMode { MODE_RED, MODE_GREEN, MODE_BLUE, MODE_HOME };
SaveMode currentMode = MODE_HOME;

/* ===== 초기화 ===== */
void setup() {
#if ENABLE_DEBUG
  Serial.begin(9600);
  delay(50);
  Serial.println("=== Robot Arm v1.0 ===");
#endif
  
  // 서보 초기화
  for (int i = 0; i < 4; i++) {
    servo[i].attach(pin[i]);
    delay(100);
  }
  
  // EEPROM에서 카운트 읽기
  loadCounts();
  
  // 초기 위치로 이동
  goToHome();
  
  // Bluetooth 초기화
#if ENABLE_BLUETOOTH
  BTSerial.begin(9600);
  delay(300);
#if ENABLE_DEBUG
  Serial.println("[BT] Ready");
#endif
#endif

#if ENABLE_DEBUG
  Serial.println("[System] Ready");
  Serial.print("[Count] R:");
  Serial.print(redCount);
  Serial.print(" G:");
  Serial.print(greenCount);
  Serial.print(" B:");
  Serial.println(blueCount);
#endif
}

/* ===== 메인 루프 ===== */
void loop() {
  handleCommand();       // 명령 처리
  
  if (!isRunning) {
    handleJoystick();    // 수동 제어
  }
}

/* ===== 조이스틱 제어 ===== */
void handleJoystick() {
  int val[4];
  val[0] = analogRead(14);              // A0 - 베이스
  val[1] = analogRead(15);              // A1 - 팔꿈치
  val[2] = 1024 - analogRead(16);       // A2 - 손목
  val[3] = 1024 - analogRead(17);       // A3 - 그립
  
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

/* ===== 명령어 처리 ===== */
void handleCommand() {
  // 타임아웃 체크
  if (cmdIndex > 0 && (millis() - lastCharTime) > CMD_TIMEOUT) {
    cmdBuffer[cmdIndex] = '\0';
    processCommand();
    cmdIndex = 0;
    return;
  }
  
  // Bluetooth 수신
  char ch = '\0';
#if ENABLE_BLUETOOTH
  if (BTSerial.available() > 0) {
    ch = BTSerial.read();
    lastCharTime = millis();
  }
#endif
  
  if (ch == '\0') return;
  
  // 종료 문자 확인
  if (ch == '\n' || ch == '\r' || ch == '_') {
    if (cmdIndex > 0) {
      cmdBuffer[cmdIndex] = '\0';
#if ENABLE_DEBUG
      Serial.print("[CMD] ");
      Serial.println(cmdBuffer);
#endif
      processCommand();
      cmdIndex = 0;
    }
    return;
  }
  
  // 공백 무시
  if (ch == ' ') return;
  
  // 버퍼에 추가
  if (cmdIndex < 29) {
    cmdBuffer[cmdIndex++] = ch;
  }
}

/* ===== 명령어 파싱 및 실행 ===== */
void processCommand() {
  // start: 자동 실행 시작
  if (strcmp(cmdBuffer, "start") == 0) {
    startAutoMode();
  }
  // stop: 자동 실행 중지
  else if (strcmp(cmdBuffer, "stop") == 0) {
    stopAutoMode();
  }
  // reset: 초기 위치
  else if (strcmp(cmdBuffer, "reset") == 0) {
    goToHome();
    sendStatus("reset");
  }
  // save: 현재 위치 저장
  else if (strcmp(cmdBuffer, "save") == 0) {
    saveCurrentPosition();
  }
  // redCheck: 빨강 구역으로 이동
  else if (strcmp(cmdBuffer, "redCheck") == 0) {
    goToRedZone();
  }
  // greenCheck: 초록 구역으로 이동
  else if (strcmp(cmdBuffer, "greenCheck") == 0) {
    goToGreenZone();
  }
  // blueCheck: 파랑 구역으로 이동
  else if (strcmp(cmdBuffer, "blueCheck") == 0) {
    goToBlueZone();
  }
  // 구역 카운트 설정: "12,7,1" 형식
  else if (strchr(cmdBuffer, ',') != NULL) {
    parseCountData(cmdBuffer);
  }
#if ENABLE_DEBUG
  else {
    Serial.print("[ERR] Unknown: ");
    Serial.println(cmdBuffer);
  }
#endif
}

/* ===== 자동 실행 시작 ===== */
void startAutoMode() {
  isRunning = true;
  
#if ENABLE_DEBUG
  Serial.println("[Auto] Start");
#endif

  sendStatus("start");
}

/* ===== 자동 실행 중지 ===== */
void stopAutoMode() {
  isRunning = false;
  
#if ENABLE_DEBUG
  Serial.println("[Auto] Stop");
#endif

  sendStatus("stop");
}

/* ===== 초기 위치로 이동 ===== */
void goToHome() {
  // EEPROM에서 초기 위치 읽기
  int homeAngles[4];
  bool dataValid = true;
  
  for (int i = 0; i < 4; i++) {
    homeAngles[i] = EEPROM.read(EEPROM_ADDR_HOME + i);
    if (homeAngles[i] < 0 || homeAngles[i] > 180) {
      dataValid = false;
      break;
    }
  }
  
  // 유효하지 않으면 기본값 사용
  if (!dataValid) {
    homeAngles[0] = 90;
    homeAngles[1] = 80;
    homeAngles[2] = 90;
    homeAngles[3] = 15;
  }
  
  moveToPosition(homeAngles);
  
#if ENABLE_DEBUG
  Serial.print("[Move] Home: ");
  printAngles(homeAngles);
#endif
}

/* ===== 빨강 구역으로 이동 ===== */
void goToRedZone() {
  currentMode = MODE_RED;
  int redAngles[4];
  
  if (loadPosition(EEPROM_ADDR_RED, redAngles)) {
    moveToPosition(redAngles);
    redCount++;
    saveCount(EEPROM_ADDR_RED_CNT, redCount);
    sendCountData();
    
#if ENABLE_DEBUG
    Serial.print("[Move] Red: ");
    printAngles(redAngles);
    Serial.print(" Count:");
    Serial.println(redCount);
#endif
  } else {
#if ENABLE_DEBUG
    Serial.println("[ERR] Red position not saved");
#endif
  }
}

/* ===== 초록 구역으로 이동 ===== */
void goToGreenZone() {
  currentMode = MODE_GREEN;
  int greenAngles[4];
  
  if (loadPosition(EEPROM_ADDR_GREEN, greenAngles)) {
    moveToPosition(greenAngles);
    greenCount++;
    saveCount(EEPROM_ADDR_GREEN_CNT, greenCount);
    sendCountData();
    
#if ENABLE_DEBUG
    Serial.print("[Move] Green: ");
    printAngles(greenAngles);
    Serial.print(" Count:");
    Serial.println(greenCount);
#endif
  } else {
#if ENABLE_DEBUG
    Serial.println("[ERR] Green position not saved");
#endif
  }
}

/* ===== 파랑 구역으로 이동 ===== */
void goToBlueZone() {
  currentMode = MODE_BLUE;
  int blueAngles[4];
  
  if (loadPosition(EEPROM_ADDR_BLUE, blueAngles)) {
    moveToPosition(blueAngles);
    blueCount++;
    saveCount(EEPROM_ADDR_BLUE_CNT, blueCount);
    sendCountData();
    
#if ENABLE_DEBUG
    Serial.print("[Move] Blue: ");
    printAngles(blueAngles);
    Serial.print(" Count:");
    Serial.println(blueCount);
#endif
  } else {
#if ENABLE_DEBUG
    Serial.println("[ERR] Blue position not saved");
#endif
  }
}

/* ===== 현재 위치 저장 ===== */
void saveCurrentPosition() {
  // 서보 일시 정지
  for (int i = 0; i < 4; i++) {
    servo[i].detach();
  }
  delay(30);
  
  int addr;
  const char* zoneName;
  
  // 현재 모드에 따라 저장 위치 결정
  switch (currentMode) {
    case MODE_RED:
      addr = EEPROM_ADDR_RED;
      zoneName = "Red";
      break;
    case MODE_GREEN:
      addr = EEPROM_ADDR_GREEN;
      zoneName = "Green";
      break;
    case MODE_BLUE:
      addr = EEPROM_ADDR_BLUE;
      zoneName = "Blue";
      break;
    case MODE_HOME:
    default:
      addr = EEPROM_ADDR_HOME;
      zoneName = "Home";
      break;
  }
  
  // EEPROM에 저장
  for (int i = 0; i < 4; i++) {
    EEPROM.write(addr + i, angles[i]);
    delay(4);
  }
  
  // 서보 재연결
  for (int i = 0; i < 4; i++) {
    servo[i].attach(pin[i]);
    servo[i].write(angles[i]);
    delay(15);
  }
  
#if ENABLE_DEBUG
  Serial.print("[Save] ");
  Serial.print(zoneName);
  Serial.print(": ");
  printAngles(angles);
#endif

  sendStatus("saved");
}

/* ===== EEPROM에서 위치 읽기 ===== */
bool loadPosition(int addr, int targetAngles[]) {
  bool dataValid = true;
  
  for (int i = 0; i < 4; i++) {
    targetAngles[i] = EEPROM.read(addr + i);
    
    // 유효성 검사
    if (targetAngles[i] < 0 || targetAngles[i] > 180) {
      dataValid = false;
      break;
    }
  }
  
  return dataValid;
}

/* ===== 카운트 데이터 파싱: "12,7,1" ===== */
void parseCountData(char* data) {
  // 첫 번째 쉼표 찾기
  char* firstComma = strchr(data, ',');
  if (firstComma == NULL) return;
  
  // 두 번째 쉼표 찾기
  char* secondComma = strchr(firstComma + 1, ',');
  if (secondComma == NULL) return;
  
  // 문자열 분리
  *firstComma = '\0';
  *secondComma = '\0';
  
  // 정수 변환
  redCount = atoi(data);
  greenCount = atoi(firstComma + 1);
  blueCount = atoi(secondComma + 1);
  
  // EEPROM에 저장
  saveCount(EEPROM_ADDR_RED_CNT, redCount);
  saveCount(EEPROM_ADDR_GREEN_CNT, greenCount);
  saveCount(EEPROM_ADDR_BLUE_CNT, blueCount);
  
#if ENABLE_DEBUG
  Serial.print("[Count] Received - R:");
  Serial.print(redCount);
  Serial.print(" G:");
  Serial.print(greenCount);
  Serial.print(" B:");
  Serial.println(blueCount);
#endif

  sendCountData();
}

/* ===== 카운트 데이터 전송 (앱으로) ===== */
void sendCountData() {
#if ENABLE_BLUETOOTH
  BTSerial.print(redCount);
  BTSerial.print(",");
  BTSerial.print(greenCount);
  BTSerial.print(",");
  BTSerial.print(blueCount);
  BTSerial.println("_");
#endif
}

/* ===== 상태 전송 ===== */
void sendStatus(const char* status) {
#if ENABLE_BLUETOOTH
  BTSerial.print(status);
  BTSerial.println("_");
#endif
}

/* ===== 카운트 저장 ===== */
void saveCount(int addr, int count) {
  EEPROM.write(addr, count);
  delay(4);
}

/* ===== 카운트 로드 ===== */
void loadCounts() {
  redCount = EEPROM.read(EEPROM_ADDR_RED_CNT);
  greenCount = EEPROM.read(EEPROM_ADDR_GREEN_CNT);
  blueCount = EEPROM.read(EEPROM_ADDR_BLUE_CNT);
  
  // 유효성 검사 (0-255)
  if (redCount > 100) redCount = 0;
  if (greenCount > 100) greenCount = 0;
  if (blueCount > 100) blueCount = 0;
}

/* ===== 목표 위치로 부드럽게 이동 ===== */
void moveToPosition(int target[]) {
  bool moving = true;
  
  while (moving) {
    handleCommand();  // 명령 체크
    
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
    
    delay(15);
  }
}

/* ===== 디버그용 각도 출력 ===== */
#if ENABLE_DEBUG
void printAngles(int ang[]) {
  Serial.print(ang[0]);
  Serial.print(",");
  Serial.print(ang[1]);
  Serial.print(",");
  Serial.print(ang[2]);
  Serial.print(",");
  Serial.println(ang[3]);
}
#endif

/*
 * ====================================================================
 * 프로토콜 규약 (간단한 형식, 헤더 없음)
 * ====================================================================
 * 
 * [앱인벤터 → 아두이노]
 * - start_         : 자동 실행 시작
 * - stop_          : 자동 실행 중지
 * - reset_         : 초기 위치로 이동
 * - save_          : 현재 위치 저장 (현재 모드에 따라)
 * - redCheck_      : 빨강 구역으로 이동 (카운트 +1)
 * - greenCheck_    : 초록 구역으로 이동 (카운트 +1)
 * - blueCheck_     : 파랑 구역으로 이동 (카운트 +1)
 * - 12,7,1_        : RED=12, GREEN=7, BLUE=1 (카운트 설정)
 * 
 * [아두이노 → 앱인벤터]
 * - 12,7,1_        : RED=12, GREEN=7, BLUE=1 (현재 카운트)
 * - start_         : 시작 확인
 * - stop_          : 중지 확인
 * - reset_         : 리셋 확인
 * - saved_         : 저장 완료
 * 
 * ====================================================================
 */

/*
 * ====================================================================
 * 사용 시나리오
 * ====================================================================
 * 
 * 1. 초기 설정:
 *    - 조이스틱으로 초기 위치 조정
 *    - 앱에서 "save_" 전송 (Home 위치 저장)
 *    - 조이스틱으로 빨강 구역 위치 조정
 *    - 앱에서 "redCheck_" + "save_" (빨강 구역 저장)
 *    - 초록, 파랑 구역도 동일하게 설정
 * 
 * 2. 자동 실행:
 *    - 앱에서 "start_" 전송
 *    - Teachable Machine 분류 결과에 따라:
 *      - 빨강 감지 → "redCheck_" 전송
 *      - 초록 감지 → "greenCheck_" 전송
 *      - 파랑 감지 → "blueCheck_" 전송
 *    - 각 구역으로 로봇팔 이동 + 카운트 증가
 *    - 앱에서 "12,7,1_" 수신 (현재 카운트)
 * 
 * 3. 중지:
 *    - 앱에서 "stop_" 전송
 * 
 * 4. 초기화:
 *    - 앱에서 "reset_" 전송 (Home 위치로 이동)
 * 
 * ====================================================================
 */
