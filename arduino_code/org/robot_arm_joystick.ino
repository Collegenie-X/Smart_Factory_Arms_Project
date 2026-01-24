/*
조이스틱을 움직일 때 해당 위치값을 시리얼 모니터로 확인
*/
int X = A0;
int Y = A1;

void setup() {
  Serial.begin(9600);
}
 
void loop() {
  Serial.print("X : ");
  Serial.println(analogRead(X));
  Serial.print("Y : ");
  Serial.println(analogRead(Y));
  delay(500);
}
