# 모듈형 컨베이어 시스템 - 아두이노 제어 프로그램

## 📋 프로젝트 개요

이 프로젝트는 스마트 팩토리 컨베이어 벨트 시스템을 모듈별로 분리하여 테스트하고 학습할 수 있도록 재구성한 것입니다.

## 🗂️ 폴더 구조

```
modular_conveyor_system/
├── 01_infrared_sensor_test/           # 적외선 센서 테스트
├── 02_color_sensor_test/              # 컬러 센서 테스트
├── 03_rgb_led_colors_test/            # RGB LED 색상 표시 테스트
├── 04_dc_motor_control_test/          # DC 모터 제어 테스트
├── 05_servo_motor_control_test/       # 서보 모터 제어 테스트
├── 06_integrated_conveyor_system/     # 통합 시스템 (기본 버전)
├── 07_serial_controlled_system/       # Serial 제어 시스템 (start/stop)
├── 08_dual_comm_system/               # Serial + Bluetooth 제어 시스템
└── README.md                          # 이 파일
```

---

## 🔧 각 모듈 상세 설명

### 1️⃣ 적외선 센서 테스트 (01_infrared_sensor_test)

**기능:**
- 적외선 센서로 물체 감지
- 디지털/아날로그 모드 지원
- 감지 시 LED 및 부저 알림
- 민감도 조절 가능

**핀 연결:**
- `A0` - 적외선 센서
- `13` - 상태 표시 LED
- `4` - 부저

**주요 설정값:**
```cpp
#define PIN_IR_SENSOR       A0
#define DETECTION_THRESHOLD 512    // 아날로그 모드 임계값 (0~1023)
#define ANALOG_MODE         false  // true: 아날로그, false: 디지털
```

**테스트 방법:**
1. 아두이노에 업로드
2. 시리얼 모니터 열기 (9600 baud)
3. 센서 앞에 물체를 가져다 대기
4. LED 켜짐 및 부저 소리 확인

---

### 2️⃣ 컬러 센서 테스트 (02_color_sensor_test)

**기능:**
- TCS34725 RGB 컬러 센서로 색상 측정
- 실시간 RGB 값 출력
- 주요 색상 자동 판별 (빨강, 초록, 파랑, 흰색, 검정)
- 색상별 다른 부저 소리

**핀 연결:**
- `SDA` - 컬러 센서 SDA (I2C)
- `SCL` - 컬러 센서 SCL (I2C)
- `4` - 부저
- `13` - 상태 LED

**주요 설정값:**
```cpp
#define SENSOR_INTEGRATION  TCS34725_INTEGRATIONTIME_50MS
#define SENSOR_GAIN         TCS34725_GAIN_4X
#define MIN_COLOR_SUM       15     // 유효 색상 최소 합계
#define COLOR_DOMINANCE     1.3    // 주요 색상 판별 배수
```

**테스트 방법:**
1. I2C 연결 확인 (SDA, SCL)
2. 아두이노에 업로드
3. 시리얼 모니터 열기 (9600 baud)
4. 다양한 색상의 물체를 센서 앞에 배치
5. RGB 값 및 판별 결과 확인

---

### 3️⃣ RGB LED 색상 표시 테스트 (03_rgb_led_colors_test)

**기능:**
- NeoPixel RGB LED 제어
- 15가지 기본 색상 표시
- 무지개 효과
- 밝기 조절
- 페이드 인/아웃 효과
- 개별 LED 제어

**핀 연결:**
- `5` - NeoPixel 데이터 핀

**주요 설정값:**
```cpp
#define PIN_RGB_LED         5
#define NUM_PIXELS          3      // LED 픽셀 개수
#define LED_MAX_BRIGHTNESS  255    // 최대 밝기
```

**표시 가능한 색상 (15가지):**
1. 빨간색 (Red)
2. 초록색 (Green)
3. 파란색 (Blue)
4. 노란색 (Yellow)
5. 자홍색 (Magenta)
6. 청록색 (Cyan)
7. 흰색 (White)
8. 주황색 (Orange)
9. 보라색 (Purple)
10. 분홍색 (Pink)
11. 진한 초록색 (Dark Green)
12. 진한 파란색 (Navy)
13. 올리브색 (Olive)
14. 금색 (Gold)
15. 은색 (Silver)

