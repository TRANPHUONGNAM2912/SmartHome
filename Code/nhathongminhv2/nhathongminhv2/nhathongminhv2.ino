//tài khoản blynk
// tk: 
// mk: 
#define BLYNK_TEMPLATE_ID "TMPL6scNnTrzH"
#define BLYNK_TEMPLATE_NAME "NHA THONG MINH IOT"
#include <Adafruit_Fingerprint.h>
#include <Key.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"
#include <ESP32Servo.h>
#include <EEPROM.h> 
#define I2CADDR 0x20
#define PIN_SG90 2 
#define DHTTYPE DHT11
#define MODEM_RX 16
#define MODEM_TX 17
#define mySerial Serial2 
#define EEPROM_SIZE 1
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Servo sg90;
LiquidCrystal_I2C lcd(0x27, 16, 2);  
const int dht1Pin = 4;
const int dht2Pin = 13;
DHT dht1(dht1Pin, DHTTYPE);
DHT dht2(dht2Pin, DHTTYPE);
char auth[] = "df3Oy9bbjrwuDM2YH0uypox3G4zJLrCs"; 
char ssid[] = "tp-link"; 
char pass[] = "anhvu6868"; 
unsigned long lastTime = 0;
const int gasPin = 34; 
const int pir = 12; 
unsigned long startTime;
const unsigned long timeoutDuration = 5000;
int thresholdValue = 500; 
int gasValue = 0;
int alertState = 0; 
const int ledPin1 = 19;  // Chân điều khiển đèn 1
const int ledPin2 = 18;  // Chân điều khiển đèn 2
int fanpin = 27;
const int sw2 = 26;
const int sw3 = 25;
const int sw4 = 33;
const int sw5 = 32;
const int sw6 = 35;
bool prevSw2State = HIGH;
bool prevSw3State = HIGH;
bool prevSw4State = HIGH;
bool prevSw5State = HIGH;
bool prevSw6State = HIGH;
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad_I2C keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR);

char password[5] = "1234";
int a=0;
int check=0;
String inputString = "";
bool stringComplete = false;  
uint8_t id;
uint8_t checkTask = 0; 
int relay = 15;
unsigned long time1 = 0;
int valuepir=0;
boolean check1 = false;
int flag=0,flag1=0,flag3=0,flag2=0,flag4=0;
int count=0;
unsigned long previousMillis = 0;
const long interval = 5000; // Thời gian đèn sáng (5 giây)
bool pirState = false;
unsigned long previousGasMillis = 0;
const long gasInterval = 5000; // Thời gian chân 14 giữ mức HIGH (5 giây)
bool gasState = false;  // Trạng thái của giá trị gas

BlynkTimer timer;
unsigned long previousMillis1 = 0;  // Biến lưu thời điểm LED được bật
const long interval1 = 5000;        // Khoảng thời gian LED sáng (3 giây)
bool ledState1 = LOW;               // Trạng thái của LED

