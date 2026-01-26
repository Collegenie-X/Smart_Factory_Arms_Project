#include <SoftwareSerial.h>


//  AT
//  AT+NAME[이름]       ex> AT+NAMEedu01
//  AT+PIN[비밀번호]     ex> AT+PIN1234
//  AT+BAUD[보드레이트 값] ex> AT+BAUD9600

#define BT_TXD 2
#define BT_RXD 3
SoftwareSerial bluetooth(BT_TXD, BT_RXD);

void setup(){
  Serial.begin(9600);
  bluetooth.begin(9600);
}

void loop(){
  if (bluetooth.available()) {
    Serial.write(bluetooth.read());
  }
  if (Serial.available()) {
    bluetooth.write(Serial.read());
  }
}
