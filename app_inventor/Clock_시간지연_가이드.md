# Clock 컴포넌트로 시간 지연 구현하기

## ⚠️ 중요: 앱인벤터에는 delay 함수가 없습니다!

많은 초보자들이 찾는 함수들:
```
❌ delay(1000)      - 없음
❌ sleep(1)         - 없음
❌ wait(1000)       - 없음
❌ setTimeout()     - 없음
❌ Thread.sleep()   - 없음
```

**대신 Clock 컴포넌트를 사용합니다!** ✅

---

## Clock 컴포넌트란?

### 기본 개념
- **타이머 역할**: 일정 시간마다 이벤트 발생
- **시간 지연**: delay 대신 사용
- **반복 실행**: 주기적인 작업 수행

### 주요 속성
```
TimerInterval: 시간 간격 (밀리초)
  - 1000 = 1초
  - 500 = 0.5초
  - 2000 = 2초

TimerEnabled: 타이머 활성화
  - true = 타이머 작동
  - false = 타이머 중지
```

### 주요 이벤트
```
Clock.Timer:
  - TimerInterval마다 자동 실행
  - 여기에 반복할 코드 작성
```

---

## 기본 사용법

### 1. 단순 지연 (1회 실행)

#### 목표: 버튼 클릭 → 1초 후 → 명령 실행

```
Designer:
  Clock1:
    - TimerInterval: 1000
    - TimerEnabled: false

Blocks:
  Button_Start.Click:
    set Clock1.TimerEnabled to true
  
  Clock1.Timer:
    set Clock1.TimerEnabled to false  // 중요: 1회만 실행
    call BluetoothClient1.SendText("90,110,150,5_")
```

**동작:**
```
버튼 클릭 → Clock 시작 → 1초 대기 → Timer 이벤트 → Clock 중지 → 명령 실행
```

### 2. 연속 지연 (여러 번 실행)

#### 목표: 버튼 클릭 → 1초 → 2초 → 3초 후 명령 실행

```
Designer:
  Clock1: TimerInterval 1000, TimerEnabled false
  Clock2: TimerInterval 1000, TimerEnabled false
  Clock3: TimerInterval 1000, TimerEnabled false

Blocks:
  Button_Start.Click:
    call BluetoothClient1.SendText("1단계")
    set Clock1.TimerEnabled to true
  
  Clock1.Timer:
    set Clock1.TimerEnabled to false
    call BluetoothClient1.SendText("2단계")
    set Clock2.TimerEnabled to true
  
  Clock2.Timer:
    set Clock2.TimerEnabled to false
    call BluetoothClient1.SendText("3단계")
    set Clock3.TimerEnabled to true
  
  Clock3.Timer:
    set Clock3.TimerEnabled to false
    call BluetoothClient1.SendText("4단계")
```

**동작:**
```
버튼 → 1단계 즉시 → 1초 → 2단계 → 1초 → 3단계 → 1초 → 4단계
```

---

## 시퀀스 제어 패턴

### 패턴 1: 단계별 실행 (권장 ⭐)

#### 장점
- 코드 간결
- 단계 추가 용이
- 유지보수 쉬움

#### 구현
```
전역 변수:
  initialize global step to 0

Button_Start.Click:
  set global step to 1
  call executeStep
  set Clock1.TimerEnabled to true

Clock1.Timer:
  set global step to (global step + 1)
  
  if global step <= 5
  then
    call executeStep
  else
    set Clock1.TimerEnabled to false
    set global step to 0

Procedure executeStep:
  if global step = 1
  then
    call BluetoothClient1.SendText("90,110,150,5_")
  else if global step = 2
  then
    call BluetoothClient1.SendText("90,150,120,5_")
  else if global step = 3
  then
    call BluetoothClient1.SendText("90,150,120,40_")
  else if global step = 4
  then
    call BluetoothClient1.SendText("90,80,60,40_")
  else if global step = 5
  then
    call BluetoothClient1.SendText("180,80,60,40_")
```

**타임라인:**
```
0초: 버튼 클릭 → 1단계 실행 → Clock 시작
1초: Timer 발생 → step=2 → 2단계 실행
2초: Timer 발생 → step=3 → 3단계 실행
3초: Timer 발생 → step=4 → 4단계 실행
4초: Timer 발생 → step=5 → 5단계 실행
5초: Timer 발생 → step=6 → Clock 중지
```

