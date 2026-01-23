/**
 * 블루투스 예제
 *
 * 앱인벤터로 제작한 앱을 통해
 * 1. 현재 컨베이어벨트의 상태 알아오기
 * 2. 가동/정지시키기
 * 3. 제품 분류 결과 알아오기
 * 를 수행할 수 있다.
 */

/* 라이브러리 불러오기 */
#include <Adafruit_NeoPixel.h>  // LED 모듈 라이브러리
#include <Adafruit_TCS34725.h>  // 컬러 센서 라이브러리
#include <Servo.h>              // 서보모터 라이브러리 불러오기
#include <SoftwareSerial.h>     // 블루투스에 이용되는 라이브러리
#include <Wire.h>               // I2C 통신에 사용되는 라이브러리

/* 상수 선언 : 핀 번호, 속도제어, 서보모터의 각도*/
#define PIN_DC_DIRECTION 13  // DC모터(레일) 방향을 정하는 핀(현재 B모터 사용)
#define PIN_DC_SPEED 11      // DC모터(레일) 속도를 정하는 핀(현재 B모터 사용)
#define PIN_SERVO 9          // 서보모터 연결 핀
#define PIN_LED 5            // LED 연결 핀
#define BLUETOOTH_TX 2       // 블루투스 TX
#define BLUETOOTH_RX 3       // 블루투스 RX
#define PIN_IR A0            // 적외선 IR센서 연결 핀

#define POS_RED 35           // 빨간 색 제품을 분류할 서보모터의 각도
#define POS_GREEN 57         // 초록 색 제품을 분류할 서보모터의 각도
#define POS_BLUE 2           // 파란 색 제품을 분류할 서보모터의 각도
#define NUM_PIXELS 3         // 네오픽셀의 픽셀 수: 3

/* 변수 선언 : HW객체, 측정값, 기타 변수, ...*/
Adafruit_TCS34725 TCS = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Servo servo;
Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);
SoftwareSerial bluetooth(BLUETOOTH_RX, BLUETOOTH_TX);

uint16_t clear, red, green, blue;  // 색상 값을 저장할 unsigned short int형 변수 선언
int r, g, b, sum;                  // 색상 값을 사용하기 위한 변환값
int railSpeed = 110;               // 레일 기본 속도, 초기값은 160
bool isRailMoving = true;          // 레일 가동/정지 변수 유지 -> 블루투스로 레일 상태 조정

void setup() {
    /* 모터 설정 */
    pinMode(PIN_DC_DIRECTION, OUTPUT);     // dc모터의 방향을 제어하는 핀을 output으로 설정
    digitalWrite(PIN_DC_DIRECTION, HIGH);  // 방향은 전진. 의도한 방향과 반대일 경우 HIGH -> LOW로 변경
    analogWrite(PIN_DC_SPEED, railSpeed);  // 레일 작동 시작
    servo.attach(PIN_SERVO);               // 서보모터를 아두이노와 연결
    servo.write(0);                        // 초기 서보모터가 가리키는 각도는 0도
    delay(500);                            // 서보모터가 완전히 동작을 끝낸 후 detach를 위해 delay를 부여
    servo.detach();                        // 서보모터와 아두이노 분리

    /* 적외선 센서 설정 */
    pinMode(PIN_IR, INPUT);  // 적외선 센서 핀을 INPUT으로 설정

    /* 컬러 센서 설정 */
    TCS.begin();  // 컬러 센서 작동 시작

    /* LED 모듈 설정 */
    pixels.begin();             // LED 모듈 작동 시작
    pixels.setBrightness(255);  // 밝기(0~255) 설정. 최대 밝기로 출력

    /* 블루투스 설정 */
    bluetooth.begin(9600);  // 9600baud rate로 블루투스 통신 시작
}

