# PIR 인체 감지 센서 예제

## 📌 개요
PIR (Passive Infrared) 센서를 사용하여 인체 동작을 감지하는 예제입니다.

## 🔧 하드웨어
- **HC-SR501 PIR 센서**
- **LED** 1개 (자동 조명)
- **부저** 1개 (알림음)

## 📍 핀 연결
```
Arduino UNO
├─ D2  → PIR 센서 OUT (디지털 입력)
├─ D3  → 자동 조명 LED
└─ D4  → 부저

PIR 센서:
  VCC → 5V
  GND → GND
  OUT → D2
```

## 🎯 동작 원리
1. **적외선 감지**: 인체의 적외선 방출 감지
2. **온도 변화**: 주변 온도와의 차이 감지
3. **동작 감지**: 움직임이 있을 때 HIGH 신호
4. **자동 조명**: 10초간 LED 점등

## ⚠️ 중요 사항
**안정화 시간**: 센서 가동 후 약 30초 필요
- 센서 내부 회로가 안정화될 때까지 대기
- 안정화 중에는 움직이지 말 것

## 📊 감지 범위
- **거리**: 약 3~7m (센서 설정에 따라 다름)
- **각도**: 약 110도
- **감지 시간**: 설정 가능 (2초~5분)

## 🔧 센서 조정
PIR 센서에는 두 개의 가변 저항이 있습니다:
1. **감도 조정** (Sensitivity): 감지 거리
2. **시간 조정** (Time Delay): 신호 지속 시간

## 📝 주요 함수
- `handleMotionDetected()`: 동작 감지 처리
- `handleLEDAutoOff()`: LED 자동 꺼짐 (10초)
- `printDetectionLog()`: 감지 로그 출력
- `monitorContinuousDetection()`: 연속 감지 모니터링
- `testSensitivity()`: 민감도 테스트

## 💻 자동 조명 시스템
```cpp
// 동작 감지 → LED 켜기 → 10초 후 자동 꺼짐
if (motionDetected) {
  ledOnTime = millis();
  digitalWrite(LED_PIN, HIGH);
}

if (millis() - ledOnTime >= 10000) {
  digitalWrite(LED_PIN, LOW);
}
```

## 🔍 응용 분야
- 자동 조명 시스템
- 보안 시스템
- 절전 모드 제어
- 사람 카운팅
- 스마트 홈 자동화

## 🎓 절전 효과
- 사람이 있을 때만 조명 ON
- 10초 무동작 시 자동 OFF
- 에너지 절감 효과 큼

## ⚠️ 주의 사항
- 햇빛 직사광선 피할 것
- 에어컨 바람 영향 주의
- 유리나 벽은 감지 불가
- 온도 변화가 큰 곳 피할 것
