/**
 * @file bluetooth_protocol.h
 * @brief 블루투스 통신 프로토콜 파싱
 * @details HC-06 모듈을 통한 명령 수신 및 파싱
 * @date 2026-01-22
 */

#ifndef BLUETOOTH_PROTOCOL_H
#define BLUETOOTH_PROTOCOL_H

#include <Arduino.h>
#include "../common/config.h"
#include "../common/types.h"

/**
 * @class BluetoothProtocol
 * @brief 블루투스 통신 프로토콜 처리 클래스
 */
class BluetoothProtocol {
private:
  Stream* btSerial;             // 블루투스 시리얼 포인터
  char buffer[64];              // 수신 버퍼
  uint8_t bufferIndex;          // 버퍼 인덱스
  
  /**
   * @brief 파라미터 파싱 (쉼표 구분)
   * @param input 입력 문자열
   * @param params 출력 파라미터 배열
   * @param count 파싱된 개수
   * @return 성공 시 true
   */
  bool parseParams(const char* input, uint8_t* params, uint8_t& count);

public:
  /**
   * @brief 생성자
   * @param serial 블루투스 시리얼 객체 포인터
   */
  BluetoothProtocol(Stream* serial);
  
  /**
   * @brief 초기화
   * @return 성공 시 true
   */
  bool begin();
  
  /**
   * @brief 명령 파싱
   * @param cmd 출력 명령 구조체
   * @return 명령이 있으면 true
   */
  bool parseCommand(Command& cmd);
  
  /**
   * @brief 응답 전송
   * @param response 응답 문자열
   */
  void sendResponse(const char* response);
  
  /**
   * @brief 상태 JSON 전송
   * @param state 현재 상태
   * @param pos 현재 위치
   * @param stats 통계
   */
  void sendStatus(SystemState state, const RobotPosition& pos, 
                  const Statistics& stats);
  
  /**
   * @brief 데이터 수신 가능 여부
   * @return 수신 가능하면 true
   */
  bool available();
};

#endif // BLUETOOTH_PROTOCOL_H
