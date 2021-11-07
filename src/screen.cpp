#include "screen.h"

void Screen::init(PubSubClient &client, String base_topic)
{
    this->MQTTclient = &client;
    this->base_topic = base_topic;
    TFT_eSprite spr = TFT_eSprite(&tft);
    tft.begin();
    ts.begin();
    ts.setRotation(2);

    modeCurrent = 1;
    modeOld = 0;

    //this chunk draws a heart
    int heart_center_x = 50;
    int heart_center_y = 160;
    int heart_height = 100;
    int heart_width = 80;
    tft.fillScreen(0x0000);                                                                                                                                                                     //fill screen Black
    tft.fillTriangle(heart_center_x, heart_center_y, (heart_center_x + heart_height), (heart_center_y - heart_width), (heart_center_x + heart_height), (heart_center_y + heart_width), 0xF800); //draw red triangle
    tft.fillCircle((heart_center_x + heart_height), (heart_center_y - (heart_width / 2)), (heart_width / 2), 0xF800);                                                                           //mound left
    tft.fillCircle((heart_center_x + heart_height), (heart_center_y + (heart_width / 2)), (heart_width / 2), 0xF800);                                                                           //mound right
    tft.fillTriangle(heart_center_x, heart_center_y, (heart_center_x + heart_height), (heart_center_y - heart_width), heart_center_x, (heart_center_y - (heart_width * 2)), 0x000);             //clean it up left
    tft.fillTriangle(heart_center_x, heart_center_y, (heart_center_x + heart_height), (heart_center_y + heart_width), heart_center_x, (heart_center_y + (heart_width * 2)), 0x000);             //clean it up right
}

void Screen::ScreenProgram()
{
    istouched = ts.touched();

    //execute the current  program
    if (modeCurrent == 0)
    {
        Serial.println("Im in menue");
        delay(2000);
        modeCurrent = 1;
    }
    else if (modeCurrent == 1)
        paintProgram();
    //screen flip flop
    if (modeCurrent != modeOld)
    {
        if (modeCurrent == 0)
            this->drawMenue();
        else if (modeCurrent == 1)
            this->drawPaint();
    }

    modeOld = modeCurrent;
}

void Screen::drawPaint()
{

    tft.fillScreen(0x0000); //fils screen black
    //the paint boxes
    //fillRect(X, Y, Width, Height, Color)
    tft.fillRect(0, 0, swatchSize, swatchSize, brush.getSwatchColor(0));
    tft.fillRect(swatchSize, 0, swatchSize, swatchSize, brush.getSwatchColor(1));
    tft.fillRect(swatchSize * 2, 0, swatchSize, swatchSize, brush.getSwatchColor(2));
    tft.fillRect(swatchSize * 3, 0, swatchSize, swatchSize, brush.getSwatchColor(3));
    tft.fillRect(swatchSize * 4, 0, swatchSize, swatchSize, brush.getSwatchColor(4));
    tft.fillRect(swatchSize * 5, 0, swatchSize, swatchSize, brush.getSwatchColor(5));
    tft.fillRect(swatchSize * 6, 0, swatchSize, swatchSize, brush.getSwatchColor(6));
    tft.fillRect(swatchSize * 7, 0, swatchSize, swatchSize, brush.getSwatchColor(7));
}

//draws the menue
void Screen::drawMenue()
{
    tft.fillScreen(0x0000); //fils screen black
    Serial.println("Menue");
    //print out IP address
    //print out Wifi info
}

void Screen::paintProgram()
{
    if (istouched) //im keeping this just in case i need to make some imporvements
    {
        //get the cordinates and condition them
        // Scale from ~0->4000 to tft.width using the calibration #'s
        p = ts.getPoint();
        p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
        p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

        if (!wastouched)
        { //the first touch
            //check here for special or workable area
            //putting it here prevents accidential postitives while drawing
            if (p.y < swatchSize) //if a swatch is picked
            {
                //save image
                if (p.x < swatchSize)
                { //set brush color 1
                    //this->saveScreentoFile("/storage/draw.img");
                    Serial.println("Sending to the internet");
                    //this->sendFileToInternet("/storage/draw.img");
                    this->sendScreenToInternet();
                    Serial.println("Full Send!!");
                    //remove("/storage/draw.img"); //wipes the file
                }
                //recal image
                else if (p.x < swatchSize * 2)
                { //set brush color 2
                    this->printScreenfromFile("/storage/draw.img");
                }
                else if (p.x < swatchSize * 3)
                {
                    modeCurrent = 0;
                } //set brush color to 3
                else if (p.x < swatchSize * 4)
                {
                    brush.setColorfromSwatch(3);
                } //set brush color to 4
                //else if(p.x < swatchSize*4){ MQTTclient->publish("justus/loveBox/2", "Hello World"); } //set brush color to 4
                else if (p.x < swatchSize * 5)
                {
                    brush.setColorfromSwatch(4);
                } //set brush color to 5
                else if (p.x < swatchSize * 6)
                {
                    brush.setColorfromSwatch(5);
                } //set brush color to 6
                else if (p.x < swatchSize * 7)
                {
                    brush.setColorfromSwatch(6);
                } //set brush color to 6
                else if (p.x < swatchSize * 8)
                {
                    brush.setColorfromSwatch(7);
                } //set brush color to 6
            }
        }
        //while drawing ... draw
        else if (p.y > swatchSize)
        {
            tft.fillCircle(p.x, p.y, brush.getSize(), brush.getColor());
        }
    }
    else if (wastouched)
    {
    } //right after no touch?
    wastouched = istouched;
}

