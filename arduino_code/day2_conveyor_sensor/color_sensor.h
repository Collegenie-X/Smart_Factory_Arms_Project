/**
 * @file color_sensor.h
 * @brief TCS3200 컬러 센서 RGB 분석 알고리즘
 * @details 유클리드 거리 기반 색상 판단
 * @date 2026-01-22
 */

#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <Arduino.h>
#include "../common/config.h"
#include "../common/types.h"

/**
 * @class ColorSensor
 * @brief 컬러 센서 제어 클래스
 */
class ColorSensor {
private:
  // RGB 기준값 (캘리브레이션 필요)
  struct ColorReference {
    RGBColor red;
    RGBColor blue;
    RGBColor yellow;
  };
  
  ColorReference refColors;  // 기준 색상
  
  /**
   * @brief 필터 설정 (R/G/B)
   * @param filter 필터 타입 (0=R, 1=G, 2=B)
   */
  void setFilter(uint8_t filter);
  
  /**
   * @brief 주파수 측정
   * @return 주파수 값
   */
  uint16_t readFrequency();
  
  /**
   * @brief 유클리드 거리 계산
   * @param c1 색상1
   * @param c2 색상2
   * @return 거리 값
   */
  float euclideanDistance(const RGBColor& c1, const RGBColor& c2);

public:
  /**
   * @brief 생성자
   */
  ColorSensor();
  
  /**
   * @brief 초기화
   * @return 성공 시 true
   */
  bool begin();
  
  /**
   * @brief RGB 값 읽기
   * @return RGB 색상
   */
  RGBColor readRGB();
  
  /**
   * @brief 색상 판단
   * @param rgb RGB 값
   * @return 판단된 색상
   */
  Color detectColor(const RGBColor& rgb);
  
  /**
   * @brief 필터링된 색상 읽기 (5회 평균)
   * @return 색상
   */
  Color readColorFiltered();
  
  /**
   * @brief 캘리브레이션
   * @param color 캘리브레이션할 색상
   */
  void calibrate(Color color);
};

#endif // COLOR_SENSOR_H
