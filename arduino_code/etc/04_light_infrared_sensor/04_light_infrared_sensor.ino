/*
 * ========================================
 * 조도센서 + 적외선센서 통합 예제
 * ========================================
 * 
 * 기능:
 * - 조도센서(CdS)로 밝기 측정 및 자동 조명 제어
 * - 적외선센서로 물체 감지
 * - LED 자동 제어 (어두우면 켜짐)
 * - Serial 모니터로 실시간 값 출력
 * 
 * 하드웨어:
 * - 조도센서 (CdS, 포토레지스터)
 * - 적외선 장애물 센서
 * - LED 2개 (조도용, 감지용)
 * 
 * 작성일: 2026-01-30
 */

// ========================================
// 핀 정의
// ========================================
#define LIGHT_SENSOR_PIN    A0    // 조도센서 아날로그 입력
#define IR_SENSOR_PIN       A1    // 적외선센서 아날로그 입력

#define LED_LIGHT_PIN       3     // 조도센서 연동 LED
#define LED_DETECT_PIN      4     // 적외선 감지 LED
#define BUZZER_PIN          5     // 부저 (물체 감지 알림)

// ========================================
// 상수 정의
// ========================================
#define LIGHT_THRESHOLD     300   // 조도 임계값 (어두움 판단)
#define IR_THRESHOLD        512   // 적외선 임계값 (물체 감지)

#define DARK_LEVEL          300   // 어두움
#define DIM_LEVEL           500   // 약간 어두움
#define BRIGHT_LEVEL        700   // 밝음

#define UPDATE_INTERVAL     500   // 센서 읽기 주기 (ms)

// ========================================
// 전역 변수
// ========================================
int lightValue = 0;           // 조도센서 값
int irValue = 0;              // 적외선센서 값

bool isLightOn = false;       // 조명 상태
bool isObjectDetected = false; // 물체 감지 상태

unsigned long previousMillis = 0;

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  조도센서 + 적외선센서 시스템");
  Serial.println("========================================");
  
  // 핀 모드 설정
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(LED_LIGHT_PIN, OUTPUT);
  pinMode(LED_DETECT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // 초기 상태
  digitalWrite(LED_LIGHT_PIN, LOW);
  digitalWrite(LED_DETECT_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n센서 임계값:");
  Serial.print("  조도센서: ");
  Serial.print(LIGHT_THRESHOLD);
  Serial.println(" (이하일 때 어두움)");
  Serial.print("  적외선센서: ");
  Serial.print(IR_THRESHOLD);
  Serial.println(" (이하일 때 물체 감지)");
  Serial.println("========================================");
  Serial.println();
}

// ========================================
// 메인 루프
// ========================================
void loop() {
  unsigned long currentMillis = millis();
  
  // 일정 주기마다 센서 읽기
  if (currentMillis - previousMillis >= UPDATE_INTERVAL) {
    previousMillis = currentMillis;
    
    // 1. 센서 값 읽기
    readSensors();
    
    // 2. 조도센서 처리
    handleLightSensor();
    
    // 3. 적외선센서 처리
    handleIRSensor();
    
    // 4. 상태 출력
    printStatus();
  }
}

// ========================================
// 센서 값 읽기 함수
// ========================================
void readSensors() {
  lightValue = analogRead(LIGHT_SENSOR_PIN);
  irValue = analogRead(IR_SENSOR_PIN);
}

// ========================================
// 조도센서 처리 함수
// ========================================
void handleLightSensor() {
  // 어두우면 LED 켜기
  if (lightValue < LIGHT_THRESHOLD) {
    if (!isLightOn) {
      digitalWrite(LED_LIGHT_PIN, HIGH);
      isLightOn = true;
      
      Serial.println("[조도센서] 어두움 감지 → LED 켜짐");
    }
  } 
  // 밝으면 LED 끄기
  else {
    if (isLightOn) {
      digitalWrite(LED_LIGHT_PIN, LOW);
      isLightOn = false;
      
      Serial.println("[조도센서] 밝음 감지 → LED 꺼짐");
    }
  }
}

// ========================================
// 적외선센서 처리 함수
// ========================================
void handleIRSensor() {
  // 물체 감지 (센서 값이 낮을 때)
  if (irValue < IR_THRESHOLD) {
    if (!isObjectDetected) {
      digitalWrite(LED_DETECT_PIN, HIGH);
      playDetectionSound();
      isObjectDetected = true;
      
      Serial.println("========================================");
      Serial.println("[적외선센서] 물체 감지!");
      Serial.println("========================================");
    }
  } 
  // 물체 없음
  else {
    if (isObjectDetected) {
      digitalWrite(LED_DETECT_PIN, LOW);
      isObjectDetected = false;
      
      Serial.println("[적외선센서] 물체 사라짐");
    }
  }
}

// ========================================
// 감지 알림음 함수
// ========================================
void playDetectionSound() {
  // 짧은 비프음 2회
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

// ========================================
// 상태 출력 함수
// ========================================
void printStatus() {
  Serial.println("--- 센서 상태 ---");
  
  // 조도센서 값 및 상태
  Serial.print("조도센서: ");
  Serial.print(lightValue);
  Serial.print(" → ");
  
  if (lightValue < DARK_LEVEL) {
    Serial.print("매우 어두움");
  } else if (lightValue < DIM_LEVEL) {
    Serial.print("어두움");
  } else if (lightValue < BRIGHT_LEVEL) {
    Serial.print("보통");
  } else {
    Serial.print("밝음");
  }
  
  Serial.print(" (LED: ");
  Serial.print(isLightOn ? "ON" : "OFF");
  Serial.println(")");
  
  // 적외선센서 값 및 상태
  Serial.print("적외선센서: ");
  Serial.print(irValue);
  Serial.print(" → ");
  Serial.println(isObjectDetected ? "물체 감지됨" : "물체 없음");
  
  Serial.println("------------------");
  Serial.println();
}

// ========================================
// 밝기 레벨 반환 함수
// ========================================
String getLightLevel(int value) {
  if (value < DARK_LEVEL) {
    return "매우 어두움";
  } else if (value < DIM_LEVEL) {
    return "어두움";
  } else if (value < BRIGHT_LEVEL) {
    return "보통";
  } else {
    return "밝음";
  }
}

// ========================================
// 거리 추정 함수 (적외선센서)
// ========================================
String getDistance(int value) {
  if (value < 200) {
    return "매우 가까움 (< 5cm)";
  } else if (value < 400) {
    return "가까움 (5~10cm)";
  } else if (value < 600) {
    return "보통 (10~20cm)";
  } else {
    return "멀리 (> 20cm)";
  }
}