void Screen::menueProgram()
{
    delay(2000);
    modeCurrent = 1;
}

void Screen::saveScreentoFile(const char *file)
{
    uint16_t *buff;
    buff = new uint16_t[240];

    FILE *drawing = fopen(file, "wb"); //opens / makes an img file called draw
    Serial.println("Saveing img to file");

    //read into buffer and save to img
    for (int i = swatchSize; i < drawArea; i++)
    {
        tft.readRect(0, i, 240, 1, buff); //get the screen line by line

        //this chunk color corects the bad data in the buffer before saveing
        for (int j = 0; j < 240; j++)
        {
            uint32_t a = buff[j] << 8;
            a = a & 0xffff;
            uint32_t b = buff[j] >> 8;
            buff[j] = a + b;
        }

        fwrite(buff, sizeof(buff[0]), 240, drawing);
    }

    Serial.println("img written to file");
    delete[] buff;
    fclose(drawing);
}

void Screen::printScreenfromFile(const char *file)
{
    uint16_t *buff;
    buff = new uint16_t[240];
    FILE *drawing;
    if ((drawing = fopen(file, "rb")))
    { //opens an img file called draw
        for (int i = swatchSize; i < drawArea; i++)
        {
            fread(buff, sizeof(buff[0]), 240, drawing);

            for (int j = 0; j < 240; j++)
            {
                tft.drawPixel(j, i, buff[j]);
            }
        }
        Serial.println("img recalled");
        fclose(drawing);
    }
    delete[] buff;
}

void Screen::printScreenFromInternet(const char *data, size_t line, size_t part)
{
    uint16_t *buff;
    size_t buff_length;
    buff = Base64::decode_to_uint16(data, buff_length);

    if (buff_length == 60)
    {
        size_t start = 60 * part;
        for (int i = 0; i < 60; i++)
        {
            tft.drawPixel((start + i), line, buff[i]);
        }
    }

    delete[] buff;
}

void Screen::sendFileToInternet(const char *file)
{
    uint16_t *buff = new uint16_t[240];
    ;                                  //the buffer that reads a line of file
    FILE *drawing = fopen(file, "rb"); //the file
    //uint16_t fileSize = sizeof(buff[0])*(drawArea-swatchSize);

    //this is test code
    //uint16_t myarr[240] = {65447, 65421, 65375, 65281, 65328, 65358, 65433, 65422, 65488, 65323, 65498, 65395, 65325, 65502, 65332, 65424, 65349, 65399, 65462, 65295, 65402, 65524, 65381, 65445, 65454, 65381, 65312, 65502, 65499, 65526, 65290, 65417, 65326, 65308, 65320, 65319, 65472, 65324, 65282, 65489, 65365, 65531, 65402, 65281, 65328, 65305, 65414, 65413, 65444, 65461, 65483, 65361, 65344, 65514, 65399, 65476, 65400, 65491, 65311, 65336, 65287, 65397, 65360, 65291, 65397, 65423, 65463, 65373, 65453, 65359, 65491, 65388, 65308, 65326, 65289, 65413, 65372, 65327, 65464, 65346, 65313, 65443, 65393, 65532, 65451, 65328, 65312, 65318, 65440, 65423, 65392, 65468, 65523, 65429, 65360, 65309, 65336, 65343, 65462, 65502, 65476, 65298, 65459, 65487, 65366, 65387, 65351, 65478, 65392, 65303, 65284, 65385, 65534, 65473, 65453, 65349, 65474, 65486, 65497, 65400, 65516, 65280, 65458, 65461, 65331, 65479, 65280, 65300, 65468, 65420, 65290, 65486, 65447, 65526, 65377, 65399, 65497, 65445, 65454, 65360, 65417, 65405, 65338, 65378, 65406, 65529, 65370, 65437, 65389, 65367, 65333, 65423, 65517, 65373, 65496, 65388, 65500, 65528, 65388, 65410, 65292, 65467, 65306, 65296, 65452, 65349, 65483, 65417, 65429, 65504, 65438, 65417, 65350, 65404, 65406, 65368, 65495, 65308, 65412, 65416, 65503, 65307, 65369, 65447, 65525, 65289, 65358, 65518, 65448, 65299, 65451, 65419, 65336, 65464, 65502, 65486, 65381, 65283, 65292, 65465, 65496, 65440, 65318, 65302, 65288, 65406, 65343, 65353, 65501, 65533, 65331, 65329, 65391, 65505, 65467, 65497, 65298, 65489, 65482, 65508, 65280, 65490, 65314, 65309, 65374, 65321, 65373, 65291, 65466, 65288, 65488, 65324, 65340, 65519, 65514, 65524, 65362, 65467, 65294, 65482};

    Serial.printf("Topic to send on: %s \n", base_topic.c_str());
    for (int i = swatchSize + 1; i < drawArea; i++)
    {

        String first_toppic = base_topic + i;
        first_toppic = first_toppic + "/";

        //read a line and encode
        fread(buff, sizeof(buff[0]), 240, drawing);
        String messsage = Base64::encode(buff, 240);

        //split the message into 4 parts and ship each part

        String final_topic;
        String final_message;
        unsigned int message_length = messsage.length() / 4;

        for (int j = 0; j < 4; j++)
        {
            final_topic = first_toppic + j;
            final_message = messsage.substring(message_length * j, message_length * (j + 1));
            MQTTclient->publish(final_topic.c_str(), final_message.c_str());
        }
    }

    fclose(drawing);
    delete[] buff;
}

