/**
 * 로봇 팔 - 4단계: EEPROM 자동화 시스템
 * 
 * 조이스틱으로 위치를 설정하고 저장한 후 자동 재생
 * - 조이스틱으로 로봇 팔 제어
 * - Serial 명령으로 위치 저장/재생
 * - EEPROM에 최대 12단계 저장
 * 
 * 명령어:
 * - save  : 현재 위치 저장
 * - play  : 저장된 동작 1회 재생
 * - auto  : 저장된 동작 계속 반복
 * - stop  : 자동 반복 중지
 * - clear : 모든 위치 삭제
 * - list  : 저장된 위치 목록
 */

#include <Servo.h>
#include <EEPROM.h>

/* ===== 서보 설정 ===== */
Servo servo[4];
int pin[4] = {4, 5, 6, 7};

// 각도 범위 설정
int minAngles[4] = {0, 50, 60, 5};
int maxAngles[4] = {180, 110, 120, 60};

int angles[4] = {90, 80, 90, 30};        // 현재 각도: 베이스, 팔꿈치, 손목, 그립(열린 상태)
int prevAngles[4] = {90, 80, 90, 30};    // 이전 각도

/* ===== EEPROM 설정 ===== */
#define EEPROM_ADDR_COUNT   0     // 저장된 위치 개수 주소
#define EEPROM_ADDR_START   4     // 위치 데이터 시작 주소
#define MAX_POSITIONS       12    // 최대 저장 가능 위치

/* ===== 전역 변수 ===== */
int savedCount = 0;               // 저장된 위치 개수
bool isPlaying = false;           // 자동 재생 중 여부
bool autoRepeat = false;          // 자동 반복 모드
int repeatCount = 0;              // 반복 횟수

char cmdBuffer[20];
int cmdIndex = 0;

void setup() {
  Serial.begin(9600);
  
  delay(100);  // Serial 안정화
  
  Serial.println("EEPROM 자동화 시스템 시작\n");
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
}