**테스트 방법:**
1. NeoPixel LED 연결 확인
2. 아두이노에 업로드
3. 시리얼 모니터 열기 (9600 baud)
4. 자동으로 다양한 색상 및 효과 표시

---

### 4️⃣ DC 모터 제어 테스트 (04_dc_motor_control_test)

**기능:**
- L298N 모터 드라이버를 통한 DC 모터 제어
- 디지털 제어 (ON/OFF)
- PWM 속도 제어 (0~255)
- 방향 전환 (전진/후진)
- 부드러운 가속/감속

**핀 연결:**
- `13` - 모터 방향 제어 (L298N IN3/IN4)
- `11` - 모터 속도 제어 (L298N ENB, PWM)
- `4` - 부저

**주요 설정값:**
```cpp
#define PIN_MOTOR_DIRECTION_B   13
#define PIN_MOTOR_SPEED_B       11

#define MOTOR_SPEED_STOP        0      // 정지
#define MOTOR_SPEED_SLOW        80     // 저속 (31%)
#define MOTOR_SPEED_MEDIUM      160    // 중속 (63%)
#define MOTOR_SPEED_FAST        220    // 고속 (86%)
#define MOTOR_SPEED_MAX         255    // 최대 속도
```

**테스트 시나리오:**
1. 디지털 ON/OFF 제어
2. PWM 속도 단계별 제어
3. 방향 전환 (전진 ↔ 후진)
4. 0에서 최대 속도까지 가속
5. 최대 속도에서 0까지 감속

**테스트 방법:**
1. L298N 모터 드라이버 연결 확인
2. 모터 전원 연결 확인
3. 아두이노에 업로드
4. 시리얼 모니터 열기 (9600 baud)
5. 모터 동작 관찰

---

### 5️⃣ 서보 모터 제어 테스트 (05_servo_motor_control_test)

**기능:**
- 서보 모터 정밀 각도 제어 (0°~180°)
- 기본 각도 이동
- 제품 분류용 특정 각도 설정
- 부드러운 스윕 동작
- Attach/Detach를 통한 전력 관리
- 정밀 제어 (1° 단위)

**핀 연결:**
- `9` - 서보 모터 제어 핀
- `13` - 상태 LED
- `4` - 부저

**주요 설정값:**
```cpp
#define PIN_SERVO_MAIN          9

// 제품 분류용 각도
#define ANGLE_PRODUCT_RED       25     // 빨간색 제품
#define ANGLE_PRODUCT_GREEN     90     // 초록색 제품 (중앙)
#define ANGLE_PRODUCT_BLUE      155    // 파란색 제품

// 이동 속도
#define SERVO_MOVE_DELAY_FAST   5      // 빠른 이동
#define SERVO_MOVE_DELAY_NORMAL 15     // 보통 이동
#define SERVO_MOVE_DELAY_SLOW   30     // 느린 이동
```

**테스트 시나리오:**
1. 기본 각도 이동 (0°, 45°, 90°, 135°, 180°)
2. 제품 분류 각도 테스트
3. 부드러운 스윕 (0°→180°, 180°→0°)
4. Attach/Detach 관리
5. 정밀 각도 제어 (1° 단위)

**테스트 방법:**
1. 서보 모터 연결 확인 (전원, GND, 신호)
2. 아두이노에 업로드
3. 시리얼 모니터 열기 (9600 baud)
4. 서보 모터 각도 변화 관찰

---

### 6️⃣ 통합 컨베이어 시스템 (06_integrated_conveyor_system)

**기능:**
- 모든 모듈을 통합한 완전한 시스템
- 자동 제품 감지 및 색상 분류
- 실시간 상태 모니터링
- 함수화 및 모듈화된 코드 구조

**핀 연결:**
- `A0` - 적외선 센서
- `13` - DC 모터 방향 제어
- `11` - DC 모터 속도 제어 (PWM)
- `9` - 서보 모터
- `5` - RGB LED 스트립
- `4` - 부저
- `SDA/SCL` - 컬러 센서 (I2C)

**동작 시나리오:**

