/*
 * ========================================
 * 압력 센서 (FSR) 측정 예제
 * ========================================
 * 
 * 기능:
 * - FSR (Force Sensitive Resistor) 압력 측정
 * - 압력에 따른 LED 밝기 제어
 * - 압력 레벨 단계별 표시
 * - Serial 모니터로 실시간 출력
 * 
 * 하드웨어:
 * - FSR 압력 센서
 * - LED 3개 (압력 단계 표시)
 * - 저항 10kΩ (풀다운)
 * 
 * 작성일: 2026-01-30
 */

// ========================================
// 핀 정의
// ========================================
#define FSR_PIN         A0    // 압력 센서 아날로그 입력

#define LED_LOW_PIN     3     // 낮은 압력 LED (PWM)
#define LED_MID_PIN     5     // 중간 압력 LED (PWM)
#define LED_HIGH_PIN    6     // 높은 압력 LED (PWM)

// ========================================
// 상수 정의
// ========================================
#define PRESSURE_THRESHOLD_LOW   200   // 낮은 압력 임계값
#define PRESSURE_THRESHOLD_MID   500   // 중간 압력 임계값
#define PRESSURE_THRESHOLD_HIGH  800   // 높은 압력 임계값

#define UPDATE_INTERVAL          100   // 업데이트 주기 (ms)

// ========================================
// 전역 변수
// ========================================
int pressureValue = 0;
int previousPressure = 0;
unsigned long previousMillis = 0;

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  압력 센서 측정 시스템");
  Serial.println("========================================");
  
  // 핀 모드 설정
  pinMode(FSR_PIN, INPUT);
  pinMode(LED_LOW_PIN, OUTPUT);
  pinMode(LED_MID_PIN, OUTPUT);
  pinMode(LED_HIGH_PIN, OUTPUT);
  
  // 초기 상태
  analogWrite(LED_LOW_PIN, 0);
  analogWrite(LED_MID_PIN, 0);
  analogWrite(LED_HIGH_PIN, 0);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n압력 레벨:");
  Serial.println("  없음: 0~200");
  Serial.println("  낮음: 200~500");
  Serial.println("  중간: 500~800");
  Serial.println("  높음: 800~1023");
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
    
    // 1. 압력 값 읽기
    pressureValue = analogRead(FSR_PIN);
    
    // 2. LED 제어
    controlLEDs(pressureValue);
    
    // 3. 상태 출력 (변화가 있을 때만)
    if (abs(pressureValue - previousPressure) > 10) {
      printPressure(pressureValue);
      previousPressure = pressureValue;
    }
  }
}

// ========================================
// LED 제어 함수
// ========================================
void controlLEDs(int pressure) {
  // 압력이 없을 때
  if (pressure < PRESSURE_THRESHOLD_LOW) {
    analogWrite(LED_LOW_PIN, 0);
    analogWrite(LED_MID_PIN, 0);
    analogWrite(LED_HIGH_PIN, 0);
    return;
  }
  
  // 낮은 압력
  if (pressure < PRESSURE_THRESHOLD_MID) {
    int brightness = map(pressure, PRESSURE_THRESHOLD_LOW, PRESSURE_THRESHOLD_MID, 0, 255);
    analogWrite(LED_LOW_PIN, brightness);
    analogWrite(LED_MID_PIN, 0);
    analogWrite(LED_HIGH_PIN, 0);
  }
  // 중간 압력
  else if (pressure < PRESSURE_THRESHOLD_HIGH) {
    int brightness = map(pressure, PRESSURE_THRESHOLD_MID, PRESSURE_THRESHOLD_HIGH, 0, 255);
    analogWrite(LED_LOW_PIN, 255);
    analogWrite(LED_MID_PIN, brightness);
    analogWrite(LED_HIGH_PIN, 0);
  }
  // 높은 압력
  else {
    int brightness = map(pressure, PRESSURE_THRESHOLD_HIGH, 1023, 0, 255);
    brightness = constrain(brightness, 0, 255);
    analogWrite(LED_LOW_PIN, 255);
    analogWrite(LED_MID_PIN, 255);
    analogWrite(LED_HIGH_PIN, brightness);
  }
}

// ========================================
// 압력 출력 함수
// ========================================
void printPressure(int pressure) {
  Serial.print("[압력 측정] 값: ");
  Serial.print(pressure);
  Serial.print(" / 1023");
  
  // 퍼센트 계산
  int percent = map(pressure, 0, 1023, 0, 100);
  Serial.print(" (");
  Serial.print(percent);
  Serial.print("%)");
  
  // 레벨 표시
  Serial.print(" → ");
  Serial.println(getPressureLevel(pressure));
  
  // 그래프 표시
  printPressureBar(pressure);
}

// ========================================
// 압력 레벨 반환 함수
// ========================================
String getPressureLevel(int pressure) {
  if (pressure < PRESSURE_THRESHOLD_LOW) {
    return "압력 없음";
  } else if (pressure < PRESSURE_THRESHOLD_MID) {
    return "낮은 압력";
  } else if (pressure < PRESSURE_THRESHOLD_HIGH) {
    return "중간 압력";
  } else {
    return "높은 압력";
  }
}

// ========================================
// 압력 바 그래프 출력 함수
// ========================================
void printPressureBar(int pressure) {
  Serial.print("  [");
  
  // 50칸 기준 바 그래프
  int barLength = map(pressure, 0, 1023, 0, 50);
  
  for (int i = 0; i < 50; i++) {
    if (i < barLength) {
      Serial.print("=");
    } else {
      Serial.print(" ");
    }
  }
  
  Serial.println("]");
}

// ========================================
// 압력 단위 변환 함수 (예시)
// ========================================
float pressureToForce(int pressure) {
  // FSR 센서 특성에 따라 변환
  // 예시: 대략적인 Newton 단위 변환
  
  if (pressure < PRESSURE_THRESHOLD_LOW) {
    return 0;
  }
  
  // 간단한 선형 변환 (실제로는 비선형)
  float force_N = map(pressure, 0, 1023, 0, 100) / 10.0;
  return force_N;
}

// ========================================
// 압력 출력 (단위 포함)
// ========================================
void printPressureWithUnit(int pressure) {
  float force = pressureToForce(pressure);
  
  Serial.print("[압력] ");
  Serial.print(pressure);
  Serial.print(" → 약 ");
  Serial.print(force);
  Serial.println(" N (추정)");
}

// ========================================
// 평균 압력 측정 함수 (노이즈 제거)
// ========================================
int measureAveragePressure(int samples) {
  long sum = 0;
  
  for (int i = 0; i < samples; i++) {
    sum += analogRead(FSR_PIN);
    delay(5);
  }
  
  return sum / samples;
}
