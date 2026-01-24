/**
 * 로봇 팔 - 5단계: Bluetooth EEPROM 자동화 시스템
 * 
 * Bluetooth로 위치를 저장하고 자동 재생
 * - 조이스틱으로 로봇 팔 제어
 * - Bluetooth/Serial 명령으로 위치 저장/재생
 * - EEPROM에 최대 12단계 저장
 * 
 * 명령어:
 * - save  : 현재 위치 저장
 * - play  : 저장된 동작 1회 재생
 * - auto  : 저장된 동작 계속 반복
 * - stop  : 자동 반복 중지
 * - clear : 모든 위치 삭제
 * - list  : 저장된 위치 목록
 * 
 * Bluetooth 연결:
 * - TX: 2번 핀
 * - RX: 3번 핀
 * - HC-05/HC-06 모듈 사용
 * 
 * 문제 발생 시: ENABLE_BLUETOOTH를 0으로 설정하여 테스트
 */

#include <Servo.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

/* ===== Bluetooth 활성화 설정 ===== */
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
int repeatCount = 0;              // 반복 횟수
bool loopStarted = false;         // loop() 시작 플래그

char cmdBuffer[20];
int cmdIndex = 0;

void setup() {
  Serial.begin(9600);
  
  delay(100);  // Serial 안정화
  
  Serial.println("Bluetooth EEPROM 자동화 시스템 시작\n");
  Serial.println("서보 모터 초기화 중...");
  
  // 서보 모터를 하나씩 천천히 초기화 (과전류 방지)
  for (int i = 0; i < 4; i++) {
    servo[i].attach(pin[i]);
    delay(200);  // 서보 연결 안정화
    
    // 현재 위치에서 목표 위치로 천천히 이동 (과전류 방지)
    int currentPos = servo[i].read();
    if (currentPos < 0 || currentPos > 180) {
      currentPos = 90;  // 잘못된 값이면 중간값으로
    }
    
    // 천천히 목표 각도로 이동
    if (currentPos < angles[i]) {
      for (int pos = currentPos; pos <= angles[i]; pos++) {
        servo[i].write(pos);
        delay(20);  // 매우 천천히 이동
      }
    } else {
      for (int pos = currentPos; pos >= angles[i]; pos--) {
        servo[i].write(pos);
        delay(20);  // 매우 천천히 이동
      }
    }
    
    delay(300);  // 다음 서보 초기화 전 대기
  }
  
  Serial.println("서보 모터 초기화 완료!\n");
  
  // EEPROM에서 저장된 위치 개수 읽기
  EEPROM.get(EEPROM_ADDR_COUNT, savedCount);
  if (savedCount < 0 || savedCount > MAX_POSITIONS) {
    savedCount = 0;
    EEPROM.put(EEPROM_ADDR_COUNT, savedCount);
  }
  
  Serial.println("명령어:");
  Serial.println("  save  : 현재 위치 저장");
  Serial.println("  play  : 저장된 동작 1회 재생");
  Serial.println("  auto  : 저장된 동작 계속 반복");
  Serial.println("  stop  : 자동 반복 중지");
  Serial.println("  clear : 모든 위치 삭제");
  Serial.println("  list  : 저장된 위치 목록\n");
  
  // 각도 범위 출력
  Serial.println("각도 범위:");
  Serial.print("  베이스: ");
  Serial.print(minAngles[0]);
  Serial.print(" ~ ");
  Serial.println(maxAngles[0]);
  Serial.print("  팔꿈치: ");
  Serial.print(minAngles[1]);
  Serial.print(" ~ ");
  Serial.println(maxAngles[1]);
  Serial.print("  손목: ");
  Serial.print(minAngles[2]);
  Serial.print(" ~ ");
  Serial.println(maxAngles[2]);
  Serial.print("  그립: ");
  Serial.print(minAngles[3]);
  Serial.print(" ~ ");
  Serial.println(maxAngles[3]);
  
  Serial.print("\n저장된 위치: ");
  Serial.print(savedCount);
  Serial.print(" / ");
  Serial.println(MAX_POSITIONS);
  Serial.println("\n조이스틱으로 제어하세요\n");
  
  // 마지막에 Bluetooth 초기화 (리셋 방지를 위해 맨 끝에)
#if ENABLE_BLUETOOTH
  Serial.println("Bluetooth 초기화 중...");
  BTSerial.begin(9600);
  delay(500);  // Bluetooth 충분히 안정화
  Serial.println("Bluetooth 활성화됨");
#else
  Serial.println("Bluetooth 비활성화 (Serial만 사용)");
#endif
  
  Serial.println(">>> Setup 완료! <<<\n");
}