```
[1] 컨베이어 벨트 가동
        ↓
[2] 적외선 센서가 제품 감지
        ↓
[3] 컨베이어 벨트 일시 정지
        ↓
[4] 부저 알림음 (삐삐)
        ↓
[5] 컬러 센서 위치까지 이동
        ↓
[6] 색상 분석 중...
        ↓
[7] 컨베이어 벨트 정지
        ↓
[8] 색상 판별 (빨강/초록/파랑)
        ↓
[9] 서보 모터로 분류 방향 설정
        ↓
[10] RGB LED로 색상 표시
        ↓
[11] 컨베이어 벨트 재가동
        ↓
[12] 제품 분류 완료!
        ↓
[1]로 돌아가서 반복...
```

**주요 개선 사항:**
- ✅ 모든 상수를 `#define`으로 정의
- ✅ 명확한 변수명 및 함수명 사용
- ✅ 기능별 함수 분리 (초기화, 제어, 디버그)
- ✅ 한글 주석 및 상세 설명
- ✅ Early return 패턴 적용
- ✅ 디버그 모드 ON/OFF 가능
- ✅ 에러 처리 강화

**주요 함수 목록:**

**초기화 함수:**
- `initializeSerial()` - 시리얼 통신 초기화
- `initializeMotor()` - DC 모터 초기화
- `initializeServo()` - 서보 모터 초기화
- `initializeIRSensor()` - 적외선 센서 초기화
- `initializeColorSensor()` - 컬러 센서 초기화
- `initializeLED()` - RGB LED 초기화
- `initializeBuzzer()` - 부저 초기화

**주요 기능 함수:**
- `isProductDetected()` - 제품 감지 확인
- `processDetectedProduct()` - 감지된 제품 처리
- `waitForColorDetection()` - 색상 감지 대기
- `identifyColor()` - 색상 판별
- `setSortingDirection()` - 분류 방향 설정
- `displayColorOnLED()` - LED에 색상 표시

**모터 제어 함수:**
- `startConveyor()` - 컨베이어 시작
- `stopConveyor()` - 컨베이어 정지
- `detachServo()` - 서보 분리

**부저 제어 함수:**
- `playStartupSound()` - 시작 알림음
- `playDetectionSound()` - 감지 알림음

**디버그 함수:**
- `printStartupMessage()` - 시작 메시지
- `printColorData()` - 색상 데이터 출력
- `debugPrint()` - 디버그 메시지 출력

**테스트 방법:**
1. 모든 하드웨어 연결 확인
2. 아두이노에 업로드
3. 시리얼 모니터 열기 (9600 baud)
4. 컨베이어 벨트에 색상 물체 배치
5. 자동 분류 과정 관찰

---

### 7️⃣ Serial 제어 시스템 (07_serial_controlled_system)

**기능:**
- Serial 명령으로 자동화 시작/중지
- 실시간 stop 명령 반응 (제품 처리 중에도 50ms 이내 중지)
- 제품 감지 및 색상 분류
- 명령어 기반 제어 시스템

**핀 연결:**
- 06번과 동일

**명령어:**
- `start_` : 자동화 시작
- `stop_` : 자동화 중지

**주요 설정값:**
```cpp
#define MOTOR_SPEED         120    // 컨베이어 속도
#define DELAY_IR_DETECT     2000   // 적외선 감지 후 대기
#define DELAY_COLOR_DETECT  1500   // 색상 분석 후 대기
#define DELAY_NEXT_PRODUCT  1000   // 다음 제품 대기
```

**주요 함수:**
- `checkSerialCommand()` - Serial 명령 확인
- `processCommand()` - 명령 처리
- `delayWithSerialCheck()` - delay 중에도 명령 체크

**특징:**
- ✅ 제품 처리 중에도 즉시 중지 가능
- ✅ `_` 문자를 구분자로 사용
- ✅ 디버깅에 최적화된 구조

**테스트 방법:**
1. Serial Monitor 열기 (9600 baud)
2. `start_` 입력 → 자동화 시작
3. 제품 처리 중 `stop_` 입력 → 즉시 중지
4. 다시 `start_` 입력 → 재시작

---

### 8️⃣ Serial + Bluetooth 이중 제어 시스템 (08_dual_comm_system)

**기능:**
- Serial과 Bluetooth 동시 지원
- 색상별 제품 카운터 (빨강, 초록, 파랑)
- 카운터 정보 실시간 전송
- 카운터 초기화 기능
- 디버깅용 이중 통신