### 패턴 2: 리스트 사용

#### 장점
- 명령 관리 용이
- 동적 추가/삭제 가능
- 재사용 쉬움

#### 구현
```
전역 변수:
  initialize global commands to create empty list
  initialize global index to 0

Screen1.Initialize:
  set global commands to make a list
    item: "90,110,150,5_"
    item: "90,150,120,5_"
    item: "90,150,120,40_"
    item: "90,80,60,40_"
    item: "180,80,60,40_"

Button_Start.Click:
  set global index to 1
  call sendCommand
  set Clock1.TimerEnabled to true

Clock1.Timer:
  set global index to (global index + 1)
  
  if global index <= length of list global commands
  then
    call sendCommand
  else
    set Clock1.TimerEnabled to false
    set global index to 0

Procedure sendCommand:
  set cmd to select list item
    list: global commands
    index: global index
  call BluetoothClient1.SendText(cmd)
```

### 패턴 3: 상태 머신

#### 장점
- 복잡한 로직 처리
- 조건부 분기 가능
- 오류 처리 용이

#### 구현
```
전역 변수:
  initialize global state to "IDLE"
  initialize global counter to 0

Button_Start.Click:
  set global state to "MOVING_TO_OBJECT"
  set global counter to 0
  call executeState
  set Clock1.TimerEnabled to true

Clock1.Timer:
  set global counter to (global counter + 1)
  call executeState

Procedure executeState:
  if global state = "MOVING_TO_OBJECT"
  then
    call BluetoothClient1.SendText("90,150,120,5_")
    set global state to "WAITING_GRAB"
    
  else if global state = "WAITING_GRAB"
  then
    if global counter >= 1
    then
      call BluetoothClient1.SendText("90,150,120,40_")
      set global state to "LIFTING"
      set global counter to 0
      
  else if global state = "LIFTING"
  then
    if global counter >= 1
    then
      call BluetoothClient1.SendText("90,80,60,40_")
      set global state to "DONE"
      
  else if global state = "DONE"
  then
    set Clock1.TimerEnabled to false
    set global state to "IDLE"
```

---

## 고급 기법

### 1. 가변 시간 지연

#### 목표: 단계마다 다른 대기 시간

```
전역 변수:
  initialize global step to 0
  initialize global delays to create empty list

Screen1.Initialize:
  set global delays to make a list
    item: 1000  // 1단계 후 1초
    item: 500   // 2단계 후 0.5초
    item: 2000  // 3단계 후 2초
    item: 1000  // 4단계 후 1초

Button_Start.Click:
  set global step to 1
  call executeStep
  call setNextDelay

Clock1.Timer:
  set global step to (global step + 1)
  
  if global step <= 4
  then
    call executeStep
    call setNextDelay
  else
    set Clock1.TimerEnabled to false

Procedure setNextDelay:
  if global step <= length of list global delays
  then
    set Clock1.TimerInterval to select list item
      list: global delays
      index: global step
    set Clock1.TimerEnabled to true
```

### 2. 조건부 지연

#### 목표: 센서 값에 따라 다음 단계 실행

```
전역 변수:
  initialize global step to 0
  initialize global sensorValue to 0

Button_Start.Click:
  set global step to 1
  call executeStep
  set Clock1.TimerEnabled to true

Clock1.Timer:
  // 센서 값 읽기 (Bluetooth로 받은 값)
  if global sensorValue > 500
  then
    // 조건 만족 → 다음 단계
    set global step to (global step + 1)
    call executeStep
  else
    // 조건 불만족 → 대기 (다시 Timer 발생 대기)
```

### 3. 반복 시퀀스

#### 목표: 시퀀스를 N번 반복

```
전역 변수:
  initialize global step to 0
  initialize global repeatCount to 0
  initialize global maxRepeat to 3

Button_Start.Click:
  set global step to 1
  set global repeatCount to 0
  call executeStep
  set Clock1.TimerEnabled to true

Clock1.Timer:
  set global step to (global step + 1)
  
  if global step > 4
  then
    // 한 사이클 완료
    set global repeatCount to (global repeatCount + 1)
    
    if global repeatCount >= global maxRepeat
    then
      // 모든 반복 완료
      set Clock1.TimerEnabled to false
    else
      // 다음 사이클 시작
      set global step to 1
  
  call executeStep
```

