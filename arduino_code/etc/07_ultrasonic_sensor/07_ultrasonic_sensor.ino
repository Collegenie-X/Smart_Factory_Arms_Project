/*
 * ========================================
 * 초음파 센서 (HC-SR04) 거리 측정 예제
 * ========================================
 * 
 * 기능:
 * - 초음파로 정밀한 거리 측정 (2~400cm)
 * - LED로 거리 단계 표시
 * - 부저로 근접 경고음
 * - Serial 모니터로 실시간 출력
 * 
 * 하드웨어:
 * - 초음파 센서 HC-SR04
 * - LED 3개 (거리 단계 표시)
 * - 부저 (근접 경고)
 * 
 * 작성일: 2026-01-30
 */

// ========================================
// 핀 정의
// ========================================
#define TRIG_PIN        7     // 초음파 송신
#define ECHO_PIN        6     // 초음파 수신

#define LED_FAR_PIN     2     // 멀리 (초록 LED, > 30cm)
#define LED_MID_PIN     3     // 중간 (노랑 LED, 10~30cm)
#define LED_NEAR_PIN    4     // 가까이 (빨강 LED, < 10cm)
#define BUZZER_PIN      5     // 부저

// ========================================
// 상수 정의
// ========================================
#define SOUND_SPEED       0.034   // 음속 (cm/μs)
#define TIMEOUT_US        30000   // 타임아웃 (30ms = 약 5m)

#define DISTANCE_NEAR     10      // 가까움 임계값 (cm)
#define DISTANCE_MID      30      // 중간 임계값 (cm)
#define DISTANCE_FAR      100     // 멀리 임계값 (cm)

#define MEASURE_INTERVAL  200     // 측정 주기 (ms)

// ========================================
// 전역 변수
// ========================================
float distance_cm = 0;
unsigned long previousMillis = 0;

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  초음파 센서 거리 측정 시스템");
  Serial.println("========================================");
  
  // 핀 모드 설정
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_FAR_PIN, OUTPUT);
  pinMode(LED_MID_PIN, OUTPUT);
  pinMode(LED_NEAR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // 초기 상태
  digitalWrite(TRIG_PIN, LOW);
  digitalWrite(LED_FAR_PIN, LOW);
  digitalWrite(LED_MID_PIN, LOW);
  digitalWrite(LED_NEAR_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n거리 범위:");
  Serial.println("  가까움: < 10cm (빨강 LED + 부저)");
  Serial.println("  중간: 10~30cm (노랑 LED)");
  Serial.println("  멀리: > 30cm (초록 LED)");
  Serial.println("========================================");
  Serial.println();
  
  delay(1000);
}

// ========================================
// 메인 루프
// ========================================
void loop() {
  unsigned long currentMillis = millis();
  
  // 일정 주기마다 측정
  if (currentMillis - previousMillis >= MEASURE_INTERVAL) {
    previousMillis = currentMillis;
    
    // 1. 거리 측정
    distance_cm = measureDistance();
    
    // 2. LED 및 부저 제어
    controlIndicators(distance_cm);
    
    // 3. 결과 출력
    printDistance(distance_cm);
  }
}

// ========================================
// 거리 측정 함수
// ========================================
float measureDistance() {
  // 트리거 핀 초기화
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // 10μs 동안 HIGH 신호 전송
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // 에코 핀에서 펄스 폭 측정 (μs)
  long duration_us = pulseIn(ECHO_PIN, HIGH, TIMEOUT_US);
  
  // 타임아웃 체크
  if (duration_us == 0) {
    return -1;  // 측정 실패
  }
  
  // 거리 계산: (시간 * 음속) / 2
  // 2로 나누는 이유: 왕복 시간이므로
  float distance = (duration_us * SOUND_SPEED) / 2.0;
  
  return distance;
}

// ========================================
// LED 및 부저 제어 함수
// ========================================
void controlIndicators(float distance) {
  // 측정 실패 처리
  if (distance < 0) {
    // 모든 LED 끄기
    digitalWrite(LED_FAR_PIN, LOW);
    digitalWrite(LED_MID_PIN, LOW);
    digitalWrite(LED_NEAR_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    return;
  }
  
  // 거리에 따른 LED 제어
  if (distance < DISTANCE_NEAR) {
    // 가까움: 빨강 LED + 부저
    digitalWrite(LED_FAR_PIN, LOW);
    digitalWrite(LED_MID_PIN, LOW);
    digitalWrite(LED_NEAR_PIN, HIGH);
    
    // 빠른 비프음 (긴급)
    tone(BUZZER_PIN, 2000, 100);
    
  } else if (distance < DISTANCE_MID) {
    // 중간: 노랑 LED
    digitalWrite(LED_FAR_PIN, LOW);
    digitalWrite(LED_MID_PIN, HIGH);
    digitalWrite(LED_NEAR_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
  } else if (distance < DISTANCE_FAR) {
    // 멀리: 초록 LED
    digitalWrite(LED_FAR_PIN, HIGH);
    digitalWrite(LED_MID_PIN, LOW);
    digitalWrite(LED_NEAR_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
  } else {
    // 매우 멀리: 모든 LED 끄기
    digitalWrite(LED_FAR_PIN, LOW);
    digitalWrite(LED_MID_PIN, LOW);
    digitalWrite(LED_NEAR_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// ========================================
// 거리 출력 함수
// ========================================
void printDistance(float distance) {
  Serial.print("[거리 측정] ");
  
  if (distance < 0) {
    Serial.println("측정 실패 (범위 초과 또는 장애물 없음)");
    return;
  }
  
  Serial.print(distance);
  Serial.print(" cm");
  
  // 상태 표시
  if (distance < DISTANCE_NEAR) {
    Serial.println(" → 가까움 (⚠️ 경고!)");
  } else if (distance < DISTANCE_MID) {
    Serial.println(" → 중간 거리");
  } else if (distance < DISTANCE_FAR) {
    Serial.println(" → 멀리");
  } else {
    Serial.println(" → 매우 멀리");
  }
}

// ========================================
// 거리 레벨 반환 함수
// ========================================
String getDistanceLevel(float distance) {
  if (distance < 0) {
    return "측정 불가";
  } else if (distance < DISTANCE_NEAR) {
    return "가까움";
  } else if (distance < DISTANCE_MID) {
    return "중간";
  } else if (distance < DISTANCE_FAR) {
    return "멀리";
  } else {
    return "매우 멀리";
  }
}

// ========================================
// 평균 거리 측정 함수 (노이즈 제거)
// ========================================
float measureAverageDistance(int samples) {
  float sum = 0;
  int validCount = 0;
  
  for (int i = 0; i < samples; i++) {
    float dist = measureDistance();
    
    if (dist > 0) {
      sum += dist;
      validCount++;
    }
    
    delay(10);
  }
  
  if (validCount == 0) {
    return -1;
  }
  
  return sum / validCount;
}
