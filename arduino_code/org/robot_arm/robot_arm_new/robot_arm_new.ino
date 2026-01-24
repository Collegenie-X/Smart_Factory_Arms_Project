      #include <VarSpeedServo.h>
      #include <SoftwareSerial.h>
      #include "LedControlMS.h"  // 8X8 LED Matrix 부품을 사용하기 위한 LedControlMS 라이브러리 포함

      #define TX 2
      #define RX 3
      

      #define HIGH_MOTOR 8  //9V
      #define MID2_MOTOR 7  //9V

      #define MID1_MOTOR 6
      #define LOW_MOTOR 5   //9V


      #define PIEZO 9


      #define DIN 13
      #define CS  12
      #define CLK 11

      
      #define SPEED 255
      #define AUTO_SPEED 10
      
      VarSpeedServo low_servo,mid1_servo,mid2_servo,high_servo; 
      bool auto_on = false;  //자동화 부분 
  
         /*** 
     *  
     *  address 0x20, 0x27, 0x3F
     *  SDA --> A4 , SCL --->A5   
     ****/
    
      
      SoftwareSerial BTSerial(TX,RX);      
      #define NBR_MTX 1 
      LedControl lc = LedControl(DIN,CLK,CS,NBR_MTX); 

      int auto_low = 0;
      int auto_high = 60;
      int auto_mid1 = 130;
      int auto_mid2 = 35; 
      int auto_arc = 120;
      #define MAX 40

      
      char cmd[MAX];
      int cmdIndex;
      char inBTChar; 

   byte e1[] =  //비행기 이모티콘 모양
    {
       B01000010,  
       B01111110,
       B01111110,
       B11011011,
       B01111110,
       B00100100,
       B01111110,
       B11011001
    };
      
  int number = 0;
  
  void setup(){
        Serial.begin(9600);
        BTSerial.begin(9600); 

         
     

      // 8X8 LED Matrix 클래스에  일일히 하나씩 LED 위치 등록하여 초기화 
        for (int i=0; i< NBR_MTX; i++){
        lc.shutdown(i,false);
        lc.setIntensity(i,8);
        lc.clearDisplay(i);
        delay(200);
       }

 
      tone(PIEZO,1000,100);
      Serial.println("START");
   
      shift_string("START",80); 

      auto_on = false;  //자동화 부분 셋팅 
   } 
      
    
  int val = 0; 

  int count_number = 0;
  char dataString[5]={0};
  
  void loop() {
       /* 
        *   "high180_" 값을 보내는 방식... 
        *   "mid180_" 값을 보내는 방식... 
        *   "mid2180_" 값을 보내는 방식... 
        *   "low180_" 값을 보내는 방식... 
        */
         
        if(BTSerial.available()){      
          inBTChar = (char)BTSerial.read();
          if(inBTChar=='_') {
          cmd[cmdIndex] = 0;
          exeCmd();  // 블루투스 값을 가지고 와서 하는 것.. 
          cmdIndex = 0; // reset the cmdIndex
          Serial.println(cmd);
          } else {      
          cmd[cmdIndex] = inBTChar;
          if(cmdIndex<MAX-1) cmdIndex++;
         }

         
         printByte(e1);  
    } //if BTSerial closed



//         if(auto_on){
//           auto_motor(); 
//         }
//         delay(50);      //loop 자동화.


  } //loop closed
      

                  
    boolean cmdStartsWith(String st) {
            for(int i=0; ; i++) {
              if(st[i]==0) return true;
              if(cmd[i]==0) return false;
              if(cmd[i]!=st[i]) return false;;
            }
            return false;
      }

    
    void exeCmd() {

            
               if( cmdStartsWith("high") ) { // example: if cmd is "high1500"
                 val = atoi(cmd+4); 
                 motor_move(HIGH_MOTOR,val,SPEED);

                
                }else if( cmdStartsWith("mid1") ) { // example: if cmd is "mid11500"

                  val = atoi(cmd+4); 
                  motor_move(MID1_MOTOR,val,SPEED);
                  
                
                }else if ( cmdStartsWith("mid2") ) { // example: if cmd is "mid21500"

             
                
                  val = atoi(cmd+4);  
                  motor_move(MID2_MOTOR,val,SPEED);
                
                } else if ( cmdStartsWith("low") ) { // example: if cmd is "low1500"

                
                
                  val = atoi(cmd+3);  
                  motor_move(LOW_MOTOR,val,SPEED);                
                }else if( cmdStartsWith("SENSOR")){

                  number_bluetooth_send(count_number);
                  shift_string(String(count_number),100);
                  Serial.print("SENSOR:");
                  Serial.println("count"+String(count_number));
                  tone(PIEZO,1000,10);
                  count_number++; 
                }else if(cmdStartsWith("AUTOON")){
                
                  shift_string(String("ON"),50);
                  Serial.println("ON");
                 
                  auto_motor();
                  auto_on = true;
                  
                }else if(cmdStartsWith("AUTOOFF")){
              
                
                  shift_string(String("OFF"),50);
                  Serial.println("OFF");
                  auto_on = false;
                  
                }else if(cmdStartsWith("autolow")){
                  auto_low = atoi(cmd+7);  
                  motor_move(LOW_MOTOR,auto_low,20);   
                  
                }else if(cmdStartsWith("autohigh")){
                  auto_high = atoi(cmd+8);  
                  motor_move(HIGH_MOTOR,auto_high,20);   
                  
                }else if(cmdStartsWith("automid1")){
                  auto_mid1 = atoi(cmd+8);  
                  motor_move(MID1_MOTOR,auto_mid1,20);   
                  
                }else if(cmdStartsWith("automid2")){
                  auto_mid2 = atoi(cmd+8);  
                  motor_move(MID2_MOTOR,auto_mid2,20);   
                }else if(cmdStartsWith("autoarc")){
                  auto_arc = atoi(cmd+7);  
                 
                }

          
        }


     void auto_motor(){


         /** 시작점에서 물건 집기 ****/
         motor_move(MID2_MOTOR,auto_mid2-20,AUTO_SPEED); 
         delay(300);
         
         motor_move(MID1_MOTOR,auto_mid1-15,AUTO_SPEED); 
         delay(200);
         motor_move(HIGH_MOTOR,auto_high+60,AUTO_SPEED); 
         delay(200);
         motor_move(MID2_MOTOR,auto_mid2+5,AUTO_SPEED); 
         motor_move(MID1_MOTOR,auto_mid1,AUTO_SPEED); 
         delay(200);
         motor_move(HIGH_MOTOR,auto_high,AUTO_SPEED+50); 
         delay(200);
         
         
         /*** 물건 집고 올리기 ***/
         motor_move(MID1_MOTOR,abs(auto_mid1-38),AUTO_SPEED); 
         delay(200);
         motor_move(MID2_MOTOR,abs(auto_mid2-20),AUTO_SPEED); 
         delay(200);

         /**** 물체 이동하기 ***/ 
         motor_move(LOW_MOTOR,auto_low+auto_arc,AUTO_SPEED); 

     
         /*** 이동할 위치에서 물건 내리기 ****/
         motor_move(MID1_MOTOR,abs(auto_mid1-20),AUTO_SPEED-5); 
         delay(200);
         motor_move(MID1_MOTOR,auto_mid1-5,AUTO_SPEED-5); 
         motor_move(MID2_MOTOR,auto_mid2-5,AUTO_SPEED); 
         delay(200);
         motor_move(HIGH_MOTOR,auto_high+10,SPEED); 
         motor_move(LOW_MOTOR,auto_low+auto_arc-4,AUTO_SPEED); 
         motor_move(HIGH_MOTOR,auto_high+60,SPEED); 
         motor_move(LOW_MOTOR,auto_low+auto_arc+4,AUTO_SPEED); 
         
         
         motor_move(MID1_MOTOR,abs(auto_mid1-5),AUTO_SPEED); 
        
         
         delay(100); 

         
         /***** 이동할 위치로 이동학기 *****/
         
         motor_move(MID1_MOTOR,abs(auto_mid1-20),AUTO_SPEED); 
         delay(100);
         motor_move(MID2_MOTOR,abs(auto_mid2-20),AUTO_SPEED); 
         delay(100);
         
         motor_move(LOW_MOTOR,auto_low,AUTO_SPEED); 
                 
         shift_string(String("END"),100);
      
     }
     
      void motor_move(int motor_number,int pos,int servo_speed){

                tone(PIEZO,1000,15);
                low_servo.detach();
                mid1_servo.detach();
                
        
              switch(motor_number){
        
                  case LOW_MOTOR: 
                   low_servo.attach(LOW_MOTOR);   // motor 1
                   Serial.println("LOW:" + String(pos));
                   low_servo.write(pos,servo_speed);   
                   low_servo.wait();            

       
                   break;
        
                  case MID1_MOTOR: 
                   mid1_servo.attach(MID1_MOTOR);   // motor 2
                   mid1_servo.write(pos,servo_speed);
                   mid1_servo.wait();            
                   Serial.println("mid1:"+String(pos));
               
                   break;
        
                  case MID2_MOTOR: 
                   mid2_servo.attach(MID2_MOTOR);   // motor 3
                   mid2_servo.write(pos,servo_speed);
                   mid2_servo.wait();
                   Serial.println("mid2:"+String(pos));
                   break;
        
                  case HIGH_MOTOR: 
                   high_servo.attach(HIGH_MOTOR);   // motor 4
                   high_servo.write(pos,servo_speed);
                   high_servo.wait(); 
                   break;
                
              }

              shift_string(String(pos),10);
             
                low_servo.detach();
                mid1_servo.detach();
             
                
     }

  
      void shift_string(String str,int delay_time){  //좌로 불러오기 (Sting/ 대기 시간)
             int count = ((str.length()-1)*6)+1; 
             int i = 0;
                              
             for(int j=count+1; j>-3;j--){  //좌우 바뀌는 것..
                 for(int z=str.length()-1; z>-1;z--){
                         int display_char = lc.getCharArrayPosition(str[z]);
                         displayShiftChar(j-count+(z*6),display_char);                           
                                 }
                    delay(delay_time);
                    lc.clearAll();
                 }
          }
  

         void printByte(byte character [])
            {
              int i = 0;
              for(i=0;i<8;i++)
              {
                lc.setColumn(0,7-i,character[i]);
        
              }
            }
          
  
  
       void displayShiftChar(int matrix, int charIndex) { //문자 좌우로 보여주기
                int count =0; 
                for (int i=matrix; i< matrix+6;i++) {
                    lc.setRow(0,i, alphabetBitmap[charIndex][count++]);
                } 
        }


      void number_bluetooth_send(int number){
        sprintf(dataString, "%02X",number);
        BTSerial.println(dataString); 
        
      }

 
