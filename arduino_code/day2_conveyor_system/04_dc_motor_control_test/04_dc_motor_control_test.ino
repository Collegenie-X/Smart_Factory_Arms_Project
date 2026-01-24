/**
 * DC 모터 제어 테스트 프로그램
 * 
 * 기능: L298N 드라이버로 DC 모터 제어
 * - 디지털 제어 (ON/OFF)
 * - PWM 속도 제어 (0~255)
 * - 방향 전환 (전진/후진)
 */

/* ===== 핀 번호 설정 ===== */
#define PIN_DIRECTION       13    // 방향 제어 핀
#define PIN_SPEED           11    // 속도 제어 핀 (PWM)
#define PIN_BUZZER          4     // 부저 핀

/* ===== 속도 설정 ===== */
#define SPEED_STOP          0     // 정지
#define SPEED_SLOW          80    // 저속
#define SPEED_MEDIUM        160   // 중속
#define SPEED_FAST          220   // 고속
#define SPEED_MAX           255   // 최대

/* ===== 타이밍 ===== */
#define TEST_TIME           3000  // 테스트 시간(ms)
#define PAUSE_TIME          1000  // 대기 시간(ms)

/**
 * 초기화
 */
void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_DIRECTION, OUTPUT);
  pinMode(PIN_SPEED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  // 초기 상태: 정지
  digitalWrite(PIN_DIRECTION, HIGH);  // 전진 방향
  analogWrite(PIN_SPEED, SPEED_STOP);
  
  Serial.println("========================================");
  Serial.println("  DC 모터 제어 테스트 시작");
  Serial.println("========================================");
  Serial.println("모터 드라이버: L298N");
  Serial.println("========================================\n");
  
  tone(PIN_BUZZER, 523, 100);
  delay(2000);
}

/**
 * 메인 루프
 */
void loop() {
  // 테스트 1: 디지털 제어 (ON/OFF)
  Serial.println("========================================");
  Serial.println("[테스트 1] 디지털 제어 (ON/OFF)");
  Serial.println("========================================");
  
  Serial.println("방향: 전진");
  digitalWrite(PIN_DIRECTION, HIGH);
  
  Serial.println("상태: ON (최대 속도)");
  digitalWrite(PIN_SPEED, HIGH);
  delay(TEST_TIME);
  
  Serial.println("상태: OFF (정지)");
  digitalWrite(PIN_SPEED, LOW);
  delay(TEST_TIME);
  Serial.println();
  
  delay(PAUSE_TIME);
  
  // 테스트 2: PWM 속도 제어
  Serial.println("========================================");
  Serial.println("[테스트 2] PWM 속도 제어");
  Serial.println("========================================");
  
  digitalWrite(PIN_DIRECTION, HIGH);  // 전진
  
  Serial.print("속도: 저속 (");
  Serial.print(SPEED_SLOW);
  Serial.println("/255)");
  analogWrite(PIN_SPEED, SPEED_SLOW);
  delay(TEST_TIME);
  
  Serial.print("속도: 중속 (");
  Serial.print(SPEED_MEDIUM);
  Serial.println("/255)");
  analogWrite(PIN_SPEED, SPEED_MEDIUM);
  delay(TEST_TIME);
  
  Serial.print("속도: 고속 (");
  Serial.print(SPEED_FAST);
  Serial.println("/255)");
  analogWrite(PIN_SPEED, SPEED_FAST);
  delay(TEST_TIME);
  
  Serial.print("속도: 최대 (");
  Serial.print(SPEED_MAX);
  Serial.println("/255)");
  analogWrite(PIN_SPEED, SPEED_MAX);
  delay(TEST_TIME);
  
  Serial.println("정지");
  analogWrite(PIN_SPEED, SPEED_STOP);
  delay(PAUSE_TIME);
  Serial.println();
  
  // 테스트 3: 방향 전환
  Serial.println("========================================");
  Serial.println("[테스트 3] 방향 전환");
  Serial.println("========================================");
  
  Serial.println("방향: 전진");
  digitalWrite(PIN_DIRECTION, HIGH);
  analogWrite(PIN_SPEED, SPEED_MEDIUM);
  delay(TEST_TIME);
  
  Serial.println("정지 (방향 전환 전)");
  analogWrite(PIN_SPEED, SPEED_STOP);
  delay(1000);
  
  Serial.println("방향: 후진");
  digitalWrite(PIN_DIRECTION, LOW);
  analogWrite(PIN_SPEED, SPEED_MEDIUM);
  delay(TEST_TIME);
  
  Serial.println("정지");
  analogWrite(PIN_SPEED, SPEED_STOP);
  delay(PAUSE_TIME);
  Serial.println();
  
  // 테스트 4: 부드러운 가속
  Serial.println("========================================");
  Serial.println("[테스트 4] 부드러운 가속");
  Serial.println("========================================");
  
  digitalWrite(PIN_DIRECTION, HIGH);
  Serial.println("0에서 최대 속도까지 가속...");
  
  for (int speed = 0; speed <= 255; speed += 5) {
    analogWrite(PIN_SPEED, speed);
    if (speed % 50 == 0) {
      Serial.print("속도: ");
      Serial.println(speed);
    }
    delay(50);
  }
  
  Serial.println("최대 속도 도달!");
  delay(2000);
  
  Serial.println("정지");
  analogWrite(PIN_SPEED, SPEED_STOP);
  Serial.println();
  
  Serial.println("모든 테스트 완료. 5초 후 재시작...\n");
  delay(5000);
}
