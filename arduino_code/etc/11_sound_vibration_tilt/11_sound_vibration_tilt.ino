/*
 * ========================================
 * 사운드 + 진동 + 기울기 센서 통합 예제
 * ========================================
 * 
 * 기능:
 * - 사운드 센서: 소음 레벨 측정
 * - 진동 센서: 진동 감지
 * - 기울기 센서: 기울임 감지
 * - LED 및 부저 알림
 * 
 * 하드웨어:
 * - 사운드 센서 (마이크 모듈)
 * - 진동 센서 (SW-420)
 * - 기울기 센서 (Tilt Switch)
 * - LED 3개
 * - 부저
 * 
 * 작성일: 2026-01-30
 */

// ========================================
// 핀 정의
// ========================================
#define SOUND_PIN       A0    // 사운드 센서 아날로그
#define VIBRATION_PIN   7     // 진동 센서 디지털
#define TILT_PIN        8     // 기울기 센서 디지털

#define LED_SOUND_PIN   2     // 사운드 감지 LED
#define LED_VIBRATION_PIN 3   // 진동 감지 LED
#define LED_TILT_PIN    4     // 기울기 감지 LED
#define BUZZER_PIN      5     // 부저

// ========================================
// 상수 정의
// ========================================
#define SOUND_THRESHOLD     300   // 소음 임계값
#define SOUND_LOUD          600   // 큰 소리 임계값

#define UPDATE_INTERVAL     100   // 업데이트 주기 (ms)

// ========================================
// 전역 변수
// ========================================
int soundValue = 0;
bool vibrationDetected = false;
bool tiltDetected = false;

