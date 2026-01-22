/**
 * @file types.h
 * @brief 스마트 팩토리 시스템 공통 자료구조 정의
 * @details 열거형, 구조체 등 시스템 전반에서 사용되는 타입 정의
 * @date 2026-01-22
 */

#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

// ============================================================
// 시스템 상태 열거형
// ============================================================
/**
 * @enum SystemState
 * @brief 로봇 시스템의 전체 상태
 */
enum SystemState {
  STATE_IDLE = 0,         // 대기 상태
  STATE_SCANNING,         // AI 스캔 중
  STATE_PICKING,          // 물체 집기 중
  STATE_MOVING,           // 이동 중
  STATE_TRANSPORTING,     // 컨베이어 이송 중
  STATE_DETECTING,        // 센서 감지 중
  STATE_VERIFYING,        // 검증 중
  STATE_SORTING,          // 분류 중
  STATE_ERROR,            // 오류 상태
  STATE_EMERGENCY_STOP    // 비상 정지
};

// ============================================================
// 색상 열거형
// ============================================================
/**
 * @enum Color
 * @brief 인식 가능한 색상 종류
 */
enum Color {
  COLOR_UNKNOWN = 0,
  COLOR_RED,
  COLOR_BLUE,
  COLOR_YELLOW,
  COLOR_GREEN
};

// ============================================================
// 작업 구역 열거형
// ============================================================
/**
 * @enum Zone
 * @brief 물체 위치 구역
 */
enum Zone {
  ZONE_NONE = 0,
  ZONE_1,     // 왼쪽
  ZONE_2,     // 중앙
  ZONE_3      // 오른쪽
};

// ============================================================
// 모터 방향 열거형
// ============================================================
/**
 * @enum MotorDirection
 * @brief DC 모터 회전 방향
 */
enum MotorDirection {
  MOTOR_STOP = 0,
  MOTOR_FORWARD,
  MOTOR_BACKWARD
};

// ============================================================
// 예외 유형 열거형
// ============================================================
/**
 * @enum ExceptionType
 * @brief 시스템 예외 유형
 */
enum ExceptionType {
  EXCEPTION_NONE = 0,
  EXCEPTION_HARDWARE_FAULT,     // 하드웨어 오류
  EXCEPTION_SENSOR_TIMEOUT,     // 센서 응답 없음
  EXCEPTION_COMMUNICATION_LOST, // 통신 끊김
  EXCEPTION_POSITION_ERROR,     // 위치 오차
  EXCEPTION_UNKNOWN             // 미정의 오류
};

// ============================================================
// 처리 결과 열거형
// ============================================================
/**
 * @enum ProcessResult
 * @brief 작업 처리 결과
 */
enum ProcessResult {
  RESULT_SUCCESS = 0,
  RESULT_FAIL,
  RESULT_RETRY,
  RESULT_RECOVERED,
  RESULT_SAFE,
  RESULT_HALTED
};

// ============================================================
// 로봇팔 위치 구조체
// ============================================================
/**
 * @struct RobotPosition
 * @brief 로봇팔 4축 각도 위치
 */
struct RobotPosition {
  uint8_t base;      // 베이스 각도 (0-180)
  uint8_t shoulder;  // 어깨 각도 (0-180)
  uint8_t elbow;     // 팔꿈치 각도 (0-180)
  uint8_t gripper;   // 그리퍼 각도 (0-90)
};

// ============================================================
// RGB 색상 구조체
// ============================================================
/**
 * @struct RGBColor
 * @brief RGB 색상 값
 */
struct RGBColor {
  uint8_t r;  // 빨강 (0-255)
  uint8_t g;  // 초록 (0-255)
  uint8_t b;  // 파랑 (0-255)
};

// ============================================================
// 센서 데이터 구조체
// ============================================================
/**
 * @struct SensorData
 * @brief 모든 센서 측정값
 */
struct SensorData {
  bool irDetected;          // IR 센서 감지 여부
  uint16_t distance;        // 초음파 거리 (cm)
  RGBColor colorRGB;        // 컬러 센서 RGB
  Color detectedColor;      // 판단된 색상
  unsigned long timestamp;  // 측정 시각 (ms)
};

// ============================================================
// 통계 데이터 구조체
// ============================================================
/**
 * @struct Statistics
 * @brief 작업 통계
 */
struct Statistics {
  uint16_t redCount;      // 빨강 처리 개수
  uint16_t blueCount;     // 파랑 처리 개수
  uint16_t yellowCount;   // 노랑 처리 개수
  uint16_t unknownCount;  // 미분류 개수
  uint16_t errorCount;    // 오류 개수
  uint16_t totalCount;    // 전체 처리 개수
};

// ============================================================
// 명령 구조체
// ============================================================
/**
 * @struct Command
 * @brief 블루투스 수신 명령
 */
