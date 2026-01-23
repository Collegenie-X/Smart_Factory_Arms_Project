/**
   적외선 송수신 센서와 서보모터를 함께 응용한 예제
   적외선 센서로부터 물체가 감지되면, 서보모터가 움직이고
   물체가 감지되지 않으면 서보모터가 원상태로 돌아오는 동작을 수행
*/

#include <Servo.h>
#define IRsensor A0                          // 적외선 수신 센서를 연결한 핀
#define SERVO_PIN 9                          // 서보모터와 연결된 보드의 핀 정의

Servo servo;                                 // 서보모터 객체를 생성

void setup() {
  Serial.begin(9600);
  pinMode(IRsensor, INPUT);               // 센서값을 입력으로 설정
  servo.attach(SERVO_PIN);                // 서보모터를 아두이노와 연결
  servo.write(0);                         // 최초 서보모터 각도는 0도
}

void loop() {
  if (digitalRead(IRsensor) == LOW) {
    servo.write(180);
    delay(30);
  }
  else
  servo.write(0);
}
