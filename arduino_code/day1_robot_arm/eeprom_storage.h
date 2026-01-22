/**
 * @file eeprom_storage.h
 * @brief EEPROM 비휘발성 메모리 저장/복구 알고리즘
 * @details 체크섬 검증을 포함한 안전한 데이터 저장
 * @date 2026-01-22
 */

#ifndef EEPROM_STORAGE_H
#define EEPROM_STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>
#include "../common/config.h"
#include "../common/types.h"

/**
 * @class EEPROMStorage
 * @brief EEPROM 저장/복구 클래스
 */
class EEPROMStorage {
private:
  /**
   * @brief 체크섬 계산
   * @param pos 위치 데이터
   * @return 체크섬 값
   */
  uint8_t calculateChecksum(const RobotPosition& pos);
  
  /**
   * @brief 슬롯 주소 계산
   * @param slot 슬롯 번호 (1-3)
   * @return EEPROM 주소
   */
  uint16_t getSlotAddress(uint8_t slot);

public:
  /**
   * @brief 초기화
   */
  void begin();
  
  /**
   * @brief 위치 저장
   * @param slot 슬롯 번호 (1-3)
   * @param pos 저장할 위치
   * @return 성공 시 true
   */
  bool savePosition(uint8_t slot, const RobotPosition& pos);
  
  /**
   * @brief 위치 불러오기
   * @param slot 슬롯 번호 (1-3)
   * @param pos 불러온 위치 (출력)
   * @return 성공 시 true
   */
  bool loadPosition(uint8_t slot, RobotPosition& pos);
  
  /**
   * @brief 슬롯 유효성 검사
   * @param slot 슬롯 번호
   * @return 유효하면 true
   */
  bool isSlotValid(uint8_t slot);
  
  /**
   * @brief EEPROM 전체 초기화
   */
  void clearAll();
};

#endif // EEPROM_STORAGE_H
