# Serial 통신 사용 가이드

## 개요
06번 코드는 **Serial 통신**과 **Bluetooth 통신**을 모두 지원합니다. Serial Monitor를 통해 명령을 입력하고 디버깅할 수 있습니다.

## Serial 통신 설정

### 1. Arduino IDE 설정
```
1. 아두이노 보드를 USB 케이블로 연결
2. Tools → Port → 적절한 포트 선택
3. Tools → Serial Monitor 열기 (Ctrl+Shift+M)
4. 보드레이트: 9600 선택
5. 줄바꿈: "줄 바꿈 없음" 또는 "개행" 선택
```

### 2. 초기 메시지
Serial Monitor를 열면 다음과 같은 메시지가 표시됩니다:

```
=== Robot Arm Angle Control v1.0 ===
Serial & Bluetooth Command Support

Saved Data: 0/12

Bluetooth Ready

Commands:
  90,110,150,5_  : Move to angles
  save_          : Save position
  play_          : Play once
  auto_          : Auto repeat
  stop_          : Stop repeat
  clear_         : Clear all
  list_          : List positions

Ready for commands...
```

## Serial 통신 우선순위

### 명령 수신 순서
1. **Serial 통신** (우선)
2. **Bluetooth 통신** (Serial이 없을 때)

```cpp
// Serial 통신 우선 확인
if (Serial.available() > 0) {
  ch = Serial.read();
}
// Bluetooth 통신 확인
else if (BTSerial.available() > 0) {
  ch = BTSerial.read();
}
```

### 장점
- Serial Monitor로 테스트 가능
- Bluetooth 없이도 개발 가능
- 실시간 디버깅 메시지 확인

## Serial Monitor 사용법

### 1. 각도 직접 제어

#### 명령 입력
```
90,110,150,5_
```

#### 출력 메시지
```
Received: [90,110,150,5]
Target angles: 90,110,150,5
Move complete
```

### 2. 위치 저장

#### 명령 입력
```
save_
```

#### 출력 메시지
```
Received: [save]
Position saved #1
```

### 3. 위치 재생

#### 명령 입력
```
play_
```

#### 출력 메시지
```
Received: [play]
Play once started
Step #1: 90,110,150,5
```

### 4. 자동 반복

#### 명령 입력
```
auto_
```

#### 출력 메시지
```
Received: [auto]
Auto repeat started (1 steps)
Step #1: 90,110,150,5
Step #1: 90,110,150,5
...
```

### 5. 중지

#### 명령 입력
```
stop_
```

#### 출력 메시지
```
Received: [stop]
Auto repeat stopped (total 3 times)
```

### 6. 위치 목록

#### 명령 입력
```
list_
```

#### 출력 메시지
```
Received: [list]
Saved positions (total 3):
  #1: 90,110,150,5
  #2: 180,110,150,5
  #3: 90,80,90,30
```

### 7. 전체 삭제

#### 명령 입력
```
clear_
```

#### 출력 메시지
```
Received: [clear]
All positions cleared
```

## 실전 사용 예시

### 예시 1: 기본 테스트

```
Serial Monitor 입력:

90,110,150,5_
save_
180,110,150,5_
save_
list_
play_
```

```
Serial Monitor 출력:

Received: [90,110,150,5]
Target angles: 90,110,150,5
Move complete

Received: [save]
Position saved #1

Received: [180,110,150,5]
Target angles: 180,110,150,5
Move complete

Received: [save]
Position saved #2

Received: [list]
Saved positions (total 2):
  #1: 90,110,150,5
  #2: 180,110,150,5

Received: [play]
Play once started
Step #1: 90,110,150,5
Step #2: 180,110,150,5
```

### 예시 2: 각도 범위 테스트

```
Serial Monitor 입력:

0,80,60,5_      (최소 각도)
180,150,120,60_ (최대 각도)
90,110,150,5_   (중간 각도)
```

```
Serial Monitor 출력:

Received: [0,80,60,5]
Target angles: 0,80,60,5
Move complete

Received: [180,150,120,60]
Target angles: 180,150,120,60
Move complete

Received: [90,110,150,5]
Target angles: 90,110,150,5
Move complete
```

### 예시 3: 범위 초과 테스트

```
Serial Monitor 입력:

200,200,200,200_
```

```
Serial Monitor 출력:

Received: [200,200,200,200]
Warning: Max angle limit [0] 180
Warning: Max angle limit [1] 150
Warning: Max angle limit [2] 120
Warning: Max angle limit [3] 60
Target angles: 180,150,120,60
Move complete
```

## 디버그 모드 제어

### 디버그 활성화 (기본값)
```cpp
#define ENABLE_DEBUG 1
```

**출력:**
- 모든 명령 수신 메시지
- 각도 이동 메시지
- 저장/재생 상태 메시지
- 오류 및 경고 메시지

### 디버그 비활성화
```cpp
#define ENABLE_DEBUG 0
```

**출력:**
- 메시지 없음 (메모리 절약)

## Serial vs Bluetooth 비교

| 항목 | Serial | Bluetooth |
|------|--------|-----------|
| 연결 방식 | USB 케이블 | 무선 |
| 속도 | 빠름 | 보통 |
| 범위 | 케이블 길이 | 10m 내외 |
| 디버깅 | 우수 | 제한적 |
| 실전 사용 | 개발 단계 | 완성 후 |
| 전원 | USB 가능 | 외부 5V 필수 |

## 동시 사용