void setup() {
  Serial.begin(115200);
  Wire.begin();
  keypad.begin();
  sg90.setPeriodHertz(50); 
  sg90.attach(PIN_SG90, 500, 2400);
  dht1.begin();
  dht2.begin();
  lcd.init(); 
  lcd.backlight(); 
  sg90.write(180);
  pinMode(pir, INPUT);
  pinMode(14, OUTPUT);
  pinMode(sw2, OUTPUT);
  pinMode(sw3, OUTPUT);
  pinMode(sw4, OUTPUT);
  pinMode(sw5, OUTPUT);
  pinMode(sw6, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(fanpin, OUTPUT);
  pinMode(gasPin, INPUT);
  startTime = millis();
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < timeoutDuration) {
    lcd.setCursor(2,0); 
    lcd.print("DANG KET NOI"); 
    lcd.setCursor(8,1); 
    lcd.print("WIFI");
    Serial.println("Connecting to WiFi...");
    delay(250);
  }
  // Serial.println("Connected to WiFi");
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.begin(auth, ssid, pass);
  }
  if (WiFi.status() != WL_CONNECTED) {
     Serial.println("Failed to connect to WiFi");
    lcd.clear();
    lcd.setCursor(1, 0); 
    lcd.print("KHONG KET NOI");
    lcd.setCursor(3, 1); 
    lcd.print("DUOC WIFI");
    delay(2000);
  }
  Serial.println("Connected to WiFi");

  // Blynk.begin(auth, ssid, pass);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
 
  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  finger.getTemplateCount();
  Serial.print(F("Sensor contains: ")); Serial.print(finger.templateCount); Serial.println(" templates");
  digitalWrite(sw2, HIGH);
  digitalWrite(sw3, HIGH);
  digitalWrite(sw4, HIGH);
  digitalWrite(sw5, HIGH);
  EEPROM.begin(32);
  for (int i = 0; i < 4; i++) {
    password[i] = EEPROM.read(i);
  }
  lcd.setCursor(2,0); 
  lcd.print("NHAP PASS SO"); 
  lcd.setCursor(2,1); 
  lcd.print("HOAC VAN TAY");
  timer.setInterval(1000, sensor);
}
void Seriall() {
    if (Serial.available() > 0) {  // Kiểm tra nếu có dữ liệu Serial
    sg90.write(0);
        // digitalWrite(19, HIGH);    // Bật LED
        previousMillis1 = millis(); // Ghi lại thời điểm hiện tại
        ledState1 = HIGH;          // Cập nhật trạng thái LED
        // Đọc dữ liệu từ Serial để xóa bộ đệm (optional)
        Serial.read();
    }
}
void loop() {
    Blynk.run();
  timer.run();
  unsigned long currentMillis = millis();  // Lấy thời gian hiện tại

    // Kiểm tra nếu LED đang bật và đã qua 3 giây
    if (ledState1 == HIGH && currentMillis - previousMillis1 >= interval1) {
        // digitalWrite(19, LOW);  // Tắt LED
        ledState1 = LOW;        // Cập nhật trạng thái LED
        sg90.write(135);
    }

    Seriall();  // Gọi hàm kiểm tra dữ liệu Serial
 Serial.println(digitalRead(pir));
if(digitalRead(pir) == 1 && pirState == false) {
    digitalWrite(ledPin2, HIGH); // Bật đèn
    previousMillis = millis(); // Ghi lại thời gian hiện tại
    pirState = true; // Đánh dấu đã phát hiện chuyển động
  }

  // Kiểm tra nếu đã hết 5 giây từ lần cuối cảm biến phát hiện chuyển động
  if(pirState == true && millis() - previousMillis >= interval) { 
    digitalWrite(ledPin2, LOW); // Tắt đèn sau 5 giây
    pirState = false; // Đặt lại trạng thái để sẵn sàng phát hiện chuyển động tiếp theo
  }

  phimso();
  vantay();
  led();
  if(flag>3){
    Blynk.virtualWrite(V9, 1);
    delay(100);
    flag=0;
  }
  if(flag<=3){
    Blynk.virtualWrite(V9, 0);
  }
    if(flag4>3){
    Blynk.virtualWrite(V9, 1);
    delay(100);
    flag4=0;
  }
  if(flag4<=3){
    Blynk.virtualWrite(V9, 0);
  }
  
  bool currentSw2State = digitalRead(sw2);
  if (currentSw2State == LOW && prevSw2State == HIGH) {
    if (digitalRead(ledPin1) == HIGH) {
      digitalWrite(ledPin1, LOW);
      delay(100);
      Blynk.virtualWrite(V6, LOW);
    } else {
      digitalWrite(ledPin1, HIGH);
      delay(100);
      Blynk.virtualWrite(V6, HIGH);
    }
  }
  prevSw2State = currentSw2State; 
 
  // bool currentSw3State = digitalRead(sw3);
  // if (currentSw3State == LOW && prevSw3State == HIGH) {
  //   if (digitalRead(ledPin2) == HIGH) {
  //     digitalWrite(ledPin2, LOW);
  //     delay(100);
  //     Blynk.virtualWrite(V7, LOW);
  //   } else {
  //     digitalWrite(ledPin2, HIGH);
  //     delay(100);
  //     Blynk.virtualWrite(V7, HIGH);
  //   }
  // }
  // prevSw3State = currentSw3State; 
  // Serial.println(digitalRead(sw6));
  bool currentSw6State = digitalRead(sw6);
  if (currentSw6State == LOW && prevSw6State == HIGH) {
    if (digitalRead(fanpin) == HIGH) {
      digitalWrite(fanpin, LOW);
      delay(100);
      Blynk.virtualWrite(V8, LOW);
    } else {
      digitalWrite(fanpin, HIGH);
      delay(100);
      Blynk.virtualWrite(V8, HIGH);
    }
  }
  prevSw6State = currentSw6State; 
}
void sensor(){

  gasValue = analogRead(gasPin);
  Blynk.virtualWrite(V4, gasValue); 
  if(gasValue >= thresholdValue && gasState == false) {
    digitalWrite(14, HIGH); // Đặt chân 14 ở mức HIGH
    previousGasMillis = millis(); // Ghi lại thời gian hiện tại
    gasState = true; // Đánh dấu đã đạt ngưỡng gas
  }

  // Kiểm tra nếu đã hết 5 giây từ lúc giá trị gas đạt ngưỡng
  if(gasState == true && millis() - previousGasMillis >= gasInterval) {
    digitalWrite(14, LOW); // Đặt chân 14 ở mức LOW sau 5 giây
    gasState = false; // Đặt lại trạng thái để sẵn sàng cho lần đo tiếp theo
  }

  float humidity1 = dht1.readHumidity();
  float temperature1 = dht1.readTemperature();
  float humidity2 = dht2.readHumidity();
  float temperature2 = dht2.readTemperature();
  Blynk.virtualWrite(V0, temperature1);
  Blynk.virtualWrite(V1, humidity1);
  Blynk.virtualWrite(V2, temperature2);
  Blynk.virtualWrite(V3, humidity2);
}


