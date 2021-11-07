#pragma once
#include <SPI.h>                 //for SPI
#include <Wire.h>                //for SPI
#include "TFT_eSPI.h"            //The display V2
#include "XPT2046_Touchscreen.h" //The Touch portion
#include "Adafruit_GFX.h"        //Graphics
#include "PubSubClient.h"        //mqtt
#include "Base64.h"              //the base64 converter library

#include "brush.h"

class Screen
{
private:
    //SPI PINS
    const int HSPI_DC = 26;
    const int HSPI_RST = 27;
    const int HSPI_CS1 = 25;
    const int HSPI_CS2 = 33;
    const int TP_IRQ = 39; //not used yet...
    const int HSPI_MOSI = 13;
    const int HSPI_MISO = 12;
    const int HSPI_CLK = 14;

    TFT_eSPI tft = TFT_eSPI();                              //disply
    XPT2046_Touchscreen ts = XPT2046_Touchscreen(HSPI_CS2); //touch
    Brush brush;

    //calibration data
    const int TS_MINX = 150;
    const int TS_MINY = 130;
    const int TS_MAXX = 3800;
    const int TS_MAXY = 4000;

    //all program variables
    boolean wastouched = true;
    boolean istouched;
    TS_Point p;
    int modeCurrent;
    int modeOld;

    //drraw pgogram variables
    const int swatchSize = 30; //pint box size
    const int drawArea = 320;

    PubSubClient *MQTTclient;
    String base_topic;

public:
    void init(PubSubClient &client, String base_topic); //initializes the touch screen
    void ScreenProgram();            //manages the screen in loop

    void drawMenue(); //draws the setup menue - to be developed later
    void drawPaint(); //draws the paint

    void paintProgram();                        //paint program
    void menueProgram();                        //menue program
    void saveScreentoFile(const char *file);    //saves screen to a file
    void printScreenfromFile(const char *file); //draws the screen from the file
    void printScreenFromInternet(const char *data, size_t line, size_t part);

    void sendFileToInternet(const char *file); //this sends whatevers in the file to the internet
    void sendScreenToInternet(); //this sends whatevers on the screen to the internet

    void colorEncode(); //for color compression ** FAILED **
    void colorDecode(); //for color decompression ** FAILED **
};
