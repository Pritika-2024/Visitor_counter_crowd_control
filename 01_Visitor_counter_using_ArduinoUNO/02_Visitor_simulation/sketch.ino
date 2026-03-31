#include<Servo.h>
#include<Keypad.h>
#include <Wire.h>
#include<LiquidCrystal_I2C.h>
#define Entry_sensor 2
#define Exit_sensor 3
#define Servo_pin  9
LiquidCrystal_I2C lcd(0x27,16,2);
Servo myServo;
const byte rows=4;
const byte cols=4;
byte rowIndex[rows]={4,5,6,7};
byte columnIndex[cols]= {8,10,11,12};
char keys[rows][cols]=
{
 {'1', '2','3','A'},
 {'4','5','6','B'},
 {'7','8','9','C'},
 {'*','0','#','D'}
};
Keypad keypad= Keypad(makeKeymap(keys),rowIndex,columnIndex,rows,cols);
void setup(){
  pinMode(Entry_sensor, INPUT);
  Serial.begin(9600);
myServo.attach(Servo_pin);
myServo.write(0);
lcd.init();
lcd.backlight();
lcd.clear();
lcd.setCursor(0,0);
lcd.print("System Ready");
}
bool visitorPresent=false;
bool isIdle=false;
bool exceedseen=false;
bool exit_status=false;
int last_estate=LOW;
const String Password="3925";
String inputPassword="";
const int maxVisitor=4;
int visitor=0;
void loop() { 
  int estate=digitalRead(Exit_sensor);
  if(estate==HIGH && last_estate==LOW && visitor>0) {
    visitor-=1;
    exit_status=true;
    delay(50);
  }
  last_estate=estate; //Decrement visitor only on rising edge
  if(visitor < maxVisitor){ //Check for visitor only when limit not reached
  if(!visitorPresent){
    int state=digitalRead(Entry_sensor);
    if(state==HIGH){ 
  visitorPresent=true;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hey,Visitor");
  lcd.setCursor(0,1);
  lcd.print("Enter Password:");
  isIdle=false; //Go to else only after visitor entered (prevents flickering)
  }
  else{
    if(exit_status)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Visitor left");
    delay(500);
    exit_status=false;
    isIdle=false;
    }
if(!isIdle){
lcd.clear();
lcd.setCursor(0,0);
lcd.print("System Ready");
lcd.setCursor(0,1);
lcd.print("VisitorCount:"+String(visitor));
isIdle=true; //Hold after one else loop execution until visitor enters
}
}
  }


if(visitorPresent){
  char key=keypad.getKey();
if(key){
  if(key=='#'){
    lcd.clear();
    if(inputPassword==Password){
      lcd.setCursor(0,0);
      lcd.print("Access Granted");
      myServo.write(90);
      delay(3000);
      myServo.write(0);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Locked");
      visitor += 1;
    visitorPresent=false; //Check for user only after visitor enters
    }
    else {
      lcd.setCursor(0,0);
      lcd.print("Access denied");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ReEnter passkey:");
    }
    inputPassword="";
  }
  else if (key=='*'){
    lcd.clear();
    inputPassword="";
    lcd.setCursor(0,0);
    lcd.print("Password:");
    }
   else{
    String masked="";
      inputPassword += key;
      for(int i=0;i<inputPassword.length();i++){
      masked += "*";
    }
    while(masked.length()<16) masked += ' ';
    lcd.setCursor(0,1);
    lcd.print(masked);
   }
}
}
exceedseen=false;
}
else{
if(!exceedseen){
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Visitors exceed");
lcd.setCursor(0,1);
lcd.print("Please wait");
exceedseen=true; //Execute this loop once after limit exceed (Prevents Flickering)
}
}
}