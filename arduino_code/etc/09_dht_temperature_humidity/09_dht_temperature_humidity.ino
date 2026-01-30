/*
 * ========================================
 * DHT11/DHT22 온습도 센서 측정 예제
 * ========================================
 * 
 * 기능:
 * - 온도 및 습도 측정
 * - 불쾌지수 계산
 * - LED로 온도 범위 표시
 * - Serial 모니터로 실시간 출력
 * 
 * 하드웨어:
 * - DHT11 또는 DHT22 센서
 * - LED 3개 (온도 범위 표시)
 * 
 * 라이브러리:
 * - DHT sensor library (Adafruit)
 * 
 * 작성일: 2026-01-30
 */

#include <DHT.h>

// ========================================
// 핀 정의
// ========================================
#define DHT_PIN         2     // DHT 센서 데이터 핀
#define DHT_TYPE        DHT11 // DHT11 또는 DHT22

#define LED_COLD_PIN    3     // 낮은 온도 (파랑)
#define LED_NORMAL_PIN  4     // 적정 온도 (초록)
#define LED_HOT_PIN     5     // 높은 온도 (빨강)

// ========================================
// 상수 정의
// ========================================
#define TEMP_COLD       20.0  // 낮은 온도 임계값 (°C)
#define TEMP_NORMAL     26.0  // 적정 온도 상한 (°C)
#define TEMP_HOT        30.0  // 높은 온도 임계값 (°C)

#define HUMIDITY_LOW    30.0  // 낮은 습도 (%)
#define HUMIDITY_HIGH   70.0  // 높은 습도 (%)

#define UPDATE_INTERVAL 2000  // 업데이트 주기 (ms)

// ========================================
// 전역 변수
// ========================================
DHT dht(DHT_PIN, DHT_TYPE);

float temperature = 0;
float humidity = 0;
unsigned long previousMillis = 0;

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  DHT 온습도 센서 측정 시스템");
  Serial.println("========================================");
  
  // DHT 센서 초기화
  dht.begin();
  
  // 핀 모드 설정
  pinMode(LED_COLD_PIN, OUTPUT);
  pinMode(LED_NORMAL_PIN, OUTPUT);
  pinMode(LED_HOT_PIN, OUTPUT);
  
  // 초기 상태
  digitalWrite(LED_COLD_PIN, LOW);
  digitalWrite(LED_NORMAL_PIN, LOW);
  digitalWrite(LED_HOT_PIN, LOW);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n온도 범위:");
  Serial.print("  낮음: < ");
  Serial.print(TEMP_COLD);
  Serial.println("°C (파랑 LED)");
  Serial.print("  적정: ");
  Serial.print(TEMP_COLD);
  Serial.print("~");
  Serial.print(TEMP_NORMAL);
  Serial.println("°C (초록 LED)");
  Serial.print("  높음: > ");
  Serial.print(TEMP_NORMAL);
  Serial.println("°C (빨강 LED)");
  Serial.println("========================================");
  Serial.println();
  
  delay(2000);  // 센서 안정화 대기
}

// ========================================
// 메인 루프
// ========================================
void loop() {
  unsigned long currentMillis = millis();
  
  // 일정 주기마다 측정
  if (currentMillis - previousMillis >= UPDATE_INTERVAL) {
    previousMillis = currentMillis;
    
    // 1. 온습도 읽기
    readSensor();
    
    // 2. LED 제어
    controlLEDs(temperature);
    
    // 3. 결과 출력
    printSensorData();
  }
}

// ========================================
// 센서 읽기 함수
// ========================================
void readSensor() {
  // 온도 읽기 (°C)
  temperature = dht.readTemperature();
  
  // 습도 읽기 (%)
  humidity = dht.readHumidity();
  
  // 읽기 실패 체크
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("[에러] DHT 센서 읽기 실패!");
    temperature = 0;
    humidity = 0;
  }
}

