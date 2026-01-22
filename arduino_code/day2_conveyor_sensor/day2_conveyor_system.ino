/**
 * @file day2_conveyor_system.ino
 * @brief Day 2 컨베이어 + 센서 통합 시스템
 * @details 컬러 센서 기반 자동 분류 시스템
 * @date 2026-01-22
 * 
 * 기능:
 * - 컨베이어 벨트 제어
 * - IR 센서 물체 감지
 * - 컬러 센서 색상 판단
 * - 게이트 서보 제어 (색상별 분류)
 * - 통계 수집 및 전송
 * 
 * 동작 순서:
 * 1. 컨베이어 작동
 * 2. IR 센서로 물체 감지
 * 3. 벨트 정지
 * 4. 컬러 센서로 색상 판단
 * 5. 게이트로 분류
 * 6. 통계 업데이트
 * 7. 앱으로 데이터 전송
 * 8. 반복
 */

#include <Servo.h>
#include "color_sensor.h"
#include "motor_controller.h"
#include "../common/config.h"
#include "../common/types.h"
#include "../day1_robot_arm/bluetooth_protocol.h"

// ============================================================
// 전역 객체
// ============================================================
ColorSensor colorSensor;              // 컬러 센서
MotorController motor;                // DC 모터
Servo gateServo;                      // 게이트 서보
BluetoothProtocol bluetooth(&Serial); // 블루투스

// ============================================================
// 전역 변수
// ============================================================
Statistics stats;                     // 통계
SystemState currentState = STATE_IDLE;// 현재 상태

// ============================================================
// 센서 함수
// ============================================================
/**
 * @brief IR 센서로 물체 감지
 * @return 감지되면 true
 */
bool detectObject() {
  int irValue = digitalRead(PIN_IR_SENSOR);
  // IR 센서는 LOW일 때 물체 감지 (일반적)
  return (irValue == LOW);
}

/**
 * @brief 게이트 제어
 * @param color 색상
 */
void controlGate(Color color) {
  uint8_t angle = GATE_ANGLE_PASS;  // 기본값: 통과
  
  switch (color) {
    case COLOR_RED:
      angle = GATE_ANGLE_RED;
      DEBUG_PRINTLN("[Gate] 빨강 → 0도");
      break;
    case COLOR_BLUE:
      angle = GATE_ANGLE_BLUE;
      DEBUG_PRINTLN("[Gate] 파랑 → 90도");
      break;
    case COLOR_YELLOW:
      angle = GATE_ANGLE_YELLOW;
      DEBUG_PRINTLN("[Gate] 노랑 → 180도");
      break;
    default:
      DEBUG_PRINTLN("[Gate] 알수없음 → 통과");
      break;
  }
  
  gateServo.write(angle);
}

/**
 * @brief 통계 업데이트
 * @param color 색상
 */
void updateStatistics(Color color) {
  switch (color) {
    case COLOR_RED:
      stats.redCount++;
      break;
    case COLOR_BLUE:
      stats.blueCount++;
      break;
    case COLOR_YELLOW:
      stats.yellowCount++;
      break;
    default:
      stats.unknownCount++;
      break;
  }
  stats.totalCount++;
  
  DEBUG_PRINT("[Stats] R:");
  DEBUG_PRINT(stats.redCount);
  DEBUG_PRINT(" B:");
  DEBUG_PRINT(stats.blueCount);
  DEBUG_PRINT(" Y:");
  DEBUG_PRINT(stats.yellowCount);
  DEBUG_PRINT(" Total:");
  DEBUG_PRINTLN(stats.totalCount);
}

/**
 * @brief 상태 전송
 */
void sendStatus() {
  RobotPosition dummyPos = {0, 0, 0, 0};  // Day2에서는 로봇팔 미사용
  bluetooth.sendStatus(currentState, dummyPos, stats);
}

