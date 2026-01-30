# 스마트 물류 팩토리 28시간 교육 프로젝트

> **"알고리즘으로 생각하고, 시스템으로 구현하고, 비즈니스로 완성한다"**  
> 피지컬 컴퓨팅 + AI를 바탕으로 물류 팩토리 시스템을 만들고 전시하는 알고리즘 중심 교육

---

## 🎯 프로젝트 개요

**대상**: 대학생 42명 (21팀, 2인 1조)  
**기간**: **28시간** (4일 × 7시간)  
**교구재**: [BP Lab AI 로봇팔 4축](https://bplab.kr/order/?idx=178) + 컨베이어 벨트  
**목표**: 알고리즘 설계 → 피지컬 AI 통합 → 스마트 물류 시스템 완성

---

## 🚀 핵심 특징

### 1. 알고리즘 중심 교육

```mermaid
graph LR
    A[문제 정의] --> B[플로우차트<br/>설계]
    B --> C[의사코드<br/>작성]
    C --> D[하드웨어<br/>구현]
    D --> E[성능<br/>최적화]
    E --> F[비즈니스<br/>가치화]
    
    style B fill:#FFD93D,color:#111
    style D fill:#6BCB77,color:#fff
```

- **플로우차트 우선**: 코드 작성 전 알고리즘 설계
- **의사코드 활용**: 논리 검증 후 구현
- **복잡도 분석**: 시간/공간 복잡도 측정
- **최적화**: 병목 지점 찾아서 개선

### 2. 피지컬 AI 통합 (3일 완성)

| 구성 요소 | 역할 | 핵심 알고리즘 | 학습 Day |
|----------|------|--------------|---------|
| **4축 로봇팔** | 물체 자동 줍기 | EEPROM 저장/복구, play_ 명령 실행 | Day 1 |
| **컨베이어 벨트** | 색상 자동 분류 | 색상 판별, 실시간 명령 체크 | Day 2 |
| **센서 시스템** | 제품 카운팅 | count_ 데이터 생성 및 전송 | Day 2 |
| **통신 모듈** | 이중 통신 | Bluetooth 버퍼링, Serial 동시 처리 | Day 1-2 |
| **스마트폰 AI** | 물체 인식 | Teachable Machine → play_ 전송 | Day 3 |
| **8x8 LED Matrix** | 상태 시각화 | 화살표 방향, 센서 상태 표시 | Day 3 |
| **앱인벤터** | 재고 관리 | count_ 파싱, 차트 시각화 | Day 3 |

**완전 자동화 플로우**:
```
RFID 인증 → 카메라 촬영 → AI 품질 판정 → 명령어 전송 → 
로봇팔 구역 이동 → LCD 상태 표시 → LED 품질 등급 표시 → 
컨베이어 분류 (선택) → 대시보드 통계 업데이트
```

### 3. AI 기반 스마트 물류 시스템 (Day 3)

```mermaid
sequenceDiagram
    participant App as 앱인벤터
    participant TM as Teachable Machine
    participant Arm as 로봇팔(Day1)
    participant Conv as 컨베이어(Day2)
    participant LED as 8x8 LED
    
    App->>TM: 카메라로 물체 촬영
    TM->>App: 물체 인식 (빨강/파랑/노랑)
    App->>Arm: play_ 명령 전송
    Arm->>Arm: 자동 줍기 시작
    Arm->>LED: 화살표 방향 표시
    Arm->>Conv: 물체를 컨베이어에 배치
    Conv->>Conv: 색상 분류 (08_dual_comm)
    Conv->>App: count_ 데이터 전송
    App->>App: 차트 업데이트 (재고 관리)
```

**핵심 통합**:
- Day 1 구역별 제어 + Day 2 자동 분류 + Day 3 AI 품질 검사 = **완전 자동화**
- RFID 인증 → AI 품질 판정 → 로봇팔 구역 이동 → LCD/LED 시각화 → 품질별 통계

### 4. 품질 관리 시스템 (Day 2-3)

**Day 2: 데이터 수집 및 모니터링**
- **이중 통신**: Serial + Bluetooth 동시 처리 (50ms 주기)
- **제품 카운팅**: 색상별 실시간 카운터 (red1_, green2_, blue3_)
- **데이터 전송**: count_ 명령으로 앱에 전송
- **모니터링 대시보드**: 일일량, 통합량, 차트

**Day 3: AI 품질 검사**
- **품질 분류 시스템**: 
  - GREEN (정상): 즉시 포장 출하 → 전방 우측
  - RED (불량): 폐기/재활용 → 전방 좌측
  - YELLOW (재검사): 인간 재검수 → 후방 우측
  - BLUE (특수): VIP 포장 → 후방 좌측
- **통계 분석**:
  - 품질별 처리 개수
  - 불량률 계산
  - 재검사율 모니터링
  - 평균 처리 속도

**비즈니스 가치**:
- 자동 품질 검사로 인건비 절감
- 실시간 불량률 파악
- 품질별 수요 분석
- 생산 효율성 측정
- VIP 제품 특별 관리

---

## 📚 교육 커리큘럼

### Day 1: 로봇팔 피지컬 컴퓨팅 (8시간)

**학습 과정**: 하드웨어 조립 → 6단계 학습 → STT/TTS 음성 제어

| 교시 | 시간 | 활동 | 학습 내용 | 산출물 |
|------|------|------|----------|--------|
| **1교시** | 1h | 하드웨어 조립 | 4축 로봇팔 조립 + 배선 | 작동하는 로봇팔 |
| **2교시** | 1h | 01~02단계 | Servo 테스트 + 조이스틱 | 실시간 제어 |
| **3교시** | 1h | 03단계 | Serial 원격 제어 | 명령 파싱 |
| **4교시** | 1.5h | 04단계 ⭐ | EEPROM 자동화 | 저장/재생 |
| **5교시** | 1.5h | 05~06단계 ⭐⭐ | Bluetooth 무선 제어 | 완성된 로봇팔 |
| **6-7교시** | 2h | 음성 제어 앱 🎤 | STT/TTS 앱인벤터 | 음성 제어 완성 |

**핵심 알고리즘**:
1. EEPROM 저장/복구 (O(1))
2. 자동 재생/무한 반복 (O(n×m))
3. Bluetooth 명령 버퍼링 (O(n))

**결과물**: ✅ 무선 제어 로봇팔 (~1,340줄) + 음성 제어 앱 (STT/TTS)

### Day 2: 컨베이어 자동 분류 시스템 (8시간)

**학습 과정**: 외형 제작 → 핀아웃 연결 → 개별 테스트 → 통합 → 원격제어 → 대시보드

| 교시 | 시간 | 활동 | 학습 내용 | 산출물 |
|------|------|------|----------|--------|
| **1교시** | 1h | 🔧 외형 제작 | 컨베이어 조립 + 센서 배치 | 완성된 하드웨어 |
| **2교시** | 1h | 📌 핀아웃 연결 | 12개 핀 배선 + 체크리스트 | 배선 완료 |
| **3교시** | 1.5h | 🧪 개별 테스트 | 센서 5종 × 모터 2종 테스트 | 각 부품 동작 확인 |
| **4교시** | 1.5h | 🔄 통합 자동화 | 전체 시스템 통합 (자동 분류) | 자동 분류 시스템 |
| **5교시** | 1h | 💻 Serial 제어 | PC → 아두이노 실시간 명령 | start/stop/init |
| **6교시** | 1h | 📡 Bluetooth 제어 | 스마트폰 → 아두이노 무선 제어 | 원격 제어 |
| **7교시** | 1h | 📊 대시보드 앱 | 일일량, 통합량, 차트 (앱인벤터) | 실시간 모니터링 앱 |

**핵심 알고리즘**:
4. 색상 판별 (최대값 비교 + 유클리드 거리, O(1))
5. 실시간 명령 체크 (비차단, O(n))
6. 제품 카운터 및 전송 (O(1))
7. 통합 자동 분류 (상태 머신, O(1))

**결과물**: ✅ 완전 자동화 컨베이어 (~2,188줄) + 원격 모니터링 대시보드 앱

**상세**: [arduino_code/README.md](arduino_code/README.md) | [알고리즘_설명서.md](arduino_code/알고리즘_설명서.md)

### Day 3: AI 품질 검사 스마트 팩토리 (8시간)

**학습 과정**: AI 모델 학습 → 명령어 프로토콜 → 구역별 동작 → 하드웨어 통합 → 스토리 구성

| 교시 | 시간 | 활동 | 학습 내용 | 산출물 |
|------|------|------|----------|--------|
| **1교시** | 1h | Teachable Machine | 4가지 클래스 학습 (GREEN/RED/YELLOW/BLUE) | AI 모델 |
| **2교시** | 1h | 명령어 프로토콜 설계 | `green_ok`, `red_defect` 등 정의 | 프로토콜 문서 |
| **3교시** | 1h | 앱인벤터 AI 연동 | TMIC Extension + 명령 전송 | AI 제어 앱 |
| **4교시** | 1h | 아두이노 구역별 동작 | 각도 배열 4~5개 구역 이동 코드 | 구역 제어 코드 |
| **5교시** | 1h | 하드웨어 통합 | LCD1602 + LED Matrix + RFID + 버튼 | 통합 시스템 |
| **6교시** | 1h | 스토리 시나리오 구성 | 품질 검사 라인 시나리오 작성 | 스토리보드 |
| **7교시** | 1h | 시스템 통합 테스트 | 전체 하드웨어 + AI 연동 | 완성 시스템 |
| **8교시** | 1h | 확장 센서 실습 | 추가 센서/모터 통합 방법 | 확장 가이드 |

**품질 분류 시스템**:
- **GREEN (정상)**: 전방 우측 구역 → 즉시 포장 출하
- **RED (불량)**: 전방 좌측 구역 → 폐기/재활용
- **YELLOW (재검사)**: 후방 우측 구역 → 인간 재검수
- **BLUE (특수)**: 후방 좌측 구역 → VIP 포장

**하드웨어 통합**:
- RFID RC522: 출입 인증
- LCD1602 (I2C): 상태 표시
- 8x8 LED Matrix: 품질 등급 시각화
- 긴급정지 버튼: 안전 제어

**결과물**: ✅ AI 품질 검사 스마트 팩토리 완전체 (Day 1 + Day 2 + Day 3 통합)

### Day 4: 제품 시연회 (4시간)

**발표 형식**: 3-4분 기술 발표 + 1-2분 제품 시연 = 팀당 5-6분

| 교시 | 시간 | 활동 | 내용 | 비고 |
|------|------|------|------|------|
| **1교시** | 1h | 최종 점검 | 시스템 테스트 + 백업 | 팀별 진행 |
| **2교시** | 1h | 리허설 | 스토리 발표 + 시연 연습 | 팀당 5-6분 |
| **3-4교시** | 2h | 발표 시연 | 본 발표 (21팀) | 팀당 5-6분 |

**발표 구성**:
- **기술 발표 (3-4분)**: 필요성 → 문제 해결 과정 → 개선 사항
- **제품 시연 (1-2분)**: 라이브 데모 (AI 인식 → 로봇팔 → 분류)

**결과물**: ✅ 포트폴리오 완성

**상세**: [curriculum/README.md](curriculum/README.md)

---

## 🧠 학습 알고리즘 (Day별 구현)

### Day 1: 로봇팔 제어 알고리즘 (3가지)

<table>
<tr>
<th>알고리즘</th>
<th>시간 복잡도</th>
<th>난이도</th>
<th>활용도</th>
<th>구현 파일</th>
</tr>
<tr>
<td><b>1. EEPROM 저장/복구</b></td>
<td>O(1)</td>
<td>⭐⭐⭐</td>
<td>⭐⭐⭐⭐</td>
<td>04_eeprom_automation</td>
</tr>
<tr>
<td><b>2. 자동 재생/무한 반복</b></td>
<td>O(n×m)</td>
<td>⭐⭐⭐</td>
<td>⭐⭐⭐⭐⭐</td>
<td>04_eeprom_automation</td>
</tr>
<tr>
<td><b>3. Bluetooth 명령 버퍼링</b></td>
<td>O(n)</td>
<td>⭐⭐⭐⭐</td>
<td>⭐⭐⭐⭐⭐</td>
<td>05_bluetooth_eeprom</td>
</tr>
</table>

### Day 2: 센서 및 통합 알고리즘 (4가지)

<table>
<tr>
<th>알고리즘</th>
<th>시간 복잡도</th>
<th>난이도</th>
<th>활용도</th>
<th>구현 파일</th>
</tr>
<tr>
<td><b>4. 색상 판별 (비율 기반)</b></td>
<td>O(1)</td>
<td>⭐⭐⭐</td>
<td>⭐⭐⭐⭐⭐</td>
<td>02_color_sensor_test</td>
</tr>
<tr>
<td><b>5. 실시간 명령 체크 (비차단)</b></td>
<td>O(n)</td>
<td>⭐⭐⭐⭐</td>
<td>⭐⭐⭐⭐⭐</td>
<td>07_serial_controlled</td>
</tr>
<tr>
<td><b>6. 제품 카운터 및 전송</b></td>
<td>O(1)</td>
<td>⭐⭐⭐</td>
<td>⭐⭐⭐⭐</td>
<td>08_dual_comm_system</td>
</tr>
<tr>
<td><b>7. 통합 자동 분류</b></td>
<td>O(1)</td>
<td>⭐⭐⭐⭐</td>
<td>⭐⭐⭐⭐⭐</td>
<td>06_integrated_system</td>
</tr>
</table>

### Day 3: AI 품질 검사 시스템

<table>
<tr>
<th>구성 요소</th>
<th>기능</th>
<th>난이도</th>
<th>활용도</th>
<th>비고</th>
</tr>
<tr>
<td><b>8. AI 품질 분류</b></td>
<td>4가지 품질 등급 인식</td>
<td>⭐⭐⭐</td>
<td>⭐⭐⭐⭐⭐</td>
<td>GREEN/RED/YELLOW/BLUE</td>
</tr>
<tr>
<td><b>9. 명령어 프로토콜</b></td>
<td>품질별 명령 전송</td>
<td>⭐⭐⭐</td>
<td>⭐⭐⭐⭐⭐</td>
<td>green_ok, red_defect 등</td>
</tr>
<tr>
<td><b>10. 구역별 동작</b></td>
<td>4~5개 구역 각도 배열</td>
<td>⭐⭐⭐⭐</td>
<td>⭐⭐⭐⭐⭐</td>
<td>Day1 + Day2 통합</td>
</tr>
<tr>
<td><b>11. RFID 인증</b></td>
<td>출입 권한 관리</td>
<td>⭐⭐</td>
<td>⭐⭐⭐⭐</td>
<td>RC522 SPI 통신</td>
</tr>
<tr>
<td><b>12. LCD/LED 표시</b></td>
<td>실시간 상태 시각화</td>
<td>⭐⭐</td>
<td>⭐⭐⭐⭐</td>
<td>I2C LCD + LED Matrix</td>
</tr>
</table>

**상세 알고리즘 설명**: [알고리즘_설명서.md](arduino_code/알고리즘_설명서.md)

---

## 🔧 하드웨어 사양

### Day 1: 로봇팔 시스템

**주요 부품**:
- **4축 로봇팔**: [BP Lab AI 로봇팔](https://bplab.kr/order/?idx=178) (SG90 서보 4개)
- **조이스틱**: 아날로그 조이스틱 2개 (실시간 제어용)
- **통신**: HC-05/HC-06 블루투스 모듈
- **제어**: 아두이노 우노 호환보드
- **전원**: 5V 외부 전원 (서보 안정 동작)

**서보 각도 범위**:
| 축 | 기능 | 각도 범위 | 용도 |
|----|------|----------|------|
| **1축 (베이스)** | 좌우 회전 | 50-130° | 구역 선택 |
| **2축 (팔1)** | 상하 움직임 | 70-120° | 높이 제어 |
| **3축 (팔2)** | 팔 굽힘 | 60-120° | 위치 조정 |
| **4축 (그립)** | 집기/놓기 | 5-50° | 물체 파지 |

### Day 2: 컨베이어 시스템

**주요 부품**:
- **컬러 센서**: TCS34725 (I2C, RGB 분석)
- **IR 센서**: 적외선 장애물 센서 (물체 감지)
- **DC 모터**: 12V 기어 모터 + L298N 드라이버
- **서보 모터**: SG90 (분류 게이트)
- **RGB LED**: NeoPixel WS2812B 3개 이상
- **부저**: Passive Buzzer (알림음)
- **통신**: HC-05/HC-06 블루투스 모듈
- **제어**: 아두이노 우노/메가
- **전원**: 5V, 12V

### Day 3: AI 품질 검사 통합 시스템 (Day 1 + Day 2 + 추가 부품)

**기존 시스템**:
- Day 1: 로봇팔 시스템 (구역별 이동 완성)
- Day 2: 컨베이어 시스템 (자동 분류 + 모니터링 완성)

**추가 부품**:
- **RFID RC522**: 출입 인증 시스템
  - SPI 통신 (MOSI, MISO, SCK, SS)
  - 카드/태그 인식
  - 권한별 접근 제어
- **LCD1602 (I2C)**: 실시간 상태 표시
  - 16x2 문자 디스플레이
  - 품질 등급 표시
  - 처리 개수 카운팅
- **8x8 LED Matrix**: MAX7219 제어
  - 품질 등급 아이콘 (✓, ✗, ?, ★)
  - 상태 애니메이션
  - 동작 상태 시각화
- **긴급정지 버튼**: 안전 제어
  - Pull-up 저항 방식
  - 즉시 정지 기능
- **스마트폰**: Teachable Machine AI
  - 카메라 품질 검사 (GREEN/RED/YELLOW/BLUE)
  - 신뢰도 표시 (%)
- **앱 인벤터**: 품질 관리 대시보드
  - 실시간 차트 (품질별 비율)
  - 통계 분석
  - green_ok/red_defect 명령 전송

---

## 📂 프로젝트 구조

```
Smart_Factory_Arms_Project/
│
├── 📂 curriculum/                                    # 교육 커리큘럼
│   ├── 00_교육철학_역방향학습.md                    # 교육 철학 및 방법론
│   ├── 01_교육_커리큘럼_Day1_로봇팔제어.md          # Day 1 가이드 (8시간)
│   ├── 02_교육_커리큘럼_Day2_컨베이어시스템.md      # Day 2 가이드 (8시간)
│   ├── 03_교육_커리큘럼_Day3_AI통합.md              # Day 3 가이드 (8시간)
│   ├── 04_교육_커리큘럼_Day4_발표시연.md            # Day 4 가이드 (4시간)
│   ├── 04_시스템_아키텍처_완전가이드.md             # 시스템 아키텍처
│   ├── 05_시스템_아키텍처_레이어별_가이드.md        # 레이어별 상세
│   └── README.md                                   # 커리큘럼 전체 개요
│
├── 📂 docs/                                          # 문서
│   ├── 00_프로젝트_개요.md                          # 프로젝트 소개
│   ├── 01_프로젝트_구조도.md                        # 알고리즘 아키텍처
│   ├── 02_블루투스_프로토콜.md                      # 통신 프로토콜
│   ├── 03_아두이노_피지컬컴퓨팅_가이드.md            # 하드웨어 가이드
│   ├── 03_바이브_코딩_가이드.md                     # 바이브 코딩 방법론
│   ├── 04_피지컬AI_학습방법.md                      # AI 학습 방법
│   ├── 05_비즈니스_모델_설계.md                     # 비즈니스 모델
│   ├── 06_프로젝트_아이디어_모음.md                 # 창의적 아이디어
│   ├── 07_앱인벤터_TM_연동가이드.md                 # AI 연동 가이드
│   └── 08_스마트팩토리_산업분석.md                  # 산업 분석
│
├── 📂 arduino_code/                                  # 아두이노 코드
│   │
│   ├── 📂 day1_arm_system/                          # Day 1: 로봇팔 학습 (6단계)
│   │   ├── 01_servo_motor_test/                    # 서보 동작 테스트
│   │   ├── 02_joystick_servo_control/              # 조이스틱 제어
│   │   ├── 03_serial_servo_control/                # Serial 원격 제어
│   │   ├── 04_eeprom_automation/                   # EEPROM 자동화 ⭐
│   │   ├── 05_bluetooth_eeprom_automation/         # Bluetooth 제어 ⭐⭐
│   │   ├── 06_bluetooth_eeprom_automation copy/    # 향상된 버전
│   │   └── README.md                               # 상세 가이드
│   │
│   ├── 📂 day2_conveyor_system/                     # Day 2: 컨베이어 학습 (8단계)
│   │   ├── 01_infrared_sensor_test/                # IR 센서
│   │   ├── 02_color_sensor_test/                   # 컬러 센서 ⭐
│   │   ├── 03_rgb_led_colors_test/                 # RGB LED
│   │   ├── 04_dc_motor_control_test/               # DC 모터
│   │   ├── 05_servo_motor_control_test/            # 서보 게이트
│   │   ├── 06_integrated_conveyor_system/          # 통합 시스템 ⭐⭐
│   │   ├── 07_serial_controlled_system/            # Serial 제어 ⭐⭐
│   │   ├── 08_dual_comm_system/                    # 이중 통신 ⭐⭐⭐
│   │   └── README.md                               # 상세 가이드
│   │
│   ├── 📂 day3_scenarios/                           # Day 3: AI 통합
│   │   ├── scenario_a_ai_smart.ino                 # AI 스마트 입고
│   │   ├── state_machine.h                         # 상태 머신
│   │   └── state_machine.cpp
│   │
│   ├── README.md                                    # 전체 사용 설명서
│   ├── 알고리즘_설명서.md                           # 알고리즘 상세 (7가지)
│   └── 프로젝트_구조.md                             # 구조 및 학습 가이드
│
├── 📂 app_inventor/                                  # 앱 인벤터 소스
│   ├── source/                                      # 기본 제어 앱 (.aia, .apk)
│   └── inventory_dashboard/                         # Day 3: 재고 관리 앱
│       ├── inventory_app.aia                        # 앱인벤터 소스
│       ├── inventory_app.apk                        # 안드로이드 앱
│       └── README.md                                # 사용 설명서
│           ├── AI 연동 방법
│           ├── 차트 설정 방법
│           └── count_ 데이터 파싱
│
└── 📂 movies/                                        # 시연 영상 (26개)
```

**핵심 파일**:
- ⭐ = 필수 학습
- ⭐⭐ = 중요 알고리즘
- ⭐⭐⭐ = 최고 난이도

---

## 🚦 시작하기

### 1단계: 필수 문서 읽기 (순서대로)

1. **[curriculum/README.md](curriculum/README.md)** ← 커리큘럼 전체 개요 (필수!)
2. **[curriculum/01_교육_커리큘럼_Day1_로봇팔제어.md](curriculum/01_교육_커리큘럼_Day1_로봇팔제어.md)** ← Day 1 가이드
3. **[curriculum/02_교육_커리큘럼_Day2_컨베이어시스템.md](curriculum/02_교육_커리큘럼_Day2_컨베이어시스템.md)** ← Day 2 가이드
4. **[curriculum/03_교육_커리큘럼_Day3_AI통합.md](curriculum/03_교육_커리큘럼_Day3_AI통합.md)** ← Day 3 가이드
5. **[curriculum/04_교육_커리큘럼_Day4_발표시연.md](curriculum/04_교육_커리큘럼_Day4_발표시연.md)** ← Day 4 가이드
6. [arduino_code/README.md](arduino_code/README.md) ← 아두이노 코드 상세
7. [arduino_code/알고리즘_설명서.md](arduino_code/알고리즘_설명서.md) ← 7가지 알고리즘
8. [curriculum/00_교육철학_역방향학습.md](curriculum/00_교육철학_역방향학습.md) ← 교육 철학 (선택)

### 2단계: 하드웨어 준비

**Day 1 (로봇팔)**:
- [ ] BP Lab 로봇팔 4축 ([구매 링크](https://bplab.kr/order/?idx=178))
- [ ] 아두이노 우노
- [ ] 아날로그 조이스틱 2개
- [ ] HC-05/HC-06 블루투스 모듈
- [ ] 5V 외부 전원 (서보용)

**Day 2 (컨베이어)**:
- [ ] TCS34725 컬러 센서
- [ ] IR 장애물 센서
- [ ] L298N + 12V DC 모터
- [ ] SG90 서보 모터
- [ ] NeoPixel RGB LED (3개 이상)
- [ ] Passive Buzzer
- [ ] HC-06 블루투스 모듈 (추가)
- [ ] 12V 전원

**Day 3 (AI 품질 검사)**:
- [ ] RFID RC522 모듈 (SPI)
- [ ] LCD1602 I2C 디스플레이
- [ ] 8x8 LED Matrix (MAX7219)
- [ ] 긴급정지 버튼 + Pull-up 저항
- [ ] 스마트폰 (안드로이드)

### 3단계: 소프트웨어 설치

```bash
# Arduino IDE 설치
1. Arduino IDE 1.8.19 이상 다운로드
2. 필수 라이브러리 설치:
   - Servo.h (기본 내장)
   - EEPROM.h (기본 내장)
   - Wire.h (기본 내장)
   - SoftwareSerial.h (기본 내장)
   - SPI.h (기본 내장)
   - Adafruit NeoPixel (라이브러리 매니저)
   - Adafruit TCS34725 (라이브러리 매니저)
   - MFRC522 (라이브러리 매니저) - Day 3 RFID
   - LiquidCrystal_I2C (라이브러리 매니저) - Day 3 LCD
   - MD_MAX72XX (라이브러리 매니저) - Day 3 LED Matrix

# 앱인벤터 설치
1. ai2.appinventor.mit.edu 접속
2. 구글 계정 로그인
3. TMIC Extension 설치 (Teachable Machine Image Classifier)

# Teachable Machine
1. teachablemachine.withgoogle.com 접속
2. 이미지 프로젝트 생성
3. 4가지 클래스 (GREEN/RED/YELLOW/BLUE) 학습
```

### 4단계: Day 1 시작 (단계별 학습)

```bash
# 1단계: 서보 모터 테스트
cd arduino_code/day1_arm_system/01_servo_motor_test
# Arduino IDE로 열기 → 업로드
# Serial Monitor (9600 baud) 열어서 동작 확인

# 2단계: 조이스틱 제어
cd ../02_joystick_servo_control
# 업로드 후 조이스틱으로 제어

# 3단계: Serial 제어
cd ../03_serial_servo_control
# 업로드 후 Serial Monitor에서 명령 테스트
# 예: arm0_90_ (베이스를 90도로)

# 4단계: EEPROM 자동화 (핵심!)
cd ../04_eeprom_automation
# 업로드 후 위치 저장/재생 테스트
# 명령: save, play, auto, stop, list

# 5단계: Bluetooth 제어
cd ../05_bluetooth_eeprom_automation
# 업로드 후 Bluetooth 연결
# 스마트폰 앱에서 save_, play_, auto_ 테스트
```

### 5단계: Day 2 시작 (모듈별 학습)

```bash
# 모듈 테스트 (01~05)
cd arduino_code/day2_conveyor_system
# 각 폴더의 .ino 파일을 순서대로 업로드

# 통합 시스템 (06~08)
cd 08_dual_comm_system
# 최종 완성본 업로드 및 테스트
# 명령: start_, stop_, init_
# 결과: red1_, green2_, blue3_ 등 count_ 데이터 수신 확인
```

### 6단계: Day 3 (AI 품질 검사 시스템)

**1교시: Teachable Machine 품질 분류 모델 (1시간)**
```bash
1. teachablemachine.withgoogle.com 접속
2. 새 이미지 프로젝트 생성
3. 4가지 클래스 설정:
   - GREEN (정상 제품): 50장 이상
   - RED (불량 제품): 50장 이상
   - YELLOW (재검사): 50장 이상
   - BLUE (특수 처리): 50장 이상
4. 모델 학습 → 내보내기 → 링크 복사
```

**2-3교시: 명령어 프로토콜 + 앱인벤터 연동 (2시간)**
```bash
1. 명령어 프로토콜 설계:
   - GREEN → green_ok
   - RED → red_defect
   - YELLOW → yellow_recheck
   - BLUE → blue_special

2. 앱인벤터 (ai2.appinventor.mit.edu)
   - TMIC Extension 추가
   - 모델 URL 설정
   - 인식 결과 → 명령어 전송
```

**4교시: 아두이노 구역별 동작 (1시간)**
```cpp
// 4개 구역 각도 배열 정의
int zone1[] = {90, 70, 80, 10};  // GREEN 정상 - 전방 우측
int zone2[] = {45, 60, 90, 10};  // RED 불량 - 전방 좌측
int zone3[] = {135, 60, 90, 10}; // YELLOW 재검사 - 후방 우측
int zone4[] = {180, 70, 80, 10}; // BLUE 특수 - 후방 좌측

// 명령어 파싱 및 구역 이동
if (cmd == "green_ok") moveToZone(zone1);
else if (cmd == "red_defect") moveToZone(zone2);
else if (cmd == "yellow_recheck") moveToZone(zone3);
else if (cmd == "blue_special") moveToZone(zone4);
```

**5교시: 하드웨어 통합 (1시간)**
```cpp
// RFID RC522
#include <MFRC522.h>
MFRC522 rfid(SS_PIN, RST_PIN);

// LCD1602 I2C
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 8x8 LED Matrix
#include <MD_MAX72xx.h>
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// 긴급정지 버튼
const int EMERGENCY_PIN = 2;
pinMode(EMERGENCY_PIN, INPUT_PULLUP);
```

**6-8교시: 스토리 구성 + 통합 테스트 + 확장 (3시간)**
- 품질 검사 라인 시나리오 작성
- 전체 시스템 통합 테스트
- 추가 센서/모터 확장 방법 학습

### 7단계: Day 4 (제품 시연회)

**1교시: 최종 점검 (1시간)**
```bash
# 하드웨어 점검
- 로봇팔 서보 4개 동작 확인
- 컨베이어 센서 정확도 테스트
- RFID 인증 테스트
- LCD/LED 표시 확인

# 소프트웨어 점검
- 아두이노 코드 업로드 확인
- 앱인벤터 AI 모델 테스트
- Bluetooth 통신 안정성

# 백업 계획
- 예비 배터리 준비
- 데모 영상 촬영
- 코드 백업
```

**2교시: 리허설 (1시간)**
```bash
# 역할 분담
- A: 기술 발표 (3-4분)
- B: 제품 시연 (1-2분)

# 타임라인
00:00 - 필요성 소개 (30초)
00:30 - 문제 해결 과정 (2분)
02:30 - 개선 사항 (1분)
03:30 - 제품 시연 시작
04:00 - AI 인식 → 로봇팔 → 분류
05:30 - 마무리

# 리허설 체크
- 시간 관리 (5-6분)
- 말의 명확성
- 시연 안정성
```

---

## 🎓 학습 목표

### Day 1 목표 (로봇팔)

- [ ] **01-03단계**: 서보 모터 제어 기초 이해
- [ ] **04단계**: EEPROM 저장/복구 알고리즘 완벽 이해 ⭐
- [ ] **04단계**: 자동 재생/무한 반복 알고리즘 구현 ⭐
- [ ] **05단계**: Bluetooth 명령 버퍼링 알고리즘 마스터 ⭐⭐
- [ ] 조이스틱 + Serial + Bluetooth 모두 제어 가능

### Day 2 목표 (컨베이어)

- [ ] **01-05단계**: 각 센서/액추에이터 개별 제어
- [ ] **02단계**: 색상 판별 알고리즘 (비율 기반) 이해 ⭐
- [ ] **06단계**: 통합 자동 분류 시스템 완성 ⭐⭐
- [ ] **07단계**: 실시간 명령 체크 (비차단) 구현 ⭐⭐
- [ ] **08단계**: 이중 통신 + 카운터 마스터 ⭐⭐⭐
- [ ] 10회 연속 분류 성공률 80% 이상

### Day 3 목표 (AI 품질 검사 시스템)

- [ ] **Teachable Machine 품질 분류 모델** ⭐⭐⭐
  - 4가지 품질 등급 학습 (GREEN/RED/YELLOW/BLUE)
  - 각 클래스당 50장 이상 데이터 수집
  - 평균 신뢰도 85% 이상 달성
- [ ] **명령어 프로토콜 설계 및 구현** ⭐⭐⭐⭐
  - green_ok, red_defect, yellow_recheck, blue_special
  - 앱인벤터 → 아두이노 명령 전송
  - 아두이노 명령 파싱 및 실행
- [ ] **구역별 동작 제어** ⭐⭐⭐⭐
  - 4~5개 구역 각도 배열 정의
  - 품질별 로봇팔 이동 (Day 1 활용)
  - 구역 도착 후 물체 배치
- [ ] **하드웨어 통합** ⭐⭐⭐
  - RFID RC522 출입 인증
  - LCD1602 상태 표시
  - 8x8 LED Matrix 품질 등급 시각화
  - 긴급정지 버튼 안전 제어
- [ ] **스토리 시나리오 구성** ⭐⭐
  - 품질 검사 라인 스토리보드 작성
  - 각 품질 등급별 처리 프로세스 정의
  - 시연 리허설 1회 이상

### Day 4 목표 (제품 시연회)

- [ ] **최종 점검 및 안정화** ⭐⭐⭐
  - 하드웨어 점검 (로봇팔 + 컨베이어 + 센서)
  - 소프트웨어 점검 (아두이노 + 앱인벤터)
  - 통신 점검 (Bluetooth + RFID)
  - AI 모델 점검 (정확도 테스트)
  - 백업 계획 수립
- [ ] **리허설** ⭐⭐
  - 스토리 발표 연습 (3-4분)
  - 제품 시연 연습 (1-2분)
  - 시간 관리 (총 5-6분)
  - 역할 분담 확정
- [ ] **본 발표** ⭐⭐⭐⭐
  - 필요성 → 문제 해결 과정 → 개선 사항 전달
  - 라이브 시연 성공
  - 질의응답 준비

### 전체 목표

- [ ] **7가지 핵심 알고리즘** 완벽 이해
- [ ] 복잡도 분석 (시간/공간)
- [ ] 병목 지점 찾아서 최적화
- [ ] 모듈화 및 클린 코드 작성
- [ ] 문제 해결 능력 향상

---

## 💡 핵심 개념

### 바이브 코딩 (VIBE Coding)

```
V - Visionary  : 알고리즘 설계 (플로우차트, 의사코드)
I - Implementer: 시스템 구현 (단계별 학습, 14단계 프로그램)
B - Debugger   : 알고리즘 최적화 (복잡도 분석, Early Return)
E - Entrepreneur: 가치 창출 (ROI 계산, 품질 검사 자동화)
```

### 단계별 학습 전략

**Day 1 (8시간)**: 하드웨어 조립 → 6단계 학습 → 음성 제어
- 1교시: 하드웨어 조립
- 2-5교시: 01~06단계 (기본 제어 → EEPROM → Bluetooth)
- 6-7교시: STT/TTS 음성 제어 앱

**Day 2 (8시간)**: 외형 제작 → 개별 테스트 → 통합 → 모니터링
- 1-2교시: 외형 제작 + 핀아웃 연결
- 3교시: 개별 센서/모터 테스트
- 4-6교시: 통합 자동화 + Serial/Bluetooth 제어
- 7교시: 모니터링 대시보드 앱

**Day 3 (8시간)**: AI 품질 검사 + 하드웨어 통합
- 1-4교시: TM 학습 + 프로토콜 + 앱 연동 + 구역별 동작
- 5-6교시: RFID + LCD + LED 통합 + 스토리 구성
- 7-8교시: 통합 테스트 + 확장 학습

**Day 4 (4시간)**: 제품 시연회
- 1교시: 최종 점검
- 2교시: 리허설
- 3-4교시: 본 발표 (21팀 × 5-6분)

### 품질 관리 시스템 (Day 3 핵심)

**AI 품질 분류 (4등급)**:
```
Teachable Machine 학습
- GREEN (정상): 즉시 포장 출하 → 전방 우측
- RED (불량): 폐기/재활용 → 전방 좌측
- YELLOW (재검사): 인간 재검수 → 후방 우측
- BLUE (특수): VIP 포장 → 후방 좌측

명령어 프로토콜
green_ok → 로봇팔 zone1 이동
red_defect → 로봇팔 zone2 이동
yellow_recheck → 로봇팔 zone3 이동
blue_special → 로봇팔 zone4 이동
```

**하드웨어 통합**:
- **RFID RC522**: 출입 권한 인증
- **LCD1602 I2C**: "GREEN: 15개 처리됨" 표시
- **8x8 LED Matrix**: 품질 등급 아이콘 (✓, ✗, ?, ★)
- **긴급정지 버튼**: 안전 제어

**통계 시각화**:
- **대시보드**: 품질별 처리 개수 (GREEN: 15, RED: 3, YELLOW: 2, BLUE: 5)
- **불량률 계산**: (RED / 전체) × 100 = 12%
- **재검사율**: (YELLOW / 전체) × 100 = 8%

### 시스템 통합 플로우 (Day 3)

```
1. RFID 카드 인증 → 출입 권한 확인
   ↓
2. 앱인벤터 카메라로 제품 촬영
   ↓
3. Teachable Machine AI 품질 판정
   - GREEN (정상 95%) → green_ok
   - RED (불량 92%) → red_defect
   - YELLOW (재검사 88%) → yellow_recheck
   - BLUE (특수 90%) → blue_special
   ↓
4. 명령어 전송 → 로봇팔 (Day 1)
   ↓
5. 구역별 이동 실행
   - GREEN → 전방 우측 (정상 구역)
   - RED → 전방 좌측 (불량 구역)
   - YELLOW → 후방 우측 (재검사 구역)
   - BLUE → 후방 좌측 (특수 구역)
   ↓
6. LCD1602 표시: "GREEN: 15개 처리됨"
   ↓
7. 8x8 LED Matrix: 품질 등급 아이콘 (✓, ✗, ?, ★)
   ↓
8. 컨베이어 자동 분류 (Day 2, 선택적)
   ↓
9. 대시보드 앱 업데이트 (품질별 통계)
```

---

## 📊 평가 기준 (Day 4)

| 영역 | 비중 | 세부 내용 |
|------|------|-----------|
| **시스템 완성도** | 30% | Day 1 + Day 2 + Day 3 통합, 작동 안정성 |
| **AI 품질 검사** | 25% | 4가지 등급 분류 정확도, 구역별 동작 |
| **하드웨어 통합** | 20% | RFID + LCD + LED Matrix 연동 |
| **스토리 구성** | 15% | 시나리오 완성도, 비즈니스 가치 |
| **발표 + 시연** | 10% | 기술 발표(3-4분), 제품 시연(1-2분) |

### 세부 평가 항목

**시스템 완성도 (30점)**:
- [ ] Day 1 로봇팔 구역별 이동 (10점)
- [ ] Day 2 컨베이어 자동 분류 + 모니터링 (10점)
- [ ] Day 3 AI 품질 검사 통합 동작 (10점)

**AI 품질 검사 (25점)**:
- [ ] Teachable Machine 4가지 등급 학습 (10점)
- [ ] 명령어 프로토콜 정확도 (green_ok 등) (5점)
- [ ] 구역별 동작 정확도 (4~5개 구역) (10점)

**하드웨어 통합 (20점)**:
- [ ] RFID 출입 인증 시스템 (5점)
- [ ] LCD1602 상태 표시 (5점)
- [ ] 8x8 LED Matrix 품질 등급 시각화 (5점)
- [ ] 긴급정지 버튼 안전 제어 (5점)

**스토리 구성 (15점)**:
- [ ] 품질 검사 라인 시나리오 완성도 (5점)
- [ ] 각 품질 등급별 처리 프로세스 명확성 (5점)
- [ ] 비즈니스 가치 전달력 (5점)

**발표 + 시연 (10점)**:
- [ ] 기술 발표 완성도 (필요성 → 문제 해결) (4점)
- [ ] 제품 시연 안정성 (라이브 데모) (4점)
- [ ] 질의응답 (2점)

---

## 🌟 기대 효과

### 기술 역량

```mermaid
mindmap
  root((28시간<br/>품질 검사<br/>스마트 팩토리))
    Day 1 로봇팔 8h
      하드웨어 조립
      6단계 학습
      EEPROM 저장
      Bluetooth 제어
      STT/TTS 음성 앱
    Day 2 컨베이어 8h
      외형 제작
      핀아웃 연결
      개별 테스트
      통합 자동화
      모니터링 대시보드
    Day 3 AI 품질 검사 8h
      TM 4등급 학습
      명령어 프로토콜
      구역별 동작
      RFID+LCD+LED
      스토리 구성
    Day 4 시연회 4h
      최종 점검
      리허설
      발표 3-4분
      시연 1-2분
    시스템 통합
      출입 인증
      품질 판정
      구역 이동
      실시간 시각화
      비즈니스 가치
```

### 포트폴리오

- 📄 **완성된 AI 품질 검사 스마트 팩토리**
  - Day 1: 로봇팔 구역별 제어 (6단계, ~1,340줄) + STT/TTS 음성 앱
  - Day 2: 컨베이어 자동 분류 (8단계, ~2,188줄) + 모니터링 대시보드
  - Day 3: AI 품질 검사 통합 (4등급 분류 + 하드웨어 통합)
  - 총 14단계 프로그램, ~3,528줄 + 앱인벤터 3종
  
- 📱 **앱인벤터 3종**
  - **음성 제어 앱 (Day 1)**: STT/TTS로 로봇팔 제어
  - **모니터링 대시보드 (Day 2)**: 일일량, 통합량, 차트
  - **품질 관리 앱 (Day 3)**: AI 연동, 품질별 명령 전송
  
- 🖥️ **하드웨어 통합**
  - 4축 로봇팔 (SG90 × 4) - 구역별 이동
  - 컨베이어 벨트 (DC 모터 + 센서)
  - RFID RC522 (출입 인증)
  - LCD1602 I2C (상태 표시)
  - 8x8 LED Matrix (품질 등급 시각화)
  - TCS34725 컬러 센서
  - NeoPixel RGB LED
  - 긴급정지 버튼
  
- 📊 **복잡도 분석 보고서**
  - O(1): EEPROM 저장, 색상 판별, 제품 카운터
  - O(n): Bluetooth 버퍼링, 실시간 명령 체크
  - O(n×m): 자동 재생/무한 반복
  
- 🎬 **시연 영상** (5분)
  - AI 인식 → 로봇팔 → 컨베이어 → 차트 업데이트
  - 완전 자동화 프로세스
  
- 📚 **학습 가이드** (3개 문서, 2,872줄)
  - README.md (전체 사용 설명서)
  - 알고리즘_설명서.md (7가지 알고리즘)
  - 프로젝트_구조.md (구조 및 학습)
  
- 📈 **발표 자료**
  - PPT (10분): 시스템 설명, 알고리즘, 결과
  - 시연 시나리오 (5분): 단계별 동작
  - 재고 관리 효과 분석

---

## 🤝 기여 및 참여

이 프로젝트는 교육용 오픈소스입니다.

### 기여 방법

1. Fork this repository
2. 알고리즘 개선 또는 새로운 시나리오 추가
3. Pull Request 제출

### 질문 및 토론

- Issues 탭에서 질문
- Discussions 탭에서 토론

---

## 📖 참고 자료

### 교구재

- [BP Lab AI 로봇팔 4축](https://bplab.kr/order/?idx=178) (99,000원)
- 아두이노 우노 호환보드
- TCS34725 컬러 센서, IR 센서
- L298N 모터 드라이버, NeoPixel LED
- HC-05/HC-06 블루투스 모듈

### 프로젝트 내부 문서 (필독!)

1. **[arduino_code/README.md](arduino_code/README.md)** - 전체 사용 설명서
2. **[arduino_code/알고리즘_설명서.md](arduino_code/알고리즘_설명서.md)** - 7가지 알고리즘 상세
3. **[arduino_code/프로젝트_구조.md](arduino_code/프로젝트_구조.md)** - 구조 및 학습 가이드
4. [arduino_code/day1_arm_system/README.md](arduino_code/day1_arm_system/README.md) - Day 1 가이드
5. [arduino_code/day2_conveyor_system/README.md](arduino_code/day2_conveyor_system/README.md) - Day 2 가이드

### 온라인 리소스

**Day 3 필수 도구**:
- [Teachable Machine](https://teachablemachine.withgoogle.com/) - AI 품질 분류 학습 (4등급)
- [앱인벤터](https://appinventor.mit.edu/) - 품질 관리 앱 제작
  - PersonalImageClassifier (TMIC Extension): AI 연동
  - ChartData: 품질별 차트 시각화
  - BluetoothClient: 통신
  - SpeechRecognizer: STT (Day 1)
  - TextToSpeech: TTS (Day 1)
- [MD_MAX72xx 라이브러리](https://github.com/MajicDesigns/MD_MAX72XX) - 8x8 LED Matrix
- [MFRC522 라이브러리](https://github.com/miguelbalboa/rfid) - RFID RC522
- [LiquidCrystal_I2C 라이브러리](https://github.com/johnrickman/LiquidCrystal_I2C) - LCD1602

**개발 환경**:
- [아두이노 IDE](https://www.arduino.cc/en/software) - 펌웨어 개발
- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) - RGB LED
- [Adafruit TCS34725](https://github.com/adafruit/Adafruit_TCS34725) - 컬러 센서

### 알고리즘 학습

- "Introduction to Algorithms" (CLRS)
- "알고리즘 문제 해결 전략" (구종만)
- [Coursera] Algorithms Specialization

### 하드웨어 데이터시트

- [TCS34725 컬러 센서](https://www.adafruit.com/product/1334)
- [L298N 모터 드라이버](https://www.st.com/resource/en/datasheet/l298.pdf)
- [HC-05/HC-06 블루투스](http://www.electronica60norte.com/mwfls/pdf/hc06.pdf)
- [WS2812B NeoPixel](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)

---

## 📄 라이선스

MIT License - 교육 목적으로 자유롭게 사용 가능

---

## 👥 제작팀

**Smart Factory Education Team**
- 28시간 교육 커리큘럼 설계 (Day 1: 8h, Day 2: 8h, Day 3: 8h, Day 4: 4h)
- 단계별 학습 시스템 개발 (14단계 프로그램)
- 7가지 핵심 알고리즘 구현 및 문서화
- 하드웨어 통합 가이드 제작 (RFID + LCD + LED Matrix)
- AI 품질 검사 시스템 설계 (4등급 분류)
- 앱인벤터 3종 개발 (음성 제어 + 모니터링 + 품질 관리)
- 역방향 학습 교육 철학 정립
- 총 문서 작성:
  - curriculum 폴더: 8개 파일 (교육 철학 + Day 1-4 + 아키텍처)
  - README.md: 946줄 (curriculum 기반 재구성)
  - 알고리즘 설명서: 1,099줄
  - 프로젝트 구조: 905줄
  - 기타 가이드 문서

---

## 🎉 성공 메시지

```
"28시간 후, 여러분은:

✅ Day 1 (8시간): 로봇팔 피지컬 컴퓨팅을 완벽히 마스터했습니다
   → 하드웨어 조립부터 6단계 학습까지
   → EEPROM 저장/복구, 자동 재생, Bluetooth 버퍼링
   → STT/TTS 음성 제어 앱 완성

✅ Day 2 (8시간): 컨베이어 자동 분류 시스템을 완성했습니다
   → 외형 제작 → 핀아웃 연결 → 개별 테스트 → 통합
   → 색상 판별, 실시간 명령 체크, 이중 통신
   → 원격 모니터링 대시보드 (일일량, 통합량, 차트)

✅ Day 3 (8시간): AI 품질 검사 스마트 팩토리를 구축했습니다
   → Teachable Machine 4가지 품질 등급 (GREEN/RED/YELLOW/BLUE)
   → 명령어 프로토콜 (green_ok, red_defect, yellow_recheck, blue_special)
   → 구역별 동작 제어 (4~5개 구역 각도 배열)
   → RFID 출입 인증 + LCD1602 + 8x8 LED Matrix 통합
   → 품질 검사 라인 스토리 완성

✅ Day 4 (4시간): 제품 시연회에서 성공적으로 발표했습니다
   → 3-4분 기술 발표 (필요성 → 문제 해결 → 개선)
   → 1-2분 제품 시연 (라이브 데모 성공)

✅ 7가지 핵심 알고리즘을 설계하고 구현할 수 있습니다
   → O(1), O(n), O(n×m) 복잡도 분석 및 최적화

✅ 약 3,528줄의 실전 코드를 작성하고 이해했습니다
   → 모듈화, 클린 코드, Early Return 패턴

✅ 하드웨어와 소프트웨어를 완전히 통합했습니다
   → 로봇팔 + 컨베이어 + RFID + LCD + LED + AI + 앱 3종

✅ AI 기반 품질 관리 시스템을 만들었습니다
   → 4가지 품질 등급 자동 분류
   → 실시간 통계 분석 및 시각화
   → 비즈니스 가치 창출 (인건비 절감, 불량률 개선)

더 중요한 것은, 여러분이 **단계별 학습**과 **시스템 통합**을 
체득했다는 것입니다.

Day 1 하드웨어 조립 → Day 2 센서 통합 → Day 3 AI 품질 검사까지
14단계 프로그램을 순차적으로 완성하며 쌓은 경험은
앞으로 모든 스마트 시스템 개발의 기반이 될 것입니다.

자, 28시간의 AI 품질 검사 스마트 팩토리 여정을 시작합니다! 🚀
```

---

**Last Updated**: 2026-01-30  
**Version**: 3.0 (Curriculum 기반 완전 재구성)  
**교구재**: [BP Lab AI 로봇팔](https://bplab.kr/order/?idx=178)  
**시스템 구성**:
- Day 1 (8h): 로봇팔 6단계 (~1,340줄) + STT/TTS 음성 앱
- Day 2 (8h): 컨베이어 8단계 (~2,188줄) + 모니터링 대시보드
- Day 3 (8h): AI 품질 검사 (4등급 분류) + 하드웨어 통합
- Day 4 (4h): 제품 시연회 (3-4분 발표 + 1-2분 시연)
- 총 14단계 프로그램 (~3,528줄) + 앱인벤터 3종
