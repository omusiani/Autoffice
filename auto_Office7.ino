#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
//**************************************
////SPECIAL CHARATERS

//PIN LAYOT
/**
8,9,10,11 -> LED1, LED2, LED3 LED4 (STATIONS)
2,3,4,5,6,7 -> STION1, STION2, STION3,STION4, LOGIN, LOGOUT
A0 -> TEMP LM35
A1 -> LDR
13 -> SECURITY LIGHT
12 -> AIR CONDITIONER

**/
byte degreez[8] = {
    0b00111,
    0b00101,
    0b00111,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
};
byte user[8] = {
    0b00000,
    0b01100,
    0b11110,
    0b11110,
    0b01100,
    0b01110,
    0b11111,
    0b11111
};

//**************************
///VARIABLES

///Pin Assignment
const int Temp_Pin=A0;
const int LDR_pin = A1;
const int SecurityLight_pin = 13;
const int Air_conPin = 12;

String systemLogo= "..AUTO OFFICE..";

float temprature;
float tempHigh_threshold = 30.00;
float tempCool_threshold = 26.00;
int ADM = 0;
boolean SecurityLight_ON, AirCon_ON=false;
int lastButton_Press= 0;

boolean login,logout = false;

float lightHigh_threshold = 1000.00;
float lightLow_threshold = 60.00;
float tempSum,temp,light_Intenssity = 0;
float intensity = 2000;
float lightSum = 0;
int tempContr,lightContr = 0;