**핀 연결:**
- 06번과 동일 + 블루투스 모듈 추가
- `D2` - Bluetooth TX/RX

**명령어:**
- `start_` : 자동화 시작
- `stop_` : 자동화 중지
- `init_` : 카운터 초기화 (빨강, 초록, 파랑 모두 0으로) + **자동으로 시스템 중지**

**카운터 전송 형식:**
- 빨간색 제품 처리 시: `red1_`, `red2_`, `red3_`, ...
- 초록색 제품 처리 시: `green1_`, `green2_`, `green3_`, ...
- 파란색 제품 처리 시: `blue1_`, `blue2_`, `blue3_`, ...

**주요 설정값:**
```cpp
#define PIN_BT_TX           2      // Bluetooth TX
#define PIN_BT_RX           2      // Bluetooth RX

int redCount = 0;                  // 빨간색 카운터
int greenCount = 0;                // 초록색 카운터
int blueCount = 0;                 // 파란색 카운터
```

**주요 함수:**
- `checkSerialCommand()` - Serial 명령 확인
- `checkBluetoothCommand()` - Bluetooth 명령 확인
- `processCommand()` - 통합 명령 처리
- `printToBoth()` - Serial과 Bluetooth 동시 출력
- `delayWithCommandCheck()` - 두 통신 모두 체크하는 delay

**특징:**
- ✅ Serial 또는 Bluetooth 중 하나만 연결해도 동작
- ✅ 두 통신으로 동시에 모니터링 가능
- ✅ 색상별 제품 카운팅
- ✅ 카운터 정보 실시간 전송
- ✅ 제품 처리 중에도 즉시 중지 가능
- ✅ 디버깅에 최적화

**동작 예시:**

```
[명령 수신 from Serial] start
========================================
  자동화 시작
========================================
컨베이어 가동 시작

========================================
제품 #1 감지됨!
========================================
--- 색상 분석 결과 ---
Raw -> R: 330, G: 121, B: 116
RGB -> R: 15, G: 5, B: 5
판별 색상: 빨간색
---------------------
[카운터 전송] red1_
제품 처리 완료

========================================
제품 #2 감지됨!
========================================
--- 색상 분석 결과 ---
Raw -> R: 77, G: 238, B: 96
RGB -> R: 3, G: 11, B: 4
판별 색상: 초록색
---------------------
[카운터 전송] green1_
제품 처리 완료
```

**테스트 방법:**
1. Serial 또는 Bluetooth 연결 (둘 다 가능)
2. `start_` 입력 → 자동화 시작
3. 제품 배치 → 색상별 카운터 확인
4. `init_` 입력 → 카운터 초기화 + 시스템 중지
5. `start_` 입력 → 다시 시작 (카운터는 0부터)
6. `stop_` 입력 → 자동화만 중지 (카운터 유지)

**상세 가이드:**
`08_dual_comm_system/README.md` 참조

---

## 📌 하드웨어 요구사항

### 필수 부품:
1. **아두이노 보드** (Uno, Mega 등)
2. **L298N 모터 드라이버**
3. **DC 기어 모터** (컨베이어 벨트용)
4. **서보 모터** (SG90 또는 MG90S)
5. **적외선 센서** (IR Obstacle Sensor)
6. **TCS34725 컬러 센서**
7. **NeoPixel RGB LED 스트립** (WS2812B, 3개 이상)
8. **부저** (Passive Buzzer)
9. **전원 공급** (5V, 12V)
10. **점퍼 와이어 및 브레드보드**
11. **블루투스 모듈** (HC-05 또는 HC-06) - 08번 시스템용 (선택사항)

### 라이브러리 요구사항:
- `Adafruit_NeoPixel` - RGB LED 제어
- `Adafruit_TCS34725` - 컬러 센서 제어
- `Servo` - 서보 모터 제어 (아두이노 기본 라이브러리)
- `Wire` - I2C 통신 (아두이노 기본 라이브러리)
- `SoftwareSerial` - 블루투스 통신 (아두이노 기본 라이브러리) - 08번용

