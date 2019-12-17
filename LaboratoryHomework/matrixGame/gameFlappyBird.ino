#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "LedControl.h"

const int RS = 6;
const int enable = 7;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;
const int columns = 16;
const int lines = 2;
const int pinY = A0;
const int pinX = A1;
const int pinSW = A2;

const int LCDnumberOfRows = 2;
const int LCDnumberOfColumns = 16;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

byte pushed = LOW;
unsigned long long currentTime;
unsigned long long previousXSwitchTime = 0;
unsigned long long previousYSwitchTime = 0;
unsigned long long previousSwitchTime = 0;
unsigned long long previousShowTime = 0;
unsigned long long startingGameTime = 0;

int  selected[4] = {0, 0, 0, 0};
int selectedOptionsNO[4] = {3, 0, 0, 2}; //main menu has 3 selectable options, start game has 0, highscore has 0, settings has 2
int currentPage = 0; //0-mainMenu, 1-startGame, 2-highscore, 3-settings
unsigned int levelDifficulty = 0;
unsigned int lastLevelDifficulty = 0; //IT JUST KEPT GIVING ME AN ERROR, this takes part in the solution
unsigned int highscore;
unsigned int highscoreCopy;
unsigned int currentScore = 0;
String playerName = "UNKNOWN";
String highscorer = "NOBODY";


int xValue = 0;
int yValue = 0;
unsigned int currentLevel;
unsigned int lives = 3;
byte gameStarted = 0;
byte gameEnded = 0;
int lastCheckpoint;


LedControl lc = LedControl(12, 11, 10, 1); //
const unsigned int numberOfRows = 8;
const unsigned int numberOfColumns = 8;
byte digitMatrix[numberOfRows][numberOfColumns] = {
{0, 0, 0, 0, 0, 0, 0, 0}, // 0
{0, 0, 0, 0, 0, 0, 0, 0}, // 1
{0, 0, 0, 0, 0, 0, 0, 0}, // 2
{0, 0, 0, 0, 0, 0, 0, 0}, // 3
{0, 0, 0, 0, 0, 0, 0, 0}, // 4
{0, 0, 0, 0, 0, 0, 0, 0}, // 5
{0, 0, 0, 0, 0, 0, 0, 0}, // 6
{0, 0, 0, 0, 0, 0, 0, 0}, // 7
};


byte sadFace[numberOfRows][numberOfColumns] = {
{0, 0, 0, 0, 0, 0, 0, 0}, // 0
{0, 0, 1, 0, 0, 1, 0, 0}, // 1
{0, 0, 1, 0, 0, 1, 0, 0}, // 2
{0, 0, 0, 0, 0, 0, 0, 0}, // 3
{0, 0, 0, 0, 0, 0, 0, 0}, // 4
{0, 1, 1, 1, 1, 1, 1, 0}, // 5
{1, 0, 0, 0, 0, 0, 0, 1}, // 6
{0, 0, 0, 0, 0, 0, 0, 0}, // 7
};

unsigned long long lastPipeMove = 0;
int velocity;
int pipeColumn;
int pipeTopPixels;
int pipeBotPixels;

unsigned int birdColumn;
unsigned int birdRow;
unsigned long long lastDescend;
unsigned long long lastAscend;
unsigned long long lastDead;
unsigned long long lastPoint;
byte ascending = LOW;
int point = 0;

void setup() {
Serial.begin(9600);
int place = 4;


/*reset eeprom
String temp = "NOBODY";
EEPROM.write(0, int(0));
for(int i = 0; i < temp.length(); ++i)
  EEPROM.write(place++, temp[i]);
EEPROM.write(place, NULL);
 */
 
highscore = EEPROM.read(0);
highscoreCopy = highscore;

String tempName = "";
char x = (char)EEPROM.read(place++);
while(x != NULL){
  tempName += x;
  x = (char)EEPROM.read(place++);
}
highscorer = tempName;


lcd.begin(LCDnumberOfColumns, LCDnumberOfRows);
randomSeed(32139);
pinMode(pinX, INPUT);
pinMode(pinSW, INPUT_PULLUP);
lc.shutdown(0, false);
lc.setIntensity(0, 2);
lc.clearDisplay(0);

}

void switchState(){
  
  int switchValue = digitalRead(pinSW);
  if(!switchValue && previousSwitchTime + 150 < currentTime){
    pushed = HIGH;
    previousSwitchTime = currentTime;
    Serial.println(pushed);
  }
  else 
    pushed = LOW;
  
}

void gameSwitchState(){
  
    int switchValue = digitalRead(pinSW);
      if(!switchValue && previousSwitchTime + 95 < currentTime){
        pushed = HIGH;
        previousSwitchTime = currentTime;
        Serial.println(pushed);
      }
      else 
        pushed = LOW;
        
}


