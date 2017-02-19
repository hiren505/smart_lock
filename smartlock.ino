/*
*  FILE          : smartlock.ino
*  PROJECT       : Final Project
*  PROGRAMMER    : Hirenkumar Tank
*  FIRST VERSION : 2016-02-2
*  DESCRIPTION   : This code demonstrates the working of the smart lock. In this Code the transmission of data
*                  Wirelessly vis BLUETOOTH MODULE from Android APP. The user when first pairs with the bluetooth 
*                  module, he is asked for two options to choose from. 
*                  1. OPEN  
*                  2. RESET.             : user is allowed to reset the code.
*                  if he enters 1 from the mobile, he is again asked for the two options. 
*                  1. OPEN DOOR          : verifies the passcode present in EEPROM and illuminates
*                                          green if passcode is right else illuminates red in order 
*                                          to demonstrate opening and closing of door latch.
*                  2. OPEN PARKING DOOR. : verifies the passcode present in EEPROM and turns servo
*                                          motor 180 degree in order to demonstrate the working
*                                          of a parking door.
*                  this code also keeps track on the no. of time the user enters the wrong code. if he enters wrong
*                  code for three times consicutively. he is blocked and asked for the master passcode in order to 
*                  normally operate this smart lock.
*                  
*                  NOTE : The android Application developed by ME in order to operate smartlock wirelessly via
*                         bluetooth is developed on APP INVENTOR platform online.  
*                  
*                  
*/

//**************************************** HEADER FILES *********************************************************//

#include <LiquidCrystal.h>    //library for 16x2 LCD
#include <EEPROM.h>           //library for storing values in EEPROM
#include <Servo.h>            //library for servo motor

//**************************************** GLOBALS **************************************************************//

const int8_t greenLed = 6;
const int8_t redLed = 7;
Servo shutter;
LiquidCrystal lcd(19, 18, 17, 16, 15, 14);  

//**************************************** FUNCTION PROTOTYPES **************************************************//

int8_t  checkOneOrTwo();
int8_t  verifyCode(int16_t code);
int8_t  verifyMasterCode(int16_t code);
int32_t enterCode();
void    displayEntrOldPassCode();
void    displayWelcome();
void    displayDoorOrShutter();
void    displayEnterCode();
void    displayWrongCode();
void    displayOpeningDoor();
void    resetCode(int16_t newCode);
void    displayEnterNewCode();
void    displayOpeningShutter();
void    displayBlocked();
void    displayYouRGood();
void    openShutter();

//**************************************** Function Declarations ************************************************//

//
// FUNCTION      : displayWelcome()
// DESCRIPTION   : diasplays the welcome message and asks for the task to perform on lcd.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything
//
void displayWelcome()
{
  lcd.setCursor(5, 0);
  lcd.printf("WelCome");
  delay(2000);
  lcd.clear();
  lcd.printf("1. OPEN");
  lcd.setCursor(0, 1);
  lcd.printf("2. Reset code");
}

//
// FUNCTION      : displayDoorOrShutter()
// DESCRIPTION   : diasplays  home and door parking on lcd.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything
//
void displayDoorOrShutter()
{
  lcd.clear();
  lcd.printf("1. Home Door");
  lcd.setCursor(0, 1);
  lcd.printf("2. Parking Door");
}

//
// FUNCTION      : displayEnterCode()
// DESCRIPTION   : diasplays enter four digit code on lcd.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything
//
void displayEnterCode()
{
  lcd.clear();
  lcd.printf("Enter Four Digit");
  lcd.setCursor(7, 1);
  lcd.printf("Code");
}

//
// FUNCTION      : displayEntrOldPassCode()
// DESCRIPTION   : diasplays  enter the old pass code on lcd.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything
//
void displayEntrOldPassCode()
{
  lcd.clear();
  lcd.printf("Enter the old");
  lcd.setCursor(4, 1);
  lcd.printf("Passcode");
}

