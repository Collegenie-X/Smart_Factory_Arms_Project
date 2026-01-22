# 스마트 팩토리 아두이노 C 코드

## 📁 프로젝트 구조

```
arduino_code/
├── common/                      # 공통 모듈
│   ├── config.h                 # 전역 설정 (핀 번호, 상수)
│   ├── types.h                  # 자료구조 정의 (열거형, 구조체)
│   ├── exception_handler.h/cpp  # 예외 처리 알고리즘
│
├── day1_robot_arm/              # Day 1: 로봇팔 기본 제어
│   ├── day1_robot_arm.ino       # 메인 프로그램
│   ├── servo_sync_movement.h/cpp    # 4축 동기화 이동 알고리즘
│   ├── bluetooth_protocol.h/cpp     # 블루투스 명령 파싱
│   └── eeprom_storage.h/cpp         # EEPROM 저장/복구
│
├── day2_conveyor_sensor/        # Day 2: 센서 + 컨베이어
│   ├── day2_conveyor_system.ino # 메인 프로그램
│   ├── color_sensor.h/cpp       # TCS3200 컬러 센서 (RGB 분석)
│   └── motor_controller.h/cpp   # DC 모터 제어 (L298N)
│
└── day3_scenarios/              # Day 3: 통합 시나리오
    ├── scenario_a_ai_smart.ino  # 시나리오 A (AI 기반 스마트 입고)
    └── state_machine.h/cpp      # 통합 상태 머신
```

---

## 🎯 각 Day별 기능

### Day 1: 로봇팔 제어
- ✅ 4축 서보 동기화 이동 (선형 보간)
- ✅ 블루투스 명령 프로토콜
- ✅ EEPROM 위치 저장/복구 (체크섬 검증)
- ✅ 집기/놓기 시퀀스 알고리즘

### Day 2: 센서 + 컨베이어
- ✅ 컬러 센서 RGB 분석 (유클리드 거리)
- ✅ DC 모터 PWM 제어 (H-Bridge)
- ✅ IR 센서 물체 감지
- ✅ 자동 색상 분류 시스템

### Day 3: AI 통합
- ✅ 상태 머신 (10가지 상태)
- ✅ AI + 센서 하이브리드 검증
- ✅ 예외 처리 (지수 백오프)

---

## 🔧 하드웨어 요구사항

### 필수 부품
| 부품 | 모델 | 용도 |
|------|------|------|
| 마이크로컨트롤러 | Arduino UNO/MEGA | 메인 제어 |
| 로봇팔 | BP Lab 4축 | 물체 피킹 |
| 컬러 센서 | TCS3200 | 색상 감지 |
| IR 센서 | 적외선 센서 | 물체 감지 |
| 모터 드라이버 | L298N | DC 모터 제어 |
| 블루투스 | HC-06 | 앱 통신 |
| 서보 모터 | SG90 × 4개 | 로봇팔 구동 |
| DC 모터 | 12V | 컨베이어 벨트 |

### 배선도
```
Arduino UNO
├─ D6  → 베이스 서보
├─ D9  → 어깨 서보
├─ D10 → 팔꿈치 서보
├─ D11 → 그리퍼 서보
├─ D7  → 모터 IN1
├─ D8  → 모터 IN2
├─ D5  → 모터 ENA (PWM)
├─ D2  → IR 센서
├─ A0-A4 → 컬러 센서
└─ D3  → 피에조 부저
```

---

## 📖 사용 방법

### 1. Arduino IDE 설정
```bash
1. Arduino IDE 설치 (v1.8.19 이상)
2. 필수 라이브러리 설치:
   - Servo.h (기본 내장)
   - EEPROM.h (기본 내장)
```

### 2. Day 1 실행
```cpp
// day1_robot_arm.ino 업로드

// 블루투스 명령어:
H           // 홈 포지션
P           // 집기 (구역 1)
L           // 왼쪽 놓기
R           // 오른쪽 놓기
M90,90,90,0 // 수동 이동 (base,shoulder,elbow,gripper)
S1          // 슬롯 1에 저장
D1          // 슬롯 1 불러오기
?           // 상태 조회
```