void joystickChange(){
  
        xValue = analogRead(pinX);
        if(xValue > 700  && previousXSwitchTime + 600 < currentTime){
          xValue = 1;
          previousXSwitchTime = currentTime;
        }
        
        else if(xValue < 300 &&  previousXSwitchTime + 600 < currentTime){
            xValue = -1;
            previousXSwitchTime = currentTime;
        }

        yValue = analogRead(pinY);
        if(yValue > 700  && previousYSwitchTime + 600 < currentTime){
          yValue = 1;
          previousYSwitchTime = currentTime;
        }
        
        else if(yValue < 300 &&  previousYSwitchTime + 600 < currentTime){
            yValue = -1;
            previousYSwitchTime = currentTime;
        }
}



void checkIfDead(){
  
  if(lastDead + velocity <= currentTime && digitMatrix[birdRow][birdColumn] == HIGH){
    --lives;
    point = 0;
    lastDead = currentTime;

  }
}


void addPoint(){

   if((lastPoint + velocity * 3 <= currentTime) && (pipeColumn == birdColumn - 1)){
      currentScore = currentScore + point;
      lastPoint = currentTime;
      point = 1;
   }
      
}

void riseBird(){

  if(birdRow > 0){
    --birdRow;
  }
  
  if(ascending == HIGH)
     ascending = LOW;
  else
     ascending = HIGH;
     
  lastAscend = currentTime;
  lastDescend = currentTime;
}


void descendBird(){
  if(birdRow + 1 < numberOfRows)
    ++birdRow;
    
  lastDescend = currentTime;
}


void moveBird(){

  int lastRow = birdRow;
  
  gameSwitchState();
  
  if(pushed == HIGH || (ascending == HIGH && lastAscend + 30 <= currentTime))
    riseBird();

  if(lastDescend + 250 <= currentTime)
    descendBird();

  if(lastRow != birdRow && !digitMatrix[lastRow][birdColumn])
    lc.setLed(0, lastRow, birdColumn, 0);

  
  if(lastRow != birdRow)
    lc.setLed(0, birdRow, birdColumn, 1);

  
}


void spawnPipe(){
  
  pipeColumn = numberOfColumns - 1;
  pipeTopPixels = random(0, numberOfRows - 3);
  pipeBotPixels = numberOfRows - 3 - pipeTopPixels;
  
  for (int i = 0; i <= pipeTopPixels; ++i){
      lc.setLed(0, i, pipeColumn, 1);
      digitMatrix[i][pipeColumn] = 1;
  }
  
  for (int i = 0; i < pipeBotPixels; ++i){
      lc.setLed(0, numberOfRows - i, pipeColumn, 1);
      digitMatrix[numberOfRows - i][pipeColumn] = 1;
  }
  
}


void movePipe(){

  if(pipeColumn - 1 < 0){
    for(int i = 0; i < numberOfRows; ++i){
      digitMatrix[i][0] = 0;
      lc.setLed(0, i, 0, 0);
    }
    
    spawnPipe();
  }
  
  else{
    --pipeColumn;
    
    for(int i = 0; i < numberOfRows; ++i){

      if(!(birdRow == i && birdColumn == pipeColumn))
        lc.setLed(0, i, pipeColumn, digitMatrix[i][pipeColumn + 1]);
        
      if(!(birdRow == i && birdColumn == pipeColumn + 1))
        lc.setLed(0, i, pipeColumn + 1, 0);
        
      digitMatrix[i][pipeColumn] = digitMatrix[i][pipeColumn + 1];
      digitMatrix[i][pipeColumn + 1] = 0;
    }
    
  }

  lastPipeMove = currentTime;
}



void gameOn(){
  
  velocity = 250 - currentLevel * 10;
  if(lastPipeMove + velocity <= currentTime)
     movePipe();
  
  moveBird();

  checkIfDead();

  if(currentScore % 5 == 0 && currentScore != lastCheckpoint)
    ++currentLevel, lastCheckpoint = currentScore;

  addPoint();
}