struct Command {
  char type;              // 명령 타입 ('H', 'P', 'M', 'S', 'L', '?')
  uint8_t params[4];      // 파라미터 배열
  bool valid;             // 유효성 플래그
};

// ============================================================
// 검증 결과 구조체
// ============================================================
/**
 * @struct VerificationResult
 * @brief 3단계 검증 결과 (시나리오 C)
 */
struct VerificationResult {
  bool stage1;    // 단계1: 위치 확인
  bool stage2;    // 단계2: 중량 확인
  bool stage3;    // 단계3: 색상 확인
  bool overall;   // 전체 결과
};

// ============================================================
// 예외 구조체
// ============================================================
/**
 * @struct Exception
 * @brief 예외 정보
 */
struct Exception {
  ExceptionType type;     // 예외 유형
  SystemState stateWhen;  // 발생 시점 상태
  unsigned long timestamp;// 발생 시각
  char message[32];       // 오류 메시지
};

// ============================================================
// 필터 버퍼 구조체 (이동 평균)
// ============================================================
/**
 * @struct FilterBuffer
 * @brief 센서 노이즈 필터링용 버퍼
 */
template<typename T, int SIZE>
struct FilterBuffer {
  T buffer[SIZE];
  int index;
  int count;
  
  FilterBuffer() : index(0), count(0) {
    for (int i = 0; i < SIZE; i++) {
      buffer[i] = 0;
    }
  }
  
  // 샘플 추가
  void add(T value) {
    buffer[index] = value;
    index = (index + 1) % SIZE;
    if (count < SIZE) count++;
  }
  
  // 평균 계산
  T getAverage() {
    if (count == 0) return 0;
    T sum = 0;
    for (int i = 0; i < count; i++) {
      sum += buffer[i];
    }
    return sum / count;
  }
  
  // 초기화
  void reset() {
    index = 0;
    count = 0;
  }
};

// ============================================================
// LED 아이콘 패턴 (8x8)
// ============================================================
/**
 * @brief LED Matrix 아이콘 패턴 정의
 */
const uint8_t ICON_HOME[8] = {
  0b00011000,
  0b00111100,
  0b01111110,
  0b11111111,
  0b00111100,
  0b00111100,
  0b00111100,
  0b00000000
};

const uint8_t ICON_PICK[8] = {
  0b00011000,
  0b00111100,
  0b01111110,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000
};

const uint8_t ICON_MOVE[8] = {
  0b00001000,
  0b00011000,
  0b00111111,
  0b01111111,
  0b01111111,
  0b00111111,
  0b00011000,
  0b00001000
};

const uint8_t ICON_DONE[8] = {
  0b00000000,
  0b00000001,
  0b00000011,
  0b10000110,
  0b11001100,
  0b01111000,
  0b00110000,
  0b00000000
};

const uint8_t ICON_ERROR[8] = {
  0b10000001,
  0b01000010,
  0b00100100,
  0b00011000,
  0b00011000,
  0b00100100,
  0b01000010,
  0b10000001
};

// ============================================================
// 유틸리티 함수 선언
// ============================================================
/**
 * @brief 각도 범위 제한 (0-180)
 */
inline uint8_t constrainAngle(int angle) {
  if (angle < 0) return 0;
  if (angle > 180) return 180;
  return (uint8_t)angle;
}

/**
 * @brief PWM 값 범위 제한 (0-255)
 */
inline uint8_t constrainPWM(int value) {
  if (value < 0) return 0;
  if (value > 255) return 255;
  return (uint8_t)value;
}

/**
 * @brief 색상 이름 문자열 반환
 */
inline const char* getColorName(Color color) {
  switch (color) {
    case COLOR_RED:     return "RED";
    case COLOR_BLUE:    return "BLUE";
    case COLOR_YELLOW:  return "YELLOW";
    case COLOR_GREEN:   return "GREEN";
    default:            return "UNKNOWN";
  }
}

/**
 * @brief 상태 이름 문자열 반환
 */
inline const char* getStateName(SystemState state) {
  switch (state) {
    case STATE_IDLE:            return "IDLE";
    case STATE_SCANNING:        return "SCANNING";
    case STATE_PICKING:         return "PICKING";
    case STATE_MOVING:          return "MOVING";
    case STATE_TRANSPORTING:    return "TRANSPORTING";
    case STATE_DETECTING:       return "DETECTING";
    case STATE_VERIFYING:       return "VERIFYING";
    case STATE_SORTING:         return "SORTING";
    case STATE_ERROR:           return "ERROR";
    case STATE_EMERGENCY_STOP:  return "EMERGENCY";
    default:                    return "UNKNOWN";
  }
}

#endif // TYPES_H
