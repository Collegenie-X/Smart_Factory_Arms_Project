/**
 * @file servo_sync_movement.cpp
 * @brief 로봇팔 4축 동기화 이동 알고리즘 구현
 * @date 2026-01-22
 */

#include "servo_sync_movement.h"

// ============================================================
// 생성자
// ============================================================
ServoSyncMovement::ServoSyncMovement() {
  // 홈 포지션 초기화
  homePos.base = 90;
  homePos.shoulder = 90;
  homePos.elbow = 90;
  homePos.gripper = GRIPPER_OPEN;
  
  // 현재 위치를 홈으로 초기화
  currentPos = homePos;
}

// ============================================================
// 서보 초기화
// ============================================================
bool ServoSyncMovement::begin() {
  DEBUG_PRINTLN("[ServoSync] 서보 초기화 시작");
  
  // 서보 핀 연결
  servos[0].attach(PIN_SERVO_BASE);
  servos[1].attach(PIN_SERVO_SHOULDER);
  servos[2].attach(PIN_SERVO_ELBOW);
  servos[3].attach(PIN_SERVO_GRIPPER);
  
  // 홈 포지션으로 이동
  writePosition(homePos);
  delay(DELAY_LONG);
  
  DEBUG_PRINTLN("[ServoSync] 서보 초기화 완료");
  return true;
}

// ============================================================
// 각도 검증 (안전 범위 확인)
// ============================================================
bool ServoSyncMovement::validatePosition(const RobotPosition& pos) {
  // Early return pattern (클린코드)
  if (pos.base < BASE_SAFE_MIN || pos.base > BASE_SAFE_MAX) {
    DEBUG_PRINTLN("[ServoSync] 오류: 베이스 각도 범위 초과");
    return false;
  }
  
  if (pos.shoulder < SHOULDER_SAFE_MIN || pos.shoulder > SHOULDER_SAFE_MAX) {
    DEBUG_PRINTLN("[ServoSync] 오류: 어깨 각도 범위 초과");
    return false;
  }
  
  if (pos.elbow < ELBOW_SAFE_MIN || pos.elbow > ELBOW_SAFE_MAX) {
    DEBUG_PRINTLN("[ServoSync] 오류: 팔꿈치 각도 범위 초과");
    return false;
  }
  
  if (pos.gripper > GRIPPER_CLOSE) {
    DEBUG_PRINTLN("[ServoSync] 오류: 그리퍼 각도 범위 초과");
    return false;
  }
  
  return true;
}

// ============================================================
// 즉시 위치 설정
// ============================================================
void ServoSyncMovement::writePosition(const RobotPosition& pos) {
  servos[0].write(pos.base);
  servos[1].write(pos.shoulder);
  servos[2].write(pos.elbow);
  servos[3].write(pos.gripper);
  currentPos = pos;
}

// ============================================================
// 홈 포지션으로 이동
// ============================================================
void ServoSyncMovement::goHome() {
  DEBUG_PRINTLN("[ServoSync] 홈 포지션 이동");
  synchronizedMove(homePos);
}

// ============================================================
// 현재 위치 반환
// ============================================================
RobotPosition ServoSyncMovement::getCurrentPosition() const {
  return currentPos;
}

// ============================================================
// 동기화 이동 알고리즘 (선형 보간)
// ============================================================
bool ServoSyncMovement::synchronizedMove(const RobotPosition& targetPos, 
                                         uint8_t stepSize, 
                                         uint8_t stepDelay) {
  // 1. 목표 위치 검증
  if (!validatePosition(targetPos)) {
    DEBUG_PRINTLN("[ServoSync] 동기화 이동 실패: 유효하지 않은 위치");
    return false;
  }
  
  // 2. 각 축별 이동 거리 계산
  int distance[4];
  distance[0] = abs((int)targetPos.base - (int)currentPos.base);
  distance[1] = abs((int)targetPos.shoulder - (int)currentPos.shoulder);
  distance[2] = abs((int)targetPos.elbow - (int)currentPos.elbow);
  distance[3] = abs((int)targetPos.gripper - (int)currentPos.gripper);
  
  // 3. 최대 이동 거리 찾기
  int maxDistance = distance[0];
  for (int i = 1; i < 4; i++) {
    if (distance[i] > maxDistance) {
      maxDistance = distance[i];
    }
  }
  
  // 이동할 필요 없는 경우
  if (maxDistance == 0) {
    DEBUG_PRINTLN("[ServoSync] 이미 목표 위치에 있음");
    return true;
  }
  
  // 4. 스텝 수 계산
  int steps = (maxDistance + stepSize - 1) / stepSize;  // 올림 계산
  
  VERBOSE_PRINT("[ServoSync] 동기화 이동 시작: ");
  VERBOSE_PRINT(steps);
  VERBOSE_PRINTLN(" 스텝");
  
  // 5. 동기화된 이동 (선형 보간)
  for (int step = 1; step <= steps; step++) {
    float progress = (float)step / (float)steps;  // 진행률 (0.0 ~ 1.0)
    
    RobotPosition newPos;
    
    // 각 축별 선형 보간 계산
    newPos.base = currentPos.base + 
                  (int)((targetPos.base - currentPos.base) * progress);
    newPos.shoulder = currentPos.shoulder + 
                      (int)((targetPos.shoulder - currentPos.shoulder) * progress);
    newPos.elbow = currentPos.elbow + 
                   (int)((targetPos.elbow - currentPos.elbow) * progress);
    newPos.gripper = currentPos.gripper + 
                     (int)((targetPos.gripper - currentPos.gripper) * progress);
    
    // 각도 범위 제한
    newPos.base = constrainAngle(newPos.base);
    newPos.shoulder = constrainAngle(newPos.shoulder);
    newPos.elbow = constrainAngle(newPos.elbow);
    newPos.gripper = constrainAngle(newPos.gripper);
    
    // 서보에 쓰기
    servos[0].write(newPos.base);
    servos[1].write(newPos.shoulder);
    servos[2].write(newPos.elbow);
    servos[3].write(newPos.gripper);
    
    delay(stepDelay);
  }
  
  // 6. 최종 위치 보정 (정확한 목표 위치)
  writePosition(targetPos);
  
  DEBUG_PRINTLN("[ServoSync] 동기화 이동 완료");
  return true;
}

