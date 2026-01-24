/**
   적외선 센서에 물체가 감지되면,
   시리얼 모니터에 "Detected"라는 문장을 출력
*/
int sensor = A0;  // 센서핀은 A0번에 연결
int val;

void setup() {
  Serial.begin(9600);
  pinMode(sensor, INPUT);  // 센서값을 입력으로 설정
  Serial.println("arduino starts");
}

void loop() {
  val = digitalRead(sensor);  // 센서값 읽어옴
  if (val == LOW) {           // IR센서는 LOW ACTIVE로 탐지 시 LOW값을 전송함
    Serial.println("Detected");
    delay(300);
  }
  else
    Serial.println("0");
    delay(300);
}
