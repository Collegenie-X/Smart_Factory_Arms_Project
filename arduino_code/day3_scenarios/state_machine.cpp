/**
 * @file state_machine.cpp
 * @brief 통합 시스템 상태 머신 구현
 * @date 2026-01-22
 */

#include "state_machine.h"

// ============================================================
// 생성자
// ============================================================
StateMachine::StateMachine(ServoSyncMovement* arm, 
                           ColorSensor* sensor, 
                           MotorController* motorCtrl)
  : currentState(STATE_IDLE)
  , previousState(STATE_IDLE)
  , stateStartTime(0)
  , retryCount(0)
  , robotArm(arm)
  , colorSensor(sensor)
  , motor(motorCtrl)
  , aiRecognizedColor(COLOR_UNKNOWN)
  , selectedZone(ZONE_NONE)
  , aiConfidence(0.0f) {
}

// ============================================================
// 초기화
// ============================================================
void StateMachine::begin() {
  DEBUG_PRINTLN("[StateMachine] 초기화");
  currentState = STATE_IDLE;
  stateStartTime = millis();
}

// ============================================================
// 상태 전이
// ============================================================
void StateMachine::transitionTo(SystemState newState) {
  previousState = currentState;
  currentState = newState;
  stateStartTime = millis();
  retryCount = 0;  // 상태 변경 시 재시도 카운터 리셋
  
  DEBUG_PRINT("[StateMachine] 전이: ");
  DEBUG_PRINT(getStateName(previousState));
  DEBUG_PRINT(" -> ");
  DEBUG_PRINTLN(getStateName(currentState));
}

// ============================================================
// 상태 경과 시간
// ============================================================
unsigned long StateMachine::getStateElapsedTime() const {
  return millis() - stateStartTime;
}

// ============================================================
// 현재 상태 반환
// ============================================================
SystemState StateMachine::getCurrentState() const {
  return currentState;
}

// ============================================================
// AI 명령 수신
// ============================================================
void StateMachine::receiveAICommand(Color color, Zone zone, float confidence) {
  DEBUG_PRINT("[StateMachine] AI 명령: ");
  DEBUG_PRINT(getColorName(color));
  DEBUG_PRINT(" Zone:");
  DEBUG_PRINT((int)zone);
  DEBUG_PRINT(" Conf:");
  DEBUG_PRINTLN(confidence);
  
  // IDLE 상태에서만 명령 수락
  if (currentState != STATE_IDLE) {
    DEBUG_PRINTLN("[StateMachine] 경고: IDLE 상태가 아님, 명령 무시");
    return;
  }
  
  // 신뢰도 확인
  if (confidence < AI_CONFIDENCE_MIN) {
    DEBUG_PRINTLN("[StateMachine] 오류: 신뢰도 부족");
    return;
  }
  
  // AI 데이터 저장
  aiRecognizedColor = color;
  selectedZone = zone;
  aiConfidence = confidence;
  
  // 상태 전이: SCANNING
  transitionTo(STATE_SCANNING);
}

// ============================================================
// 비상 정지
// ============================================================
void StateMachine::emergencyStop() {
  DEBUG_PRINTLN("[StateMachine] 비상 정지!");
  
  // 모든 액추에이터 정지
  motor->stop();
  
  transitionTo(STATE_EMERGENCY_STOP);
}

// ============================================================
// 재시작
// ============================================================
void StateMachine::reset() {
  DEBUG_PRINTLN("[StateMachine] 시스템 재시작");
  
  motor->stop();
  robotArm->goHome();
  
  aiRecognizedColor = COLOR_UNKNOWN;
  selectedZone = ZONE_NONE;
  aiConfidence = 0.0f;
  
  transitionTo(STATE_IDLE);
}