// ============================================================
// 집기 동작 시퀀스
// ============================================================
bool ServoSyncMovement::pickSequence(Zone zone) {
  DEBUG_PRINT("[ServoSync] 집기 시작 - 구역: ");
  DEBUG_PRINTLN((int)zone);
  
  RobotPosition pickPos;
  
  // 구역별 각도 설정
  switch (zone) {
    case ZONE_1:
      pickPos.base = ZONE1_BASE;
      pickPos.shoulder = ZONE1_SHOULDER;
      pickPos.elbow = ZONE1_ELBOW;
      break;
    case ZONE_2:
      pickPos.base = ZONE2_BASE;
      pickPos.shoulder = ZONE2_SHOULDER;
      pickPos.elbow = ZONE2_ELBOW;
      break;
    case ZONE_3:
      pickPos.base = ZONE3_BASE;
      pickPos.shoulder = ZONE3_SHOULDER;
      pickPos.elbow = ZONE3_ELBOW;
      break;
    default:
      DEBUG_PRINTLN("[ServoSync] 오류: 유효하지 않은 구역");
      return false;
  }
  
  pickPos.gripper = GRIPPER_OPEN;
  
  // 1단계: 그리퍼 열기
  DEBUG_PRINTLN("[ServoSync] 1단계: 그리퍼 열기");
  servos[3].write(GRIPPER_OPEN);
  delay(DELAY_GRIPPER);
  
  // 2단계: 해당 구역 위로 이동
  DEBUG_PRINTLN("[ServoSync] 2단계: 구역 위로 이동");
  if (!synchronizedMove(pickPos)) {
    return false;
  }
  
  // 3단계: 어깨 내리기 (집기 준비)
  DEBUG_PRINTLN("[ServoSync] 3단계: 내려가기");
  pickPos.shoulder = 110;  // 아래로
  if (!synchronizedMove(pickPos)) {
    return false;
  }
  delay(DELAY_SHOULDER);
  
  // 4단계: 그리퍼 닫기
  DEBUG_PRINTLN("[ServoSync] 4단계: 그리퍼 닫기");
  servos[3].write(GRIPPER_CLOSE);
  currentPos.gripper = GRIPPER_CLOSE;
  delay(DELAY_GRIPPER);
  
  // 5단계: 어깨 올리기
  DEBUG_PRINTLN("[ServoSync] 5단계: 올리기");
  pickPos.shoulder = 90;
  if (!synchronizedMove(pickPos)) {
    return false;
  }
  delay(DELAY_SHOULDER);
  
  DEBUG_PRINTLN("[ServoSync] 집기 완료");
  return true;
}

// ============================================================
// 놓기 동작 시퀀스
// ============================================================
bool ServoSyncMovement::placeSequence(Zone zone) {
  DEBUG_PRINT("[ServoSync] 놓기 시작 - 구역: ");
  DEBUG_PRINTLN((int)zone);
  
  RobotPosition placePos;
  
  // 구역별 각도 설정
  switch (zone) {
    case ZONE_1:
      placePos.base = ZONE1_BASE;
      placePos.shoulder = 100;
      placePos.elbow = ZONE1_ELBOW;
      break;
    case ZONE_2:
      placePos.base = ZONE2_BASE;
      placePos.shoulder = 100;
      placePos.elbow = ZONE2_ELBOW;
      break;
    case ZONE_3:
      placePos.base = ZONE3_BASE;
      placePos.shoulder = 100;
      placePos.elbow = ZONE3_ELBOW;
      break;
    default:
      return false;
  }
  
  placePos.gripper = GRIPPER_CLOSE;  // 현재 닫혀있음
  
  // 1단계: 해당 구역으로 이동
  DEBUG_PRINTLN("[ServoSync] 1단계: 구역 이동");
  if (!synchronizedMove(placePos)) {
    return false;
  }
  
  // 2단계: 내려가기
  DEBUG_PRINTLN("[ServoSync] 2단계: 내려가기");
  placePos.shoulder = 110;
  if (!synchronizedMove(placePos)) {
    return false;
  }
  delay(DELAY_SHOULDER);
  
  // 3단계: 그리퍼 열기
  DEBUG_PRINTLN("[ServoSync] 3단계: 그리퍼 열기");
  servos[3].write(GRIPPER_OPEN);
  currentPos.gripper = GRIPPER_OPEN;
  delay(DELAY_GRIPPER);
  
  // 4단계: 올리기
  DEBUG_PRINTLN("[ServoSync] 4단계: 올리기");
  placePos.shoulder = 90;
  if (!synchronizedMove(placePos)) {
    return false;
  }
  
  DEBUG_PRINTLN("[ServoSync] 놓기 완료");
  return true;
}

// ============================================================
// 수동 이동
// ============================================================
bool ServoSyncMovement::manualMove(uint8_t base, uint8_t shoulder, 
                                    uint8_t elbow, uint8_t gripper) {
  RobotPosition targetPos;
  targetPos.base = base;
  targetPos.shoulder = shoulder;
  targetPos.elbow = elbow;
  targetPos.gripper = gripper;
  
  return synchronizedMove(targetPos);
}
