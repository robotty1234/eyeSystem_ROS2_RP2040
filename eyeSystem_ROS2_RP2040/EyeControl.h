#ifndef EYE_CONTROL_H
#define EYE_CONTROL_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include "eyeImg.h"
#include "eyeInitInfo.h"

class EyeControl{
    public:
        //Set the control pin and image size of the display that shows the pupil image
        EyeControl(SPIClass *spi, int csPin, int dcPin, int rstPin);
        //Make initial settings for the display
        void initDisplay(uint16_t fillColoc = 0000);
        //Save the value of eyeLid after modification
        void setLidValue(int lidVal);
        //Save the value of eyeLidKind after modification
        void setLidTypeValue(int lidTypeVal);
        //Save the value of eyePupil after modification
        void setPuilValue(int pupilVal);
        //Save the value of eyeBlink after modification
        void setBlinkValue(int blinkVal);
        //Get the After variable of eyeLidKind
        int getLidKindValue(void);
        //Get the Anser differnt LidKind after and LidKind before
        boolean differentLidType(void);
        //Get the After variable of eyeLid
        int getLidValue(void);
        //Get the After variable of eyePupil
        int getPupilValue(void);
        //Get variable of eyeBlink
        int getBlinkValue(void);
        //Display the passed image
        void dispEye(uint16_t img[]);
        void dispEye(void);
        //While generating an image that eliminates the difference between the after variable and the before variable for each change, bring the value of the before variable closer to the after variable
        //Returns true when there is no difference between the after variable and before variable of the item that is true
        boolean creatEyeImg(boolean lidSignal, boolean lidTypeSignal, boolean pupilSignal, boolean reversal = false);
    private:
        Adafruit_SSD1331 oled =  Adafruit_SSD1331(0, 1, 2, 3, 4);
        int eyeLid_Before = 0;
        int eyeLid_After = 0;
        int eyeLidType_Before = 0;
        int eyeLidType_After = 0;
        int eyePupil_Before = 0;
        int eyePupil_After = 0;
        int eyeBlink = 0;
        uint16_t eyeImg[DISPLAY_BUFFER];
        //A function that brings "a" closer to "b" by one
        int _approximationValue(int a, int b);
};

#endif
