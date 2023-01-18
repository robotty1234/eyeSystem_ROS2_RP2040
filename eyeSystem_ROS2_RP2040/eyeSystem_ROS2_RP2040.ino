#include <EX_StringFunc.h>
#include "EyeControl.h"
#include <Wire.h>
#include <rgb_lcd.h>

EyeControl eye_L = EyeControl(&SPI, CS_L, DC_L, RST_L);
EyeControl eye_R = EyeControl(&SPI1, CS_R, DC_R, RST_R);

//Receive pupil image values from serial port
void eyeInfoReceiv_task(void);
//Function to blink
void eyeBlink_task(void);
//Create an eye image
void eyeEncodeImg(void);

//A variable that tells the blink moment
boolean blinkFlag = false;
//Store whether or not the value has been updated
boolean updateFlag = false;

//Trans eye img speed[ms]
int transSpeed = 20;

//Comment out if you want to display debug messages
//#define DEBUG
//#define DEBUG_LCD

#ifdef DEBUG_LCD
  rgb_lcd lcd;
#endif

void setup() {
  Serial.begin(115200);
  #ifdef DEBUG
    Serial.println("start eyeSystem_ROS2_RP2040");
  #endif
  #ifdef DEBUG_LCD
    // set up the LCD's number of columns and rows:
    Wire.setSDA(8);
    Wire.setSCL(9);
    Wire.begin();
    lcd.begin(16, 2);
    lcd.print("eyeSystem_ROS2_RP2040 Start");
  #endif

  #ifndef DEBUG_LCD
    SPI.setTX(MOSI_L);
    SPI.setSCK(SCK_L);
    eye_L.initDisplay();

    SPI1.setTX(MOSI_R);
    SPI1.setSCK(SCK_R);
    eye_R.initDisplay();
  #endif
  
   //If you don't wait a bit, the image update will be delayed
  delay(100);
  //Init eye display
  #ifdef DEBUG
    Serial.println("Init eye display");
  #endif
  eyeEncodeImg();
}

void loop() {
  if(Serial.available() > 0){
    eyeInfoReceiv_task();
  }else if(blinkFlag == true){
    eyeBlink_task();
    blinkFlag = false;
  }
}

void setup1(){

}

void loop1(){
  static int countTime = 0;
  if(updateFlag == true){
    countTime = 0;
    updateFlag = false;
  }else if(eye_L.getBlinkValue() > 0 && eye_R.getBlinkValue() > 0){
    if(countTime >= eye_L.getBlinkValue() || countTime >= eye_R.getBlinkValue()){
      blinkFlag = true;
      countTime = 0;
    }else{
      countTime++;
      #ifdef DEBUG
        Serial.println(countTime);
      #endif
      delay(1);
    }
  }
}

void eyeInfoReceiv_task(void){
  //Save received content
  String s;
  do{
      s = readStringQ(Serial,true);
      //Remove '\n'
      s.remove(s.length()-1);
      int val = splitToInt(s, ':');
      //Save to each variable
      if(s.indexOf("eye_lid_l:") > -1){
          eye_L.setLidValue(val);
          #ifdef DEBUG
            Serial.println("set value eye lid L");
            Serial.println(s.indexOf("eye_lid_l:"));
          #endif
      }else if(s.indexOf("eye_lid_r:") > -1){
          eye_R.setLidValue(val);
          #ifdef DEBUG
            Serial.println("set value eye lid R");
            Serial.println(s.indexOf("eye_lid_r:"));
          #endif
      }else if(s.indexOf("eye_pupil_l:") > -1){
          eye_L.setPuilValue(val);
          #ifdef DEBUG
            Serial.println("set value eye pupil L");
            Serial.println(s.indexOf("eye_pupil_l:"));
          #endif
      }else if(s.indexOf("eye_pupil_r:") > -1){
          eye_R.setPuilValue(val);
          #ifdef DEBUG
            Serial.println("set value eye pupil R");
            Serial.println(s.indexOf("eye_pupil_r:"));
          #endif
      }else if(s.indexOf("eye_blink:") > -1){
          eye_L.setBlinkValue(val);
          eye_R.setBlinkValue(val);
          #ifdef DEBUG
            Serial.println("set value blink");
            Serial.println(s.indexOf("eye_blink:"));
          #endif
      }
      Serial.println(s);
      delay(10);
  }while(s.indexOf("finish") <= -1); //When sending a value that is not the end of sending
  //Update the saved pupil image
  eyeEncodeImg();
  #ifdef DEBUG_LCD
    String strs = "{" + String(eye_L.getLidValue()) + "," + String(eye_R.getLidValue()) + "," + String(eye_L.getPupilValue()) + "," + String(eye_R.getPupilValue()) + "," + String(eye_L.getBlinkValue()) + "}";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(strs);
  #endif
  updateFlag = true;
}

void eyeBlink_task(void){
  //Save original eyelid variables
  int originLid_L = eye_L.getLidValue();
  int originLid_R = eye_R.getLidValue();
  //Close your eyes
  eye_L.setLidValue(0);
  eye_R.setLidValue(0);
  eyeEncodeImg();
  //Open your eyes
  eye_L.setLidValue(originLid_L);
  eye_R.setLidValue(originLid_R);
  eyeEncodeImg();
}

void eyeEncodeImg(void){
  //Both continue to change as long as the variable that specifies the pupil image is different between "before" and "after"
  boolean flug_L;
  boolean flug_R;
  #ifdef DEBUG
    Serial.println("start encoding eye img");
  #endif
  do{
      //Create an image for each change other than the eyelid
      flug_L = eye_L.creatEyeImg(false, true, false);
      flug_R = eye_R.creatEyeImg(false, true, true);
      #ifdef DEBUG
        Serial.println("created eye img");
      #endif
      //Show pupil image
      #ifndef DEBUG_LCD
        eye_L.dispEye();
        eye_R.dispEye();
        delay(transSpeed);
      #endif
      //Repeat until updated
  }while(flug_L == false || flug_R == false);
  #ifdef DEBUG
    Serial.println("finish other than the eyelid");
  #endif
  do{
      //Create an image for each change other than the eyelid
      flug_L = eye_L.creatEyeImg(true, false, false);
      flug_R = eye_R.creatEyeImg(true, false, true);
      #ifdef DEBUG
        Serial.println("created eye img");
      #endif
      //Show pupil image
      #ifndef DEBUG_LCD
        eye_L.dispEye();
        eye_R.dispEye();
        delay(transSpeed);
      #endif
      //Repeat until updated
  }while(flug_L == false || flug_R == false);
  #ifdef DEBUG
    Serial.println("finish other than the eyelid");
  #endif
}
