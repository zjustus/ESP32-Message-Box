#include "brush.h"


Brush::Brush(){
    this->size = 5;
    this->color = 0xFFFF;

    swatch[0] = 0x0000; //black
    swatch[1] = 0xFFFF; //white
    swatch[2] = 0xF800; //red
    swatch[3] = 0x07E0; //green
    swatch[4] = 0x001F; //blue
    swatch[5] = 0xFFE0; //yellow
    swatch[6] = 0xF81F; //magenta
    swatch[7] = 0x07FF; //cyian
}
Brush::Brush(unsigned int size, unsigned int color){
    this->size = size;
    this->color = color;

    swatch[0] = 0x0000; //black
    swatch[1] = 0xFFFF; //white
    swatch[2] = 0xF800; //red
    swatch[3] = 0x07E0; //green
    swatch[4] = 0x001F; //blue
    swatch[5] = 0xFFE0; //yellow
    swatch[6] = 0xF81F; //magenta
    swatch[7] = 0x07FF; //cyian
}

unsigned int Brush::getSize(){
    return this->size;
}
void Brush::setSize(unsigned int size){
    this->size = size;
}

unsigned int Brush::getColor(){
    return this->color;
}
void Brush::setColor(unsigned int color){
    this->color = color;
}

void Brush::setSwatch(unsigned int swatch, unsigned int color){
    this->swatch[swatch] = color;
}

unsigned int Brush::getSwatchColor(unsigned int swatch){
    return this->swatch[swatch];
}
void Brush::setColorfromSwatch(unsigned int swatch){
    this->color = this->swatch[swatch];
}

unsigned int Brush::getSwatchFromColor(unsigned int color){
    if(this->swatch[0] == color) return 0;
    else if(this->swatch[1] == color) return 1;
    else if(this->swatch[2] == color) return 2;
    else if(this->swatch[3] == color) return 3;
    else if(this->swatch[4] == color) return 4;
    else if(this->swatch[5] == color) return 5;
    else if(this->swatch[6] == color) return 6;
    else if(this->swatch[7] == color) return 7;
    else return 0;
}