**라이브러리 설치 방법:**
1. 아두이노 IDE 실행
2. `스케치` → `라이브러리 포함하기` → `라이브러리 관리...`
3. 검색: "Adafruit NeoPixel" → 설치
4. 검색: "Adafruit TCS34725" → 설치

---

## 🔌 핀 연결 요약표

| 부품 | 핀 번호 | 아두이노 핀 | 비고 |
|------|---------|-------------|------|
| **DC 모터** | IN3/IN4 | 13 | 방향 제어 (디지털) |
| **DC 모터** | ENB | 11 | 속도 제어 (PWM) |
| **서보 모터** | 신호선 | 9 | PWM 핀 |
| **RGB LED** | DIN | 5 | 데이터 핀 |
| **적외선 센서** | OUT | A0 | 아날로그/디지털 입력 |
| **컬러 센서** | SDA | A4 (SDA) | I2C 데이터 |
| **컬러 센서** | SCL | A5 (SCL) | I2C 클럭 |
| **부저** | + | 4 | 디지털 출력 |
| **블루투스** | TXD/RXD | 2 | 08번용 (선택) |

---

## 🚀 사용 방법

### 단계별 학습 순서:

1. **기초 모듈 테스트** (01~05)
   - 각 센서/액츄에이터를 개별적으로 테스트
   - 동작 원리 및 제어 방법 학습
   - 핀 연결 및 설정값 확인

2. **통합 시스템 실행** (06)
   - 모든 모듈을 통합한 완전한 시스템
   - 실제 컨베이어 벨트 자동화 구현
   - 실시간 모니터링 및 디버깅

3. **Serial 제어 시스템** (07)
   - Serial 명령어로 시작/중지 제어
   - 실시간 명령 반응
   - 디버깅 최적화

4. **이중 통신 시스템** (08) - 고급
   - Serial + Bluetooth 동시 지원
   - 색상별 카운터 기능
   - 실시간 데이터 전송
   - 디버깅 및 모니터링 최적화

### 커스터마이징:

**속도 조절:**
```cpp
#define MOTOR_SPEED_NORMAL      120    // 값을 변경 (0~255)
```

**서보 각도 조절:**
```cpp
#define SERVO_ANGLE_RED         25     // 빨간색 분류 각도
#define SERVO_ANGLE_GREEN       57     // 초록색 분류 각도
#define SERVO_ANGLE_BLUE        2      // 파란색 분류 각도
```

**색상 감지 민감도:**
```cpp
#define COLOR_DETECT_THRESHOLD  15     // 값을 낮추면 더 민감
```

**디버그 모드:**
```cpp
#define DEBUG_ENABLED           true   // false로 변경 시 디버그 출력 비활성화
```

---

## 🐛 문제 해결 (Troubleshooting)

### 1. 컬러 센서가 인식되지 않음
- I2C 연결 확인 (SDA, SCL)
- 전원 연결 확인 (VCC, GND)
- 시리얼 모니터에서 오류 메시지 확인

### 2. DC 모터가 작동하지 않음
- L298N 전원 연결 확인 (12V)
- 점퍼 캡 위치 확인
- 방향 핀 및 속도 핀 연결 확인

### 3. RGB LED가 켜지지 않음
- 전원 연결 확인 (5V)
- 데이터 핀 연결 확인
- NeoPixel 개수 설정 확인

### 4. 서보 모터가 떨림
- 전원 용량 확인 (별도 5V 전원 권장)
- Detach 시간 조절
- 부하 감소

### 5. 적외선 센서 오감지
- 임계값 조절 (`DETECTION_THRESHOLD`)
- 센서 거리 조절
- 주변 광량 확인

---

## 📖 참고 자료

- [Adafruit NeoPixel 라이브러리](https://github.com/adafruit/Adafruit_NeoPixel)
- [TCS34725 센서 데이터시트](https://www.adafruit.com/product/1334)
- [L298N 모터 드라이버 사용법](https://lastminuteengineers.com/l298n-dc-stepper-driver-arduino-tutorial/)
- [아두이노 Servo 라이브러리](https://www.arduino.cc/reference/en/libraries/servo/)

---

## 📝 라이선스

이 프로젝트는 교육 목적으로 제작되었습니다.

---

## 👨‍💻 작성자

스마트 팩토리 교육 프로젝트 팀

**최종 업데이트:** 2026년 1월
