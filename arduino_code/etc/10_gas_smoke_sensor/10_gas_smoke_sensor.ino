/*
 * ========================================
 * MQ-2 가스/연기 센서 측정 예제
 * ========================================
 * 
 * 기능:
 * - 가스 및 연기 농도 측정
 * - 위험 수준별 경보
 * - LED 및 부저 알림
 * - Serial 모니터로 실시간 출력
 * 
 * 하드웨어:
 * - MQ-2 가스 센서
 * - LED 3개 (안전/경고/위험)
 * - 부저 (경고음)
 * 
 * 주의: 센서 예열 시간 필요 (약 1~2분)
 * 
 * 작성일: 2026-01-30
 */

// ========================================
// 핀 정의
// ========================================
#define MQ2_PIN         A0    // MQ-2 아날로그 입력

#define LED_SAFE_PIN    2     // 안전 (초록)
#define LED_WARN_PIN    3     // 경고 (노랑)
#define LED_DANGER_PIN  4     // 위험 (빨강)
#define BUZZER_PIN      5     // 부저

// ========================================
// 상수 정의
// ========================================
#define GAS_THRESHOLD_WARN    300   // 경고 임계값
#define GAS_THRESHOLD_DANGER  600   // 위험 임계값

#define WARMUP_TIME           60000 // 예열 시간 (60초)
#define UPDATE_INTERVAL       500   // 업데이트 주기 (ms)

// ========================================
// 전역 변수
// ========================================
int gasValue = 0;
bool isWarmedUp = false;
unsigned long startTime = 0;
unsigned long previousMillis = 0;

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  MQ-2 가스/연기 센서 시스템");
  Serial.println("========================================");
  
  // 핀 모드 설정
  pinMode(MQ2_PIN, INPUT);
  pinMode(LED_SAFE_PIN, OUTPUT);
  pinMode(LED_WARN_PIN, OUTPUT);
  pinMode(LED_DANGER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // 초기 상태
  digitalWrite(LED_SAFE_PIN, LOW);
  digitalWrite(LED_WARN_PIN, LOW);
  digitalWrite(LED_DANGER_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  startTime = millis();
  
  Serial.println("[초기화 완료]");
  Serial.println("\n⚠️ 센서 예열 중... (약 60초 소요)");
  Serial.println("========================================");
  Serial.println();
}

// ========================================
// 메인 루프
// ========================================
void loop() {
  unsigned long currentMillis = millis();
  
  // 예열 시간 체크
  if (!isWarmedUp) {
    checkWarmupStatus(currentMillis);
    return;
  }
  
  // 일정 주기마다 측정
  if (currentMillis - previousMillis >= UPDATE_INTERVAL) {
    previousMillis = currentMillis;
    
    // 1. 가스 값 읽기
    gasValue = analogRead(MQ2_PIN);
    
    // 2. 경보 제어
    controlAlarm(gasValue);
    
    // 3. 결과 출력
    printGasData(gasValue);
  }
}

// ========================================
// 예열 상태 체크 함수
// ========================================
void checkWarmupStatus(unsigned long currentMillis) {
  unsigned long elapsed = currentMillis - startTime;
  
  // 예열 진행 상황 표시 (10초마다)
  static unsigned long lastPrint = 0;
  if (currentMillis - lastPrint >= 10000) {
    lastPrint = currentMillis;
    int remaining = (WARMUP_TIME - elapsed) / 1000;
    Serial.print("[예열 중] 남은 시간: ");
    Serial.print(remaining);
    Serial.println("초");
  }
  
  // 예열 완료 체크
  if (elapsed >= WARMUP_TIME) {
    isWarmedUp = true;
    digitalWrite(LED_SAFE_PIN, HIGH);
    
    Serial.println("\n========================================");
    Serial.println("✅ 센서 예열 완료!");
    Serial.println("========================================");
    Serial.println();
  }
}

// ========================================
// 경보 제어 함수
// ========================================
void controlAlarm(int gasLevel) {
  // 위험 수준
  if (gasLevel >= GAS_THRESHOLD_DANGER) {
    digitalWrite(LED_SAFE_PIN, LOW);
    digitalWrite(LED_WARN_PIN, LOW);
    digitalWrite(LED_DANGER_PIN, HIGH);
    
    // 빠른 경보음
    tone(BUZZER_PIN, 2000);
    delay(100);
    noTone(BUZZER_PIN);
    delay(100);
  }
  // 경고 수준
  else if (gasLevel >= GAS_THRESHOLD_WARN) {
    digitalWrite(LED_SAFE_PIN, LOW);
    digitalWrite(LED_WARN_PIN, HIGH);
    digitalWrite(LED_DANGER_PIN, LOW);
    
    // 느린 경보음
    tone(BUZZER_PIN, 1500, 200);
  }
  // 안전 수준
  else {
    digitalWrite(LED_SAFE_PIN, HIGH);
    digitalWrite(LED_WARN_PIN, LOW);
    digitalWrite(LED_DANGER_PIN, LOW);
    noTone(BUZZER_PIN);
  }
}

// ========================================
// 가스 데이터 출력 함수
// ========================================
void printGasData(int gasLevel) {
  Serial.print("[가스 농도] 값: ");
  Serial.print(gasLevel);
  Serial.print(" / 1023");
  
  // 퍼센트 계산
  int percent = map(gasLevel, 0, 1023, 0, 100);
  Serial.print(" (");
  Serial.print(percent);
  Serial.print("%)");
  
  // 레벨 표시
  Serial.print(" → ");
  Serial.print(getGasLevel(gasLevel));
  
  // 경고 표시
  if (gasLevel >= GAS_THRESHOLD_DANGER) {
    Serial.println(" ⚠️⚠️⚠️ 위험!");
  } else if (gasLevel >= GAS_THRESHOLD_WARN) {
    Serial.println(" ⚠️ 경고!");
  } else {
    Serial.println(" ✅ 안전");
  }
}

// ========================================
// 가스 레벨 반환 함수
// ========================================
String getGasLevel(int gasLevel) {
  if (gasLevel < GAS_THRESHOLD_WARN) {
    return "안전";
  } else if (gasLevel < GAS_THRESHOLD_DANGER) {
    return "경고";
  } else {
    return "위험";
  }
}

// ========================================
// PPM 변환 함수 (근사값)
// ========================================
float calculatePPM(int gasLevel) {
  // MQ-2 센서 특성 곡선에 따른 근사 변환
  // 실제로는 센서별 캘리브레이션 필요
  
  if (gasLevel < 100) {
    return 0;
  }
  
  // 간단한 선형 변환 (예시)
  float ppm = map(gasLevel, 100, 1023, 0, 10000);
  return ppm;
}

// ========================================
// 평균 가스 농도 측정 함수
// ========================================
int measureAverageGas(int samples) {
  long sum = 0;
  
  for (int i = 0; i < samples; i++) {
    sum += analogRead(MQ2_PIN);
    delay(50);
  }
  
  return sum / samples;
}

// ========================================
// 캘리브레이션 함수
// ========================================
int calibrateSensor() {
  Serial.println("\n[캘리브레이션 시작]");
  Serial.println("깨끗한 공기에서 10초간 측정...");
  
  long sum = 0;
  int samples = 20;
  
  for (int i = 0; i < samples; i++) {
    sum += analogRead(MQ2_PIN);
    delay(500);
    Serial.print(".");
  }
  
  int baseline = sum / samples;
  
  Serial.println();
  Serial.print("[캘리브레이션 완료] 기준값: ");
  Serial.println(baseline);
  
  return baseline;
}
