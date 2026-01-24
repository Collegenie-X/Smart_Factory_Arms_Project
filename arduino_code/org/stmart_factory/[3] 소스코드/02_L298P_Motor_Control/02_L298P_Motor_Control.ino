/**
   정방향으로 모터가 동작하며 속도가 점차 증가.
   정지 및 1초 대기 후 역방향으로 모터의 속도가 점차 증가하는 형태로 동작
*/

int motorSpeedPin = 10;      // 1번(A) 모터 회전속도(speed)
int motorDirectionPin = 12;  // 1번(A) 모터 방향제어(forward, backward)
int value;                   // 모터의 속도를 담는 변수
void setup() {
  pinMode(motorDirectionPin, OUTPUT);  // 방향제어핀을 pinmode_OUTPUT으로 지정
  noTone(4);
}
void loop() {
  digitalWrite(motorDirectionPin, HIGH);       // 정방향으로 모터 제어
  for (value = 0; value <= 255; value += 5) {  // 11행의 delay(30)에 따라 0.03초마다 속도가 0~255까지 5씩 증가
    analogWrite(motorSpeedPin, value);
    delay(30);
  }
  delay(1000);  // 1초 대기

  digitalWrite(motorDirectionPin, LOW);        // 역방향으로 모터 제어
  for (value = 0; value <= 255; value += 5) {  // 18행의 delay(30)에 따라 0.03초마다 속도가 0~255까지 5씩 증가
    analogWrite(motorSpeedPin, value);
    delay(30);
  }
  delay(1000);  // 1초 대기
}