BLYNK_WRITE(V5) {
  thresholdValue = param.asInt(); 
}
BLYNK_WRITE(V6) {
  int led1state = param.asInt(); 
  digitalWrite(ledPin1, led1state);
}
BLYNK_WRITE(V7) {
  int led2state = param.asInt(); 
  digitalWrite(ledPin2, led2state);
}
BLYNK_WRITE(V8) {
  int fanstate = param.asInt(); 
  digitalWrite(fanpin, fanstate);
}

void phimso(){
  char key = keypad.getKey();
  
  if (key) {
      
    a++;
    Serial.println(key);
    if(a==1){
      lcd.clear();
    }  
    lcd.setCursor(a+4,0);
    lcd.print("*"); 
    if(a==4){
      lcd.clear();
      lcd.setCursor(2,0); 
      lcd.print("MAT KHAU SAI"); 
      lcd.setCursor(2,1); 
      lcd.print("HAY NHAP LAI");
      a=0;
    }
    if (key == 'C') {
      checkpass();
      changePassword();
    } 
    if (key == 'A') {
    checkpass();
    // Kiểm tra vân tay mới có khớp với vân tay trong cơ sở dữ liệu không
    Serial.println("Checking fingerprint...");
    
      // Nếu vân tay mới khớp, bắt đầu quá trình thêm vân tay mới
      Serial.println("Fingerprint matched. Adding new fingerprint...");
      checkTask = 1;
     
    } else if (key == 'D') {
      checkpass();
      // Xoá tất cả dấu vân tay
      Serial.println("Deleting all fingerprints...");
      lcd.clear();
      lcd.setCursor(2,0); 
      lcd.print("DELETE ALL"); 
      lcd.setCursor(2,1); 
      lcd.print("FINGERPRINTS");
      delay(1000);
      finger.emptyDatabase();
      Serial.println("Now database is empty");
    }
    else if (checkPassword(key)==true) {
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("OPEN DOOR");
      Blynk.virtualWrite(V0, 1);
      delay(1000);
      
      a=0;
      lcd.setCursor(2,0); 
      lcd.print("NHAP PASS SO"); 
      lcd.setCursor(2,1); 
      lcd.print("HOAC VAN TAY"); 
      sg90.write(0);
      
      Serial.println("a");
      check=1;
      lastTime = millis();
    }
  }
  // Serial.println(millis()-lastTime);
  if (check==1 && millis() - lastTime >= 3000) {
      sg90.write(135);
      Blynk.virtualWrite(V0, 0);
      check =0;
      Serial.println("tatled");
    }
}
bool checkPassword(char key) {
  static int index = 0;
  if (key == password[index]) {
    index++;
    if (index == 4) {
      index = 0; 
      return true; 
      flag=0;
    }
  } else {
    index = 0; 
    count++;
    if(count==4){
      flag++;
      count=0;
    }
    if(flag>4){
      flag=1;
    }
  }
  return false;
}
void checkpass(){
    bool passwordAccepted = false;
  while (!passwordAccepted) {
    Serial.println("Enter current password:");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CURRENT PASSWORD:"); 
   
    char entered_password[5] = {'\0', '\0', '\0', '\0', '\0'};

    // Nhập mật khẩu cũ
    for (int i = 0; i < 4; i++) {
      char key = keypad.getKey();
      while (key == NO_KEY) {
        key = keypad.getKey();
      }
      entered_password[i] = key;
      Serial.print('*');
      lcd.setCursor(i+5,1);
      lcd.print("*"); 
    }

    // Kiểm tra mật khẩu cũ
    if (checkEnteredPassword(entered_password)) {
      passwordAccepted = true;
    } else {
      Serial.println("\nIncorrect password. Please try again.");
    }
  }
}
void changePassword() {

  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("NEW PASSWORD:"); 
  Serial.println("Enter new password:");
  char new_password[5] = {'\0', '\0', '\0', '\0', '\0'};
  for (int i = 0; i < 4; i++) {
    char key = keypad.getKey();
    while (key == NO_KEY) {
      key = keypad.getKey();
      // Serial.println(i);
      if(i>0){
      lcd.setCursor(i+5,1);
      lcd.print("*"); 
      }
    }
    new_password[i] = key;
    Serial.print('*'); 

  }
  if (new_password[0] != '\0') {
    for (int i = 0; i < 4; i++) {
      password[i] = new_password[i];
      EEPROM.write(i, password[i]);
      EEPROM.commit();
    }
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("DA THAY DOI"); 
    lcd.setCursor(4,1);
    lcd.print("MAT KHAU"); 
    Blynk.virtualWrite(V1, 0);
    delay(1000);
    a=0;
    lcd.setCursor(2,0); 
    lcd.print("NHAP PASS SO"); 
    lcd.setCursor(2,1); 
    lcd.print("HOAC VAN TAY");
    Serial.println("\nPassword changed successfully!");
    flag2=0;
  } else {
    lcd.clear();
    lcd.setCursor(6,0);
    lcd.print("LOI");
    delay(1000);
    Serial.println("\nPassword change canceled.");
  }
}
bool checkEnteredPassword(char* entered_password) {
  for (int i = 0; i < 4; i++) {
    if (entered_password[i] != password[i]) {
      flag++;
      return false;
    }
  }
  return true;
  flag=0;
}
void vantay(){
  getFingerprintID();
  delay(50);            //don't ned to run this at full speed.
  if(checkTask == 1) //Add template fingerprint
  {
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("NHAP ID VAN TAY"); 
    lcd.setCursor(6,1); 
    lcd.print("ID=");
    id = readnumber();
    lcd.setCursor(9,1); 
    lcd.print(id); 
    checkTask = 0;
    Blynk.virtualWrite(V2, 0);
    if (id == 0) {// ID #0 not allowed, try again!
       return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(id);
    while (!  getFingerprintEnroll() )
    {
    }
    checkTask = 0;
  }
  else if(checkTask == 2) //check template
  {
    checkTask = 0;
    uint8_t p = finger.loadModel(id);
    if(p == FINGERPRINT_OK)
    {
      Serial.print("ID ");
      Serial.print(id);
      Serial.println(" is exist");
    }
    else
    {
      Serial.print("ID ");
      Serial.print(id);
      Serial.println(" is NO exist");
    }
  }
  else if(checkTask == 3) //delete template
  {
    checkTask = 0;
    if(id == 0) //delete all template
    {
      finger.emptyDatabase();
      Serial.println("Now database is empty");
    }
    else
      deleteFingerprint(id);
  }
}
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a 'X', set a flag so the main loop can
    // do something about it:
    if (inChar == 'X') {
      if(inputString[0] == 'C' && inputString[4] == 'X') //to check ID
      {
        id = (inputString[1] - 48)*100 + (inputString[2] - 48)*10 + (inputString[3] - 48);
        checkTask = 2;
        Serial.print("Check ID...");
        Serial.println(id);
      }
      else if(inputString == "ADDX") //add new fingerprint
      {
        Serial.println("Add fingerprint...");
        checkTask = 1;
      }
      else if(inputString[0] == 'D' && inputString[4] == 'X') //delete fingerprint in memmory
      {
        id = (inputString[1] - 48)*100 + (inputString[2] - 48)*10 + (inputString[3] - 48);
        Serial.print("Delete template ID...");
        Serial.println(id);
        checkTask = 3;
      }
    }
  }
  inputString = "";
}
 