void loop() {
    if (bluetooth.available()) {  // 블루투스 통신으로부터 전달 된 값이 있다면
        btHandler();              // btHandler()함수에서 처리
    }

    if (!isRailMoving) {  // 컨베이어벨트가 중지상태라면
        return;           // loop에 대한 return문장은 그 즉시 loop문을 종료하고, 처음부터 loop을 시작하게 함
    }

    /* 제품 적재여부 확인 */
    if (digitalRead(PIN_IR) == HIGH) {  // 적외선 센서는 물건 감지 시 LOW값을 전달. HIGH라는 것은 감지되지 않았음
        return;                         // loop에 대한 return문장은 그 즉시 loop문을 종료하고, 처음부터 loop을 시작하게 함
    }

    analogWrite(PIN_DC_SPEED, 0);               // 적외선 센서에서 제품을 감지하여 일시 정지
    toneDetected();                             // 감지되었을 때 나오는 소리를 부저에 출력
    delay(2000);                                // 2초간 정지
    analogWrite(PIN_DC_SPEED, railSpeed - 20);  // 레일을 컬러센서까지 움직이기 시작

    do {                                              // do-while 반복문을 사용하면 최초 1회는 반드시 실행됨
        TCS.getRawData(&red, &green, &blue, &clear);  // 색상 감지 센서에서 측정 값 받아오기
        r = map(red, 0, 21504, 0, 1000);              // 빨간색 데이터 값
        g = map(green, 0, 21504, 0, 1000);            // 초록색 데이터 값
        b = map(blue, 0, 21504, 0, 1000);             // 파란색 데이터 값
        sum = r + g + b;                              // 제품의 색상이 온전하게 감지되었는지 확인하기
    } while (sum < 20);

    analogWrite(PIN_DC_SPEED, 0);  // 컬러 센서에서 일시 정지
    toneDetected();                // 감지되었을 때 나오는 소리를 부저에 출력
    delay(1500);                   // 1.5초 간 레일 대기

    if (r > g && r > b) {         // 빨간 색 제품
        servo.attach(PIN_SERVO);  // 서보모터를 아두이노와 연결
        servo.write(POS_RED);     // 서보모터를 빨간 색 제품 분류방향으로 회전

        /* LED모듈에 있는 3개의 LED모듈에 모두 빨간색에 해당하는 값(#ff0000) 입력*/
        pixels.setPixelColor(0, pixels.Color(255, 0, 0));
        pixels.setPixelColor(1, pixels.Color(255, 0, 0));
        pixels.setPixelColor(2, pixels.Color(255, 0, 0));

        bluetooth.write('r');  // 블루투스를 통해 스마트폰에 RED 상품 분류된 것을 전달
    }

    else if (g > r && g > b) {    // 초록 색 제품
        servo.attach(PIN_SERVO);  // 서보모터를 아두이노와 연결
        servo.write(POS_GREEN);   // 서보모터를 빨간 색 제품 분류방향으로 회전

        /* LED모듈에 있는 3개의 LED모듈에 모두 초록색에 해당하는 값(#00ff00) 입력*/
        pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        pixels.setPixelColor(1, pixels.Color(0, 255, 0));
        pixels.setPixelColor(2, pixels.Color(0, 255, 0));

        bluetooth.write('g');  // 블루투스를 통해 스마트폰에 GREEN 상품 분류된 것을 전달
    }

    else {                        // 파란 색 제품
        servo.attach(PIN_SERVO);  // 서보모터를 아두이노와 연결
        servo.write(POS_BLUE);    // 서보모터를 빨간 색 제품 분류방향으로 회전

        /* LED모듈에 있는 3개의 LED모듈에 모두 초록색에 해당하는 값(#0000ff) 입력*/
        pixels.setPixelColor(0, pixels.Color(0, 0, 255));
        pixels.setPixelColor(1, pixels.Color(0, 0, 255));
        pixels.setPixelColor(2, pixels.Color(0, 0, 255));

        bluetooth.write('b');  // 블루투스를 통해 스마트폰에 BLUE 상품 분류된 것을 전달
    }
    
    pixels.show();  // 입력한 색상 값을 LED 모듈에 출력

    delay(1500);                           // 측정 결과 표기 후 1.5초 간 레일 대기
    servo.detach();                        // 서보모터와 아두이노 연결 해제
    analogWrite(PIN_DC_SPEED, railSpeed);  // 레일 작동 시작

    delay(1000);  // 다음 제품은 1초 뒤부터 인식 가능
}

/* 블루투스 동작을 수행하는 함수 */
void btHandler() {
    char b = bluetooth.read();  // 블루투스로부터 읽어온 값을 변수에 저장
    bluetooth.flush();          // 나머지 의미없는 통신 값을 정리

    if (b == 's') {                // 's' : 시스템의 현재 상태를 요구
        if (isRailMoving) {        // 레일이 움직이고 있을 때
            bluetooth.write('y');  // "Yes"의 'y'를 스마트폰에 전달
        } else {                   // 레일이 정지상태일 때
            bluetooth.write('n');  // "NO"의 'n'을 스마트폰에 전달
        }

    } else if (b == '0') {             // '0' : 작동중인 스마트 팩토리를 정지
        isRailMoving = false;          // 레일의 움직임 상태를 나타내는 isRailMoving 변수의 값을 false로 바꿈
        analogWrite(PIN_DC_SPEED, 0);  // 레일 정지
        bluetooth.write('n');          // "NO"의 'n'을 스마트폰에 전달

    } else if (b == '1') {                     // '1' : 정지중인 스마트 팩토리를 가동
        isRailMoving = true;                   // 레일의 움직임 상태를 나타내는 isRailMoving 변수의 값을 true로 바꿈
        analogWrite(PIN_DC_SPEED, railSpeed);  // 레일 작동 시작
        bluetooth.write('y');                  // "Yes"의 'y'를 스마트폰에 전달
    }
}   // void btHandler() 

/* 적외선 센서, 색상감지 센서에서 물체를 감지했을 때 나오는 소리를 출력 */
void toneDetected() {
    tone(4, 523, 50);  // '도'에 해당. 0.05초간 출력
    delay(100);        // 0.1초간 대기(출력시간 0.05초 + 대기시간 0.05초 = 0.1초)
    tone(4, 784, 50);  // '미'에 해당. 0.05초간 출력
    delay(100);        // 0.1초간 대기(출력시간 0.05초 + 대기시간 0.05초 = 0.1초)
}   // void toneDetected()
