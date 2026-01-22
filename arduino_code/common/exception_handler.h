/**
 * @file exception_handler.h
 * @brief 예외 처리 알고리즘
 * @details 시스템 오류 감지 및 복구
 * @date 2026-01-22
 */

#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

#include <Arduino.h>
#include "config.h"
#include "types.h"

/**
 * @class ExceptionHandler
 * @brief 예외 처리 클래스
 */
class ExceptionHandler {
private:
  Exception currentException;  // 현재 예외
  uint8_t retryCount;         // 재시도 카운터
  
  /**
   * @brief 재시도 지연 계산 (지수 백오프)
   * @param attempt 시도 횟수
   * @return 지연 시간 (ms)
   */
  unsigned long getBackoffDelay(uint8_t attempt);

public:
  /**
   * @brief 생성자
   */
  ExceptionHandler();
  
  /**
   * @brief 예외 처리
   * @param ex 예외 객체
   * @return 처리 결과
   */
  ProcessResult handleException(const Exception& ex);
  
  /**
   * @brief 하드웨어 오류 처리
   * @return 처리 결과
   */
  ProcessResult handleHardwareFault();
  
  /**
   * @brief 센서 타임아웃 처리
   * @return 처리 결과
   */
  ProcessResult handleSensorTimeout();
  
  /**
   * @brief 통신 끊김 처리
   * @return 처리 결과
   */
  ProcessResult handleCommunicationLost();
  
  /**
   * @brief 위치 오차 처리
   * @return 처리 결과
   */
  ProcessResult handlePositionError();
  
  /**
   * @brief 재시도 카운터 리셋
   */
  void resetRetryCount();
};

#endif // EXCEPTION_HANDLER_H
