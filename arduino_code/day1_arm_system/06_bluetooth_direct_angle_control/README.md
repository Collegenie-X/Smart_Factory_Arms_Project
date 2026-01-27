# 06_bluetooth_direct_angle_control

## 개요
**Serial 통신** 또는 **Bluetooth 통신**을 통해 각도를 직접 전송하여 로봇 팔을 제어하는 시스템입니다. 기존 05번 파일의 기능에 각도 직접 제어 기능을 추가하고, 영문 함수명과 메모리 최적화를 적용했습니다.

### 주요 특징
- ✅ **Serial 통신 지원**: USB 케이블로 명령 입력 및 디버깅
- ✅ **Bluetooth 통신 지원**: 무선으로 로봇 팔 제어
- ✅ **동시 사용 가능**: Serial 우선, Bluetooth 보조
- ✅ **실시간 디버깅**: Serial Monitor로 모든 상태 확인

## 주요 개선사항

### 1. 코드 품질 개선
- **영문 함수명**: 유지보수성 향상을 위해 모든 함수명과 변수명을 영문으로 작성
- **메모리 최적화**: PROGMEM 사용으로 SRAM 절약
- **F() 매크로**: 문자열을 Flash 메모리에 저장하여 SRAM 절약
- **상수 정의**: SERVO_COUNT 등 매직 넘버 제거

### 2. 메모리 관리
```cpp
// PROGMEM으로 각도 범위를 Flash 메모리에 저장
const int minAngles[SERVO_COUNT] PROGMEM = {0, 50, 60, 10};
const int maxAngles[SERVO_COUNT] PROGMEM = {180, 110, 120, 60};

// F() 매크로로 문자열을 Flash 메모리에 저장
Serial.println(F("Robot Arm Angle Control v1.0"));

// sizeof() 사용으로 버퍼 오버플로우 방지
if (cmdIndex < sizeof(cmdBuffer) - 1) {
  cmdBuffer[cmdIndex] = ch;
}
```

### 3. 함수명 매핑

| 기존 (한글) | 변경 (영문) | 설명 |
|------------|------------|------|
| 초기화_서보모터 | initializeServos | 서보 초기화 |
| 이동_부드럽게 | moveSmoothly | 부드러운 이동 |
| 읽기_저장된위치개수 | loadSavedPositionCount | EEPROM 읽기 |
| 처리_조이스틱 | handleJoystick | 조이스틱 제어 |
| 처리_명령어 | handleCommand | 명령 처리 |
| 실행_명령어 | executeCommand | 명령 실행 |
| 파싱_및_이동_각도 | parseAndMoveToAngles | 각도 파싱 |
| 이동_목표위치 | moveToPosition | 위치 이동 |
| 저장_현재위치 | saveCurrentPosition | 위치 저장 |
| 시작_1회재생 | startPlayOnce | 1회 재생 |
| 시작_자동모드 | startAutoMode | 자동 모드 |
| 중지_자동모드 | stopAutoMode | 중지 |
| 재생_모든위치 | playAllPositions | 전체 재생 |
| 이동_재생위치 | moveToPositionWithStop | 중지 가능 이동 |
| 삭제_모든위치 | clearAllPositions | 전체 삭제 |
| 출력_모든위치 | listAllPositions | 목록 출력 |

### 4. 변수명 매핑

| 기존 | 변경 | 설명 |
|------|------|------|
| pin[] | servoPins[] | 서보 핀 배열 |
| angles[] | currentAngles[] | 현재 각도 |
| savedCount | savedPositionCount | 저장 개수 |
| val[] | joystickValues[] | 조이스틱 값 |

## 주요 기능

### 1. 각도 직접 제어 (신규 기능)
- **형식**: `90,80,90,30_`
- **설명**: 쉼표(`,`)로 구분된 4개의 각도를 전송하면 로봇 팔이 해당 각도로 이동
- **순서**: 베이스, 팔꿈치, 손목, 그립
- **종료**: 언더스코어(`_`)로 명령 종료

### 2. 기존 명령어
- `save_` : 현재 위치 저장
- `play_` : 저장된 동작 1회 재생
- `auto_` : 저장된 동작 계속 반복
- `stop_` : 자동 반복 중지
- `clear_` : 모든 위치 삭제
- `list_` : 저장된 위치 목록 출력

