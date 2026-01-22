/**
 * @file servo_sync_movement.h
 * @brief 로봇팔 4축 동기화 이동 알고리즘
 * @details 선형 보간을 이용한 부드러운 동기화 제어
 * @date 2026-01-22
 */

#ifndef SERVO_SYNC_MOVEMENT_H
#define SERVO_SYNC_MOVEMENT_H

#include <Arduino.h>
#include <Servo.h>
#include "../common/config.h"
#include "../common/types.h"

/**
 * @class ServoSyncMovement
 * @brief 4축 서보 동기화 이동 클래스
 */
class ServoSyncMovement {
private:
  Servo servos[4];              // 4개 서보 객체
  RobotPosition currentPos;     // 현재 위치
  RobotPosition homePos;        // 홈 포지션
  
  /**
   * @brief 각도 안전 범위 검증
   * @param pos 검증할 위치
   * @return 유효하면 true
   */
  bool validatePosition(const RobotPosition& pos);
  
  /**
   * @brief 4축 각도 즉시 설정
   * @param pos 목표 위치
   */
  void writePosition(const RobotPosition& pos);

public:
  /**
   * @brief 생성자
   */
  ServoSyncMovement();
  
  /**
   * @brief 서보 초기화 및 핀 연결
   * @return 성공 시 true
   */
  bool begin();
  
  /**
   * @brief 홈 포지션으로 이동
   */
  void goHome();
  
  /**
   * @brief 현재 위치 반환
   * @return 현재 로봇 위치
   */
  RobotPosition getCurrentPosition() const;
  
  /**
   * @brief 동기화 이동 (선형 보간)
   * @param targetPos 목표 위치
   * @param stepSize 스텝 크기 (기본 1도)
   * @param stepDelay 스텝 지연 (기본 15ms)
   * @return 성공 시 true
   */
  bool synchronizedMove(const RobotPosition& targetPos, 
                        uint8_t stepSize = SYNC_STEP_SIZE, 
                        uint8_t stepDelay = SYNC_STEP_DELAY);
  
  /**
   * @brief 집기 동작 시퀀스
   * @param zone 집기 구역
   * @return 성공 시 true
   */
  bool pickSequence(Zone zone);
  
  /**
   * @brief 놓기 동작 시퀀스
   * @param zone 놓기 구역
   * @return 성공 시 true
   */
  bool placeSequence(Zone zone);
  
  /**
   * @brief 수동 각도 설정
   * @param base 베이스 각도
   * @param shoulder 어깨 각도
   * @param elbow 팔꿈치 각도
   * @param gripper 그리퍼 각도
   * @return 성공 시 true
   */
  bool manualMove(uint8_t base, uint8_t shoulder, 
                  uint8_t elbow, uint8_t gripper);
};

#endif // SERVO_SYNC_MOVEMENT_H
