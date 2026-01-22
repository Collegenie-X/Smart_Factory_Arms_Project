/**
 * @file motor_controller.cpp
 * @brief DC 모터 제어 알고리즘 구현
 * @date 2026-01-22
 */

#include "motor_controller.h"

// ============================================================
// 생성자
// ============================================================
MotorController::MotorController() 
  : currentSpeed(0), currentDirection(MOTOR_STOP) {
}

// ============================================================
// 초기화
// ============================================================
bool MotorController::begin() {
  DEBUG_PRINTLN("[Motor] 초기화 시작");
  
  // 핀 모드 설정
  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);
  pinMode(PIN_MOTOR_ENA, OUTPUT);
  
  // 초기 상태: 정지
  stop();
  
  DEBUG_PRINTLN("[Motor] 초기화 완료");
  return true;
}

// ============================================================
// 방향 설정 (H-Bridge 제어)
// ============================================================
void MotorController::setDirection(MotorDirection direction) {
  currentDirection = direction;
  
  switch (direction) {
    case MOTOR_FORWARD:
      digitalWrite(PIN_MOTOR_IN1, HIGH);
      digitalWrite(PIN_MOTOR_IN2, LOW);
      VERBOSE_PRINTLN("[Motor] 방향: FORWARD");
      break;
    
    case MOTOR_BACKWARD:
      digitalWrite(PIN_MOTOR_IN1, LOW);
      digitalWrite(PIN_MOTOR_IN2, HIGH);
      VERBOSE_PRINTLN("[Motor] 방향: BACKWARD");
      break;
    
    case MOTOR_STOP:
    default:
      digitalWrite(PIN_MOTOR_IN1, LOW);
      digitalWrite(PIN_MOTOR_IN2, LOW);
      VERBOSE_PRINTLN("[Motor] 방향: STOP");
      break;
  }
}

// ============================================================
// 속도 설정 알고리즘
// ============================================================
void MotorController::setSpeed(uint8_t speed, MotorDirection direction) {
  // 1. 속도 검증 (Early return)
  if (speed > MOTOR_SPEED_MAX) {
    DEBUG_PRINTLN("[Motor] 경고: 속도 제한 초과, 최대값으로 설정");
    speed = MOTOR_SPEED_MAX;
  }
  
  // 2. 방향 설정
  if (speed == 0) {
    setDirection(MOTOR_STOP);
  } else {
    setDirection(direction);
  }
  
  // 3. PWM 속도 제어
  analogWrite(PIN_MOTOR_ENA, speed);
  currentSpeed = speed;
  
  VERBOSE_PRINT("[Motor] 속도: ");
  VERBOSE_PRINTLN(speed);
}

// ============================================================
// 정지
// ============================================================
void MotorController::stop() {
  DEBUG_PRINTLN("[Motor] 정지");
  setSpeed(0, MOTOR_STOP);
}

// ============================================================
// 부드러운 가속 알고리즘
// ============================================================
void MotorController::smoothAccel(uint8_t targetSpeed, 
                                   uint8_t accelRate, 
                                   uint8_t accelDelay) {
  DEBUG_PRINT("[Motor] 부드러운 가속: ");
  DEBUG_PRINT(currentSpeed);
  DEBUG_PRINT(" -> ");
  DEBUG_PRINTLN(targetSpeed);
  
  // 목표 속도 검증
  if (targetSpeed > MOTOR_SPEED_MAX) {
    targetSpeed = MOTOR_SPEED_MAX;
  }
  
  // 가속/감속 방향 결정
  int step = (targetSpeed > currentSpeed) ? accelRate : -accelRate;
  
  // 점진적 속도 변경
  while (abs((int)targetSpeed - (int)currentSpeed) > accelRate) {
    currentSpeed += step;
    
    // 범위 제한
    if (currentSpeed > MOTOR_SPEED_MAX) {
      currentSpeed = MOTOR_SPEED_MAX;
    }
    if ((int)currentSpeed < 0) {
      currentSpeed = 0;
    }
    
    analogWrite(PIN_MOTOR_ENA, currentSpeed);
    delay(accelDelay);
    
    VERBOSE_PRINT(".");
  }
  
  // 최종 속도 설정
  currentSpeed = targetSpeed;
  analogWrite(PIN_MOTOR_ENA, currentSpeed);
  
  VERBOSE_PRINTLN(" 완료");
}

// ============================================================
// Getter 함수들
// ============================================================
uint8_t MotorController::getCurrentSpeed() const {
  return currentSpeed;
}

MotorDirection MotorController::getCurrentDirection() const {
  return currentDirection;
}
