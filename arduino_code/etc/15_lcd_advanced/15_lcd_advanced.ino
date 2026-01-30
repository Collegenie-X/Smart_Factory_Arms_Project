/*
 * ========================================
 * LCD1602 고급 기능 예제
 * ========================================
 * 
 * 기능:
 * - 커스텀 문자 생성
 * - 스크롤 텍스트
 * - 진행률 바
 * - 애니메이션 효과
 * - 메뉴 시스템
 * 
 * 하드웨어:
 * - LCD1602 I2C 모듈
 * - 버튼 3개 (위, 아래, 선택)
 * 
 * 작성일: 2026-01-30
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ========================================
// LCD 설정
// ========================================
#define LCD_ADDRESS  0x27
#define LCD_COLS     16
#define LCD_ROWS     2

// ========================================
// 버튼 핀 정의
// ========================================
#define BTN_UP       2
#define BTN_DOWN     3
#define BTN_SELECT   4

// ========================================
// 전역 변수
// ========================================
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

int menuIndex = 0;
int maxMenuItems = 5;

// ========================================
// 커스텀 문자 정의
// ========================================
// 하트
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

// 벨
byte bell[8] = {
  0b00100,
  0b01110,
  0b01110,
  0b01110,
  0b11111,
  0b00000,
  0b00100,
  0b00000
};

// 온도계
byte thermometer[8] = {
  0b00100,
  0b01010,
  0b01010,
  0b01110,
  0b01110,
  0b11111,
  0b11111,
  0b01110
};

// 물방울
byte droplet[8] = {
  0b00100,
  0b00100,
  0b01010,
  0b01010,
  0b10001,
  0b10001,
  0b10001,
  0b01110
};

// 체크
byte checkMark[8] = {
  0b00000,
  0b00001,
  0b00011,
  0b10110,
  0b11100,
  0b01000,
  0b00000,
  0b00000
};

// 스피커
byte speaker[8] = {
  0b00001,
  0b00011,
  0b01111,
  0b01111,
  0b01111,
  0b00011,
  0b00001,
  0b00000
};

// 진행률 바 (왼쪽)
byte progressLeft[8] = {
  0b11111,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b11111,
  0b00000
};

// 진행률 바 (가운데)
byte progressMiddle[8] = {
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b00000
};

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  LCD1602 고급 기능 시스템");
  Serial.println("========================================");
  
  // LCD 초기화
  lcd.init();
  lcd.backlight();
  
  // 커스텀 문자 등록
  lcd.createChar(0, heart);
  lcd.createChar(1, bell);
  lcd.createChar(2, thermometer);
  lcd.createChar(3, droplet);
  lcd.createChar(4, checkMark);
  lcd.createChar(5, speaker);
  lcd.createChar(6, progressLeft);
  lcd.createChar(7, progressMiddle);
  
  // 버튼 핀 설정
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n명령어:");
  Serial.println("  custom - 커스텀 문자");
  Serial.println("  scroll - 스크롤 텍스트");
  Serial.println("  progress - 진행률 바");
  Serial.println("  anim - 애니메이션");
  Serial.println("  menu - 메뉴 시스템");
  Serial.println("  sensor - 센서 표시");
  Serial.println("  loading - 로딩 화면");
  Serial.println("========================================");
  Serial.println();
  
  // 시작 화면
  showStartScreen();
}

// ========================================
// 메인 루프
// ========================================
void loop() {
  // Serial 명령 처리
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    handleCommand(command);
  }
}

// ========================================
// 명령 처리 함수
// ========================================
void handleCommand(String cmd) {
  Serial.print("[명령 수신] ");
  Serial.println(cmd);
  
  if (cmd == "custom") {
    showCustomCharacters();
    
  } else if (cmd == "scroll") {
    scrollText();
    
  } else if (cmd == "progress") {
    showProgressBar();
    
  } else if (cmd == "anim") {
    playAnimation();
    
  } else if (cmd == "menu") {
    showMenu();
    
  } else if (cmd == "sensor") {
    showSensorData();
    
  } else if (cmd == "loading") {
    showLoadingScreen();
    
  } else {
    Serial.println("[에러] 알 수 없는 명령어");
  }
}

// ========================================
// 시작 화면 함수
// ========================================
void showStartScreen() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Smart Factory");
  lcd.setCursor(4, 1);
  lcd.print("LCD System");
  delay(2000);
}

// ========================================
// 커스텀 문자 표시 함수
// ========================================
void showCustomCharacters() {
  Serial.println("[표시] 커스텀 문자");
  
  lcd.clear();
  
  // 첫 번째 줄
  lcd.setCursor(0, 0);
  lcd.write(0);  // 하트
  lcd.print(" ");
  lcd.write(1);  // 벨
  lcd.print(" ");
  lcd.write(2);  // 온도계
  lcd.print(" ");
  lcd.write(3);  // 물방울
  lcd.print(" ");
  lcd.write(4);  // 체크
  lcd.print(" ");
  lcd.write(5);  // 스피커
  
  // 두 번째 줄
  lcd.setCursor(0, 1);
  lcd.print("Custom Icons!");
}

// ========================================
// 스크롤 텍스트 함수
// ========================================
void scrollText() {
  Serial.println("[실행] 스크롤 텍스트");
  
  String message = "Smart Factory System - LCD1602 Display Test - ";
  
  for (int offset = 0; offset < message.length(); offset++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    // 16자만 표시
    String visible = message.substring(offset);
    if (visible.length() > 16) {
      visible = visible.substring(0, 16);
    }
    
    lcd.print(visible);
    delay(300);
  }
  
  Serial.println("[완료] 스크롤 종료");
}

// ========================================
// 진행률 바 표시 함수
// ========================================
void showProgressBar() {
  Serial.println("[실행] 진행률 바");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loading...");
  
  for (int progress = 0; progress <= 100; progress += 5) {
    lcd.setCursor(0, 1);
    
    // 진행률 바 그리기 (16칸)
    int filled = (progress * 16) / 100;
    
    for (int i = 0; i < 16; i++) {
      if (i < filled) {
        lcd.write(255);  // 전체 채움
      } else {
        lcd.print(" ");
      }
    }
    
    // 퍼센트 표시
    lcd.setCursor(12, 0);
    lcd.print(progress);
    lcd.print("%  ");
    
    delay(200);
  }
  
  // 완료 메시지
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Complete!");
  lcd.setCursor(7, 1);
  lcd.write(4);  // 체크 마크
  
  delay(2000);
  
  Serial.println("[완료] 진행률 바 종료");
}

// ========================================
// 애니메이션 함수
// ========================================
void playAnimation() {
  Serial.println("[실행] 애니메이션");
  
  // 하트 깜빡임
  for (int i = 0; i < 5; i++) {
    lcd.clear();
    lcd.setCursor(7, 0);
    lcd.write(0);  // 하트
    delay(500);
    
    lcd.clear();
    delay(500);
  }
  
  // 스피너 애니메이션
  char spinner[] = {'|', '/', '-', '\\'};
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Processing...");
  
  for (int i = 0; i < 20; i++) {
    lcd.setCursor(7, 1);
    lcd.print(spinner[i % 4]);
    delay(200);
  }
  
  Serial.println("[완료] 애니메이션 종료");
}

// ========================================
// 메뉴 시스템 함수
// ========================================
void showMenu() {
  Serial.println("[실행] 메뉴 시스템");
  Serial.println("버튼으로 조작하거나 Serial 입력");
  
  String menuItems[] = {
    "1. Sensors",
    "2. Settings",
    "3. About",
    "4. Exit"
  };
  
  menuIndex = 0;
  
  while (true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(menuItems[menuIndex]);
    lcd.setCursor(0, 1);
    if (menuIndex + 1 < maxMenuItems) {
      lcd.print(menuItems[menuIndex + 1]);
    }
    
    delay(200);
    
    // Serial 입력 확인
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == 'u') {
        menuIndex = (menuIndex - 1 + maxMenuItems) % maxMenuItems;
      } else if (c == 'd') {
        menuIndex = (menuIndex + 1) % maxMenuItems;
      } else if (c == 's') {
        Serial.print("선택: ");
        Serial.println(menuItems[menuIndex]);
        break;
      }
    }
  }
  
  Serial.println("[완료] 메뉴 시스템 종료");
}

// ========================================
// 센서 데이터 표시 함수
// ========================================
void showSensorData() {
  Serial.println("[실행] 센서 데이터 표시 (10초)");
  
  for (int i = 0; i < 100; i++) {
    // 임의 센서 값
    int temp = random(20, 30);
    int humidity = random(40, 80);
    
    lcd.clear();
    
    // 첫 번째 줄: 온도
    lcd.setCursor(0, 0);
    lcd.write(2);  // 온도계 아이콘
    lcd.print(" ");
    lcd.print(temp);
    lcd.print("C");
    
    // 두 번째 줄: 습도
    lcd.setCursor(0, 1);
    lcd.write(3);  // 물방울 아이콘
    lcd.print(" ");
    lcd.print(humidity);
    lcd.print("%");
    
    // 상태 표시
    lcd.setCursor(10, 0);
    if (temp < 25) {
      lcd.print("COOL");
    } else {
      lcd.print("WARM");
    }
    
    lcd.setCursor(10, 1);
    if (humidity < 60) {
      lcd.print("DRY");
    } else {
      lcd.print("HUMID");
    }
    
    delay(100);
  }
  
  Serial.println("[완료] 센서 표시 종료");
}

// ========================================
// 로딩 화면 함수
// ========================================
void showLoadingScreen() {
  Serial.println("[실행] 로딩 화면");
  
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Loading");
  
  for (int i = 0; i < 20; i++) {
    lcd.setCursor(10 + (i % 4), 0);
    lcd.print(".");
    
    lcd.setCursor(6, 1);
    lcd.print(i * 5);
    lcd.print("%");
    
    delay(200);
  }
  
  Serial.println("[완료] 로딩 화면 종료");
}
