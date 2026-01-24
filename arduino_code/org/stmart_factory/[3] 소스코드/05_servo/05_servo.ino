/**
   서보모터 동작을 확인하는 예제코드
   시리얼 모니터를 통해 원하는 각도 값을 입력해 서보모터를 제어하는 예제
*/

#include <Servo.h>                      // 서보모터 라이브러리 불러오기
#define SERVO_PIN 9                     // 서보모터와 연결된 보드의 핀 정의
Servo servo;                            // 서보모터 객체를 생성

void setup() {
  Serial.begin(9600);                   // 시리얼 통신 시작
  servo.attach(SERVO_PIN);              // 서보모터를 아두이노와 연결
  servo.write(0);
  delay(500);
}
void loop() {
  if (Serial.available()) {             // 시리얼 모니터에 값이 입력되었다면
    int value = Serial.parseInt();      // 해당 값을 value로 지정
    servo.write(value);
    Serial.println(value);
    delay(100);
  }
}
