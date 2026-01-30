# 디스플레이 종합 가이드

## 📌 개요
스마트 팩토리에서 활용 가능한 다양한 디스플레이의 종합 가이드입니다.

---

## 📁 디스플레이 분류

### 1. 숫자 디스플레이
- **13. 7 Segment**: 숫자, 시간 표시 (TM1637)
- **02. 8x8 LED Matrix**: 숫자, 아이콘 (MAX7219)

### 2. 텍스트 디스플레이
- **03. LCD1602**: 16x2 문자 (I2C)
- **15. LCD1602 고급**: 커스텀 문자, 진행률 바

### 3. 그래픽 디스플레이
- **14. OLED (SSD1306)**: 128x64 픽셀, 그래픽
- **16. 8x8 Matrix 고급**: 애니메이션, 게임

---

## 🎯 디스플레이 특성 비교

### 전체 비교표

| 디스플레이 | 해상도 | 통신 | 핀 수 | 색상 | 가격 | 소비 전력 |
|----------|--------|------|------|------|------|----------|
| **7 Segment** | 4자리 | I2C | 2 | 빨강/초록 | 저렴 | 낮음 |
| **8x8 Matrix** | 8x8 픽셀 | SPI | 3 | 빨강 | 저렴 | 낮음 |
| **LCD1602** | 16x2 문자 | I2C | 2 | 파랑/초록 | 저렴 | 중간 |
| **OLED** | 128x64 픽셀 | I2C | 2 | 흰색/파랑 | 중간 | 낮음 |

### 상세 비교

#### 1. 해상도 및 표시 능력
| 디스플레이 | 숫자 | 문자 | 그래픽 | 애니메이션 |
|----------|------|------|--------|----------|
| 7 Segment | ⭐⭐⭐ | ❌ | ❌ | ⭐ |
| 8x8 Matrix | ⭐⭐ | ⭐ | ⭐⭐ | ⭐⭐⭐ |
| LCD1602 | ⭐⭐ | ⭐⭐⭐ | ⭐ | ⭐⭐ |
| OLED | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |

#### 2. 사용 편의성
| 디스플레이 | 배선 | 라이브러리 | 코딩 난이도 |
|----------|------|-----------|-----------|
| 7 Segment | 쉬움 (2핀) | 쉬움 | 쉬움 |
| 8x8 Matrix | 쉬움 (3핀) | 보통 | 보통 |
| LCD1602 | 쉬움 (2핀) | 쉬움 | 쉬움 |
| OLED | 쉬움 (2핀) | 보통 | 보통 |

---

## 📊 용도별 디스플레이 선택

### 1. 숫자만 표시 (시계, 카운터)
```
추천: 7 Segment (TM1637)

장점:
- 깔끔한 숫자 표시
- 높은 가독성
- 밝기 조절 가능
- 콜론 표시 (시간용)

예시:
- 디지털 시계
- 타이머/카운터
- 온도계
```

### 2. 간단한 텍스트 (센서 값, 상태)
```
추천: LCD1602 (I2C)

장점:
- 2줄 16자 표시
- 백라이트 가능
- 저렴한 가격
- 커스텀 문자

예시:
- 센서 모니터링
- 메뉴 시스템
- 상태 표시
```

### 3. 그래픽 + 텍스트
```
추천: OLED (SSD1306)

장점:
- 고해상도 (128x64)
- 그래픽 가능
- 선명한 표시
- 저전력

예시:
- 센서 그래프
- 복잡한 UI
- 이미지 표시
- 데이터 시각화
```

### 4. 애니메이션 + 효과
```
추천: 8x8 LED Matrix (MAX7219)

장점:
- 간단한 애니메이션
- 다중 연결 가능
- 스크롤 텍스트
- 게임 구현

예시:
- 메시지 보드
- 상태 아이콘
- 간단한 게임
- 알림 효과
```

---

## 💻 통신 프로토콜 비교

### I2C (2핀: SDA, SCL)
**사용 디스플레이**: LCD1602, OLED

**장점**:
- 2핀만 사용 (핀 절약)
- 여러 장치 연결 가능 (주소로 구분)
- 배선 간단

**단점**:
- 속도 느림 (약 100~400kHz)
- 긴 배선 시 불안정

**핀 연결**:
```
Arduino UNO
├─ A4 → SDA
└─ A5 → SCL
```

### SPI (3핀: MOSI, SCK, SS)
**사용 디스플레이**: 8x8 Matrix

**장점**:
- 빠른 속도 (MHz 단위)
- 안정적인 통신
- 장거리 전송 가능

**단점**:
- 3핀 사용
- 장치마다 SS 핀 추가 필요

**핀 연결**:
```
Arduino UNO
├─ D11 → MOSI (DIN)
├─ D13 → SCK (CLK)
└─ D10 → SS (CS)
```

### 전용 통신 (TM1637)
**사용 디스플레이**: 7 Segment

**장점**:
- 2핀만 사용
- 간단한 프로토콜
- 밝기 제어 내장

**단점**:
- 전용 라이브러리 필요

---

