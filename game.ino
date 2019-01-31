/* 
Programa: Minigame arduino
Autor:	Mario Fernández Gómez
*/

#include <LiquidCrystal.h>

#define BUTTONS_PIN A5 //pin for lecture of push buttons 
#define MAP 9 //pin of map pick
#define BUZZER 12 // pin of the active buzzer
#define DEFUSE 13 // pin of the defuse wire

LiquidCrystal lcd(8, 2, 4, 5, 6, 7); // New Object LiquidCrystal. Params: (rs, enable, d4, d5, d6, d7)

//Vars definitions
int count_down = 900; // letf time in seconds
int punishment = 60; // seconds of punishment if answer was wrong
int final_code[10] = {1, 2, 3, 2, 3, 1, 2, 3, 2, 1};
int all_answers[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int phase = 0;
int old_button = -1;
int current_button = -1;

bool last_seconds = true;
bool enter_event = false;


void setup() {
  Serial.begin(9600);   
  lcd.begin(16,2); // Init LCD screen with dimensions.

  pinMode(BUTTONS_PIN,INPUT); //ensure A5 is an input
  digitalWrite(BUTTONS_PIN,LOW); //ensure pullup is off on A5
  
  pinMode(MAP,INPUT);
  digitalWrite(MAP,LOW); //ensure pullup is off on MAP

  pinMode(BUZZER,OUTPUT);//initialize the buzzer pin as an output

  pinMode(DEFUSE, INPUT);
  digitalWrite(DEFUSE, LOW);
  // if u cant see the lcd working try to use potentiometer on contrast p

  String mens = "Para descifrar  el codigo que   desactiva la    bomba deberás   contestar       correctamente   las preguntas.   Preparate!!!!!!!";
  showMsgLCD(mens, 300);
  delay(1000);
  lcd.clear();
  finalSound();
}
void loop() { 

   Serial.print("\n Fase "  + String(phase));
   Serial.print(": event = " + String(enter_event));
    
    lcd.noBlink();
    lcd.setCursor(0,0);
    lcd.print("Time: ");
    printTime();
    delay(1000);

  if( phase != 10 && !enter_event){

    lcd.setCursor(0,1); 
    //lcd.print("Code:");
    printSecondRow("Code:");
    printAllAnswers();

    current_button = getButton();
    Serial.print(" pulsado " + String(current_button));

    if (old_button != current_button && current_button != -1){
     checkAnswer();
    }
  
    old_button = current_button;


  } else {
    switch (phase){
      case 3: //map event
        printSecondRow("MARCA EN EL MAPA");
        if (digitalRead(MAP) == HIGH){
          Serial.print("\n marcado ");
          enter_event = false;
          
          printSecondRow("       OK       ");
          delay(3000);
          
        }else{
          Serial.print("\n NO ");
        }
        //delay(1000);
        break;
      case 5:
        printSecondRow("BUSCA EL OBJETO");
        delay(10000);
        enter_event = false;
        break;
       /*case 8:
        
        break;*/

       case 10:
          if (last_seconds){
            count_down = 20;
            last_seconds = false;
          }
          
          printSecondRow("CORTA EL CABLE!");
          finalSound();
          Serial.print("defuse: " + String(digitalRead(DEFUSE)));
          if (digitalRead(DEFUSE) == LOW){
            delay(5000);
            lcd.clear();
            showMsgLCD("DESACTIVADA", 300);
            delay(5000);
            while(true){
              lcd.clear();
              lcd.print("MIRA DENTRO DEL");
              lcd.setCursor(0,1);
              lcd.print("    TNT");
              delay(100000);
            }
          }
       break;
       default:
       break;
    }
      
  }

  if(count_down > 0){
    count_down--;
  }
  

}

/*
 * Print the cunt_down in format mins:secs
 */
 void printTime() {
  int mins = count_down / 60;
  int secs = count_down % 60;

  if(mins < 10){ //format fix
    lcd.print("0");
  }
  lcd.print(String(mins) + ":");
  if(secs < 10){ //format fix
    lcd.print("0"); 
  }
  lcd.print(String(secs));
 }

int getButton(){
  int button = analogRead(BUTTONS_PIN);
  Serial.print(" read" + String(button)); 
  if (button<600 && button>500) {//3 button pressed
    return 3;  
  } else if (button<450 && button>330) {//2 button pressed
    return 2; 
  } else if (button<300 && button>100) {//1 button pressed
    return 1; }
return -1; //error
}

void checkAnswer(){
  if (final_code[phase] == current_button){
    all_answers[phase] = current_button;
    phase++;

    if (phase == 3 || phase == 5 || phase == 10){
      enter_event = true;
    }
    
  }else{
    count_down -= punishment;
    alarm();
  }
  
}

void printAllAnswers(){
  for (int i=0; i<10; i++){
    if (all_answers[i] == -1){
      i = 10;
    } else {
      lcd.print(all_answers[i]);
    }
  }
  
}

void showMsgLCD(String msg, int temp){
  String row = "";
  lcd.blink();
  int i=0;
  lcd.setCursor(0,0);
  for(int j=0; j<16 && i<msg.length(); j++){
      lcd.print(msg[i]);
      i++;
      delay(temp);
  }
    
  while(i<msg.length()){
    row = "";
    lcd.setCursor(0,1);
    for(int t=0; t<16 && i<msg.length(); t++){
      lcd.print(msg[i]);
      row += msg[i];
      i++;
      delay(temp);
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(row); 
  }

  delay(temp);
}

void alarm(){
  //output an frequency
  for(int i=0;i<80;i++)
  {
    digitalWrite(BUZZER,HIGH);
    delay(3);//wait for 3ms
    digitalWrite(BUZZER,LOW);
    delay(2);//wait for 2ms
    }
    //output another frequency
    for(int i=0;i<100;i++)
    {
      digitalWrite(BUZZER,HIGH);
      delay(2);//wait for 2ms
      digitalWrite(BUZZER,LOW);
      delay(2);//wait for 2ms
    }    
}

void finalSound(){
    digitalWrite(BUZZER,HIGH);
    delay(10);//wait for 3ms
    digitalWrite(BUZZER,LOW);
    delay(10);//wait for 2ms
}

void printSecondRow(String msg){ // msg 16 chars or less only
  lcd.setCursor(0,1);
  lcd.print("                "); //clear second row
  lcd.setCursor(0,1);
  lcd.print(msg);
}
