/**
 * @file day1_robot_arm.ino
 * @brief Day 1 로봇팔 제어 메인 프로그램
 * @details 4축 로봇팔 + 블루투스 제어 + EEPROM 저장
 * @date 2026-01-22
 * 
 * 기능:
 * - 4축 동기화 이동
 * - 블루투스 명령 제어
 * - EEPROM 위치 저장/복구
 * - LED Matrix + 부저 피드백
 * 
 * 명령어:
 * H - 홈 포지션
 * P - 집기 동작 (구역 1)
 * L - 왼쪽 놓기
 * R - 오른쪽 놓기
 * M base,shoulder,elbow,gripper - 수동 이동
 * S slot - 현재 위치 저장 (slot: 1-3)
 * D slot - 위치 불러오기 (slot: 1-3)
 * ? - 상태 조회
 */

#include <Servo.h>
#include <EEPROM.h>
#include "servo_sync_movement.h"
#include "bluetooth_protocol.h"
#include "eeprom_storage.h"
#include "../common/config.h"
#include "../common/types.h"

// ============================================================
// 전역 객체
// ============================================================
ServoSyncMovement robotArm;          // 로봇팔 제어
BluetoothProtocol bluetooth(&Serial); // 블루투스 (HardwareSerial 사용)
EEPROMStorage storage;               // EEPROM 저장소

// ============================================================
// 전역 변수
// ============================================================
SystemState currentState = STATE_IDLE;  // 현재 시스템 상태
Statistics stats;                       // 작업 통계

// ============================================================
// 피드백 함수
// ============================================================
/**
 * @brief 부저 톤 재생
 * @param frequency 주파수
 * @param duration 지속 시간 (ms)
 */
void playTone(uint16_t frequency, uint16_t duration) {
  tone(PIN_BUZZER, frequency, duration);
  delay(duration);
  noTone(PIN_BUZZER);
}

/**
 * @brief 상태별 피드백 (부저 + 시리얼)
 * @param state 상태
 */
void stateFeedback(SystemState state) {
  DEBUG_PRINT("[Main] 상태: ");
  DEBUG_PRINTLN(getStateName(state));
  
  // 상태별 주파수
  switch (state) {
    case STATE_IDLE:
      playTone(FREQ_HOME, 200);
      break;
    case STATE_PICKING:
      playTone(FREQ_PICK, 200);
      break;
    case STATE_MOVING:
      playTone(FREQ_MOVE, 200);
      break;
    case STATE_SORTING:
      playTone(FREQ_DONE, 400);
      break;
    case STATE_ERROR:
      playTone(FREQ_ERROR, 500);
      break;
    default:
      break;
  }
}

// ============================================================
// setup() - 초기화
// ============================================================
void setup() {
  // 시리얼 통신 시작 (블루투스 + 디버그)
  Serial.begin(BLUETOOTH_BAUD);
  delay(1000);
  
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN("   스마트 팩토리 로봇팔 시스템 Day 1");
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN();
  
  // 1. 서보 초기화
  DEBUG_PRINTLN("[Main] 1/3 서보 초기화 중...");
  if (!robotArm.begin()) {
    DEBUG_PRINTLN("[Main] 오류: 서보 초기화 실패");
    while (1);  // 초기화 실패 시 정지
  }
  delay(500);
  
  // 2. 블루투스 초기화
  DEBUG_PRINTLN("[Main] 2/3 블루투스 초기화 중...");
  bluetooth.begin();
  delay(500);
  
  // 3. EEPROM 초기화
  DEBUG_PRINTLN("[Main] 3/3 EEPROM 초기화 중...");
  storage.begin();
  delay(500);
  
  // 4. 통계 초기화
  memset(&stats, 0, sizeof(stats));
  
  // 5. 부저 초기화
  pinMode(PIN_BUZZER, OUTPUT);
  
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN("   초기화 완료 - 대기 중");
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN();
  
  // 준비 완료 사운드
  playTone(FREQ_HOME, 200);
  delay(100);
  playTone(FREQ_DONE, 200);
  
  currentState = STATE_IDLE;
}

// ============================================================
// loop() - 메인 루프
// ============================================================
void loop() {
  // 명령 수신 확인
  if (bluetooth.available()) {
    Command cmd;
    
    // 명령 파싱
    if (bluetooth.parseCommand(cmd)) {
      executeCommand(cmd);
    }
  }
  
  // 루프 지연 (CPU 부하 감소)
  delay(STATE_LOOP_DELAY);
}

// ============================================================
// 명령 실행 함수
// ============================================================
/**
 * @brief 명령 실행
 * @param cmd 명령 구조체
 */
