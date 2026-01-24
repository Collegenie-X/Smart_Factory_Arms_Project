/**
 * 로봇 팔 - 1단계: 서보 모터 동작 테스트
 * 
 * 4개 서보 모터를 순차적으로 테스트
 * 핀: 4(베이스), 5(팔1), 6(팔2), 7(그립)
 */

#include <Servo.h>

Servo servo[4];
int pin[4] = {4, 5, 6, 7};
int angles[4] = {90, 80, 90, 30};  // 초기 각도: 베이스, 팔꿈치, 손목, 그립(열린 상태)

void setup() {
  Serial.begin(9600);
  Serial.println("서보 모터 테스트 시작");
  
  // 서보 연결 및 초기화
  for (int i = 0; i < 4; i++) {
    servo[i].attach(pin[i]);
    servo[i].write(angles[i]);
  }
  
  delay(1000);
}

void loop() {
  // 베이스 회전 테스트
  Serial.println("베이스: 50 → 130");
  for (int angle = 50; angle <= 130; angle++) {
    servo[0].write(angle);
    delay(10);
  }
  delay(500);
  servo[0].write(90);
  delay(1000);
  
  // 팔1 테스트
  Serial.println("팔꿈치: 70 → 120");
  for (int angle = 70; angle <= 120; angle++) {
    servo[1].write(angle);
    delay(10);
  }
  delay(500);
  servo[1].write(90);
  delay(1000);
  
  // 팔2 테스트
  Serial.println("손목: 60 → 120");
  for (int angle = 60; angle <= 120; angle++) {
    servo[2].write(angle);
    delay(10);
  }
  delay(500);
  servo[2].write(90);
  delay(1000);
  
  // 그립 테스트
  Serial.println("그립: 5 → 50");
  for (int angle = 5; angle <= 50; angle++) {
    servo[3].write(angle);
    delay(10);
  }
  delay(500);
  servo[3].write(5);
  delay(2000);
  
  Serial.println("테스트 완료\n");
}
