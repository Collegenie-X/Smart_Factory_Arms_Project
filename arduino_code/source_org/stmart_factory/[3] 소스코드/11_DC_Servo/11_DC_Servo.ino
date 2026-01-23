/**
 * 자동분류 시스템에 사용되는 서보모터, DC모터를 제어하기
 *
 * 동작순서
 * 1. 정지
 * 2. 레일(DC모터) 동작 후 일시정지
 * 3. 서보모터가 방향 #1을 가리킴
 * 4. 레일(DC모터) 동작 후 정지
 * 5. 서보모터가 방향 #2를 가리킴
 * 6. 레일(DC모터) 동작 후 정지
 * 7. 서보모터가 방향 #3을 가리킴. 이후 1~7반복
 */

#include <Servo.h>                                         // 서보모터 라이브러리 불러오기

#define PIN_DC_DIRECTION 13                                // DC모터(레일) 방향을 정하는 핀 (L298P 쉴드의 MOTOR B포트 핀 사용)
#define PIN_DC_SPEED 11                                    // DC모터(레일) 속도를 정하는 핀 (L298P 쉴드의 MOTOR B포트 핀 사용)
#define PIN_SERVO 9                                        // 서보모터 연결 핀

const int railSpeed = 100;                                 // 레일(DC모터) 속도 선언. 레일 속도 조정 필요 시 이 값을 변경
const int railMoveMillis = 2000;                           // 레일이 움직일 시간 선언(밀리초). 조정 필요 시 이 값을 변경

Servo servo;

void setup() {
    pinMode(PIN_DC_DIRECTION, OUTPUT);                     // dc모터의 방향을 제어하는 핀을 output으로 설정
    servo.attach(PIN_SERVO);                               // 서보모터를 아두이노와 연결
    digitalWrite(PIN_DC_DIRECTION, HIGH);                  // 방향은 전진. 의도한 방향과 반대일 경우 HIGH -> LOW로 변경
    servo.write(0);                                        // 서보모터가 처음 가리키는 위치는 0도로 고정
}
void loop() {
    moveRail();
    servo.write(90);
    delay(1000);

    moveRail();
    servo.write(180);
    delay(1000);
    
    moveRail();
    servo.write(0);
    delay(1000);
}
void moveRail(){
    analogWrite(PIN_DC_SPEED, railSpeed);                   // 레일이 움직이기 시작
    delay(railMoveMillis);                                  // railMoveMillis(=2000ms = 2s)간 움직임
    analogWrite(PIN_DC_SPEED, 0);                           // 정지
    delay(1000);
}