## 🔧 실전 프로젝트 예시

### 1. 온습도 모니터링 시스템
```
LCD1602 (I2C) + DHT 센서

첫 번째 줄: 🌡️ 25°C
두 번째 줄: 💧 60%

특징:
- 커스텀 아이콘 (온도계, 물방울)
- 실시간 업데이트
- 백라이트 자동 제어
```

### 2. 센서 데이터 시각화
```
OLED (SSD1306) + 초음파 센서

상단: 거리 수치
하단: 실시간 그래프

특징:
- 그래픽 차트
- 트렌드 확인 가능
- 다중 센서 대시보드
```

### 3. 디지털 시계
```
7 Segment (TM1637) + RTC 모듈

표시: 12:34

특징:
- 깔끔한 숫자
- 콜론 깜빡임
- 높은 가독성
```

### 4. 메시지 보드
```
8x8 Matrix (MAX7219) x 4개

스크롤 텍스트: "Smart Factory →"

특징:
- 긴 메시지 표시
- 스크롤 효과
- 애니메이션 아이콘
```

---

## 📝 코딩 가이드

### 1. 7 Segment 기본
```cpp
#include <TM1637Display.h>

TM1637Display display(CLK, DIO);

void setup() {
  display.setBrightness(5);
}

void loop() {
  display.showNumberDec(1234);
  delay(1000);
}
```

### 2. LCD1602 기본
```cpp
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello World!");
}
```

### 3. OLED 기본
```cpp
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hello World!");
  display.display();
}
```

### 4. 8x8 Matrix 기본
```cpp
#include <LedControl.h>

LedControl lc = LedControl(11, 13, 10, 1);

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}

void loop() {
  // 하트 그리기
  byte heart[8] = {0x00, 0x66, 0xFF, 0xFF, 
                   0xFF, 0x7E, 0x3C, 0x18};
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, heart[i]);
  }
}
```

---

## 🎓 고급 기법

### 1. 다중 디스플레이 통합
```cpp
// I2C 주소 분리
LCD1602 lcd1(0x27);  // 첫 번째 LCD
LCD1602 lcd2(0x3F);  // 두 번째 LCD

// 역할 분담
lcd1: 센서 데이터
lcd2: 시스템 상태
```

### 2. 커스텀 문자 (LCD)
```cpp
byte degree[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

lcd.createChar(0, degree);
lcd.write(0);  // ° 표시
```

### 3. 그래픽 애니메이션 (OLED)
```cpp
for (int r = 0; r < 32; r++) {
  display.clearDisplay();
  display.drawCircle(64, 32, r, WHITE);
  display.display();
  delay(50);
}
```

### 4. 스크롤 텍스트 (Matrix)
```cpp
// 비트 시프트로 좌 스크롤
for (int offset = 0; offset < 64; offset++) {
  // 각 행을 왼쪽으로 시프트
  for (int row = 0; row < 8; row++) {
    byte pattern = ...; // 시프트된 패턴
    lc.setRow(0, row, pattern);
  }
  delay(100);
}
```

---

## 🔍 문제 해결

### 1. LCD/OLED가 작동하지 않을 때
```
문제: I2C 주소 불일치

해결:
1. I2C 스캐너 코드 실행
2. Serial Monitor에서 주소 확인
3. 코드에 올바른 주소 입력

일반적 주소:
- LCD1602: 0x27 또는 0x3F
- OLED: 0x3C 또는 0x3D
```

### 2. 7 Segment 숫자가 이상할 때
```
문제: 배선 또는 라이브러리

해결:
1. CLK, DIO 핀 확인
2. 전원 5V, GND 확인
3. TM1637Display 라이브러리 설치
4. 밝기 조절: setBrightness(0~15)
```

### 3. 8x8 Matrix 패턴이 뒤집힐 때
```
문제: 행/열 순서

해결:
1. setRow() vs setColumn() 확인
2. 패턴 데이터 좌우 반전
3. 모듈 회전 (물리적으로 90도)
```

---

## 💡 선택 가이드 체크리스트

### 프로젝트 요구사항
- [ ] 숫자만 표시하는가?
- [ ] 텍스트가 필요한가?
- [ ] 그래픽이 필요한가?
- [ ] 애니메이션이 필요한가?
- [ ] 실시간 업데이트 빈도는?
- [ ] 가독성 요구사항은?
- [ ] 전력 제약이 있는가?
- [ ] 예산 제약은?

### 환경 고려사항
- [ ] 실내/실외 사용?
- [ ] 밝은 곳/어두운 곳?
- [ ] 햇빛 직사광선?
- [ ] 시야각 요구사항?

---

## 📚 라이브러리 정리

### 필수 라이브러리
```
7 Segment:   TM1637Display
LCD1602:     LiquidCrystal_I2C
OLED:        Adafruit_GFX, Adafruit_SSD1306
8x8 Matrix:  LedControl
```

### 설치 방법
```
Arduino IDE
→ 스케치
→ 라이브러리 포함하기
→ 라이브러리 관리
→ 검색 후 설치
```

---

**Happy Displaying! 🖥️**