// ============================================================
// 상태 머신 업데이트 (시나리오 A: AI 기반 스마트 입고)
// ============================================================
void StateMachine::update() {
  switch (currentState) {
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // IDLE: 대기 상태
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_IDLE: {
      // AI 명령 대기 (receiveAICommand로 전이)
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // SCANNING: AI 스캔 완료 → 집기 시작
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_SCANNING: {
      DEBUG_PRINTLN("[StateMachine] AI 스캔 완료, 집기 시작");
      transitionTo(STATE_PICKING);
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // PICKING: 로봇팔 집기
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_PICKING: {
      DEBUG_PRINTLN("[StateMachine] 집기 동작");
      
      bool success = robotArm->pickSequence(selectedZone);
      
      if (success) {
        DEBUG_PRINTLN("[StateMachine] 집기 성공");
        transitionTo(STATE_TRANSPORTING);
      } else {
        DEBUG_PRINTLN("[StateMachine] 집기 실패");
        retryCount++;
        
        if (retryCount >= MAX_RETRY_COUNT) {
          DEBUG_PRINTLN("[StateMachine] 최대 재시도 초과");
          transitionTo(STATE_ERROR);
        }
        // 재시도 (상태 유지)
      }
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // TRANSPORTING: 컨베이어 투입 및 이송
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_TRANSPORTING: {
      DEBUG_PRINTLN("[StateMachine] 컨베이어 투입");
      
      // 컨베이어 위치로 이동하여 놓기
      robotArm->placeSequence(ZONE_2);  // 중앙 (컨베이어)
      
      // 컨베이어 작동
      motor->setSpeed(MOTOR_SPEED_MEDIUM, MOTOR_FORWARD);
      
      // 타이머 시작 (3초 후 감지 단계)
      transitionTo(STATE_DETECTING);
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // DETECTING: IR 센서 감지 대기
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_DETECTING: {
      // 3초 이상 경과 시 IR 체크 시작
      if (getStateElapsedTime() >= 3000) {
        // IR 센서 읽기
        bool irDetected = (digitalRead(PIN_IR_SENSOR) == LOW);
        
        if (irDetected) {
          DEBUG_PRINTLN("[StateMachine] IR 감지 - 벨트 정지");
          motor->stop();
          delay(1000);  // 안정화
          
          transitionTo(STATE_VERIFYING);
        }
      }
      
      // 타임아웃 (10초)
      if (getStateElapsedTime() > 10000) {
        DEBUG_PRINTLN("[StateMachine] IR 감지 타임아웃");
        motor->stop();
        transitionTo(STATE_ERROR);
      }
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // VERIFYING: 색상 검증 (AI vs 센서)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_VERIFYING: {
      DEBUG_PRINTLN("[StateMachine] 색상 검증 시작");
      
      // 컬러 센서로 측정
      Color sensorColor = colorSensor->readColorFiltered();
      
      DEBUG_PRINT("[StateMachine] AI: ");
      DEBUG_PRINT(getColorName(aiRecognizedColor));
      DEBUG_PRINT(" Sensor: ");
      DEBUG_PRINTLN(getColorName(sensorColor));
      
      // 색상 일치 확인
      if (sensorColor == aiRecognizedColor) {
        DEBUG_PRINTLN("[StateMachine] 검증 성공 - 색상 일치");
        transitionTo(STATE_SORTING);
      } else {
        DEBUG_PRINTLN("[StateMachine] 검증 실패 - 색상 불일치");
        transitionTo(STATE_ERROR);
      }
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // SORTING: 분류 완료
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_SORTING: {
      DEBUG_PRINTLN("[StateMachine] 분류 완료");
      
      // 성공 사운드 재생 (선택적)
      // playTone(FREQ_DONE, 400);
      
      // 통계 업데이트는 외부에서 처리
      
      delay(2000);
      
      // IDLE로 복귀
      transitionTo(STATE_IDLE);
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // ERROR: 오류 처리
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_ERROR: {
      DEBUG_PRINTLN("[StateMachine] 오류 처리");
      
      motor->stop();
      
      // 오류 피드백
      // playTone(FREQ_ERROR, 500);
      
      delay(2000);
      
      // 복구 시도
      reset();
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // EMERGENCY_STOP: 비상 정지
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case STATE_EMERGENCY_STOP: {
      // 사용자 재시작 명령 대기
      break;
    }
    
    default:
      DEBUG_PRINTLN("[StateMachine] 알 수 없는 상태");
      transitionTo(STATE_ERROR);
      break;
  }
}