uint8_t readnumber(void) {
  char key;
  uint8_t num = 0;

  while (num == 0) {
    key = keypad.getKey();

    // Nếu key là một số từ '1' đến '9', chuyển đổi thành số nguyên và gán cho biến num
    if (key >= '1' && key <= '9') {
      num = key - '0';  // Chuyển đổi ký tự sang số nguyên
    }
  }

  return num;
}
 
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      // Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  // OK success!
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    flag4=0;

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    flag4++;
    
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
 
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  check1 = true;
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("OPEN DOOR");
  
  sg90.write(0);
  // Serial.println(" Relay ON ");
  // delay(2000);
  // digitalWrite(relay, LOW); // Control relay
  // Serial.println(" Relay OFF ");
  return finger.fingerID;
}
void led(){
  unsigned long time2 = millis();
  // Serial.println(time2-time1);
  if(check1 == true){
    if(time2-time1>=3000){
      Blynk.virtualWrite(V0, 0);
      sg90.write(135);
      time1 = time2;
      check1 = false;
      lcd.clear();
      lcd.setCursor(2,0); 
      lcd.print("NHAP PASS SO"); 
      lcd.setCursor(2,1); 
      lcd.print("HOAC VAN TAY");

    }

  }
  else{
    time1=time2;
  }

}
uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }
 
  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
 
  Serial.println("Remove finger");
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("NHA TAY");
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("DAT LAI CUNG");
  lcd.setCursor(2,1);
  lcd.print("MOT NGON TAY");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }
 
  // OK success!
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

    checkTask = 0;
    lcd.clear();
    lcd.setCursor(2,0); 
    lcd.print("NHAP PASS SO"); 
    lcd.setCursor(2,1); 
    lcd.print("HOAC VAN TAY");
    Blynk.virtualWrite(V2, 0);
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    lcd.clear();
    lcd.setCursor(2,0); 
    lcd.print("DAU VAN TAY"); 
    lcd.setCursor(2,1); 
    lcd.print("KHONG KHOP");
    delay(1000);
    lcd.clear();
    lcd.setCursor(2,0); 
    lcd.print("NHAP PASS SO"); 
    lcd.setCursor(2,1); 
    lcd.print("HOAC VAN TAY");
    return p;
    Serial.println("Fingerprints did not match");
   
    flag3=0;
    checkTask = 0;
    Blynk.virtualWrite(V2, 0);

    // lcd.clear();

    // lcd.setCursor(2,0); 
    // lcd.print("DAU VAN TAY"); 
    // lcd.setCursor(2,1); 
    // lcd.print("KHONG KHOP");
    
    // delay(1000);
    flag3=0;
    checkTask = 0;
    Blynk.virtualWrite(V2, 0);
    lcd.clear();
    lcd.setCursor(2,0); 
    lcd.print("NHAP PASS SO"); 
    lcd.setCursor(2,1); 
    lcd.print("HOAC VAN TAY");
  } else {
    Serial.println("Unknown error");
    return p;
  }
 
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    flag3=0;
    checkTask = 0;
    Blynk.virtualWrite(V2, 0);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  return true;
}
 
 
 
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
  }
  return p;
}