void Screen::sendScreenToInternet()
{
    uint16_t *buff = new uint16_t[240];
    ; //the buffer that reads a line of file

    //this is test code
    //uint16_t myarr[240] = {65447, 65421, 65375, 65281, 65328, 65358, 65433, 65422, 65488, 65323, 65498, 65395, 65325, 65502, 65332, 65424, 65349, 65399, 65462, 65295, 65402, 65524, 65381, 65445, 65454, 65381, 65312, 65502, 65499, 65526, 65290, 65417, 65326, 65308, 65320, 65319, 65472, 65324, 65282, 65489, 65365, 65531, 65402, 65281, 65328, 65305, 65414, 65413, 65444, 65461, 65483, 65361, 65344, 65514, 65399, 65476, 65400, 65491, 65311, 65336, 65287, 65397, 65360, 65291, 65397, 65423, 65463, 65373, 65453, 65359, 65491, 65388, 65308, 65326, 65289, 65413, 65372, 65327, 65464, 65346, 65313, 65443, 65393, 65532, 65451, 65328, 65312, 65318, 65440, 65423, 65392, 65468, 65523, 65429, 65360, 65309, 65336, 65343, 65462, 65502, 65476, 65298, 65459, 65487, 65366, 65387, 65351, 65478, 65392, 65303, 65284, 65385, 65534, 65473, 65453, 65349, 65474, 65486, 65497, 65400, 65516, 65280, 65458, 65461, 65331, 65479, 65280, 65300, 65468, 65420, 65290, 65486, 65447, 65526, 65377, 65399, 65497, 65445, 65454, 65360, 65417, 65405, 65338, 65378, 65406, 65529, 65370, 65437, 65389, 65367, 65333, 65423, 65517, 65373, 65496, 65388, 65500, 65528, 65388, 65410, 65292, 65467, 65306, 65296, 65452, 65349, 65483, 65417, 65429, 65504, 65438, 65417, 65350, 65404, 65406, 65368, 65495, 65308, 65412, 65416, 65503, 65307, 65369, 65447, 65525, 65289, 65358, 65518, 65448, 65299, 65451, 65419, 65336, 65464, 65502, 65486, 65381, 65283, 65292, 65465, 65496, 65440, 65318, 65302, 65288, 65406, 65343, 65353, 65501, 65533, 65331, 65329, 65391, 65505, 65467, 65497, 65298, 65489, 65482, 65508, 65280, 65490, 65314, 65309, 65374, 65321, 65373, 65291, 65466, 65288, 65488, 65324, 65340, 65519, 65514, 65524, 65362, 65467, 65294, 65482};

    Serial.printf("Topic to send on: %s \n", base_topic.c_str());

    String screen_clear = base_topic + "0/0";
    MQTTclient->publish(screen_clear.c_str(), "A");

    for (int i = swatchSize + 1; i < drawArea; i++)
    {

        String first_toppic = base_topic + i;
        first_toppic = first_toppic + "/";

        //get the screen line by line
        tft.readRect(0, i, 240, 1, buff);

        //this chunk color corects the bad data in the buffer before saveing
        for (int j = 0; j < 240; j++)
        {
            uint32_t a = buff[j] << 8;
            a = a & 0xffff;
            uint32_t b = buff[j] >> 8;
            buff[j] = a + b;
        }
        String messsage = Base64::encode(buff, 240);

        //split the message into 4 parts and ship each part
        String final_topic;
        String final_message;
        unsigned int message_length = messsage.length() / 4;

        for (int j = 0; j < 4; j++)
        {
            final_topic = first_toppic + j;
            final_message = messsage.substring(message_length * j, message_length * (j + 1));
            bool not_black = false;

            //checks if the line to send is all black
            for (int k = 0; k < final_message.length(); k++)
            {
                if (final_message[k] != 'A')
                {
                    not_black = true;
                    break;
                }
            }
            //send only if the line is has some content
            if (not_black)
                MQTTclient->publish(final_topic.c_str(), final_message.c_str());
        }
    }

    //fclose(drawing);
    delete[] buff;
}