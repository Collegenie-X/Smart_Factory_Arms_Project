/**
 * 적외선 센서 테스트 프로그램
 * 
 * 기능: 적외선 센서로 물체 감지
 * - 물체 감지 시 LED 켜기 및 부저 울림
 * - 시리얼 모니터로 상태 출력
 */

/* ===== 핀 번호 설정 ===== */
#define PIN_IR_SENSOR       A0    // 적외선 센서 입력 핀
#define PIN_LED             13    // LED 핀
#define PIN_BUZZER          4     // 부저 핀

/* ===== 설정값 ===== */
#define LOOP_DELAY          100   // 루프 대기 시간(ms)

/* ===== 전역 변수 ===== */
bool lastDetected = false;        // 이전 감지 상태

/**
 * 초기화
 */
void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_IR_SENSOR, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  digitalWrite(PIN_LED, LOW);
  
  Serial.println("========================================");
  Serial.println("  적외선 센서 테스트 시작");
  Serial.println("========================================\n");
}

/**
 * 메인 루프
 */
void loop() {
  // 센서 읽기 (LOW = 감지됨, HIGH = 미감지)
  bool detected = (digitalRead(PIN_IR_SENSOR) == LOW);
  
  // LED 제어
  if (detected) {
    digitalWrite(PIN_LED, HIGH);
    
    // 처음 감지되었을 때만 부저 울림
    if (!lastDetected) {
      tone(PIN_BUZZER, 523, 50);
      Serial.println("[상태 변화] 물체 감지됨");
    }
  } else {
    digitalWrite(PIN_LED, LOW);
    
    if (lastDetected) {
      Serial.println("[상태 변화] 물체 미감지");
    }
  }
  
  lastDetected = detected;
  delay(LOOP_DELAY);
}