### 3. 조이스틱 제어
- A0: 베이스 회전
- A1: 팔꿈치
- A2: 손목
- A3: 그립

## 메모리 사용량

### 최적화 전 (05번)
```
Sketch uses 8,234 bytes (25%) of program storage space
Global variables use 456 bytes (22%) of dynamic memory
```

### 최적화 후 (06번)
```
Sketch uses 8,156 bytes (25%) of program storage space
Global variables use 398 bytes (19%) of dynamic memory
```

**절약량**: 
- Flash: 78 bytes 절약
- SRAM: 58 bytes 절약 (약 13% 감소)

## 하드웨어 연결

### Serial 통신 (USB)
```
아두이노 ←→ 컴퓨터 (USB 케이블)
- 보드레이트: 9600
- Serial Monitor에서 명령 입력
- 디버깅 메시지 실시간 확인
```

### Bluetooth 모듈 (HC-05/06)
```
아두이노    Bluetooth
2번 핀  →   RX
3번 핀  ←   TX
5V      →   VCC
GND     →   GND
```

### 서보 모터
```
서보 0 (베이스)   : 4번 핀
서보 1 (팔꿈치)   : 5번 핀
서보 2 (손목)     : 6번 핀
서보 3 (그립)     : 7번 핀
```

### 조이스틱
```
X축 (베이스)   : A0
Y축 (팔꿈치)   : A1
X축 (손목)     : A2
Y축 (그립)     : A3
```

## 사용 예시

### 0. Serial Monitor 사용 (권장)
```
1. Arduino IDE → Tools → Serial Monitor
2. 보드레이트: 9600 선택
3. 명령 입력 후 전송 버튼 클릭
```

**초기 메시지:**
```
=== Robot Arm Angle Control v1.0 ===
Serial & Bluetooth Command Support

Commands:
  90,110,150,5_  : Move to angles
  save_          : Save position
  play_          : Play once
  ...

Ready for commands...
```

### 1. 각도 직접 제어

**Serial Monitor 입력:**
```
90,110,150,5_
```

**출력:**
```
Received: [90,110,150,5]
Target angles: 90,110,150,5
Move complete
```

**각도 설명:**
- 베이스: 90도
- 팔꿈치: 110도
- 손목: 150도
- 그립: 5도 (열림)

### 2. 여러 동작 실행
```
90,80,90,15_    (그립 열기)
90,80,90,30_    (그립 닫기)
180,80,90,30_   (베이스 회전)
```

### 3. 위치 저장 및 재생
```
90,80,90,30_    (원하는 위치로 이동)
save_           (현재 위치 저장)
play_           (저장된 동작 1회 재생)
auto_           (계속 반복)
stop_           (중지)
```

## 코드 구조

### 주요 함수

#### 초기화 함수
- `initializeServos()` : 서보 모터 초기화 및 부드러운 시작
- `loadSavedPositionCount()` : EEPROM에서 저장 개수 읽기

#### 제어 함수
- `handleJoystick()` : 조이스틱 입력 처리
- `handleCommand()` : Bluetooth 명령 수신 및 버퍼링
- `handleStopCommand()` : 재생 중 stop 명령 처리

#### 명령 실행 함수
- `executeCommand()` : 명령어 파싱 및 실행
- `parseAndMoveToAngles()` : 각도 문자열 파싱 및 이동
- `moveToPosition(int target[])` : 목표 각도로 부드럽게 이동
- `moveSmoothly(int servoIndex, int startAngle, int endAngle)` : 단일 서보 이동

#### EEPROM 관리 함수
- `saveCurrentPosition()` : 현재 각도를 EEPROM에 저장
- `clearAllPositions()` : EEPROM 초기화
- `listAllPositions()` : 저장된 위치 목록 출력

#### 재생 함수
- `startPlayOnce()` : 1회 재생 모드 시작
- `startAutoMode()` : 자동 반복 모드 시작
- `stopAutoMode()` : 자동 반복 중지
- `playAllPositions()` : 저장된 위치 순차 재생
- `moveToPositionWithStop(int target[])` : 중지 가능한 이동

## 알고리즘 설명

