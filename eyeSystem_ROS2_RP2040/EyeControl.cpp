#include "EyeControl.h"

//Set the control pin and image size of the display that shows the pupil image
EyeControl::EyeControl(SPIClass *spi, int csPin, int dcPin, int rstPin){
    oled = Adafruit_SSD1331(spi, csPin, dcPin, rstPin);
}
//Make initial settings for the display
void EyeControl::initDisplay(uint16_t fillColoc){
    oled.begin();
    oled.fillScreen(fillColoc);
}
//Save the value of eyeLid after modification
void EyeControl::setLidValue(int lidVal){
    if(lidVal < 0){
        eyeLid_After = 0;
    }else if(lidVal >= LIDS_MAX){
        eyeLid_After = LIDS_MAX - 1;
    }else{
        eyeLid_After = lidVal;
    }
}
//Save the value of eyeLidKind after modification
void EyeControl::setLidTypeValue(int lidKindVal){
    if(lidKindVal < 0){
        eyeLidType_After = 0;
    }else{
        eyeLidType_After = lidKindVal;
    }
}
//Save the value of eyePupil after modification
void EyeControl::setPuilValue(int pupilVal){
    if(pupilVal < 0){
        eyePupil_After = 0;
    }else if(pupilVal >= PUPIL_MAX){
        eyePupil_After = PUPIL_MAX - 1;
    }else{
        eyePupil_After = pupilVal;
    }
}
//Save the value of eyeBlink after modification
void EyeControl::setBlinkValue(int blinkVal){
    if(blinkVal < 0){
        eyeBlink = 0;
    }else{
        eyeBlink = blinkVal;
    }
}
//Get the After variable of eyeLidKind
int EyeControl::getLidKindValue(void){
    return eyeLid_After;
}
//Get the Anser differnt LidKind after and LidKind before
boolean EyeControl::differentLidType(void){
    if(eyeLidType_After != eyeLidType_Before){
        return true;
    }else{
        return false;
    }
}
//Get the After variable of eyeLid
int EyeControl::getLidValue(void){
    return eyeLid_After;
}
//Get the After variable of eyePupil
int EyeControl::getPupilValue(void){
    return eyePupil_After;
}
//Get variable of eyeBlink
int EyeControl::getBlinkValue(void){
    return eyeBlink;
}
//Display the passed image
void EyeControl::dispEye(uint16_t img[]){
    oled.drawRGBBitmap(0, 0, img, DISPLAY_X, DISPLAY_Y);
}
void EyeControl::dispEye(void){
    oled.drawRGBBitmap(0, 0, eyeImg, DISPLAY_X, DISPLAY_Y);
}
//While generating an image that eliminates the difference between the after variable and the before variable for each change, bring the value of the before variable closer to the after variable
//Returns true when there is no difference between the after variable and before variable of the item that is true
boolean EyeControl::creatEyeImg(boolean lidSignal, boolean lidTypeSignal, boolean pupilSignal, boolean reversal){
    boolean result = true;
    //Differential change of after and before variables
    //eyeLid
    if(lidSignal == true){
        if(eyeLid_Before != eyeLid_After){
            result = false;
            eyeLid_Before = _approximationValue(eyeLid_Before, eyeLid_After);
        }
    }
    //eyePupil
    if(pupilSignal == true){
        if(eyePupil_Before != eyePupil_After){
            result = false;
            eyePupil_Before = _approximationValue(eyePupil_Before, eyePupil_After);
        }
    }
    //Whether to update the eyelid type value
    if(lidTypeSignal == true){
        eyeLidType_Before = eyeLidType_After;
    }
    //Create a base
    for(int i=0;i<DISPLAY_BUFFER;i++){
        eyeImg[i] = 0xffff;
    }
    if(reversal == false){
        //Pupil image synthesis
        for(int y=0;y<DISPLAY_Y;y++){
            for(int x=0;x<DISPLAY_X;x++){
                if(pupilImgsData[eyePupil_Before][(DISPLAY_X * y) + x] != 0xffff){
                    eyeImg[(DISPLAY_X * y) + x] = pupilImgsData[eyePupil_Before][(DISPLAY_X * y) + x];
                }
            }
        }
        //Eyelid image synthesis
        for(int y=0;y<DISPLAY_Y;y++){
            for(int x=0;x<DISPLAY_X;x++){
                if(eyelidsImgData[eyeLidType_Before][eyeLid_Before][(DISPLAY_X * y) + x] != 0xffff){
                    eyeImg[(DISPLAY_X * y) + x] = eyelidsImgData[eyeLidType_Before][eyeLid_Before][(DISPLAY_X * y) + x];
                }
            }
        }
    }else{
        //Pupil image synthesis
        for(int y=0;y<DISPLAY_Y;y++){
            for(int x=(DISPLAY_X - 1);x>=0;x--){
                if(pupilImgsData[eyePupil_Before][(DISPLAY_X * y) + x] != 0xffff){
                    eyeImg[(DISPLAY_X * y) + ((DISPLAY_X - 1) - x)] = pupilImgsData[eyePupil_Before][(DISPLAY_X * y) + x];
                }
            }
        }
        //Eyelid image synthesis
        for(int y=0;y<DISPLAY_Y;y++){
            for(int x=(DISPLAY_X -1);x>=0;x--){
                if(eyelidsImgData[eyeLidType_Before][eyeLid_Before][(DISPLAY_X * y) + x] != 0xffff){
                    eyeImg[(DISPLAY_X * y) + ((DISPLAY_X - 1) - x)] = eyelidsImgData[eyeLidType_Before][eyeLid_Before][(DISPLAY_X * y) + x];
                }
            }
        }
    }
    //eyePupil
    return result;
}


int EyeControl::_approximationValue(int a, int b){
    if(a > b){
        return (a - 1);
    }else if(a < b){
        return (a + 1);
    }else{
        return a;
    }
}