//
// FUNCTION      : displayOpeningDoor()
// DESCRIPTION   : diasplays opening door on lcd and calls displayWelcome() 
//                 message funnction.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything
//
void displayOpeningDoor()
{
  lcd.clear();
  digitalWrite(greenLed,HIGH);
  digitalWrite(redLed,LOW);
  lcd.printf("Opening door");
  delay(2000);
  digitalWrite(greenLed,LOW);
  lcd.clear();
  displayWelcome();
}


//
// FUNCTION      : displayOpeningShutter()
// DESCRIPTION   : diasplays opening shutter on lcd and calls displayWelcome() 
//                 message funnction.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything
//
void displayOpeningShutter()
{
  lcd.clear();
  digitalWrite(greenLed,HIGH);
  digitalWrite(redLed,LOW);
  lcd.printf("Opening Shutter");
  openShutter();
  //delay(2000);
  digitalWrite(greenLed,LOW);
  lcd.clear();
  displayWelcome();
}


//
// FUNCTION      : displayWrongCode()
// DESCRIPTION   : diasplays wrong code on lcd.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything
//
void displayWrongCode()
{
  lcd.clear();
  digitalWrite(greenLed,LOW);
  digitalWrite(redLed,HIGH);
  lcd.printf("Wrong Code");
  delay(2000);
  digitalWrite(redLed,LOW);
  lcd.clear();
  
}

//
// FUNCTION      : displayEnterNewCode()
// DESCRIPTION   : diasplays enter new code on lcd.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything
//
void displayEnterNewCode()
{
  lcd.clear();
  lcd.printf("Enter New Code");
}


//
// FUNCTION      : displayBlocked()
// DESCRIPTION   : diasplays enter new code on lcd.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything
//
void displayBlocked()
{
  lcd.clear();
  digitalWrite(greenLed,LOW);
  digitalWrite(redLed,HIGH);
  lcd.printf("You are Blocked!");
  lcd.setCursor(0, 1);
  lcd.printf("entr master code");
}


//
// FUNCTION      : displayYouRGood()
// DESCRIPTION   : diasplays you are good.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything
//
void displayYouRGood()
{
  lcd.clear();
  lcd.printf("You are good!!");
  digitalWrite(redLed,LOW);
  digitalWrite(greenLed,HIGH);
  delay(2000);
  digitalWrite(greenLed,LOW);
  lcd.clear();
}

//
// FUNCTION      : openShutter()
// DESCRIPTION   : turns servo motor 180 degree in clockwise
//                 direction waits for 1 sec and turns servo
//                 motor in anti clockwise direction.
// PARAMETERS    : No parameters passsed.
// RETURNS       : Does not return anything.
//
void openShutter()
{
  static int pos = 0;
  for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    shutter.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  delay(1000);
  for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    shutter.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
}

//
// FUNCTION      : checkOneOrTwo()
// DESCRIPTION   : waits for the user to select appropriate option.
// PARAMETERS    : No parameters passed.
// RETURNS       : Returns the entered digit by user else returns -1.
//
int8_t checkOneOrTwo()
{
  if (Serial1.available())
  {
    return (Serial1.parseInt());
  }
  else
  {
    return -1;
  }
}

//
// FUNCTION      : enterCode()
// DESCRIPTION   : takes the value from mobile via BLUETOOTH MODULE.
// PARAMETERS    : No parameters passed.
// RETURNS       : Returns the code entered by the user if the code is enterd 
//                 and returns -1 if code is still not entered by the user.
//
int32_t enterCode()
{
  if (Serial1.available())
  {
    return (Serial1.parseInt());
  }
  else
  {
    return -1;
  }
}

