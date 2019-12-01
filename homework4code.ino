#include <LiquidCrystal.h>
#include <EEPROM.h>

const int RS = 12;
const int enable = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;
const int columns = 16;
const int lines = 2;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

const int pinX = A1;
const int pinSW = 8;
byte pushed = LOW;
unsigned long long currentTime;
unsigned long long previousXSwitchTime = 0;
unsigned long long previousSwitchTime = 0;
unsigned long long previousShowTime = 0;
unsigned long long previousGameTime = 0;
unsigned long long startingGameTime = 0;

int  selected = 0;
int selectedOptionsNO = 4; //main menu has 3 selectable options, start game has 0, highscore has 0, settings has 2
int currentPage = 0; //0-mainMenu, 1-startGame, 2-highscore, 3-settings
int levelDifficulty = 0;
String playerName = "UNKNOWN";
String highscorer = "NOBODY";
int highscore = 0;

int currentLevel;
int currentScore = 0;
byte gameStarted = 0;
byte gameEnded = 0;

void setup() {
Serial.begin(9600);
lcd.begin(16, 2);
lcd.print("hello, world!");
//incarca din eerpom in player


pinMode(pinX, INPUT);
pinMode(pinSW, INPUT_PULLUP);
}

void switchState(){
  int switchValue = digitalRead(pinSW);
  if(!switchValue && previousSwitchTime + 1000 < currentTime){
    pushed = HIGH;
    previousSwitchTime = currentTime;
  }
  else 
    pushed = LOW;
  
}

void selectedOptionChange(){
    int xValue = analogRead(pinX);

    if(currentPage == 0){
       if(xValue > 600  && previousXSwitchTime + 400 < currentTime){
          if(selected + 2 >= selectedOptionsNO)
            selected = 0;
          else
            ++selected;
          previousXSwitchTime = currentTime;
        }
        
        else
          if(xValue < 400 &&  previousXSwitchTime + 400 < currentTime){
            if(selected - 1 < 0)
              selected = selectedOptionsNO - 1;
            else
              --selected;
            previousXSwitchTime = currentTime;
        }
       
    }
    else
        if(currentPage == 3){
          if(xValue < 400 &&  previousXSwitchTime + 400 < currentTime){
            --levelDifficulty;
            previousXSwitchTime = currentTime;
           }
          if(xValue > 600 &&  previousXSwitchTime + 400 < currentTime){
            ++levelDifficulty;
            previousXSwitchTime = currentTime;
           }
        }

}

void showMenu(){
  if(previousShowTime + 50 >= currentTime) return;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  
  char menu[3][30] = {"StartGame", "Score", "Settings"}; 
  int menuElementsNO = 3;
  int lineFreeSpace = columns;
  int currentLine = 0;
  
  for(int i = 0; i < menuElementsNO; ++i){
     if(strlen(menu[i]) + 1 > lineFreeSpace){ // if the command can't pe printed on the current line, +2 for the space after the command and selected arrow ">"
           lcd.setCursor(0, ++currentLine);//go to the next line in the matrix
           lineFreeSpace = columns;
     }
     
     if(selected == i)lcd.print(">");
     else lcd.print(" ");
     lcd.print(menu[i]);
     lineFreeSpace -= strlen(menu[i]) + 1;
          
  }
  previousShowTime = currentTime;
  
}


void showHighscore(){
  if(previousShowTime + 50 >= currentTime) return;

  lcd.clear();
  lcd.setCursor(0, 0);
  
  lcd.print("Highscore:");
  lcd.print(highscore);
  lcd.setCursor(0, 1);
  lcd.print("By:");
  lcd.print(playerName);
  
  previousShowTime = currentTime;
}



void showSettings(){
  if(previousShowTime + 50 >= currentTime) return;

  if(Serial.available() > 0){
    playerName = "";
    String prevPlayerName = "";
    while(Serial.available() > 0){
      playerName = prevPlayerName;
      char letter;
      letter = (char)Serial.read();
      prevPlayerName += letter;
    }
    
  }

    
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("Nume:");
  lcd.print(playerName);
  lcd.setCursor(0, 1);
  lcd.print("Difficulty:");
  lcd.print(levelDifficulty);

  previousShowTime = currentTime;
}



void startGame(){

  if(gameEnded == HIGH)
    return;
  
  if(gameStarted == HIGH && startingGameTime + 10000 <=currentTime ){
    if( previousShowTime + 50 <= currentTime){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Score:");
          lcd.print((currentLevel + 1) * 3);
          lcd.setCursor(0, 1);
          lcd.print("PushButtonToExit");

          if((currentLevel + 1) * 3 > highscore){
            highscore = (currentLevel + 1) * 3;
            highscorer = playerName;
          }
          
          gameEnded = HIGH;
          previousShowTime = currentTime;
       }
    return;
  }


  
  if(previousShowTime + 50 <= currentTime){
  
  lcd.clear();
  lcd.setCursor(0, 0);
  
  lcd.print("Lives:3 ");
  lcd.print("Level:");
  lcd.print(currentLevel);
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(currentLevel * 3);

  previousShowTime = currentTime;
  
  }

  if(gameStarted == LOW) {
    gameStarted = HIGH;
    previousGameTime = currentTime;
    startingGameTime = currentTime;
    currentLevel = levelDifficulty;
  }
  else
    if(previousGameTime + 5000 <= currentTime){
      ++currentLevel;
      previousGameTime = currentTime;
    }
    
}


void switchMenu(){
  if(pushed == LOW || (gameStarted == HIGH && gameEnded ==LOW))
    return;
    
  switch(currentPage){
    case(0): //mainMenu
      currentPage = selected + 1;
      selected = 0;
      gameStarted = LOW;
      gameEnded = LOW;
      break;

    default://startGame
      currentPage = 0;
      selected = 0;
      break;
    
   }
   
   
}

void showPage(){
  
  switch(currentPage){
    case(0):
      showMenu();
      break;
    case(1):
      startGame();
      break;
    case(2):
      showHighscore();
      break;
    case(3):
      showSettings();
      break;
    
  }
  
}

void loop() {
  currentTime = millis();

  switchState();
  selectedOptionChange();
  switchMenu();
  showPage();
}