// ============================================================
// setup() - 초기화
// ============================================================
void setup() {
  // 시리얼 통신 시작
  Serial.begin(BLUETOOTH_BAUD);
  delay(1000);
  
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN("   스마트 팩토리 컨베이어 시스템 Day 2");
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN();
  
  // 1. 컬러 센서 초기화
  DEBUG_PRINTLN("[Main] 1/5 컬러 센서 초기화 중...");
  if (!colorSensor.begin()) {
    DEBUG_PRINTLN("[Main] 오류: 컬러 센서 초기화 실패");
    while (1);
  }
  delay(500);
  
  // 2. 모터 초기화
  DEBUG_PRINTLN("[Main] 2/5 모터 초기화 중...");
  if (!motor.begin()) {
    DEBUG_PRINTLN("[Main] 오류: 모터 초기화 실패");
    while (1);
  }
  delay(500);
  
  // 3. IR 센서 초기화
  DEBUG_PRINTLN("[Main] 3/5 IR 센서 초기화 중...");
  pinMode(PIN_IR_SENSOR, INPUT);
  delay(500);
  
  // 4. 게이트 서보 초기화
  DEBUG_PRINTLN("[Main] 4/5 게이트 서보 초기화 중...");
  gateServo.attach(PIN_SERVO_BASE);  // 임시로 베이스 핀 사용
  gateServo.write(GATE_ANGLE_PASS);
  delay(500);
  
  // 5. 블루투스 초기화
  DEBUG_PRINTLN("[Main] 5/5 블루투스 초기화 중...");
  bluetooth.begin();
  delay(500);
  
  // 통계 초기화
  memset(&stats, 0, sizeof(stats));
  
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN("   초기화 완료 - 컨베이어 작동 시작");
  DEBUG_PRINTLN("========================================");
  DEBUG_PRINTLN();
  
  currentState = STATE_TRANSPORTING;
  
  // 컨베이어 시작
  motor.setSpeed(MOTOR_SPEED_MEDIUM, MOTOR_FORWARD);
}

// ============================================================
// loop() - 메인 루프 (시나리오 B: 센서 기반 분류)
// ============================================================
void loop() {
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  // 상태 머신
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  switch (currentState) {
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // TRANSPORTING: 컨베이어 작동 + IR 폴링
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_TRANSPORTING: {
      // IR 센서로 물체 감지
      if (detectObject()) {
        DEBUG_PRINTLN("[Main] 물체 감지!");
        
        // 벨트 정지
        motor.stop();
        delay(500);  // 안정화 대기
        
        currentState = STATE_DETECTING;
      }
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // DETECTING: 색상 측정
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_DETECTING: {
      DEBUG_PRINTLN("[Main] 색상 측정 시작");
      
      // 컬러 센서로 색상 판단 (필터링)
      Color detectedColor = colorSensor.readColorFiltered();
      
      currentState = STATE_SORTING;
      
      // 게이트 제어
      controlGate(detectedColor);
      
      // 통계 업데이트
      updateStatistics(detectedColor);
      
      // 상태 전송
      sendStatus();
      
      // 대기
      delay(2000);
      
      // 게이트 원위치
      gateServo.write(GATE_ANGLE_PASS);
      delay(500);
      
      DEBUG_PRINTLN("[Main] 분류 완료, 컨베이어 재시작");
      
      // 컨베이어 재시작
      motor.setSpeed(MOTOR_SPEED_MEDIUM, MOTOR_FORWARD);
      currentState = STATE_TRANSPORTING;
      
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // ERROR: 오류 처리
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_ERROR: {
      DEBUG_PRINTLN("[Main] 오류 상태 - 복구 시도");
      motor.stop();
      delay(2000);
      
      // 복구
      motor.setSpeed(MOTOR_SPEED_MEDIUM, MOTOR_FORWARD);
      currentState = STATE_TRANSPORTING;
      break;
    }
    
    default:
      break;
  }
  
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  // 블루투스 명령 처리 (선택적)
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  if (bluetooth.available()) {
    Command cmd;
    if (bluetooth.parseCommand(cmd)) {
      // 상태 조회
      if (cmd.type == '?') {
        sendStatus();
      }
      // 캘리브레이션 (C R/B/Y)
      else if (cmd.type == 'C' || cmd.type == 'c') {
        Color calColor = (Color)cmd.params[0];
        DEBUG_PRINT("[Main] 캘리브레이션: ");
        DEBUG_PRINTLN(getColorName(calColor));
        
        motor.stop();
        colorSensor.calibrate(calColor);
        motor.setSpeed(MOTOR_SPEED_MEDIUM, MOTOR_FORWARD);
        
        bluetooth.sendResponse("OK:CALIBRATE");
      }
    }
  }
  
  // 루프 지연
  delay(100);  // IR 폴링 주기
}
