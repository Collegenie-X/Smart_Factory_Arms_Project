/**
 * @file motor_controller.h
 * @brief DC 모터 제어 알고리즘 (L298N 드라이버)
 * @details PWM 속도 제어 + 방향 제어
 * @date 2026-01-22
 */

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include "../common/config.h"
#include "../common/types.h"

/**
 * @class MotorController
 * @brief DC 모터 제어 클래스
 */
class MotorController {
private:
  uint8_t currentSpeed;           // 현재 속도 (0-255)
  MotorDirection currentDirection;// 현재 방향
  
  /**
   * @brief 방향 설정 (H-Bridge)
   * @param direction 방향
   */
  void setDirection(MotorDirection direction);

public:
  /**
   * @brief 생성자
   */
  MotorController();
  
  /**
   * @brief 초기화
   * @return 성공 시 true
   */
  bool begin();
  
  /**
   * @brief 모터 속도 설정
   * @param speed 속도 (0-255)
   * @param direction 방향
   */
  void setSpeed(uint8_t speed, MotorDirection direction = MOTOR_FORWARD);
  
  /**
   * @brief 모터 정지
   */
  void stop();
  
  /**
   * @brief 부드러운 가속
   * @param targetSpeed 목표 속도
   * @param accelRate 가속률 (기본 10)
   * @param accelDelay 가속 지연 (기본 20ms)
   */
  void smoothAccel(uint8_t targetSpeed, 
                   uint8_t accelRate = 10, 
                   uint8_t accelDelay = 20);
  
  /**
   * @brief 현재 속도 반환
   * @return 현재 속도
   */
  uint8_t getCurrentSpeed() const;
  
  /**
   * @brief 현재 방향 반환
   * @return 현재 방향
   */
  MotorDirection getCurrentDirection() const;
};

#endif // MOTOR_CONTROLLER_H
