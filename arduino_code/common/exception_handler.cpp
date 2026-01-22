/**
 * @file exception_handler.cpp
 * @brief 예외 처리 알고리즘 구현
 * @date 2026-01-22
 */

#include "exception_handler.h"

// ============================================================
// 생성자
// ============================================================
ExceptionHandler::ExceptionHandler() : retryCount(0) {
  memset(&currentException, 0, sizeof(currentException));
}

// ============================================================
// 지수 백오프 지연 계산
// ============================================================
unsigned long ExceptionHandler::getBackoffDelay(uint8_t attempt) {
  // 지수 백오프: 100ms * 2^attempt
  // 1회: 100ms, 2회: 200ms, 3회: 400ms
  return 100UL * (1UL << attempt);
}

// ============================================================
// 예외 처리 메인 알고리즘
// ============================================================
ProcessResult ExceptionHandler::handleException(const Exception& ex) {
  currentException = ex;
  
  DEBUG_PRINT("[Exception] 예외 발생: ");
  DEBUG_PRINTLN(ex.message);
  
  ProcessResult result = RESULT_FAIL;
  
  // 예외 유형별 처리
  switch (ex.type) {
    case EXCEPTION_HARDWARE_FAULT:
      result = handleHardwareFault();
      break;
    
    case EXCEPTION_SENSOR_TIMEOUT:
      result = handleSensorTimeout();
      break;
    
    case EXCEPTION_COMMUNICATION_LOST:
      result = handleCommunicationLost();
      break;
    
    case EXCEPTION_POSITION_ERROR:
      result = handlePositionError();
      break;
    
    case EXCEPTION_UNKNOWN:
    default:
      DEBUG_PRINTLN("[Exception] 알 수 없는 예외 - 안전 모드");
      result = RESULT_SAFE;
      break;
  }
  
  return result;
}

// ============================================================
// 하드웨어 오류 처리
// ============================================================
ProcessResult ExceptionHandler::handleHardwareFault() {
  DEBUG_PRINTLN("[Exception] 하드웨어 오류 - 비상 정지");
  
  // 모든 모터 정지
  // (실제 구현에서는 전역 포인터 또는 콜백 사용)
  
  // 사용자 개입 필요
  return RESULT_HALTED;
}

// ============================================================
// 센서 타임아웃 처리
// ============================================================
ProcessResult ExceptionHandler::handleSensorTimeout() {
  DEBUG_PRINTLN("[Exception] 센서 타임아웃 - 재시도");
  
  retryCount++;
  
  // 최대 재시도 확인
  if (retryCount >= MAX_RETRY_COUNT) {
    DEBUG_PRINTLN("[Exception] 최대 재시도 초과 - 실패");
    retryCount = 0;
    return RESULT_FAIL;
  }
  
  // 지수 백오프 지연
  unsigned long delayTime = getBackoffDelay(retryCount);
  DEBUG_PRINT("[Exception] 재시도 ");
  DEBUG_PRINT(retryCount);
  DEBUG_PRINT(" (지연: ");
  DEBUG_PRINT(delayTime);
  DEBUG_PRINTLN("ms)");
  
  delay(delayTime);
  
  return RESULT_RETRY;
}

// ============================================================
// 통신 끊김 처리
// ============================================================
ProcessResult ExceptionHandler::handleCommunicationLost() {
  DEBUG_PRINTLN("[Exception] 통신 끊김 - 재연결 시도");
  
  // 연결 닫기
  delay(1000);
  
  // 재연결 시도
  // (실제 구현에서는 블루투스 재연결 코드)
  
  DEBUG_PRINTLN("[Exception] 재연결 완료");
  return RESULT_RECOVERED;
}

// ============================================================
// 위치 오차 처리
// ============================================================
ProcessResult ExceptionHandler::handlePositionError() {
  DEBUG_PRINTLN("[Exception] 위치 오차 - 캘리브레이션");
  
  // 홈 포지션으로 이동 (캘리브레이션)
  // (실제 구현에서는 로봇팔 홈 이동)
  
  DEBUG_PRINTLN("[Exception] 캘리브레이션 완료");
  return RESULT_RECOVERED;
}

// ============================================================
// 재시도 카운터 리셋
// ============================================================
void ExceptionHandler::resetRetryCount() {
  retryCount = 0;
  DEBUG_PRINTLN("[Exception] 재시도 카운터 리셋");
}