// ========================================
// LED 제어 함수
// ========================================
void controlLEDs(float temp) {
  // 센서 읽기 실패 시
  if (temp == 0) {
    digitalWrite(LED_COLD_PIN, LOW);
    digitalWrite(LED_NORMAL_PIN, LOW);
    digitalWrite(LED_HOT_PIN, LOW);
    return;
  }
  
  // 온도에 따른 LED 제어
  if (temp < TEMP_COLD) {
    // 낮은 온도: 파랑 LED
    digitalWrite(LED_COLD_PIN, HIGH);
    digitalWrite(LED_NORMAL_PIN, LOW);
    digitalWrite(LED_HOT_PIN, LOW);
    
  } else if (temp < TEMP_NORMAL) {
    // 적정 온도: 초록 LED
    digitalWrite(LED_COLD_PIN, LOW);
    digitalWrite(LED_NORMAL_PIN, HIGH);
    digitalWrite(LED_HOT_PIN, LOW);
    
  } else {
    // 높은 온도: 빨강 LED
    digitalWrite(LED_COLD_PIN, LOW);
    digitalWrite(LED_NORMAL_PIN, LOW);
    digitalWrite(LED_HOT_PIN, HIGH);
  }
}

// ========================================
// 센서 데이터 출력 함수
// ========================================
void printSensorData() {
  Serial.println("========================================");
  
  // 온도 출력
  Serial.print("온도: ");
  Serial.print(temperature);
  Serial.print(" °C");
  Serial.print(" → ");
  Serial.println(getTemperatureLevel(temperature));
  
  // 습도 출력
  Serial.print("습도: ");
  Serial.print(humidity);
  Serial.print(" %");
  Serial.print(" → ");
  Serial.println(getHumidityLevel(humidity));
  
  // 불쾌지수 출력
  float discomfortIndex = calculateDiscomfortIndex(temperature, humidity);
  Serial.print("불쾌지수: ");
  Serial.print(discomfortIndex);
  Serial.print(" → ");
  Serial.println(getDiscomfortLevel(discomfortIndex));
  
  // 체감온도 출력
  float heatIndex = calculateHeatIndex(temperature, humidity);
  Serial.print("체감온도: ");
  Serial.print(heatIndex);
  Serial.println(" °C");
  
  Serial.println("========================================");
  Serial.println();
}

// ========================================
// 온도 레벨 반환 함수
// ========================================
String getTemperatureLevel(float temp) {
  if (temp == 0) {
    return "측정 불가";
  } else if (temp < TEMP_COLD) {
    return "춥다";
  } else if (temp < TEMP_NORMAL) {
    return "적정";
  } else if (temp < TEMP_HOT) {
    return "따뜻함";
  } else {
    return "더움";
  }
}

// ========================================
// 습도 레벨 반환 함수
// ========================================
String getHumidityLevel(float humid) {
  if (humid == 0) {
    return "측정 불가";
  } else if (humid < HUMIDITY_LOW) {
    return "건조";
  } else if (humid < HUMIDITY_HIGH) {
    return "적정";
  } else {
    return "습함";
  }
}

// ========================================
// 불쾌지수 계산 함수
// ========================================
float calculateDiscomfortIndex(float temp, float humid) {
  // 불쾌지수 = 0.81 × 온도 + 0.01 × 습도 × (0.99 × 온도 - 14.3) + 46.3
  float di = 0.81 * temp + 0.01 * humid * (0.99 * temp - 14.3) + 46.3;
  return di;
}

// ========================================
// 불쾌지수 레벨 반환 함수
// ========================================
String getDiscomfortLevel(float di) {
  if (di < 68) {
    return "쾌적";
  } else if (di < 75) {
    return "보통";
  } else if (di < 80) {
    return "약간 불쾌";
  } else if (di < 85) {
    return "불쾌";
  } else {
    return "매우 불쾌";
  }
}

// ========================================
// 체감온도 계산 함수 (Heat Index)
// ========================================
float calculateHeatIndex(float temp, float humid) {
  // 간단한 체감온도 계산
  float hi = -8.78469475556 + 
             1.61139411 * temp + 
             2.33854883889 * humid + 
             -0.14611605 * temp * humid + 
             -0.012308094 * temp * temp + 
             -0.0164248277778 * humid * humid;
  
  return hi;
}

// ========================================
// 화씨 변환 함수
// ========================================
float celsiusToFahrenheit(float celsius) {
  return (celsius * 9.0 / 5.0) + 32.0;
}

// ========================================
// 이슬점 계산 함수
// ========================================
float calculateDewPoint(float temp, float humid) {
  // 간단한 이슬점 근사 공식
  float a = 17.27;
  float b = 237.7;
  float alpha = ((a * temp) / (b + temp)) + log(humid / 100.0);
  float dewPoint = (b * alpha) / (a - alpha);
  
  return dewPoint;
}
