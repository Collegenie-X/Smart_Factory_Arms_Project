/**
 * @file state_machine.h
 * @brief 통합 시스템 상태 머신
 * @details 로봇팔 + 컨베이어 + 센서 + AI 통합 제어
 * @date 2026-01-22
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include "../common/config.h"
#include "../common/types.h"
#include "../day1_robot_arm/servo_sync_movement.h"
#include "../day2_conveyor_sensor/color_sensor.h"
#include "../day2_conveyor_sensor/motor_controller.h"

/**
 * @class StateMachine
 * @brief 통합 시스템 상태 머신 클래스
 */
class StateMachine {
private:
  SystemState currentState;     // 현재 상태
  SystemState previousState;    // 이전 상태
  unsigned long stateStartTime; // 상태 시작 시간
  uint8_t retryCount;          // 재시도 카운터
  
  // 시스템 컴포넌트 포인터
  ServoSyncMovement* robotArm;
  ColorSensor* colorSensor;
  MotorController* motor;
  
  // AI 데이터
  Color aiRecognizedColor;
  Zone selectedZone;
  float aiConfidence;
  
  /**
   * @brief 상태 전이
   * @param newState 새로운 상태
   */
  void transitionTo(SystemState newState);
  
  /**
   * @brief 상태 경과 시간 (ms)
   * @return 경과 시간
   */
  unsigned long getStateElapsedTime() const;

public:
  /**
   * @brief 생성자
   * @param arm 로봇팔 포인터
   * @param sensor 컬러 센서 포인터
   * @param motorCtrl 모터 포인터
   */
  StateMachine(ServoSyncMovement* arm, 
               ColorSensor* sensor, 
               MotorController* motorCtrl);
  
  /**
   * @brief 초기화
   */
  void begin();
  
  /**
   * @brief 상태 머신 업데이트 (매 루프 호출)
   */
  void update();
  
  /**
   * @brief 현재 상태 반환
   * @return 현재 상태
   */
  SystemState getCurrentState() const;
  
  /**
   * @brief AI 명령 수신
   * @param color AI가 인식한 색상
   * @param zone 사용자가 선택한 구역
   * @param confidence 신뢰도
   */
  void receiveAICommand(Color color, Zone zone, float confidence);
  
  /**
   * @brief 비상 정지
   */
  void emergencyStop();
  
  /**
   * @brief 재시작
   */
  void reset();
};

#endif // STATE_MACHINE_H