void startGame(){

  if(gameEnded == HIGH)
    return;

  if(gameStarted == HIGH && lives == 0 ){//END GAME
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Score:");
          lcd.print(currentScore);
          lcd.setCursor(0, 1);
          lcd.print("PushButtonToExit");

          
          for(int i = 0; i < 8; ++i)
            for(int j = 0; j < 8; ++j)
              lc.setLed(0, i, j, sadFace[i][j]);

          if(currentScore > highscore){
            highscoreCopy = currentScore;
            highscore = highscoreCopy;
            highscorer = playerName;
            
            EEPROM.write(0, currentScore);
            int place = 4;
            for(int i = 0; i < highscorer.length(); ++i)
              EEPROM.write(place++, highscorer[i]);
            EEPROM.write(place, NULL);
          }
          
          gameEnded = HIGH;
          previousShowTime = currentTime;
       
       
    return;
  }


  if(previousShowTime + 100 <= currentTime){
  
    lcd.clear();
    lcd.setCursor(0, 0);
    
    lcd.print("Lives:");
    lcd.print(lives);
    lcd.print(" ");
    lcd.print("Lv:");
    lcd.print(currentLevel);
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(currentScore);
 
    previousShowTime = currentTime;
  
  }

  if(gameStarted == LOW) {//start game
    for(int i = 0; i < numberOfRows; ++i)
       for(int j = 0; j < numberOfColumns; ++j)
         lc.setLed(0, i, j, 0),digitMatrix[i][j] = 0;
    
         
    gameStarted = HIGH;
    lives = 3;
    birdRow = numberOfRows / 2;
    birdColumn = 2;
    lastPipeMove = 0;
    lastDescend = 0;
    lastAscend = 0;
    lastDead = 0;
    point = 1;
    velocity = 250 - levelDifficulty * 10;
    pipeColumn = 0;
    lastDead = 0;
    currentLevel = levelDifficulty;
    currentScore = 0;
    lastLevelDifficulty = levelDifficulty;
    highscoreCopy = highscore;
    ascending = LOW;
    lastCheckpoint = 0;
  }

  gameOn();
}

void selectedOptionChange(){
  

    joystickChange(); //reads the joystick X and Y inputs
    if(currentPage == 0)
      if(xValue == 1)
        if(selected[currentPage] + 1 >= selectedOptionsNO[currentPage])
          selected[currentPage] = 0;
        else
          ++selected[currentPage];
          
          
       else{
         if(xValue == -1)
            if(selected[currentPage] - 1 < 0)
              selected[currentPage] = selectedOptionsNO[currentPage] - 1;
            else
              --selected[currentPage];
       }
       
     else
        if(currentPage == 3)
           if(yValue == 1){
             ++levelDifficulty;
             lastLevelDifficulty = levelDifficulty;
           }
           else
             if(yValue == -1 && levelDifficulty > 0){
               --levelDifficulty;
               lastLevelDifficulty = levelDifficulty;
             }


                
    xValue = 0;
    yValue = 0;
}


void showMenu(){
  if(previousShowTime + 50 >= currentTime) return;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  
  char menu[3][30] = {"StartGame", "Score", "Settings"}; 


  
  int first, last, currentLine = 0;
   if(selected[currentPage] >= LCDnumberOfRows - 1){
    first = selected[currentPage] - LCDnumberOfRows + 1;
    last = selected[currentPage];
   }
   else{
    first = 0;
    last = LCDnumberOfRows - 1;
   }

   for (int i = first; i <= last; ++i){
       lcd.setCursor(0, currentLine++);
       if (selected[currentPage] == i)lcd.print(">");
       else lcd.print(" ");
       lcd.print(menu[i]);
       lcd.print(" ");
   }

  previousShowTime = currentTime;
  
}

void showHighscore(){
  if(previousShowTime + 50 >= currentTime) return;

  lcd.clear();
  lcd.setCursor(0, 0);
  
  lcd.print("Highscore:");
  lcd.print(highscore);
  highscore = highscoreCopy;
  lcd.setCursor(0, 1);
  lcd.print("By:");
  lcd.print(highscorer);
  
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
  levelDifficulty = lastLevelDifficulty;

  previousShowTime = currentTime;
}

void switchMenu(){
  if(pushed == LOW || (gameStarted == HIGH && gameEnded ==LOW)) // don't return to menu while the game is playing
    return;
    
  switch(currentPage){
    case(0): //mainMenu
      currentPage = selected[currentPage] + 1; //mergem la pagina selectata acum
      selected[currentPage] = 0;
      gameStarted = LOW;
      gameEnded = LOW;
      break;
      
    default://Main Menu daca este apasat butonul
      currentPage = 0;
      selected[currentPage] = 0;
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

//DACA PRINTEZ SERIAL.PRINTLN FUNCTIONEAZA FIE PUSHED FIE DIFFICULTY
//MARE ATENTIE, nu merg amandoua deodata

void loop() {
  currentTime = millis();
  switchState();
  selectedOptionChange();
  switchMenu();
  showPage();
  levelDifficulty = lastLevelDifficulty;//NECESAR
  highscore = highscoreCopy;
}
