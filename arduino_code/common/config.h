/**
 * @file config.h
 * @brief 스마트 팩토리 시스템 전역 설정 파일
 * @details 핀 번호, 상수, 임계값 등 시스템 전체 설정 정의
 * @date 2026-01-22
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================
// 서보 모터 핀 설정 (로봇팔 4축)
// ============================================================
#define PIN_SERVO_BASE      6   // 1축: 베이스 회전 (0-180도)
#define PIN_SERVO_SHOULDER  9   // 2축: 어깨 상하 (0-180도)
#define PIN_SERVO_ELBOW     10  // 3축: 팔꿈치 굽힘 (0-180도)
#define PIN_SERVO_GRIPPER   11  // 4축: 그리퍼 집기 (0-90도)

// ============================================================
// DC 모터 제어 핀 (L298N 드라이버)
// ============================================================
#define PIN_MOTOR_IN1       7   // 방향 제어 1
#define PIN_MOTOR_IN2       8   // 방향 제어 2
#define PIN_MOTOR_ENA       5   // PWM 속도 제어 (0-255)

// ============================================================
// 센서 핀 설정
// ============================================================
// IR 센서
#define PIN_IR_SENSOR       2   // 물체 감지 센서 (디지털)

// 초음파 센서 (HC-SR04)
#define PIN_ULTRASONIC_TRIG 3   // 트리거 핀
#define PIN_ULTRASONIC_ECHO 4   // 에코 핀

// 컬러 센서 (TCS3200)
#define PIN_COLOR_S0        A0  // 주파수 스케일링
#define PIN_COLOR_S1        A1  // 주파수 스케일링
#define PIN_COLOR_S2        A2  // 색상 필터 선택
#define PIN_COLOR_S3        A3  // 색상 필터 선택
#define PIN_COLOR_OUT       A4  // 주파수 출력

// ============================================================
// 피드백 장치 핀
// ============================================================
// LED Matrix (8x8, MAX7219)
#define PIN_MATRIX_DIN      12  // 데이터 입력
#define PIN_MATRIX_CLK      13  // 클럭
#define PIN_MATRIX_CS       A5  // 칩 셀렉트

// 피에조 부저
#define PIN_BUZZER          3   // PWM 지원 핀

// ============================================================
// 서보 모터 각도 제한
// ============================================================
#define SERVO_MIN_ANGLE     0
#define SERVO_MAX_ANGLE     180

// 안전 각도 범위 (과부하 방지)
#define BASE_SAFE_MIN       0
#define BASE_SAFE_MAX       180
#define SHOULDER_SAFE_MIN   45
#define SHOULDER_SAFE_MAX   135
#define ELBOW_SAFE_MIN      45
#define ELBOW_SAFE_MAX      135
#define GRIPPER_OPEN        0
#define GRIPPER_CLOSE       70

// ============================================================
// 동기화 이동 파라미터
// ============================================================
#define SYNC_STEP_SIZE      1   // 한 스텝당 이동 각도 (도)
#define SYNC_STEP_DELAY     15  // 스텝 간 지연 시간 (밀리초)

// ============================================================
// 모터 속도 설정
// ============================================================
#define MOTOR_SPEED_STOP    0
#define MOTOR_SPEED_SLOW    100
#define MOTOR_SPEED_MEDIUM  150
#define MOTOR_SPEED_FAST    200
#define MOTOR_SPEED_MAX     255

// ============================================================
// 센서 임계값
// ============================================================
// IR 센서
#define IR_DETECT_THRESHOLD 512 // 아날로그 센서인 경우

// 초음파 센서
#define ULTRASONIC_MAX_CM   200 // 최대 측정 거리 (cm)
#define ULTRASONIC_MIN_CM   2   // 최소 측정 거리 (cm)

// 컬러 센서
#define COLOR_THRESHOLD     50  // 색상 판단 허용 오차

// ============================================================
// 타이밍 상수
// ============================================================
#define DELAY_SHORT         200   // 짧은 대기 (ms)
#define DELAY_MEDIUM        500   // 중간 대기 (ms)
#define DELAY_LONG          1000  // 긴 대기 (ms)
#define DELAY_GRIPPER       500   // 그리퍼 동작 대기 (ms)
#define DELAY_SHOULDER      1000  // 어깨 이동 대기 (ms)

// ============================================================
// EEPROM 메모리 맵
// ============================================================
#define EEPROM_SLOT_SIZE    4     // 슬롯당 바이트 (4축 × 1바이트)
#define EEPROM_SLOT_1       0     // 포지션 1 주소
#define EEPROM_SLOT_2       4     // 포지션 2 주소
#define EEPROM_SLOT_3       8     // 포지션 3 주소
#define EEPROM_CHECKSUM     12    // 체크섬 시작 주소

// ============================================================
// 블루투스 통신 설정
// ============================================================
#define BLUETOOTH_BAUD      9600  // HC-06 기본 보레이트
#define SERIAL_BAUD         9600  // 시리얼 모니터 보레이트
#define BLUETOOTH_TIMEOUT   1000  // 타임아웃 (ms)

// ============================================================
// 상태 머신 상수
// ============================================================
#define MAX_RETRY_COUNT     3     // 최대 재시도 횟수
#define STATE_LOOP_DELAY    10    // 상태 머신 루프 지연 (ms)

// ============================================================
// AI 설정
// ============================================================
#define AI_CONFIDENCE_MIN   0.8   // 최소 신뢰도 (80%)

// ============================================================
// 작업 구역 각도 설정
// ============================================================
// 구역 1 (왼쪽)
#define ZONE1_BASE          45
#define ZONE1_SHOULDER      100
#define ZONE1_ELBOW         90

// 구역 2 (중앙)
#define ZONE2_BASE          90
#define ZONE2_SHOULDER      100
#define ZONE2_ELBOW         90

// 구역 3 (오른쪽)
#define ZONE3_BASE          135
#define ZONE3_SHOULDER      100
#define ZONE3_ELBOW         90

// ============================================================
// 게이트 각도 설정 (색상별)
// ============================================================
#define GATE_ANGLE_RED      0     // 빨강 → 0도
#define GATE_ANGLE_BLUE     90    // 파랑 → 90도
#define GATE_ANGLE_YELLOW   180   // 노랑 → 180도
#define GATE_ANGLE_PASS     90    // 통과 → 90도

// ============================================================
// 디버그 설정
// ============================================================
#define DEBUG_MODE          true  // 디버그 출력 활성화
#define DEBUG_VERBOSE       false // 상세 로그 출력

// 디버그 매크로
#if DEBUG_MODE
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

#if DEBUG_VERBOSE
  #define VERBOSE_PRINT(x)    Serial.print(x)
  #define VERBOSE_PRINTLN(x)  Serial.println(x)
#else
  #define VERBOSE_PRINT(x)
  #define VERBOSE_PRINTLN(x)
#endif

// ============================================================
// 부저 주파수 (상태별)
// ============================================================
#define FREQ_HOME           440   // 라 (A4)
#define FREQ_PICK           523   // 도 (C5)
#define FREQ_MOVE           659   // 미 (E5)
#define FREQ_DONE           784   // 솔 (G5)
#define FREQ_ERROR          220   // 라 (A3) - 낮은 음

#endif // CONFIG_H
