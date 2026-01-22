/**
 * @file color_sensor.cpp
 * @brief 컬러 센서 알고리즘 구현
 * @date 2026-01-22
 */

#include "color_sensor.h"

// ============================================================
// 생성자
// ============================================================
ColorSensor::ColorSensor() {
  // 기본 기준값 설정 (캘리브레이션 필요)
  // 빨강: R이 낮은 주파수 (더 많은 빨강)
  refColors.red.r = 50;
  refColors.red.g = 150;
  refColors.red.b = 150;
  
  // 파랑: B가 낮은 주파수
  refColors.blue.r = 150;
  refColors.blue.g = 150;
  refColors.blue.b = 50;
  
  // 노랑: R과 G가 낮은 주파수
  refColors.yellow.r = 60;
  refColors.yellow.g = 60;
  refColors.yellow.b = 150;
}

// ============================================================
// 초기화
// ============================================================
bool ColorSensor::begin() {
  DEBUG_PRINTLN("[ColorSensor] 초기화 시작");
  
  // S0, S1 핀 설정 (주파수 스케일링)
  pinMode(PIN_COLOR_S0, OUTPUT);
  pinMode(PIN_COLOR_S1, OUTPUT);
  pinMode(PIN_COLOR_S2, OUTPUT);
  pinMode(PIN_COLOR_S3, OUTPUT);
  pinMode(PIN_COLOR_OUT, INPUT);
  
  // 주파수 스케일링 20% (S0=HIGH, S1=LOW)
  digitalWrite(PIN_COLOR_S0, HIGH);
  digitalWrite(PIN_COLOR_S1, LOW);
  
  DEBUG_PRINTLN("[ColorSensor] 초기화 완료");
  return true;
}

// ============================================================
// 필터 설정
// ============================================================
void ColorSensor::setFilter(uint8_t filter) {
  switch (filter) {
    case 0:  // Red filter
      digitalWrite(PIN_COLOR_S2, LOW);
      digitalWrite(PIN_COLOR_S3, LOW);
      break;
    case 1:  // Green filter
      digitalWrite(PIN_COLOR_S2, HIGH);
      digitalWrite(PIN_COLOR_S3, HIGH);
      break;
    case 2:  // Blue filter
      digitalWrite(PIN_COLOR_S2, LOW);
      digitalWrite(PIN_COLOR_S3, HIGH);
      break;
    default:
      break;
  }
  
  delayMicroseconds(100);  // 필터 안정화
}

// ============================================================
// 주파수 측정
// ============================================================
uint16_t ColorSensor::readFrequency() {
  // pulseIn(): HIGH 펄스 지속 시간 측정 (마이크로초)
  // 주파수가 높을수록 펄스 시간이 짧음
  unsigned long duration = pulseIn(PIN_COLOR_OUT, LOW, 50000);
  
  // 타임아웃 처리
  if (duration == 0) {
    return 255;  // 최대값 (검출 실패)
  }
  
  // 주파수 계산 (간단히 지속시간으로 사용)
  return (uint16_t)(duration / 100);
}

// ============================================================
// RGB 값 읽기
// ============================================================
RGBColor ColorSensor::readRGB() {
  RGBColor rgb;
  
  // 1. Red 필터
  setFilter(0);
  rgb.r = readFrequency();
  
  // 2. Green 필터
  setFilter(1);
  rgb.g = readFrequency();
  
  // 3. Blue 필터
  setFilter(2);
  rgb.b = readFrequency();
  
  VERBOSE_PRINT("[ColorSensor] RGB: [");
  VERBOSE_PRINT(rgb.r); VERBOSE_PRINT(", ");
  VERBOSE_PRINT(rgb.g); VERBOSE_PRINT(", ");
  VERBOSE_PRINT(rgb.b); VERBOSE_PRINTLN("]");
  
  return rgb;
}

// ============================================================
// 유클리드 거리 계산
// ============================================================
float ColorSensor::euclideanDistance(const RGBColor& c1, const RGBColor& c2) {
  int dr = (int)c1.r - (int)c2.r;
  int dg = (int)c1.g - (int)c2.g;
  int db = (int)c1.b - (int)c2.b;
  
  return sqrt(dr*dr + dg*dg + db*db);
}