void loop() {
  // 첫 loop() 진입 확인 (setup이 무한 반복되는지 디버깅)
  if (!loopStarted) {
    Serial.println(">>> Loop 시작! <<<");
    loopStarted = true;
  }
  
  // Serial/Bluetooth 명령 처리 (항상 체크)
  handleCommand();
  
  // 자동 반복 모드
  if (autoRepeat && !isPlaying) {
    playAllPositions();
    return;
  }
  
  // 자동 재생 중이 아닐 때만 조이스틱 제어
  if (!isPlaying) {
    handleJoystick();
  }
}

/**
 * 조이스틱 제어
 * 
 * 주의: 조이스틱 출력은 Serial만 사용!
 * BTSerial 출력 시 SoftwareSerial과 Servo 타이머 충돌로 과부하 발생
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
  
  // 변경된 서보만 출력 (Serial만 사용, 과부하 방지)
  const char* names[] = {"베이스", "팔꿈치", "손목", "그립"};
  for (int i = 0; i < 4; i++) {
    if (angles[i] != prevAngles[i]) {
      Serial.print(names[i]);
      Serial.print(": ");
      Serial.print(prevAngles[i]);
      Serial.print(" → ");
      Serial.println(angles[i]);
      prevAngles[i] = angles[i];
    }
  }
  
  delay(20);
}

/**
 * Serial/Bluetooth 명령 처리
 */