//******************************************************************************************************--SETUP
void setup() {
    Serial.begin(9600);
    for(int i = 2; i <= 7;i++){ pinMode(i, INPUT);} //Set all button pins to INPUT from pin 2 to 7
    for(int i = 8; i <= 11;i++){ pinMode(i, OUTPUT);} //Set all button pins to INPUT from pin 2 to 7
    pinMode(Air_conPin, OUTPUT); 
    pinMode(SecurityLight_pin, OUTPUT);
    
    //Set HIGH to the -ve pin of LED to turn it off
    digitalWrite(SecurityLight_pin, HIGH);
    digitalWrite(Air_conPin, HIGH);

    lcd.begin(16, 2);
    #if 1 
    // create a new character
      lcd.createChar(1, degreez); //Degrees celciuos little circle
      lcd.createChar(2, user); //Figure of a persone to represent users
    #endif 
      lcd.setRGB(0, 288, 288);//LCD Color
   
}
//******************************************************************************************************--MAIN LOOP
void loop() { //---------------------------------------------Temprature
  //Display system logo at the first Row
                 lcd.setCursor(0, 0);
                 lcd.print(systemLogo);
                 
  //Display Temprature reading from tempRead() method below           
          temprature = tempRead(Temp_Pin);
                 lcd.setCursor(0, 1);
                 lcd.print("Temp:");
                 lcd.print(temprature); 
                 lcd.write(1);
                 lcd.print("C   ");
                 
   // Display Users Logged in with the User charater at the end -- :1$ 
                 lcd.setCursor(13, 1);
                 lcd.print(":");
                 lcd.print(ADM);
                 lcd.write(2);
                 
   //Check if Temperature is greater that 30 degrees if so Air con is turned On             
         if((temprature > tempHigh_threshold)  && (AirCon_ON == false)){
                 lcd.setCursor(0, 1);
                 lcd.print("Temp too high!     ");
                 delay(1500);
                 lcd.setCursor(0, 1);
                 lcd.print("Turning AC on...   ");
                 delay(1500); 
            AirCon_ON=true;
     
           }else if((temprature < tempCool_threshold) && (AirCon_ON == true)){
                  lcd.setCursor(0, 1);
                  lcd.print("Temp Normal!     ");
                  delay(1500);
                  lcd.setCursor(0, 1);
                  lcd.print("Turning AC Off...   ");
                  delay(1500);
             AirCon_ON=false;
               }  
    //Set AirCon Pin high or low to turn it off or on        
           if( AirCon_ON == false){
             digitalWrite(Air_conPin, HIGH);
            }else{
              digitalWrite(Air_conPin, LOW);
            }
   //----------------------------------------------------------------------Button Check
      //Check if button is pressed using the button check method
      
          if (buttonCheck(6) == 6){
                  login = true; //Set login capabilities to true
                  lcd.setCursor(0, 1);
                  lcd.print("Login...           ");
                  delay(1000);}
           
           if (buttonCheck(7) == 7){
                  logout = true;//Set logout capabilities to true
                  lcd.setCursor(0, 1);
                  lcd.print("LogOut...       ");
                  delay(1000);}
   
   
        while((login == true) || (logout == true)) {
                  
                    lcd.setCursor(0, 1);
                    lcd.print("Enter Station:_");
                   
          
              //******
              for(int i = 2; i <= 5; i++){
                  if ((buttonCheck(i) == i) && (login == true)){
                        if(digitalRead(i+6) == 0){
                            digitalWrite(i+6, HIGH);
                            lcd.setCursor(0, 1);
                            lcd.print("Station:"); 
                            lcd.print(i-1);
                            lcd.print(" ON    ");
                            ADM++;}
                        login = false;
                     }
                    if ((buttonCheck(i) == i) && (logout == true)){
                      if(digitalRead(i+6) == 1){
                              digitalWrite(i+6, LOW);
                              lcd.setCursor(0, 1);
                              lcd.print("Station:"); 
                              lcd.print(i-1);
                              lcd.print(" OFF    ");
                              ADM--;}
                        logout = false;
                     }
              }
                if((ADM <= 0) && (logout == true)){
                      lcd.setCursor(0, 1);
                      lcd.print("No User Loggedin.    ");
                      delay(1300);  
                      logout = false;
                    }else if((ADM >= 4) && (login == true)){
                      lcd.setCursor(0, 1);
                      lcd.print("Stations Active.    ");
                      delay(1300);  
                      login = false;
                    }else{
                      if((login == false)&&(logout == false)){
                        delay(800);  
                        lcd.setCursor(0, 1);
                        lcd.print("      {:");
                        lcd.print(ADM);
                        lcd.write(2);
                        lcd.print("}       ");
                        delay(800); 
                    }
                  }
        }
        
 //---------------------------------------------------------------SECURITY LIGHTS

        light_Intenssity = luminance(LDR_pin);
        if((light_Intenssity < lightLow_threshold)&&(SecurityLight_ON==false)){
                   digitalWrite(SecurityLight_pin, LOW);
                   lcd.setCursor(0, 1);
                   lcd.print("Security :ON    ");
                   delay(1300);
             SecurityLight_ON=true;
             
           }else if((light_Intenssity > lightHigh_threshold)&&(SecurityLight_ON==true)){
                   digitalWrite(SecurityLight_pin, HIGH);
                   lcd.setCursor(0, 1);
                   lcd.print("Security :OFF   ");
                   delay(1300);
             SecurityLight_ON=false;
           }    
      
  }

//*****************************************METHODS
//BITTONCHECK METHOD CHECK IF PIN HAS BEEN PRESSED
int buttonCheck(int buttonPin1){
   if(digitalRead(buttonPin1) == 0){
        delay(50);
          if(digitalRead(buttonPin1) == 0){
             return buttonPin1;
            }else{return 0;}
      }
}
////Temprature method fo detecting temp intensity and getting the average of 50
           float tempRead(int tempPin){
              tempContr++;
              tempSum = tempSum + (analogRead(tempPin)* 0.48828125);
              if(tempContr >= 100){
                temp = tempSum/100;
                tempContr = 0;
                tempSum = 0;
              }
             return temp;
           }
           
  //LDR method fo detecting light intensity and getting the average of 50
           float luminance(int LDRP){
            lightContr++;
            lightSum = lightSum + (analogRead(LDRP)*100);
            if(lightContr >= 50){
                intensity = lightSum/50;
                lightContr = 0;
                lightSum = 0;
            }
             return intensity;
           }