unsigned long previousMillis = 0;

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  사운드 + 진동 + 기울기 센서 시스템");
  Serial.println("========================================");
  
  // 핀 모드 설정
  pinMode(SOUND_PIN, INPUT);
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(TILT_PIN, INPUT_PULLUP);  // 풀업 저항 사용
  
  pinMode(LED_SOUND_PIN, OUTPUT);
  pinMode(LED_VIBRATION_PIN, OUTPUT);
  pinMode(LED_TILT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // 초기 상태
  digitalWrite(LED_SOUND_PIN, LOW);
  digitalWrite(LED_VIBRATION_PIN, LOW);
  digitalWrite(LED_TILT_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n센서 임계값:");
  Serial.print("  사운드: ");
  Serial.println(SOUND_THRESHOLD);
  Serial.println("  진동: 디지털 감지");
  Serial.println("  기울기: 디지털 감지");
  Serial.println("========================================");
  Serial.println();
}

// ========================================
// 메인 루프
// ========================================
void loop() {
  unsigned long currentMillis = millis();
  
  // 일정 주기마다 측정
  if (currentMillis - previousMillis >= UPDATE_INTERVAL) {
    previousMillis = currentMillis;
    
    // 1. 센서 값 읽기
    readSensors();
    
    // 2. LED 및 부저 제어
    controlIndicators();
    
    // 3. 상태 출력
    printStatus();
  }
}

// ========================================
// 센서 읽기 함수
// ========================================
void readSensors() {
  // 사운드 센서 읽기
  soundValue = analogRead(SOUND_PIN);
  
  // 진동 센서 읽기 (LOW = 진동 감지)
  vibrationDetected = (digitalRead(VIBRATION_PIN) == LOW);
  
  // 기울기 센서 읽기 (LOW = 기울어짐)
  tiltDetected = (digitalRead(TILT_PIN) == LOW);
}

// ========================================
// LED 및 부저 제어 함수
// ========================================
void controlIndicators() {
  // 사운드 LED 제어
  if (soundValue > SOUND_THRESHOLD) {
    digitalWrite(LED_SOUND_PIN, HIGH);
    
    // 큰 소리일 때 부저
    if (soundValue > SOUND_LOUD) {
      tone(BUZZER_PIN, 1000, 100);
    }
  } else {
    digitalWrite(LED_SOUND_PIN, LOW);
  }
  
  // 진동 LED 제어
  if (vibrationDetected) {
    digitalWrite(LED_VIBRATION_PIN, HIGH);
    tone(BUZZER_PIN, 1500, 50);
  } else {
    digitalWrite(LED_VIBRATION_PIN, LOW);
  }
  
  // 기울기 LED 제어
  if (tiltDetected) {
    digitalWrite(LED_TILT_PIN, HIGH);
    tone(BUZZER_PIN, 2000, 100);
  } else {
    digitalWrite(LED_TILT_PIN, LOW);
  }
}

// ========================================
// 상태 출력 함수
// ========================================
void printStatus() {
  // 변화가 있을 때만 출력
  static int lastSound = 0;
  static bool lastVibration = false;
  static bool lastTilt = false;
  
  bool changed = false;
  
  // 사운드 변화 체크 (10 이상 차이)
  if (abs(soundValue - lastSound) > 10) {
    changed = true;
    lastSound = soundValue;
  }
  
  // 진동 변화 체크
  if (vibrationDetected != lastVibration) {
    changed = true;
    lastVibration = vibrationDetected;
  }
  
  // 기울기 변화 체크
  if (tiltDetected != lastTilt) {
    changed = true;
    lastTilt = tiltDetected;
  }
  
  // 변화가 있을 때만 출력
  if (changed) {
    Serial.println("--- 센서 상태 ---");
    
    // 사운드
    Serial.print("사운드: ");
    Serial.print(soundValue);
    Serial.print(" → ");
    Serial.println(getSoundLevel(soundValue));
    
    // 진동
    Serial.print("진동: ");
    Serial.println(vibrationDetected ? "감지됨 ⚠️" : "없음");
    
    // 기울기
    Serial.print("기울기: ");
    Serial.println(tiltDetected ? "기울어짐 ⚠️" : "정상");
    
    Serial.println("------------------");
    Serial.println();
  }
}

// ========================================
// 사운드 레벨 반환 함수
// ========================================
String getSoundLevel(int sound) {
  if (sound < SOUND_THRESHOLD) {
    return "조용함";
  } else if (sound < SOUND_LOUD) {
    return "보통";
  } else {
    return "시끄러움";
  }
}

// ========================================
// 데시벨 추정 함수 (근사값)
// ========================================
float estimateDecibel(int sound) {
  // 간단한 데시벨 추정 (실제로는 캘리브레이션 필요)
  if (sound < 100) {
    return 30.0;  // 매우 조용함
  }
  
  // 로그 스케일 근사
  float db = 20.0 * log10(sound / 100.0) + 40.0;
  return constrain(db, 30.0, 100.0);
}

// ========================================
// 진동 카운터 함수
// ========================================
void countVibrations(unsigned long duration) {
  Serial.print("[진동 카운트] ");
  Serial.print(duration / 1000);
  Serial.println("초간 측정...");
  
  int count = 0;
  unsigned long startTime = millis();
  bool lastState = false;
  
  while (millis() - startTime < duration) {
    bool currentState = (digitalRead(VIBRATION_PIN) == LOW);
    
    // 상태 변화 감지 (LOW → HIGH)
    if (currentState && !lastState) {
      count++;
      Serial.print(".");
    }
    
    lastState = currentState;
    delay(10);
  }
  
  Serial.println();
  Serial.print("[결과] 총 진동 횟수: ");
  Serial.println(count);
}

// ========================================
// 소음 모니터링 함수
// ========================================
void monitorNoise(unsigned long duration) {
  Serial.print("[소음 모니터링] ");
  Serial.print(duration / 1000);
  Serial.println("초간 측정...");
  
  int maxSound = 0;
  int minSound = 1023;
  long sumSound = 0;
  int samples = 0;
  
  unsigned long startTime = millis();
  
  while (millis() - startTime < duration) {
    int sound = analogRead(SOUND_PIN);
    
    if (sound > maxSound) maxSound = sound;
    if (sound < minSound) minSound = sound;
    sumSound += sound;
    samples++;
    
    delay(50);
  }
  
  int avgSound = sumSound / samples;
  
  Serial.println("\n[결과]");
  Serial.print("  최대: ");
  Serial.println(maxSound);
  Serial.print("  최소: ");
  Serial.println(minSound);
  Serial.print("  평균: ");
  Serial.println(avgSound);
}