void handleCommand() {
  // Serial 또는 Bluetooth에서 입력 받기
  char ch = '\0';
  bool fromBT = false;
  
#if ENABLE_BLUETOOTH
  if (BTSerial.available() > 0) {
    ch = BTSerial.read();
    fromBT = true;
  }
  else
#endif
  if (Serial.available() > 0) {
    ch = Serial.read();
    fromBT = false;
  }
  
  if (ch == '\0') return;
  
  // 개행 문자를 만나면 명령 처리
  if (ch == '\n' || ch == '\r') {
    if (cmdIndex > 0) {
      cmdBuffer[cmdIndex] = '\0';
      processCommand(fromBT);
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
 * 명령 처리
 */
void processCommand(bool fromBT) {
  // 명령 출처 출력
  Serial.print("[명령 from ");
#if ENABLE_BLUETOOTH
  if (fromBT) {
    Serial.print("Bluetooth");
  } else {
    Serial.print("Serial");
  }
#else
  Serial.print("Serial");
#endif
  Serial.print("] ");
  Serial.println(cmdBuffer);
  
  // save 명령
  if (strcmp(cmdBuffer, "save") == 0) {
    saveCurrentPosition();
  }
  // play 명령 (1회 재생)
  else if (strcmp(cmdBuffer, "play") == 0) {
    autoRepeat = false;
    playAllPositions();
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
  else {
    printMessage("[오류] 알 수 없는 명령");
    printMessage("사용 가능: save, play, auto, stop, clear, list");
  }
}

/**
 * 현재 위치 저장
 */
void saveCurrentPosition() {
  if (savedCount >= MAX_POSITIONS) {
    printMessage("[오류] 저장 공간이 가득 찼습니다");
    printMessage("clear 명령으로 초기화하세요");
    return;
  }
  
  // EEPROM에 현재 각도 저장
  int addr = EEPROM_ADDR_START + (savedCount * 4);
  for (int i = 0; i < 4; i++) {
    EEPROM.write(addr + i, angles[i]);
  }
  
  savedCount++;
  EEPROM.put(EEPROM_ADDR_COUNT, savedCount);
  
  // 저장 완료 메시지
  Serial.print("[저장 완료] 위치 #");
  Serial.print(savedCount);
  Serial.print(" - 베이스:");
  Serial.print(angles[0]);
  Serial.print(" 팔꿈치:");
  Serial.print(angles[1]);
  Serial.print(" 손목:");
  Serial.print(angles[2]);
  Serial.print(" 그립:");
  Serial.println(angles[3]);
  
#if ENABLE_BLUETOOTH
  BTSerial.print("[저장 완료] 위치 #");
  BTSerial.print(savedCount);
  BTSerial.print(" - 베이스:");
  BTSerial.print(angles[0]);
  BTSerial.print(" 팔꿈치:");
  BTSerial.print(angles[1]);
  BTSerial.print(" 손목:");
  BTSerial.print(angles[2]);
  BTSerial.print(" 그립:");
  BTSerial.println(angles[3]);
#endif
  
  printSavedCount();
  printMessage("");
}

/**
 * 자동 반복 모드 시작
 */
void startAutoMode() {
  if (savedCount == 0) {
    printMessage("[알림] 저장된 위치가 없습니다");
    printMessage("save 명령으로 위치를 저장하세요");
    return;
  }
  
  autoRepeat = true;
  repeatCount = 0;
  
  printMessage("========================================");
  printMessage("  자동 반복 모드 시작");
  printMessage("========================================");
  
  Serial.print("저장된 위치 수: ");
  Serial.println(savedCount);
#if ENABLE_BLUETOOTH
  BTSerial.print("저장된 위치 수: ");
  BTSerial.println(savedCount);
#endif
  
  printMessage("'stop' 명령으로 중지");
  printMessage("========================================\n");
}

/**
 * 자동 반복 모드 중지
 */
void stopAutoMode() {
  if (!autoRepeat) {
    printMessage("[알림] 자동 반복 모드가 실행 중이 아닙니다");
    return;
  }
  
  autoRepeat = false;
  isPlaying = false;
  
  printMessage("\n========================================");
  printMessage("  자동 반복 모드 중지");
  printMessage("========================================");
  
  Serial.print("총 반복 횟수: ");
  Serial.println(repeatCount);
#if ENABLE_BLUETOOTH
  BTSerial.print("총 반복 횟수: ");
  BTSerial.println(repeatCount);
#endif
  
  printMessage("========================================\n");
  printMessage("조이스틱으로 제어하세요\n");
}

/**
 * 저장된 모든 위치 재생
 */
void playAllPositions() {
  if (savedCount == 0) {
    printMessage("[알림] 저장된 위치가 없습니다");
    printMessage("save 명령으로 위치를 저장하세요");
    autoRepeat = false;
    return;
  }
  
  if (autoRepeat) {
    repeatCount++;
    printMessage("========================================");
    
    Serial.print("  반복 #");
    Serial.print(repeatCount);
    Serial.println(" 실행 중...");
#if ENABLE_BLUETOOTH
    BTSerial.print("  반복 #");
    BTSerial.print(repeatCount);
    BTSerial.println(" 실행 중...");
#endif
    
    printMessage("========================================");
  } else {
    printMessage("========================================");
    printMessage("  자동 재생 시작 (1회)");
    printMessage("========================================");
    
    Serial.print("저장된 위치 수: ");
    Serial.println(savedCount);
#if ENABLE_BLUETOOTH
    BTSerial.print("저장된 위치 수: ");
    BTSerial.println(savedCount);
#endif
    
    printMessage("");
  }
  
  isPlaying = true;
  
  // 모든 저장된 위치를 순차적으로 재생
  for (int pos = 0; pos < savedCount; pos++) {
    // stop 명령 체크
    handleCommand();
    if (!autoRepeat && isPlaying) {
      // 1회 재생 모드는 계속 진행
    } else if (!autoRepeat && !isPlaying) {
      // stop 명령으로 중지됨
      return;
    }
    
    // 위치 이동 메시지
    Serial.print("→ 위치 #");
    Serial.print(pos + 1);
    Serial.println(" 이동");
    
    // EEPROM에서 각도 읽기
    int addr = EEPROM_ADDR_START + (pos * 4);
    int targetAngles[4];
    for (int i = 0; i < 4; i++) {
      targetAngles[i] = EEPROM.read(addr + i);
    }
    
    // 목표 위치 출력 (Serial만)
    Serial.print("  베이스:");
    Serial.print(targetAngles[0]);
    Serial.print(" 팔꿈치:");
    Serial.print(targetAngles[1]);
    Serial.print(" 손목:");
    Serial.print(targetAngles[2]);
    Serial.print(" 그립:");
    Serial.println(targetAngles[3]);
    
    // 부드럽게 이동
    if (!moveToPosition(targetAngles)) {
      // stop 명령으로 중지됨
      return;
    }
    
    // 다음 위치로 이동 전 대기 (stop 명령 체크 포함)
    for (int i = 0; i < 100; i++) {
      handleCommand();
      if (!autoRepeat && !isPlaying) {
        return;
      }
      delay(10);  // 총 1000ms = 1초
    }
  }
  
  if (!autoRepeat) {
    printMessage("\n========================================");
    printMessage("  자동 재생 완료");
    printMessage("========================================\n");
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

/**
 * 모든 저장된 위치 삭제
 */
void clearAllPositions() {
  // 자동 반복 중지
  if (autoRepeat) {
    autoRepeat = false;
    printMessage("[알림] 자동 반복 모드 중지됨\n");
  }
  
  savedCount = 0;
  repeatCount = 0;
  EEPROM.put(EEPROM_ADDR_COUNT, savedCount);
  
  printMessage("========================================");
  printMessage("  모든 위치 삭제 완료");
  printMessage("========================================");
  printMessage("새로운 동작을 저장할 수 있습니다\n");
}

/**
 * 저장된 위치 목록 출력
 */
void listAllPositions() {
  printMessage("========================================");
  printMessage("  저장된 위치 목록");
  printMessage("========================================");
  printSavedCount();
  
  if (savedCount == 0) {
    printMessage("저장된 위치가 없습니다");
  } else {
    printMessage("");
    for (int pos = 0; pos < savedCount; pos++) {
      int addr = EEPROM_ADDR_START + (pos * 4);
      
      Serial.print("위치 #");
      Serial.print(pos + 1);
      Serial.print(": 베이스:");
      Serial.print(EEPROM.read(addr));
      Serial.print(" 팔꿈치:");
      Serial.print(EEPROM.read(addr + 1));
      Serial.print(" 손목:");
      Serial.print(EEPROM.read(addr + 2));
      Serial.print(" 그립:");
      Serial.println(EEPROM.read(addr + 3));
      
#if ENABLE_BLUETOOTH
      BTSerial.print("위치 #");
      BTSerial.print(pos + 1);
      BTSerial.print(": 베이스:");
      BTSerial.print(EEPROM.read(addr));
      BTSerial.print(" 팔꿈치:");
      BTSerial.print(EEPROM.read(addr + 1));
      BTSerial.print(" 손목:");
      BTSerial.print(EEPROM.read(addr + 2));
      BTSerial.print(" 그립:");
      BTSerial.println(EEPROM.read(addr + 3));
#endif
    }
  }
  printMessage("========================================\n");
}

/**
 * Serial과 Bluetooth 모두에 메시지 출력
 */
void printMessage(const char* msg) {
  Serial.println(msg);
#if ENABLE_BLUETOOTH
  BTSerial.println(msg);
#endif
}

/**
 * 각도 범위 출력 (사용 안 함 - 직접 출력으로 대체)
 */
void printAngleRange(const char* name, int minVal, int maxVal) {
  Serial.print(name);
  Serial.print(minVal);
  Serial.print(" ~ ");
  Serial.println(maxVal);
}

/**
 * 저장된 위치 개수 출력
 */
void printSavedCount() {
  Serial.print("저장된 위치: ");
  Serial.print(savedCount);
  Serial.print(" / ");
  Serial.println(MAX_POSITIONS);
#if ENABLE_BLUETOOTH
  BTSerial.print("저장된 위치: ");
  BTSerial.print(savedCount);
  BTSerial.print(" / ");
  BTSerial.println(MAX_POSITIONS);
#endif
}

/**
 * 서보 변경 출력 (조이스틱용 - Serial만 사용)
 */
void printServoChange(const char* name, int oldVal, int newVal) {
  Serial.print(name);
  Serial.print(": ");
  Serial.print(oldVal);
  Serial.print(" → ");
  Serial.println(newVal);
  // Bluetooth 출력 안 함 (과부하 방지)
}