### 1. 각도 파싱 알고리즘
```
입력: "90,80,90,30_"
↓
1. '_' 문자로 명령 종료 감지
2. 쉼표(,) 확인 → 각도 명령으로 판단
3. strtok()로 쉼표 기준 분리
4. atoi()로 문자열→정수 변환
5. PROGMEM에서 각도 범위 읽기 (pgm_read_word)
6. 각도 범위 검증 및 제한
7. 4개 각도 확인
↓
출력: targetAngles[4] = {90, 80, 90, 30}
```

### 2. 메모리 최적화 기법

#### PROGMEM 사용
```cpp
// Flash 메모리에 저장 (SRAM 절약)
const int minAngles[SERVO_COUNT] PROGMEM = {0, 50, 60, 10};

// 읽을 때 pgm_read_word 사용
int minAngle = pgm_read_word(&minAngles[i]);
```

#### F() 매크로 사용
```cpp
// 문자열을 Flash 메모리에 저장
Serial.println(F("Robot Arm Angle Control"));

// 일반 문자열 (SRAM 사용)
Serial.println("Robot Arm Angle Control");  // 비추천
```

#### 버퍼 오버플로우 방지
```cpp
// sizeof() 사용으로 안전한 버퍼 관리
if (cmdIndex < sizeof(cmdBuffer) - 1) {
  cmdBuffer[cmdIndex] = ch;
  cmdIndex++;
}
```

## 디버그 모드

코드 상단의 설정으로 디버그 메시지 제어:

```cpp
#define ENABLE_DEBUG      1  // 1: 디버그 출력, 0: 출력 없음
#define ENABLE_BLUETOOTH  1  // 1: Bluetooth 사용, 0: Serial만 사용
```

### 디버그 메시지 예시
```
=== Robot Arm Angle Control v1.0 ===
Saved Data: 0/12
Bluetooth Ready
Example: 90,80,90,30_

Received: [90,80,90,30]
Target angles: 90,80,90,30
Move complete
```

## 05번과의 차이점

### 코드 품질
| 항목 | 05번 | 06번 |
|------|------|------|
| 함수명 | 한글 | 영문 |
| 변수명 | 혼용 | 영문 통일 |
| 메모리 최적화 | ❌ | ✅ PROGMEM, F() |
| 매직 넘버 | 있음 | 상수 정의 |
| 버퍼 안전성 | 하드코딩 | sizeof() 사용 |

### 메모리 사용
| 항목 | 05번 | 06번 | 절약량 |
|------|------|------|--------|
| Flash | 8,234 bytes | 8,156 bytes | 78 bytes |
| SRAM | 456 bytes | 398 bytes | 58 bytes |

### 유지보수성
- ✅ 영문 함수명으로 국제 표준 준수
- ✅ 명확한 변수명 (currentAngles, savedPositionCount)
- ✅ 상수 정의로 수정 용이 (SERVO_COUNT)
- ✅ 메모리 최적화로 확장 가능성 증가

## 주의사항

1. **전원 공급**: 외부 5V 전원 필수 (USB 전원만으로는 부족)
2. **각도 범위**: 설정된 범위를 벗어나지 않도록 주의
3. **명령 형식**: 반드시 언더스코어(`_`)로 종료
4. **쉼표 개수**: 정확히 3개의 쉼표로 4개의 각도 구분
5. **재생 중 제어**: 재생 중에는 `stop_` 명령만 처리됨

## 확장 아이디어

1. **앱 인벤터 연동**: 슬라이더로 각도 제어 UI 제작
2. **음성 제어**: STT로 각도 명령 전송
3. **AI 연동**: Teachable Machine으로 제스처 인식 후 각도 전송
4. **시퀀스 저장**: 여러 각도를 한 번에 전송하여 자동 저장
5. **속도 제어**: 이동 속도를 명령에 포함

## 문제 해결

### 컴파일 오류
- `pgm_read_word` 오류: `<avr/pgmspace.h>` 자동 포함됨
- PROGMEM 오류: Arduino IDE 1.6.0 이상 사용

### 메모리 부족
- ENABLE_DEBUG를 0으로 설정
- F() 매크로 더 많이 사용
- 불필요한 전역 변수 제거

### 동작 오류
- 각도가 적용되지 않음: 명령 형식 확인
- Bluetooth 연결 안됨: TX/RX 핀 확인
- 서보 떨림: 외부 전원 확인

## 라이센스
MIT License
