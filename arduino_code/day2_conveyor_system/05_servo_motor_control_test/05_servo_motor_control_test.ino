/**
 * 서보 모터 제어 테스트 프로그램
 * 
 * 기능: 서보 모터 각도 제어
 * - 기본 각도 이동 (0°, 45°, 90°, 135°, 180°)
 * - 부드러운 각도 전환
 * - 제품 분류용 각도 설정
 */

/* ===== 라이브러리 ===== */
#include <Servo.h>

/* ===== 핀 번호 설정 ===== */
#define PIN_SERVO           9     // 서보 모터 핀
#define PIN_BUZZER          4     // 부저 핀

/* ===== 제품 분류 각도 ===== */
#define ANGLE_RED           25    // 빨간색 제품
#define ANGLE_GREEN         90    // 초록색 제품 (중앙)
#define ANGLE_BLUE          155   // 파란색 제품

/* ===== 타이밍 ===== */
#define MOVE_DELAY          1000  // 이동 후 대기(ms)

/* ===== 전역 변수 ===== */
Servo servo;
int currentAngle = 90;

/**
 * 초기화
 */
void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_BUZZER, OUTPUT);
  
  // 서보 초기화
  Serial.println("========================================");
  Serial.println("  서보 모터 테스트 시작");
  Serial.println("========================================");
  Serial.println("서보 초기화 중...");
  
  servo.attach(PIN_SERVO);
  servo.write(90);  // 중앙 위치
  delay(500);
  
  Serial.println("서보 모터 초기화 완료");
  Serial.println("제어 범위: 0도 ~ 180도");
  Serial.println("초기 위치: 90도 (중앙)");
  Serial.println("========================================\n");
  
  tone(PIN_BUZZER, 659, 100);
  delay(2000);
}

/**
 * 메인 루프
 */
void loop() {
  // 테스트 1: 기본 각도 이동
  Serial.println("========================================");
  Serial.println("[테스트 1] 기본 각도 이동");
  Serial.println("========================================");
  
  int angles[] = {0, 45, 90, 135, 180};
  
  for (int i = 0; i < 5; i++) {
    Serial.print("목표 각도: ");
    Serial.print(angles[i]);
    Serial.println("도");
    
    servo.write(angles[i]);
    currentAngle = angles[i];
    delay(MOVE_DELAY);
  }
  
  Serial.println("중앙(90도)으로 복귀");
  servo.write(90);
  currentAngle = 90;
  delay(MOVE_DELAY);
  Serial.println();
  
  delay(2000);
  
  // 테스트 2: 제품 분류 각도
  Serial.println("========================================");
  Serial.println("[테스트 2] 제품 분류 각도");
  Serial.println("========================================");
  
  Serial.print("빨간색 제품 위치: ");
  Serial.print(ANGLE_RED);
  Serial.println("도");
  servo.write(ANGLE_RED);
  delay(1500);
  
  Serial.print("초록색 제품 위치: ");
  Serial.print(ANGLE_GREEN);
  Serial.println("도");
  servo.write(ANGLE_GREEN);
  delay(1500);
  
  Serial.print("파란색 제품 위치: ");
  Serial.print(ANGLE_BLUE);
  Serial.println("도");
  servo.write(ANGLE_BLUE);
  delay(1500);
  
  Serial.println("중앙으로 복귀");
  servo.write(ANGLE_GREEN);
  delay(MOVE_DELAY);
  Serial.println();
  
  delay(2000);
  
  // 테스트 3: 부드러운 스윕
  Serial.println("========================================");
  Serial.println("[테스트 3] 부드러운 스윕");
  Serial.println("========================================");
  
  Serial.println("0도에서 180도로 이동...");
  for (int angle = 0; angle <= 180; angle++) {
    servo.write(angle);
    delay(15);
  }
  Serial.println("도착: 180도");
  delay(1000);
  
  Serial.println("180도에서 0도로 이동...");
  for (int angle = 180; angle >= 0; angle--) {
    servo.write(angle);
    delay(15);
  }
  Serial.println("도착: 0도");
  delay(1000);
  
  Serial.println("중앙으로 복귀");
  for (int angle = 0; angle <= 90; angle++) {
    servo.write(angle);
    delay(10);
  }
  Serial.println();
  
  delay(2000);
  
  // 테스트 4: Detach (전력 절약)
  Serial.println("========================================");
  Serial.println("[테스트 4] Detach 테스트");
  Serial.println("========================================");
  
  Serial.println("45도로 이동");
  servo.write(45);
  delay(1000);
  
  Serial.println("서보 Detach (전력 절약 모드)");
  servo.detach();
  delay(3000);
  
  Serial.println("서보 Re-attach");
  servo.attach(PIN_SERVO);
  delay(500);
  
  Serial.println("90도로 이동");
  servo.write(90);
  delay(1000);
  Serial.println();
  
  Serial.println("모든 테스트 완료. 5초 후 재시작...\n");
  delay(5000);
}