// ============================================================
// 색상 판단 알고리즘 (유클리드 거리)
// ============================================================
Color ColorSensor::detectColor(const RGBColor& rgb) {
  // 각 기준 색상과의 거리 계산
  float distRed = euclideanDistance(rgb, refColors.red);
  float distBlue = euclideanDistance(rgb, refColors.blue);
  float distYellow = euclideanDistance(rgb, refColors.yellow);
  
  VERBOSE_PRINT("[ColorSensor] 거리 - R:");
  VERBOSE_PRINT(distRed);
  VERBOSE_PRINT(" B:");
  VERBOSE_PRINT(distBlue);
  VERBOSE_PRINT(" Y:");
  VERBOSE_PRINTLN(distYellow);
  
  // 최소 거리 찾기
  float minDist = distRed;
  Color detectedColor = COLOR_RED;
  
  if (distBlue < minDist) {
    minDist = distBlue;
    detectedColor = COLOR_BLUE;
  }
  
  if (distYellow < minDist) {
    minDist = distYellow;
    detectedColor = COLOR_YELLOW;
  }
  
  // 임계값 확인 (너무 멀면 UNKNOWN)
  if (minDist > COLOR_THRESHOLD) {
    detectedColor = COLOR_UNKNOWN;
    DEBUG_PRINTLN("[ColorSensor] 판단: UNKNOWN (임계값 초과)");
  } else {
    DEBUG_PRINT("[ColorSensor] 판단: ");
    DEBUG_PRINTLN(getColorName(detectedColor));
  }
  
  return detectedColor;
}

// ============================================================
// 필터링된 색상 읽기 (중앙값 필터)
// ============================================================
Color ColorSensor::readColorFiltered() {
  const uint8_t SAMPLE_COUNT = 5;
  Color samples[SAMPLE_COUNT];
  
  DEBUG_PRINTLN("[ColorSensor] 필터링 시작 (5회 샘플)");
  
  // 5회 측정
  for (uint8_t i = 0; i < SAMPLE_COUNT; i++) {
    RGBColor rgb = readRGB();
    samples[i] = detectColor(rgb);
    delay(50);
  }
  
  // 중앙값 선택 (간단히 최빈값 사용)
  uint8_t countRed = 0, countBlue = 0, countYellow = 0;
  
  for (uint8_t i = 0; i < SAMPLE_COUNT; i++) {
    if (samples[i] == COLOR_RED) countRed++;
    else if (samples[i] == COLOR_BLUE) countBlue++;
    else if (samples[i] == COLOR_YELLOW) countYellow++;
  }
  
  // 최빈값 반환
  Color result = COLOR_UNKNOWN;
  uint8_t maxCount = 0;
  
  if (countRed > maxCount) {
    maxCount = countRed;
    result = COLOR_RED;
  }
  if (countBlue > maxCount) {
    maxCount = countBlue;
    result = COLOR_BLUE;
  }
  if (countYellow > maxCount) {
    maxCount = countYellow;
    result = COLOR_YELLOW;
  }
  
  DEBUG_PRINT("[ColorSensor] 필터링 결과: ");
  DEBUG_PRINT(getColorName(result));
  DEBUG_PRINT(" (R:");
  DEBUG_PRINT(countRed);
  DEBUG_PRINT(" B:");
  DEBUG_PRINT(countBlue);
  DEBUG_PRINT(" Y:");
  DEBUG_PRINT(countYellow);
  DEBUG_PRINTLN(")");
  
  return result;
}

// ============================================================
// 캘리브레이션
// ============================================================
void ColorSensor::calibrate(Color color) {
  DEBUG_PRINT("[ColorSensor] 캘리브레이션 시작: ");
  DEBUG_PRINTLN(getColorName(color));
  
  // 10회 측정 평균
  uint32_t sumR = 0, sumG = 0, sumB = 0;
  
  for (uint8_t i = 0; i < 10; i++) {
    RGBColor rgb = readRGB();
    sumR += rgb.r;
    sumG += rgb.g;
    sumB += rgb.b;
    delay(100);
  }
  
  RGBColor avgColor;
  avgColor.r = sumR / 10;
  avgColor.g = sumG / 10;
  avgColor.b = sumB / 10;
  
  // 기준값 업데이트
  switch (color) {
    case COLOR_RED:
      refColors.red = avgColor;
      break;
    case COLOR_BLUE:
      refColors.blue = avgColor;
      break;
    case COLOR_YELLOW:
      refColors.yellow = avgColor;
      break;
    default:
      break;
  }
  
  DEBUG_PRINT("[ColorSensor] 캘리브레이션 완료: [");
  DEBUG_PRINT(avgColor.r); DEBUG_PRINT(", ");
  DEBUG_PRINT(avgColor.g); DEBUG_PRINT(", ");
  DEBUG_PRINT(avgColor.b); DEBUG_PRINTLN("]");
}