//
// FUNCTION      : verifyCode
// DESCRIPTION   : verifies the code entered by the user by
//                 comparing it with the code stored in EEPROM.
// PARAMETERS    : int16_t : code entered by the user.
// RETURNS       : Returns 1 if the code is correct and
//                 returns -1 if code is not correct.
//
int8_t verifyCode(int16_t code)
{
  int16_t passCode = code;
  int8_t  getEpromValue = 0;
  int16_t storedPasscode = 0;
  getEpromValue = EEPROM.read(0);
  storedPasscode = storedPasscode + getEpromValue * 1000;
  getEpromValue = EEPROM.read(1);
  storedPasscode = storedPasscode + getEpromValue * 100;
  getEpromValue = EEPROM.read(2);
  storedPasscode = storedPasscode + getEpromValue * 10;
  getEpromValue = EEPROM.read(3);
  storedPasscode = storedPasscode + getEpromValue;

  Serial.printf("\npassCode : %d\n", passCode);
  Serial.printf("\nstoredPasscode : %d\n", storedPasscode);

  if (passCode == storedPasscode)
  {
    return 1;
  }
  else
  {
    return -1;
  }
}

//
// FUNCTION      : verifyMasterCode
// DESCRIPTION   : verifies the Master code entered by the user by
//                 comparing it with the mastercode stored in EEPROM.
// PARAMETERS    : int16_t : mastercode entered by the user.
// RETURNS       : Returns 1 if the mastercode is correct and
//                 returns -1 if mastercode is not correct.
//
int8_t verifyMasterCode(int16_t code)
{
  int16_t passCode = code;
  int8_t  getEpromValue = 0;
  int16_t storedPasscode = 0;
  getEpromValue = EEPROM.read(11);
  storedPasscode = storedPasscode + getEpromValue * 1000;
  getEpromValue = EEPROM.read(12);
  storedPasscode = storedPasscode + getEpromValue * 100;
  getEpromValue = EEPROM.read(13);
  storedPasscode = storedPasscode + getEpromValue * 10;
  getEpromValue = EEPROM.read(14);
  storedPasscode = storedPasscode + getEpromValue;

  Serial.printf("\npassCode : %d\n", passCode);
  Serial.printf("\nstoredPasscode : %d\n", storedPasscode);

  if (passCode == storedPasscode)
  {
    return 1;
  }
  else
  {
    return -1;
  }
}

//
// FUNCTION      : resetCode
// DESCRIPTION   : Stores the new code in EEPROM.
// PARAMETERS    : int16_t : NEW code entered by the user.
// RETURNS       : Returns 1 if the mastercode is correct and
//                 returns -1 if mastercode is not correct.
//
void resetCode(int16_t newCode)
{
  int8_t firstDigit = 0;
  int8_t secondDigit = 0;
  int8_t thirdDigit = 0;
  int8_t fourthDigit = 0;

  firstDigit  = newCode / 1000;
  secondDigit = (newCode % 1000) / 100;
  thirdDigit  = ( newCode % 100 ) / 10;
  fourthDigit = newCode % 10;

  EEPROM.write(0, firstDigit);
  EEPROM.write(1, secondDigit);
  EEPROM.write(2, thirdDigit);
  EEPROM.write(3, fourthDigit);

  lcd.clear();
  lcd.printf("Code Reset ");
  lcd.setCursor(0, 1);
  lcd.printf("SuccessFull.!");
  delay(3000);
  lcd.clear();
}

//**************************************** SETUP LOOP * *********************************************************//

void setup ()
{
  Serial1.begin (9600);
  Serial.begin(9600);
  lcd.begin (2, 16);
  Serial1.flush ();
  displayWelcome();
  pinMode(greenLed,OUTPUT);
  pinMode(redLed,OUTPUT);
  shutter.attach(8);
  Serial.print("hello");
  int8_t blockStatus = 0;
  blockStatus = EEPROM.read(50);
  if(blockStatus == 1)
  {
    displayBlocked();
  }
}

//**************************************** VOID LOOP ************************************************************//