### 3. Day 2 실행
```cpp
// day2_conveyor_system.ino 업로드

// 동작:
1. 컨베이어 자동 작동
2. IR 센서가 물체 감지
3. 벨트 정지
4. 컬러 센서로 색상 판단
5. 게이트로 분류
6. 통계 전송
```

### 4. Day 3 실행
```cpp
// scenario_a_ai_smart.ino 업로드

// 블루투스 명령어:
A RED 1     // AI: 빨강, 구역 1
A BLUE 2    // AI: 파랑, 구역 2
A YELLOW 3  // AI: 노랑, 구역 3
E           // 비상 정지
H           // 홈 포지션
?           // 상태 조회
```

---

## 🧠 핵심 알고리즘

### 1. 동기화 이동 알고리즘 (선형 보간)
```cpp
// servo_sync_movement.cpp
// 4축이 동시에 목표 위치에 도달하도록 비례 이동
bool synchronizedMove(RobotPosition target) {
  // 최대 이동 거리 계산
  int maxDistance = getMaxDistance(current, target);
  int steps = maxDistance / STEP_SIZE;
  
  // 각 스텝마다 비례 이동
  for (int step = 1; step <= steps; step++) {
    float progress = (float)step / steps;
    
    // 각 축별 선형 보간
    for (int i = 0; i < 4; i++) {
      int newAngle = current[i] + 
                     (target[i] - current[i]) * progress;
      servos[i].write(newAngle);
    }
    delay(STEP_DELAY);
  }
  
  return true;
}
```

### 2. 색상 판단 알고리즘 (유클리드 거리)
```cpp
// color_sensor.cpp
Color detectColor(RGBColor rgb) {
  // 기준 색상과의 거리 계산
  float distRed = euclideanDistance(rgb, refRed);
  float distBlue = euclideanDistance(rgb, refBlue);
  float distYellow = euclideanDistance(rgb, refYellow);
  
  // 최소 거리 찾기
  float minDist = min(distRed, min(distBlue, distYellow));
  
  if (minDist == distRed) return COLOR_RED;
  if (minDist == distBlue) return COLOR_BLUE;
  if (minDist == distYellow) return COLOR_YELLOW;
  
  return COLOR_UNKNOWN;
}
```

### 3. 상태 머신 (시나리오 A)
```cpp
// state_machine.cpp
void update() {
  switch (currentState) {
    case STATE_IDLE:
      // AI 명령 대기
      break;
    
    case STATE_PICKING:
      // 로봇팔 집기
      if (pickSequence(zone)) {
        transitionTo(STATE_TRANSPORTING);
      }
      break;
    
    case STATE_VERIFYING:
      // AI vs 센서 검증
      if (aiColor == sensorColor) {
        transitionTo(STATE_SORTING);
      } else {
        transitionTo(STATE_ERROR);
      }
      break;
    
    // ... 기타 상태
  }
}
```

### 4. 예외 처리 (지수 백오프)
```cpp
// exception_handler.cpp
ProcessResult handleSensorTimeout() {
  retryCount++;
  
  if (retryCount >= MAX_RETRY) {
    return RESULT_FAIL;
  }
  
  // 지수 백오프: 100ms * 2^retry
  unsigned long delay = 100 * (1 << retryCount);
  delay(delay);
  
  return RESULT_RETRY;
}
```

---

## 📊 시리얼 모니터 출력 예시

### Day 1 실행 로그
```
========================================
   스마트 팩토리 로봇팔 시스템 Day 1
========================================

[Main] 1/3 서보 초기화 중...
[ServoSync] 서보 초기화 시작
[ServoSync] 서보 초기화 완료
[Main] 2/3 블루투스 초기화 중...
[Bluetooth] 초기화 완료
[Main] 3/3 EEPROM 초기화 중...
[EEPROM] 초기화 완료

========================================
   초기화 완료 - 대기 중
========================================

[Bluetooth] 명령 수신: P
[Bluetooth] 명령: PICK
[Main] 상태: PICKING
[ServoSync] 집기 시작 - 구역: 1
[ServoSync] 1단계: 그리퍼 열기
[ServoSync] 2단계: 구역 위로 이동
[ServoSync] 동기화 이동 시작: 45 스텝
[ServoSync] 동기화 이동 완료
[ServoSync] 3단계: 내려가기
[ServoSync] 4단계: 그리퍼 닫기
[ServoSync] 5단계: 올리기
[ServoSync] 집기 완료
[Main] 상태: IDLE
```

