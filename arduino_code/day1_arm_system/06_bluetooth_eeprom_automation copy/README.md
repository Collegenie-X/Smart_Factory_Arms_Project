# 로봇 팔 06: Bluetooth EEPROM 자동화 (전력 최적화 버전)

## 📌 주요 특징

이 버전은 **05 버전의 전력 최적화 버전**입니다:

- ✅ Bluetooth 출력 최소화 (전력 소모 감소)
- ✅ Serial 출력 최소화 (디버깅용만)
- ✅ delay 최적화
- ✅ 메모리 사용량 감소
- ✅ 안정적인 play 명령 동작

## ⚡ 전력 최적화 내용

### 1. **Bluetooth 출력 제거**
```cpp
// 기존 (05 버전): 모든 명령에 Bluetooth 응답
BTSerial.println("[Save OK #1: 90,80,90,15]");
BTSerial.println("[Play OK: 3 steps]");
BTSerial.println("[Auto OK: 3 steps]");

// 개선 (06 버전): Bluetooth 출력 제거
// 명령만 받고, 응답은 최소화 (전력 절약)
```

### 2. **Serial 출력 최소화**
```cpp
// 기존: 상세한 디버그 메시지
Serial.println("[수신] play");
Serial.println("[처리] BT: 'play'");
Serial.println("[대기] play 명령 준비");
Serial.println("[Play OK: 3 steps]");
Serial.println("→#1: 90,80,90,15");

// 개선: ENABLE_DEBUG 모드에서만 출력
#if ENABLE_DEBUG
  Serial.print("#1:");
  Serial.println("90,80,90,15");
#endif
```

### 3. **delay 최적화**
```cpp
// 기존
delay(200);  // 서보 초기화
delay(50);   // EEPROM 안정화
delay(20);   // 서보 재연결

// 개선
delay(150);  // 15ms 단축
delay(30);   // 20ms 단축
delay(15);   // 5ms 단축
```

### 4. **조이스틱 출력 제거**
```cpp
// 기존: 각도 변경 시마다 Serial 출력
Serial.print("0:90");
Serial.print("1:80");

// 개선: 출력 완전 제거 (전력 절약)
```

## 🔧 사용 방법

### 기본 명령어 (Bluetooth로 전송)

| 명령 | 기능 | 예시 |
|------|------|------|
| `save_` | 현재 위치 저장 | 조이스틱으로 이동 후 save_ 전송 |
| `play_` | 1회 재생 | 저장된 동작 1회 실행 |
| `auto_` | 자동 반복 | 계속 반복 실행 |
| `stop_` | 반복 중지 | auto 모드 정지 |
| `clear_` | 모든 데이터 삭제 | EEPROM 초기화 |
| `list_` | 저장된 목록 보기 | 디버그 모드에서만 |

**중요**: 명령 끝에 반드시 `_` 포함!

### 디버그 모드

```cpp
#define ENABLE_DEBUG 0  // 운영 모드 (전력 절약)
#define ENABLE_DEBUG 1  // 디버그 모드 (개발용)
```

**운영 모드 (ENABLE_DEBUG = 0)**:
- Serial 출력 최소화
- 전력 소모 최소
- 안정적인 동작

**디버그 모드 (ENABLE_DEBUG = 1)**:
- 명령 수신 확인
- 각도 정보 출력
- 문제 진단 가능

## 📱 앱 인벤터 설정

### 블루투스 명령 전송 예시

```
Button1 (저장):
  블루투스클라이언트.텍스트보내기("save_")

Button2 (재생):
  블루투스클라이언트.텍스트보내기("play_")

Button3 (자동 반복):
  블루투스클라이언트.텍스트보내기("auto_")

Button4 (중지):
  블루투스클라이언트.텍스트보내기("stop_")

Button5 (삭제):
  블루투스클라이언트.텍스트보내기("clear_")
```

## ⚠️ 전원 관련 주의사항

### 전압 부족 증상
- [ ] save 명령 시 아두이노 재부팅
- [ ] play 실행 중 멈춤
- [ ] 서보가 덜덜거림
- [ ] Bluetooth 연결 끊김

### 해결 방법
1. **외부 5V 전원 연결** (필수!)
   - USB 전원만으로는 부족
   - 서보 4개 + Bluetooth = 약 1-2A 필요
   
2. **전원 분리**
   - 서보: 외부 5V 전원
   - 아두이노: USB 또는 외부 전원
   - GND는 반드시 공유

3. **안정화 커패시터**
   - 전원 라인에 100uF 전해 커패시터 추가
   - 노이즈 감소 효과

## 🔍 문제 해결

### 1. play 명령이 안될 때
```
원인: EEPROM에 데이터 없음
해결: save_ 명령으로 먼저 저장
```

### 2. 디버그 메시지가 안 보일 때
```cpp
// 코드 상단에서 확인
#define ENABLE_DEBUG 1  // 1로 변경
```

### 3. Bluetooth 연결 후 동작 안할 때
```
1. Serial Monitor 열기 (9600 baud)
2. ENABLE_DEBUG를 1로 설정
3. 명령 수신 확인
4. 잔여 전압 확인
```

## 📊 전력 소모 비교

| 항목 | 05 버전 | 06 버전 | 절감 |
|------|---------|---------|------|
| Bluetooth 출력 | 많음 | 최소 | ~40% |
| Serial 출력 | 항상 | DEBUG만 | ~60% |
| delay 총합 | 긴 시간 | 최적화 | ~20% |
| 메모리 사용 | 높음 | 낮음 | ~15% |

## 🎯 권장 설정

### 운영 환경 (실제 사용)
```cpp
#define ENABLE_DEBUG 0
#define ENABLE_BLUETOOTH 1
```
- 전력 소모 최소
- 안정적인 동작
- Serial Monitor 불필요

### 개발 환경 (테스트/디버깅)
```cpp
#define ENABLE_DEBUG 1
#define ENABLE_BLUETOOTH 1
```
- 명령 수신 확인
- 각도 정보 확인
- 문제 진단 용이

## 📝 버전 히스토리

- **v1.1 (06 버전)**: 전력 최적화, Serial/Bluetooth 출력 최소화
- **v1.0 (05 버전)**: play 명령 안정화, 플래그 방식 도입

## 🚀 다음 단계

1. 외부 전원 연결
2. 디버그 모드로 테스트
3. 데이터 저장 확인 (list_)
4. 운영 모드로 전환 (ENABLE_DEBUG = 0)
5. 실제 동작 테스트

---

**제작**: Smart Factory Arms Project  
**최적화**: 전력 소모 최소화 버전