void loop()
{
  static int8_t mode = 1;
  int8_t openOrResetStatus = 0;
  int8_t doorOrShutter = 0;
  int16_t code = 0;
  int16_t newCode = 0;
  int8_t codeStatus = 0;
  static int8_t pinAttempts = 0;
  int8_t blockStatus = 0;

  blockStatus = EEPROM.read(50);

  if(blockStatus == 1)
  {
    mode = 7;
  }

  /*states:    1   display OPEN OR RESET option on lcd and waits for the 
                   user to select the appropriate option.
               2   display DOOR OR SHUTTER option on lcd and waits for 
                   the user to select appropriate option.
               3   checks for pin attempts. then asks user to enter the 
                   pin. if the pin entered is correct, it illuminates
                   green led else red led.
               4   checks for pin attempts. then asks user to enter the 
                   pin. if the pin entered is correct, it illuminates
                   green led and also rotate servo motor for 180 degree
                   else red led.
               5   gives user option to change the existing passcode, 
                   only if he enters the old passcode correct. here it 
                   verifies the old passcode. 
               6   Accepts the new code from the user.
               7   if the user intensionally or unintensionally enters
                   the wrong code for three times, he comes in this state
                   and user is asked for the master passcode.
  */
  
  switch (mode)
  {
    case 1  : openOrResetStatus = checkOneOrTwo();
              if (openOrResetStatus != -1)
              {
                if (openOrResetStatus == 1)
                {
                  mode = 2;
                  displayDoorOrShutter();
                  break;
                }
                if (openOrResetStatus == 2)
                {
                  mode = 5;
                  displayEntrOldPassCode();
                  break;
                }
              }
              break;

    case 2  : doorOrShutter = checkOneOrTwo();
              if (doorOrShutter != -1)
              {
                if (doorOrShutter == 1)
                {
                  displayEnterCode();
                  mode = 3;
                  break;
                }
                if (doorOrShutter == 2)
                {
                  displayEnterCode();
                  mode = 4;
                  break;
                }
              }
              break;

    case 3  : if(pinAttempts == 3)
              {
                displayBlocked();
                EEPROM.write(50,1);
                //Serial.printf("\n\nvalue at eeprom 50 = %d\n\n");
                mode = 7;
                break;
              }
              code = enterCode();
              if (code != -1 && code != 0)
              {
                codeStatus = verifyCode(code);
                if (codeStatus == 1)
                {
                  displayOpeningDoor();
                  pinAttempts = 0;
                  mode = 1;
                }
                if (codeStatus == -1)
                {
                  displayWrongCode();
                  pinAttempts = pinAttempts + 1;
                  displayEnterCode();
                  mode = 4;
                  Serial.printf("\n\nDone\n\n");
                  break;
                }
              }
              break;

    case 4  : if(pinAttempts == 3)
              {
                displayBlocked();
                EEPROM.write(50,1);
                //Serial.printf("\n\nvalue at eeprom 50 = %d\n\n");
                mode = 7;
                break;
              }
              code = enterCode();
              if (code != -1 && code != 0)
              {
                codeStatus = verifyCode(code);
                if (codeStatus == 1)
                {
                  displayOpeningShutter();
                  pinAttempts = 0;
                  mode = 1;
                }
                if (codeStatus == -1)
                {
                  displayWrongCode();
                  pinAttempts = pinAttempts + 1;
                  displayEnterCode();
                  mode = 4;
                  Serial.printf("\n\nDone\n\n");
                  break;
                }
              }
              break;
              
    case 5  : code = enterCode();
              if (code != -1 && code != 0)
              {
                codeStatus = verifyCode(code);
                if (codeStatus == 1)
                {
                  displayEnterNewCode();
                  mode = 6;
                  break;
                }
                if (codeStatus == -1)
                {
                  displayWrongCode();
                }
                mode = 1;
              }
              break;
              
    case 6  : newCode = enterCode();
              if (newCode != -1 && newCode != 0)
              {
                resetCode(newCode);
                displayWelcome();
                mode = 1;
              }
              break;
             
    case 7  : code = enterCode();
              if (code != -1 && code != 0)
              {
                codeStatus = verifyMasterCode(code);
                if (codeStatus == 1)
                {
                  displayYouRGood();
                  pinAttempts = 0;
                  EEPROM.write(50,0);
                  displayWelcome();
                  mode = 1;
                  break;
                }
                if (codeStatus == -1)
                {
                  displayWrongCode();
                  displayBlocked();
                  mode = 7;
                }
              }
              break;
              
    default : Serial.printf("Developed by Hirenn..!!");
              break;
  }
  
}