### Serial과 Bluetooth 동시 연결
```
1. USB 케이블 연결 (Serial)
2. Bluetooth 모듈 연결
3. 외부 5V 전원 연결
```

### 명령 우선순위
- **Serial 명령이 우선** 처리됨
- Bluetooth는 Serial이 없을 때만 처리

### 사용 시나리오
```
개발자: Serial Monitor로 명령 입력 및 디버깅
사용자: Bluetooth 앱으로 로봇 팔 제어
→ 개발자 명령이 우선 처리됨
```

## 문제 해결

### Serial Monitor에 아무것도 안 나옴

**원인:**
- 보드레이트 불일치
- Serial 케이블 연결 불량
- 잘못된 포트 선택

**해결:**
1. 보드레이트를 9600으로 설정
2. USB 케이블 재연결
3. Tools → Port에서 올바른 포트 선택
4. 아두이노 리셋 버튼 누르기

### 명령이 실행되지 않음

**원인:**
- 언더스코어(_) 누락
- 잘못된 형식

**해결:**
```
틀림: 90,110,150,5
맞음: 90,110,150,5_

틀림: save
맞음: save_
```

### 이상한 문자가 출력됨

**원인:**
- 보드레이트 불일치

**해결:**
- Serial Monitor 보드레이트를 9600으로 설정

### Serial과 Bluetooth 충돌

**원인:**
- 동시에 명령 전송

**해결:**
- Serial이 우선 처리되므로 문제없음
- 한 번에 하나씩 명령 전송

## 고급 기능

### 1. 연속 명령 입력

```
90,110,150,5_
save_
180,110,150,5_
save_
play_
```

**주의:** 명령 사이에 약간의 딜레이 필요 (이동 시간)

### 2. 매크로 테스트

Arduino IDE의 Serial Monitor에서는 한 줄씩만 가능하지만, 
다른 터미널 프로그램(PuTTY, CoolTerm 등)을 사용하면 
스크립트로 여러 명령을 자동 전송할 수 있습니다.

```bash
# 예시 (bash script)
echo "90,110,150,5_" > /dev/ttyUSB0
sleep 2
echo "save_" > /dev/ttyUSB0
sleep 1
echo "180,110,150,5_" > /dev/ttyUSB0
sleep 2
echo "save_" > /dev/ttyUSB0
sleep 1
echo "play_" > /dev/ttyUSB0
```

### 3. Python으로 제어

```python
import serial
import time

# Serial 포트 열기
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
time.sleep(2)  # 아두이노 초기화 대기

# 명령 전송 함수
def send_command(cmd):
    ser.write((cmd + '_').encode())
    time.sleep(0.1)
    # 응답 읽기
    while ser.in_waiting:
        print(ser.readline().decode().strip())

# 테스트 시퀀스
send_command('90,110,150,5')
time.sleep(2)
send_command('save')
time.sleep(1)
send_command('180,110,150,5')
time.sleep(2)
send_command('save')
time.sleep(1)
send_command('play')

ser.close()
```

## 디버깅 팁

### 1. 실시간 각도 확인

현재 코드는 명령 수신 시에만 출력합니다. 
실시간 각도를 보려면 다음 코드를 `loop()`에 추가:

```cpp
void loop() {
  static unsigned long lastPrint = 0;
  
  // 1초마다 현재 각도 출력
  if (millis() - lastPrint > 1000) {
    Serial.print(F("Current: "));
    for (int i = 0; i < SERVO_COUNT; i++) {
      Serial.print(currentAngles[i]);
      if (i < SERVO_COUNT - 1) Serial.print(F(","));
    }
    Serial.println();
    lastPrint = millis();
  }
  
  // 기존 코드...
  handleCommand();
  // ...
}
```

### 2. 조이스틱 값 확인

조이스틱 입력을 확인하려면 `handleJoystick()`에 추가:

```cpp
void handleJoystick() {
  // 조이스틱 값 읽기
  int joystickValues[SERVO_COUNT];
  joystickValues[0] = analogRead(A0);
  joystickValues[1] = analogRead(A1);
  joystickValues[2] = 1024 - analogRead(A2);
  joystickValues[3] = 1024 - analogRead(A3);
  
  // 디버그 출력 (옵션)
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 500) {
    Serial.print(F("Joystick: "));
    for (int i = 0; i < SERVO_COUNT; i++) {
      Serial.print(joystickValues[i]);
      Serial.print(F(" "));
    }
    Serial.println();
    lastDebug = millis();
  }
  
  // 기존 코드...
}
```

### 3. 메모리 사용량 확인

```cpp
void setup() {
  // 기존 코드...
  
#if ENABLE_DEBUG
  Serial.print(F("Free RAM: "));
  Serial.print(freeRam());
  Serial.println(F(" bytes"));
#endif
}

// 여유 RAM 계산 함수
int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
```

## 요약

### Serial 통신 장점
✅ USB 케이블만으로 테스트 가능  
✅ 실시간 디버깅 메시지 확인  
✅ Bluetooth 없이도 개발 가능  
✅ 빠른 응답 속도  
✅ 안정적인 연결  

### 사용 시나리오
- **개발 단계**: Serial Monitor로 테스트
- **디버깅**: Serial로 상태 확인
- **완성 후**: Bluetooth로 무선 제어
- **동시 사용**: Serial 우선, Bluetooth 보조

### 명령 형식 (동일)
```
각도 제어: 90,110,150,5_
저장: save_
재생: play_
자동: auto_
중지: stop_
목록: list_
삭제: clear_
```

**중요:** 모든 명령은 언더스코어(`_`)로 종료해야 합니다!

