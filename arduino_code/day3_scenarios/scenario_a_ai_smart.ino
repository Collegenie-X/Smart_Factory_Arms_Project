/**
 * @file scenario_a_ai_smart.ino
 * @brief 시나리오 A: AI 기반 스마트 입고 시스템
 * @details 로봇팔 + AI + 센서 하이브리드 검증
 * @date 2026-01-22
 * 
 * 시스템 구성:
 * - 로봇팔 4축
 * - 컨베이어 벨트
 * - 컬러 센서 (TCS3200)
 * - IR 센서
 * - 블루투스 (HC-06)
 * - AI (Teachable Machine)
 * 
 * 동작 흐름:
 * 1. 앱에서 물체를 AI로 스캔 (카메라)
 * 2. AI가 색상 인식 (신뢰도 80% 이상)
 * 3. 사용자가 구역 선택 (1/2/3)
 * 4. 로봇팔이 해당 구역에서 물체 집기
 * 5. 컨베이어에 투입
 * 6. IR 센서가 물체 감지
 * 7. 컬러 센서로 색상 검증
 * 8. AI 인식과 센서 측정이 일치하면 분류 성공
 * 9. 통계 업데이트 및 앱으로 전송
 * 
 * 명령어 (블루투스):
 * A RED 1    - AI 인식: 빨강, 구역 1
 * A BLUE 2   - AI 인식: 파랑, 구역 2
 * A YELLOW 3 - AI 인식: 노랑, 구역 3
 * ?          - 상태 조회
 * E          - 비상 정지
 * H          - 홈 포지션
 */

#include <Servo.h>
#include "state_machine.h"
#include "../day1_robot_arm/servo_sync_movement.h"
#include "../day1_robot_arm/bluetooth_protocol.h"
#include "../day1_robot_arm/eeprom_storage.h"
#include "../day2_conveyor_sensor/color_sensor.h"
#include "../day2_conveyor_sensor/motor_controller.h"
#include "../common/config.h"
#include "../common/types.h"

// ============================================================
// 전역 객체
// ============================================================
ServoSyncMovement robotArm;          // 로봇팔
ColorSensor colorSensor;             // 컬러 센서
MotorController motor;               // DC 모터
StateMachine stateMachine(&robotArm, &colorSensor, &motor);  // 상태 머신
BluetoothProtocol bluetooth(&Serial);// 블루투스

// ============================================================
// 전역 변수
// ============================================================
Statistics stats;                    // 작업 통계
unsigned long lastStatsTime = 0;     // 마지막 통계 전송 시간

// ============================================================
// 피드백 함수
// ============================================================
void playTone(uint16_t frequency, uint16_t duration) {
  tone(PIN_BUZZER, frequency, duration);
  delay(duration);
  noTone(PIN_BUZZER);
}

void stateFeedback(SystemState state) {
  DEBUG_PRINT("[Main] 상태: ");
  DEBUG_PRINTLN(getStateName(state));
  
  switch (state) {
    case STATE_IDLE:
      playTone(FREQ_HOME, 200);
      break;
    case STATE_PICKING:
      playTone(FREQ_PICK, 200);
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
  Serial.begin(BLUETOOTH_BAUD);
  delay(1000);
  
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN("  시나리오 A: AI 기반 스마트 입고");
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN();
  
  // 1. 로봇팔 초기화
  DEBUG_PRINTLN("[Main] 1/5 로봇팔 초기화...");
  if (!robotArm.begin()) {
    DEBUG_PRINTLN("[Main] 오류: 로봇팔 초기화 실패");
    while (1);
  }
  delay(500);
  
  // 2. 컬러 센서 초기화
  DEBUG_PRINTLN("[Main] 2/5 컬러 센서 초기화...");
  if (!colorSensor.begin()) {
    DEBUG_PRINTLN("[Main] 오류: 컬러 센서 초기화 실패");
    while (1);
  }
  delay(500);
  
  // 3. 모터 초기화
  DEBUG_PRINTLN("[Main] 3/5 모터 초기화...");
  if (!motor.begin()) {
    DEBUG_PRINTLN("[Main] 오류: 모터 초기화 실패");
    while (1);
  }
  delay(500);
  
  // 4. IR 센서 초기화
  DEBUG_PRINTLN("[Main] 4/5 IR 센서 초기화...");
  pinMode(PIN_IR_SENSOR, INPUT);
  delay(500);
  
  // 5. 블루투스 초기화
  DEBUG_PRINTLN("[Main] 5/5 블루투스 초기화...");
  bluetooth.begin();
  delay(500);
  
  // 부저 초기화
  pinMode(PIN_BUZZER, OUTPUT);
  
  // 상태 머신 초기화
  stateMachine.begin();
  
  // 통계 초기화
  memset(&stats, 0, sizeof(stats));
  
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN("  초기화 완료 - AI 명령 대기 중");
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN();
  
  // 준비 완료 사운드
  playTone(FREQ_HOME, 200);
  delay(100);
  playTone(FREQ_DONE, 200);
}

// ============================================================
// loop() - 메인 루프
// ============================================================
void loop() {
  // 1. 상태 머신 업데이트
  SystemState prevState = stateMachine.getCurrentState();
  stateMachine.update();
  SystemState currentState = stateMachine.getCurrentState();
  
  // 상태 변경 시 피드백
  if (currentState != prevState) {
    stateFeedback(currentState);
  }
  
  // 2. 블루투스 명령 처리
  if (bluetooth.available()) {
    Command cmd;
    if (bluetooth.parseCommand(cmd)) {
      executeCommand(cmd);
    }
  }
  
  // 3. 주기적 통계 전송 (10초마다)
  if (millis() - lastStatsTime > 10000) {
    sendStatus();
    lastStatsTime = millis();
  }
  
  // 루프 지연
  delay(STATE_LOOP_DELAY);
}

// ============================================================
// 명령 실행
// ============================================================
void executeCommand(const Command& cmd) {
  switch (cmd.type) {
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // AI 명령 (A color zone)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'A':
    case 'a': {
      Color aiColor = (Color)cmd.params[0];
      Zone zone = (Zone)cmd.params[1];
      float confidence = 0.85f;  // 기본 신뢰도
      
      DEBUG_PRINT("[Main] AI 명령 수신: ");
      DEBUG_PRINT(getColorName(aiColor));
      DEBUG_PRINT(" Zone ");
      DEBUG_PRINTLN((int)zone);
      
      stateMachine.receiveAICommand(aiColor, zone, confidence);
      
      bluetooth.sendResponse("OK:AI_COMMAND_RECEIVED");
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 상태 조회 (?)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case '?': {
      sendStatus();
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 비상 정지 (E)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'E':
    case 'e': {
      DEBUG_PRINTLN("[Main] 비상 정지 명령");
      stateMachine.emergencyStop();
      bluetooth.sendResponse("OK:EMERGENCY_STOP");
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 홈 포지션 (H)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'H':
    case 'h': {
      DEBUG_PRINTLN("[Main] 홈 포지션 명령");
      stateMachine.reset();
      bluetooth.sendResponse("OK:HOME");
      break;
    }
    
    default:
      bluetooth.sendResponse("ERROR:UNKNOWN_COMMAND");
      break;
  }
}

// ============================================================
// 상태 전송
// ============================================================
void sendStatus() {
  RobotPosition pos = robotArm.getCurrentPosition();
  SystemState state = stateMachine.getCurrentState();
  bluetooth.sendStatus(state, pos, stats);
}