### 4. 병렬 실행

#### 목표: 여러 작업을 동시에 진행

```
Designer:
  Clock_Task1: TimerInterval 1000
  Clock_Task2: TimerInterval 500
  Clock_Task3: TimerInterval 2000

Blocks:
  Button_Start.Click:
    set Clock_Task1.TimerEnabled to true
    set Clock_Task2.TimerEnabled to true
    set Clock_Task3.TimerEnabled to true
  
  Clock_Task1.Timer:
    // 1초마다 실행
    call BluetoothClient1.SendText("task1_")
  
  Clock_Task2.Timer:
    // 0.5초마다 실행
    call updateSensor
  
  Clock_Task3.Timer:
    // 2초마다 실행
    call checkStatus
```

---

## 실전 예제

### 예제 1: 물건 집기 (8단계, 각 1초)

```
전역 변수:
  initialize global step to 0

Button_PickUp.Click:
  set global step to 1
  call doPickUp
  set Clock1.TimerEnabled to true

Clock1.Timer:
  set global step to (global step + 1)
  
  if global step <= 8
  then
    call doPickUp
  else
    set Clock1.TimerEnabled to false
    set global step to 0
    call Notifier1.ShowAlert("완료!")

Procedure doPickUp:
  if global step = 1
  then
    call BluetoothClient1.SendText("90,110,150,5_")
    set Label_Status.Text to "1/8: 대기"
  else if global step = 2
  then
    call BluetoothClient1.SendText("90,150,120,5_")
    set Label_Status.Text to "2/8: 물건 위로"
  else if global step = 3
  then
    call BluetoothClient1.SendText("90,150,120,40_")
    set Label_Status.Text to "3/8: 잡기"
  else if global step = 4
  then
    call BluetoothClient1.SendText("90,80,60,40_")
    set Label_Status.Text to "4/8: 들기"
  else if global step = 5
  then
    call BluetoothClient1.SendText("180,80,60,40_")
    set Label_Status.Text to "5/8: 회전"
  else if global step = 6
  then
    call BluetoothClient1.SendText("180,150,120,40_")
    set Label_Status.Text to "6/8: 내리기"
  else if global step = 7
  then
    call BluetoothClient1.SendText("180,150,120,5_")
    set Label_Status.Text to "7/8: 놓기"
  else if global step = 8
  then
    call BluetoothClient1.SendText("90,110,150,5_")
    set Label_Status.Text to "8/8: 복귀"
```

### 예제 2: 좌우 스캔 (가변 속도)

```
전역 변수:
  initialize global scanStep to 0
  initialize global scanAngles to create empty list
  initialize global scanDelays to create empty list

Screen1.Initialize:
  set global scanAngles to make a list
    item: "0,110,150,5_"
    item: "45,110,150,5_"
    item: "90,110,150,5_"
    item: "135,110,150,5_"
    item: "180,110,150,5_"
  
  set global scanDelays to make a list
    item: 1000  // 0도 → 45도: 1초
    item: 500   // 45도 → 90도: 0.5초
    item: 500   // 90도 → 135도: 0.5초
    item: 1000  // 135도 → 180도: 1초

Button_Scan.Click:
  set global scanStep to 1
  call doScan
  call setNextScanDelay

Clock_Scan.Timer:
  set global scanStep to (global scanStep + 1)
  
  if global scanStep <= length of list global scanAngles
  then
    call doScan
    call setNextScanDelay
  else
    set Clock_Scan.TimerEnabled to false
    set global scanStep to 0

Procedure doScan:
  set angle to select list item
    list: global scanAngles
    index: global scanStep
  call BluetoothClient1.SendText(angle)
  set Label_Status.Text to join("스캔 ", global scanStep, "/5")

Procedure setNextScanDelay:
  if global scanStep < length of list global scanDelays
  then
    set Clock_Scan.TimerInterval to select list item
      list: global scanDelays
      index: global scanStep
    set Clock_Scan.TimerEnabled to true
```

