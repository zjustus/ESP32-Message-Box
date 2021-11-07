/* 
The Brush Class
    This class defines the paint brush. it will have swatches(colors) and brush size

*/

class Brush{
private:
unsigned int size;
unsigned int color; //is this redundant?
//int swatches[]; //this should be an array of hex values, not ints

unsigned int swatch[8];



public:
Brush();
Brush(unsigned int size, unsigned int color);

//gets n sets brush size
unsigned int getSize();
void setSize(unsigned int size);

//gets n sets brush color
unsigned int getColor();
void setColor(unsigned int color);


//this sets a swatch color
void setSwatch(unsigned int swatch, unsigned int color);
//this gets a swatch color
unsigned int getSwatchColor(unsigned int swatch);

//this sets brush color from specific swatch
void setColorfromSwatch(unsigned int swatch);

//this gets a swatch number from a color, default is first swatch
unsigned int getSwatchFromColor(unsigned int color);

};