### Day 2 실행 로그
```
========================================
   스마트 팩토리 컨베이어 시스템 Day 2
========================================

[ColorSensor] 초기화 시작
[ColorSensor] 초기화 완료
[Motor] 초기화 시작
[Motor] 초기화 완료

[Main] 물체 감지!
[Main] 색상 측정 시작
[ColorSensor] 필터링 시작 (5회 샘플)
[ColorSensor] RGB: [50, 150, 150]
[ColorSensor] 판단: RED
[ColorSensor] 필터링 결과: RED (R:5 B:0 Y:0)
[Gate] 빨강 → 0도
[Stats] R:1 B:0 Y:0 Total:1
[Main] 분류 완료, 컨베이어 재시작
```

---

## 🐛 문제 해결 (Troubleshooting)

### 문제 1: 서보가 떨림
**원인**: 전원 부족  
**해결**: 외부 12V 전원 사용 (USB 전원 사용 금지)

### 문제 2: 컬러 센서 오인식
**원인**: 조명 변화  
**해결**: 캘리브레이션 실행 (`C R` / `C B` / `C Y` 명령)

### 문제 3: 블루투스 연결 끊김
**원인**: 거리 또는 장애물  
**해결**: HC-06을 2m 이내로 배치, 재연결 시도

### 문제 4: EEPROM 복구 실패
**원인**: 체크섬 불일치  
**해결**: EEPROM 초기화 후 재저장

---

## 🔬 성능 최적화 팁

### 1. 속도 향상
```cpp
// config.h 수정
#define SYNC_STEP_SIZE      2   // 1 → 2 (빠르지만 덜 부드러움)
#define SYNC_STEP_DELAY     10  // 15 → 10 (더 빠른 이동)
```

### 2. 정확도 향상
```cpp
// 컬러 센서 샘플 증가
const uint8_t SAMPLE_COUNT = 10;  // 5 → 10
```

### 3. 안정성 향상
```cpp
// 재시도 횟수 증가
#define MAX_RETRY_COUNT     5   // 3 → 5
```

---

## 📚 참고 자료

### 알고리즘 문서
- [선형 보간 (Linear Interpolation)](https://en.wikipedia.org/wiki/Linear_interpolation)
- [유클리드 거리 (Euclidean Distance)](https://en.wikipedia.org/wiki/Euclidean_distance)
- [상태 머신 (State Machine)](https://en.wikipedia.org/wiki/Finite-state_machine)
- [지수 백오프 (Exponential Backoff)](https://en.wikipedia.org/wiki/Exponential_backoff)

### Arduino 라이브러리
- [Servo Library](https://www.arduino.cc/reference/en/libraries/servo/)
- [EEPROM Library](https://www.arduino.cc/en/Reference/EEPROM)

### 하드웨어 데이터시트
- [TCS3200 컬러 센서](https://www.mouser.com/datasheet/2/588/TCS3200-E11-1114230.pdf)
- [L298N 모터 드라이버](https://www.st.com/resource/en/datasheet/l298.pdf)
- [HC-06 블루투스](http://www.electronica60norte.com/mwfls/pdf/hc06.pdf)

---

## 📝 라이선스

본 코드는 교육 목적으로 제작되었습니다.  
**제작**: Smart Factory Education Team  
**날짜**: 2026-01-22

---

## 🤝 기여

버그 리포트 또는 개선 제안은 이슈로 등록해 주세요.

---

## 📞 문의

교육 관련 문의: smartfactory@education.com  
기술 지원: tech@smartfactory.com

---

**Happy Coding! 🚀**
