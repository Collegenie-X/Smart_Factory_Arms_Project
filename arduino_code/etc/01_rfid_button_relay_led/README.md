# RFID + 버튼 + 릴레이 + LED 통합 예제

## 📌 개요
RFID 카드 인식으로 릴레이를 제어하고, 버튼으로 수동 제어하는 출입 제어 시스템 예제입니다.

## 🔧 하드웨어
- **RFID-RC522 모듈** (SPI 통신)
- **버튼** 1개 (풀업 저항)
- **릴레이 모듈** 1개 (5V)
- **LED** 3개 (빨강, 초록, 파랑)

## 📍 핀 연결
```
Arduino UNO
├─ D9  → RFID RST
├─ D10 → RFID SS
├─ D11 → RFID MOSI
├─ D12 → RFID MISO
├─ D13 → RFID SCK
├─ D2  → 버튼 (풀업)
├─ D3  → 릴레이 제어
├─ D4  → 빨강 LED (대기)
├─ D5  → 초록 LED (인식)
└─ D6  → 파랑 LED (에러)
```

## 🎯 동작 원리
1. **대기 상태**: 빨강 LED 점등
2. **RFID 카드 태그**: 
   - 릴레이 ON
   - 초록 LED 점등
   - 5초 후 자동 OFF
3. **버튼 입력**: 즉시 릴레이 OFF
4. **에러 발생**: 파랑 LED 점등

## 📊 동작 순서도
```
[대기] → [카드 감지] → [UID 읽기] → [릴레이 ON]
   ↑                                        ↓
   └────────────← [5초 후 또는 버튼] ←──────┘
```

## 💻 사용 방법
1. 아두이노에 업로드
2. Serial Monitor 열기 (9600 baud)
3. RFID 카드를 태그
4. 릴레이 작동 확인
5. 버튼으로 수동 제어

## 📝 주요 함수
- `handleButton()`: 버튼 입력 처리 (디바운싱)
- `handleRFID()`: RFID 카드 감지 및 UID 읽기
- `turnOnRelay()`: 릴레이 ON 및 타이머 시작
- `turnOffRelay()`: 릴레이 OFF
- `handleRelayAutoOff()`: 5초 후 자동 OFF
- `setLED_State()`: LED 상태 표시

## 🔍 응용 분야
- 출입 제어 시스템
- 자동 도어락
- 물품 보관함
- 주차 관리 시스템
