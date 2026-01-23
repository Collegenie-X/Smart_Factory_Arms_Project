/* 이 소스는 에듀이노(Eduino)에 의해서 번역, 수정, 작성되었고 소유권 또한 에듀이노의 것입니다.
  소유권자의 허락을 받지 않고 무단으로 수정, 삭제하여 배포할 시 법적인 처벌을 받을 수도 있습니다.
*/
#include <SoftwareSerial.h>
#define BT_RXD 3
#define BT_TXD 2
SoftwareSerial bluetooth(BT_RXD, BT_TXD);
char rec_data;
bool rec_chk = false;

void setup() {
  Serial.begin(9600);                             // PC와의 시리얼 통신속도
  bluetooth.begin(9600);                          // 스마트폰 블루투스 통신속도
  pinMode(13, OUTPUT);
}
void loop() {
  if (bluetooth.available()) {                     // 블루투스 명령 수신
    rec_data = bluetooth.read();
    Serial.write(rec_data);
    rec_chk = true;
  }
  if (rec_data == 'A') { 
    digitalWrite(13, HIGH);
  }
  if (rec_data == 'B') { 
    digitalWrite(13, LOW);
  }
}