void executeCommand(const Command& cmd) {
  bool success = false;
  
  switch (cmd.type) {
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 홈 포지션 (H)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'H':
    case 'h':
      currentState = STATE_MOVING;
      stateFeedback(currentState);
      
      robotArm.goHome();
      
      currentState = STATE_IDLE;
      stateFeedback(currentState);
      
      bluetooth.sendResponse("OK:HOME");
      break;
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 집기 (P) - 구역 1
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'P':
    case 'p':
      currentState = STATE_PICKING;
      stateFeedback(currentState);
      
      success = robotArm.pickSequence(ZONE_1);
      
      if (success) {
        stats.totalCount++;
        currentState = STATE_IDLE;
        stateFeedback(currentState);
        bluetooth.sendResponse("OK:PICK");
      } else {
        stats.errorCount++;
        currentState = STATE_ERROR;
        stateFeedback(currentState);
        bluetooth.sendResponse("ERROR:PICK_FAILED");
      }
      break;
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 왼쪽 놓기 (L)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'L':
    case 'l':
      currentState = STATE_MOVING;
      stateFeedback(currentState);
      
      success = robotArm.placeSequence(ZONE_1);
      
      if (success) {
        currentState = STATE_IDLE;
        stateFeedback(currentState);
        bluetooth.sendResponse("OK:PLACE_LEFT");
      } else {
        stats.errorCount++;
        currentState = STATE_ERROR;
        stateFeedback(currentState);
        bluetooth.sendResponse("ERROR:PLACE_FAILED");
      }
      break;
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 오른쪽 놓기 (R)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'R':
    case 'r':
      currentState = STATE_MOVING;
      stateFeedback(currentState);
      
      success = robotArm.placeSequence(ZONE_3);
      
      if (success) {
        currentState = STATE_IDLE;
        stateFeedback(currentState);
        bluetooth.sendResponse("OK:PLACE_RIGHT");
      } else {
        stats.errorCount++;
        currentState = STATE_ERROR;
        stateFeedback(currentState);
        bluetooth.sendResponse("ERROR:PLACE_FAILED");
      }
      break;
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 수동 이동 (M)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'M':
    case 'm':
      currentState = STATE_MOVING;
      stateFeedback(currentState);
      
      success = robotArm.manualMove(
        cmd.params[0],  // base
        cmd.params[1],  // shoulder
        cmd.params[2],  // elbow
        cmd.params[3]   // gripper
      );
      
      if (success) {
        currentState = STATE_IDLE;
        stateFeedback(currentState);
        bluetooth.sendResponse("OK:MOVE");
      } else {
        currentState = STATE_ERROR;
        stateFeedback(currentState);
        bluetooth.sendResponse("ERROR:INVALID_POSITION");
      }
      break;
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 저장 (S)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'S':
    case 's': {
      uint8_t slot = cmd.params[0];
      RobotPosition currentPos = robotArm.getCurrentPosition();
      
      success = storage.savePosition(slot, currentPos);
      
      if (success) {
        playTone(FREQ_DONE, 200);
        char response[32];
        sprintf(response, "OK:SAVE:SLOT_%d", slot);
        bluetooth.sendResponse(response);
      } else {
        playTone(FREQ_ERROR, 500);
        bluetooth.sendResponse("ERROR:SAVE_FAILED");
      }
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 불러오기 (D)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'D':
    case 'd': {
      uint8_t slot = cmd.params[0];
      RobotPosition loadedPos;
      
      success = storage.loadPosition(slot, loadedPos);
      
      if (success) {
        currentState = STATE_MOVING;
        stateFeedback(currentState);
        
        robotArm.synchronizedMove(loadedPos);
        
        currentState = STATE_IDLE;
        stateFeedback(currentState);
        
        char response[32];
        sprintf(response, "OK:LOAD:SLOT_%d", slot);
        bluetooth.sendResponse(response);
      } else {
        playTone(FREQ_ERROR, 500);
        bluetooth.sendResponse("ERROR:LOAD_FAILED");
      }
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 상태 조회 (?)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case '?': {
      RobotPosition currentPos = robotArm.getCurrentPosition();
      bluetooth.sendStatus(currentState, currentPos, stats);
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // AI 명령 (A) - Day 2에서 구현
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'A':
    case 'a':
      bluetooth.sendResponse("INFO:AI_NOT_IMPLEMENTED_YET");
      break;
    
    default:
      bluetooth.sendResponse("ERROR:UNKNOWN_COMMAND");
      break;
  }
}
