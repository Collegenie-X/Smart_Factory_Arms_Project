/**
 * @file eeprom_storage.cpp
 * @brief EEPROM 저장/복구 알고리즘 구현
 * @date 2026-01-22
 */

#include "eeprom_storage.h"

// ============================================================
// 초기화
// ============================================================
void EEPROMStorage::begin() {
  DEBUG_PRINTLN("[EEPROM] 초기화 완료");
}

// ============================================================
// 슬롯 주소 계산
// ============================================================
uint16_t EEPROMStorage::getSlotAddress(uint8_t slot) {
  // 슬롯 1 → 주소 0
  // 슬롯 2 → 주소 4
  // 슬롯 3 → 주소 8
  return (slot - 1) * EEPROM_SLOT_SIZE;
}

// ============================================================
// 체크섬 계산
// ============================================================
uint8_t EEPROMStorage::calculateChecksum(const RobotPosition& pos) {
  // 단순 합산 MOD 256
  uint16_t sum = (uint16_t)pos.base + 
                 (uint16_t)pos.shoulder + 
                 (uint16_t)pos.elbow + 
                 (uint16_t)pos.gripper;
  return (uint8_t)(sum % 256);
}

// ============================================================
// 위치 저장 알고리즘
// ============================================================
bool EEPROMStorage::savePosition(uint8_t slot, const RobotPosition& pos) {
  DEBUG_PRINT("[EEPROM] 저장 시작 - 슬롯 ");
  DEBUG_PRINTLN(slot);
  
  // 1. 슬롯 번호 검증 (Early return)
  if (slot < 1 || slot > 3) {
    DEBUG_PRINTLN("[EEPROM] 오류: 슬롯 번호 범위 초과 (1-3)");
    return false;
  }
  
  // 2. 각도 검증
  if (pos.base > SERVO_MAX_ANGLE || 
      pos.shoulder > SERVO_MAX_ANGLE || 
      pos.elbow > SERVO_MAX_ANGLE || 
      pos.gripper > SERVO_MAX_ANGLE) {
    DEBUG_PRINTLN("[EEPROM] 오류: 각도 범위 초과");
    return false;
  }
  
  // 3. 주소 계산
  uint16_t baseAddr = getSlotAddress(slot);
  
  DEBUG_PRINT("[EEPROM] 주소: ");
  DEBUG_PRINTLN(baseAddr);
  
  // 4. 데이터 쓰기 (4바이트)
  EEPROM.write(baseAddr + 0, pos.base);
  EEPROM.write(baseAddr + 1, pos.shoulder);
  EEPROM.write(baseAddr + 2, pos.elbow);
  EEPROM.write(baseAddr + 3, pos.gripper);
  
  // 5. 체크섬 계산 및 저장
  uint8_t checksum = calculateChecksum(pos);
  uint16_t checksumAddr = EEPROM_CHECKSUM + (slot - 1);
  EEPROM.write(checksumAddr, checksum);
  
  DEBUG_PRINT("[EEPROM] 체크섬: ");
  DEBUG_PRINTLN(checksum);
  
  DEBUG_PRINTLN("[EEPROM] 저장 완료");
  return true;
}

// ============================================================
// 위치 불러오기 알고리즘
// ============================================================
bool EEPROMStorage::loadPosition(uint8_t slot, RobotPosition& pos) {
  DEBUG_PRINT("[EEPROM] 불러오기 시작 - 슬롯 ");
  DEBUG_PRINTLN(slot);
  
  // 1. 슬롯 번호 검증 (Early return)
  if (slot < 1 || slot > 3) {
    DEBUG_PRINTLN("[EEPROM] 오류: 슬롯 번호 범위 초과");
    return false;
  }
  
  // 2. 주소 계산
  uint16_t baseAddr = getSlotAddress(slot);
  uint16_t checksumAddr = EEPROM_CHECKSUM + (slot - 1);
  
  // 3. 저장된 체크섬 읽기
  uint8_t storedChecksum = EEPROM.read(checksumAddr);
  
  // 4. 데이터 읽기 (4바이트)
  pos.base = EEPROM.read(baseAddr + 0);
  pos.shoulder = EEPROM.read(baseAddr + 1);
  pos.elbow = EEPROM.read(baseAddr + 2);
  pos.gripper = EEPROM.read(baseAddr + 3);
  
  DEBUG_PRINT("[EEPROM] 읽은 데이터: [");
  DEBUG_PRINT(pos.base); DEBUG_PRINT(", ");
  DEBUG_PRINT(pos.shoulder); DEBUG_PRINT(", ");
  DEBUG_PRINT(pos.elbow); DEBUG_PRINT(", ");
  DEBUG_PRINT(pos.gripper); DEBUG_PRINTLN("]");
  
  // 5. 체크섬 검증
  uint8_t calculatedChecksum = calculateChecksum(pos);
  
  if (storedChecksum != calculatedChecksum) {
    DEBUG_PRINT("[EEPROM] 오류: 체크섬 불일치 (저장됨=");
    DEBUG_PRINT(storedChecksum);
    DEBUG_PRINT(", 계산됨=");
    DEBUG_PRINT(calculatedChecksum);
    DEBUG_PRINTLN(")");
    return false;
  }
  
  // 6. 각도 범위 검증 (손상 방지)
  if (pos.base > SERVO_MAX_ANGLE) {
    DEBUG_PRINTLN("[EEPROM] 오류: 베이스 각도 유효하지 않음");
    return false;
  }
  
  if (pos.shoulder > SERVO_MAX_ANGLE) {
    DEBUG_PRINTLN("[EEPROM] 오류: 어깨 각도 유효하지 않음");
    return false;
  }
  
  if (pos.elbow > SERVO_MAX_ANGLE) {
    DEBUG_PRINTLN("[EEPROM] 오류: 팔꿈치 각도 유효하지 않음");
    return false;
  }
  
  if (pos.gripper > SERVO_MAX_ANGLE) {
    DEBUG_PRINTLN("[EEPROM] 오류: 그리퍼 각도 유효하지 않음");
    return false;
  }
  
  DEBUG_PRINTLN("[EEPROM] 불러오기 완료 (검증 성공)");
  return true;
}

// ============================================================
// 슬롯 유효성 검사
// ============================================================
bool EEPROMStorage::isSlotValid(uint8_t slot) {
  if (slot < 1 || slot > 3) {
    return false;
  }
  
  RobotPosition tempPos;
  return loadPosition(slot, tempPos);
}

// ============================================================
// EEPROM 전체 초기화
// ============================================================
void EEPROMStorage::clearAll() {
  DEBUG_PRINTLN("[EEPROM] 전체 초기화 시작");
  
  // 모든 슬롯 영역을 0xFF로 채움
  for (uint16_t i = 0; i < 20; i++) {
    EEPROM.write(i, 0xFF);
  }
  
  DEBUG_PRINTLN("[EEPROM] 전체 초기화 완료");
}