void loop() {
  // Serial 명령 처리 (항상 체크)
  handleSerialCommand();
  
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
  
  // 변경된 서보만 출력
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
 * Serial 명령 처리
 */
void handleSerialCommand() {
  if (Serial.available() > 0) {
    char ch = Serial.read();
    
    // 개행 문자를 만나면 명령 처리
    if (ch == '\n' || ch == '\r') {
      if (cmdIndex > 0) {
        cmdBuffer[cmdIndex] = '\0';
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
}

/**
 * 명령 처리
 */
void processCommand() {
  Serial.print("[명령] ");
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
    Serial.println("[오류] 알 수 없는 명령");
    Serial.println("사용 가능: save, play, auto, stop, clear, list");
  }
}

/**
 * 현재 위치 저장
 */
void saveCurrentPosition() {
  if (savedCount >= MAX_POSITIONS) {
    Serial.println("[오류] 저장 공간이 가득 찼습니다");
    Serial.println("clear 명령으로 초기화하세요");
    return;
  }
  
  // EEPROM에 현재 각도 저장
  int addr = EEPROM_ADDR_START + (savedCount * 4);
  for (int i = 0; i < 4; i++) {
    EEPROM.write(addr + i, angles[i]);
  }
  
  savedCount++;
  EEPROM.put(EEPROM_ADDR_COUNT, savedCount);
  
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
  
  Serial.print("저장된 위치: ");
  Serial.print(savedCount);
  Serial.print(" / ");
  Serial.println(MAX_POSITIONS);
  Serial.println();
}

/**
 * 자동 반복 모드 시작
 */
void startAutoMode() {
  if (savedCount == 0) {
    Serial.println("[알림] 저장된 위치가 없습니다");
    Serial.println("save 명령으로 위치를 저장하세요");
    return;
  }
  
  autoRepeat = true;
  repeatCount = 0;
  
  Serial.println("========================================");
  Serial.println("  자동 반복 모드 시작");
  Serial.println("========================================");
  Serial.print("저장된 위치 수: ");
  Serial.println(savedCount);
  Serial.println("'stop' 명령으로 중지");
  Serial.println("========================================\n");
}

/**
 * 자동 반복 모드 중지
 */
void stopAutoMode() {
  if (!autoRepeat) {
    Serial.println("[알림] 자동 반복 모드가 실행 중이 아닙니다");
    return;
  }
  
  autoRepeat = false;
  isPlaying = false;
  
  Serial.println("\n========================================");
  Serial.println("  자동 반복 모드 중지");
  Serial.println("========================================");
  Serial.print("총 반복 횟수: ");
  Serial.println(repeatCount);
  Serial.println("========================================\n");
  Serial.println("조이스틱으로 제어하세요\n");
}

/**
 * 저장된 모든 위치 재생
 */
void playAllPositions() {
  if (savedCount == 0) {
    Serial.println("[알림] 저장된 위치가 없습니다");
    Serial.println("save 명령으로 위치를 저장하세요");
    autoRepeat = false;
    return;
  }
  
  if (autoRepeat) {
    repeatCount++;
    Serial.println("========================================");
    Serial.print("  반복 #");
    Serial.print(repeatCount);
    Serial.println(" 실행 중...");
    Serial.println("========================================");
  } else {
    Serial.println("========================================");
    Serial.println("  자동 재생 시작 (1회)");
    Serial.println("========================================");
    Serial.print("저장된 위치 수: ");
    Serial.println(savedCount);
    Serial.println();
  }
  
  isPlaying = true;
  
  // 모든 저장된 위치를 순차적으로 재생
  for (int pos = 0; pos < savedCount; pos++) {
    // stop 명령 체크
    handleSerialCommand();
    if (!autoRepeat && isPlaying) {
      // 1회 재생 모드는 계속 진행
    } else if (!autoRepeat && !isPlaying) {
      // stop 명령으로 중지됨
      return;
    }
    
    Serial.print("→ 위치 #");
    Serial.print(pos + 1);
    Serial.println(" 이동");
    
    // EEPROM에서 각도 읽기
    int addr = EEPROM_ADDR_START + (pos * 4);
    int targetAngles[4];
    for (int i = 0; i < 4; i++) {
      targetAngles[i] = EEPROM.read(addr + i);
    }
    
    // 목표 위치 출력
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
      handleSerialCommand();
      if (!autoRepeat && !isPlaying) {
        return;
      }
      delay(10);  // 총 1000ms = 1초
    }
  }
  
  if (!autoRepeat) {
    Serial.println("\n========================================");
    Serial.println("  자동 재생 완료");
    Serial.println("========================================\n");
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
    handleSerialCommand();
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
    Serial.println("[알림] 자동 반복 모드 중지됨\n");
  }
  
  savedCount = 0;
  repeatCount = 0;
  EEPROM.put(EEPROM_ADDR_COUNT, savedCount);
  
  Serial.println("========================================");
  Serial.println("  모든 위치 삭제 완료");
  Serial.println("========================================");
  Serial.println("새로운 동작을 저장할 수 있습니다\n");
}

/**
 * 저장된 위치 목록 출력
 */
void listAllPositions() {
  Serial.println("========================================");
  Serial.println("  저장된 위치 목록");
  Serial.println("========================================");
  Serial.print("저장된 위치 수: ");
  Serial.print(savedCount);
  Serial.print(" / ");
  Serial.println(MAX_POSITIONS);
  
  if (savedCount == 0) {
    Serial.println("저장된 위치가 없습니다");
  } else {
    Serial.println();
    for (int pos = 0; pos < savedCount; pos++) {
      int addr = EEPROM_ADDR_START + (pos * 4);
      Serial.print("위치 #");
      Serial.print(pos + 1);
      Serial.print(": ");
      
      Serial.print("베이스:");
      Serial.print(EEPROM.read(addr));
      Serial.print(" 팔꿈치:");
      Serial.print(EEPROM.read(addr + 1));
      Serial.print(" 손목:");
      Serial.print(EEPROM.read(addr + 2));
      Serial.print(" 그립:");
      Serial.println(EEPROM.read(addr + 3));
    }
  }
  Serial.println("========================================\n");
}