### 예제 3: 색상 분류 (조건부)

```
전역 변수:
  initialize global state to "IDLE"
  initialize global detectedColor to ""

Button_Start.Click:
  set global state to "SCANNING"
  call executeColorSort
  set Clock_Sort.TimerEnabled to true

Clock_Sort.Timer:
  call executeColorSort

Procedure executeColorSort:
  if global state = "SCANNING"
  then
    // 카메라로 색상 감지
    call Camera1.TakePicture
    set global state to "WAITING_DETECTION"
    
  else if global state = "WAITING_DETECTION"
  then
    // 색상 감지 대기 (AI 처리 시간)
    // Camera1.AfterPicture에서 detectedColor 설정
    
  else if global state = "MOVING_TO_BOX"
  then
    if global detectedColor = "RED"
    then
      call BluetoothClient1.SendText("45,110,150,5_")
    else if global detectedColor = "BLUE"
    then
      call BluetoothClient1.SendText("135,110,150,5_")
    set global state to "DROPPING"
    
  else if global state = "DROPPING"
  then
    call BluetoothClient1.SendText("90,150,120,5_")
    set global state to "RETURNING"
    
  else if global state = "RETURNING"
  then
    call BluetoothClient1.SendText("90,110,150,5_")
    set global state to "IDLE"
    set Clock_Sort.TimerEnabled to false

Camera1.AfterPicture:
  call ImageClassifier1.ClassifyImage(image)

ImageClassifier1.GotClassification:
  set global detectedColor to result
  set global state to "MOVING_TO_BOX"
```

---

## 디버깅 팁

### 1. 현재 단계 표시
```
Clock1.Timer:
  set global step to (global step + 1)
  set Label_Debug.Text to join("Step: ", global step)  // 디버그
  call executeStep
```

### 2. 타이머 상태 확인
```
Clock_Check.Timer (매 100ms):
  if Clock1.TimerEnabled
  then
    set Label_Timer.Text to "타이머 작동 중"
    set Label_Timer.BackgroundColor to 초록색
  else
    set Label_Timer.Text to "타이머 중지"
    set Label_Timer.BackgroundColor to 빨간색
```

### 3. 실행 시간 측정
```
전역 변수:
  initialize global startTime to 0

Button_Start.Click:
  set global startTime to Clock1.Now
  // 시퀀스 시작

Clock1.Timer (마지막 단계):
  set elapsed to (Clock1.Now - global startTime)
  set Label_Time.Text to join("소요 시간: ", elapsed, "ms")
```

---

## 주의사항

### ❌ 하지 말아야 할 것

1. **Clock을 끄지 않음**
```
Clock1.Timer:
  call BluetoothClient1.SendText("test_")
  // Clock을 끄지 않으면 계속 반복! ❌
```

2. **너무 짧은 간격**
```
Clock1.TimerInterval: 10  // 너무 빠름! ❌
// 아두이노가 처리 못함
```

3. **무한 루프**
```
Clock1.Timer:
  set global step to (global step + 1)
  // 중지 조건이 없음! ❌
```

### ✅ 해야 할 것

1. **항상 Clock 중지**
```
Clock1.Timer:
  if 완료조건
  then
    set Clock1.TimerEnabled to false  ✅
```

2. **적절한 간격**
```
Clock1.TimerInterval: 1000  // 1초 권장 ✅
```

3. **중지 조건**
```
Clock1.Timer:
  if global step > maxStep
  then
    set Clock1.TimerEnabled to false  ✅
```

---

## 요약

### Clock 사용 4단계
```
1. Clock 컴포넌트 추가
2. TimerInterval 설정 (1000 = 1초)
3. 버튼에서 TimerEnabled = true
4. Timer 이벤트에서 동작 + 중지
```

### 시퀀스 제어 핵심
```
1. 전역 변수 step
2. Timer에서 step++
3. Procedure로 단계 실행
4. 마지막 단계에서 Clock 중지
```

### 권장 패턴
```
✅ 단계별 실행 (if-else)
✅ 리스트 사용 (동적)
✅ 상태 머신 (복잡한 로직)
```

**Clock 마스터하기! ⏰**

