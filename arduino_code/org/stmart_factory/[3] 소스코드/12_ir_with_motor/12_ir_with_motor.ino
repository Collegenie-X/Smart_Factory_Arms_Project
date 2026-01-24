#define PIN_DC_DIRECTION 13                           // DC모터(레일) 방향을 정하는 핀
#define PIN_DC_SPEED 11                               // DC모터(레일) 속도를 정하는 핀
#define PIN_IR A0                                     // 적외선 IR센서 연결 핀

const int railSpeed = 100;                            // 레일(DC모터) 속도 선언. 레일 속도 조정 필요 시 이 값을 변경

void setup() {
  pinMode(PIN_DC_DIRECTION, OUTPUT);                  // dc모터의 방향을 제어하는 핀을 output으로 설정
  pinMode(PIN_IR, INPUT);                             // 적외선 센서 핀을 INPUT으로 설정

}

void loop() {
  if (digitalRead(PIN_IR) == LOW) {                 // IR센서가 물체를 인식(IR센서는 값을 인식할 경우 LOW신호를 전달함)
    analogWrite(PIN_DC_SPEED, 0);                   // 레일 작동 시작
  } else {                         
    digitalWrite(PIN_DC_DIRECTION, HIGH);  
    analogWrite(PIN_DC_SPEED, railSpeed);   
  }
  delay(2000);           
}
