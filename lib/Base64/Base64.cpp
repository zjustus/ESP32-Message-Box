#include "Base64.h"
String Base64::encode(uint16_t *x, size_t size_x)
{
    size_t size_y;
    uint8_t *y = uint16_to_uint8(x, size_x, size_y);
    String output = encode(y, size_y);
    delete[] y;
    return output;
}

String Base64::encode(uint8_t *y, size_t size_y)
{
    String encoded = "";
    for (int i = 0; i < size_y; i += 3)
    {
        uint8_t a = (y[i] & 0xFC) >> 2;                                 //the first 6 bits of 1
        uint8_t b = ((y[i] & 0x3) << 4) | ((y[i + 1] & 0xF0) >> 4);     //the last 2 bist of 1 plus the first 4 of 2
        uint8_t c = ((y[i + 1] & 0xF) << 2) | ((y[i + 2] & 0xC0) >> 6); //the last 4 of 2 plus the first 2 of 3
        uint8_t d = y[i + 2] & 0x3F;                                    //the last 6 bits of 3

        encoded += toAscii(a);
        encoded += toAscii(b);
        encoded += toAscii(c);
        encoded += toAscii(d);
    }
    return encoded;
}

uint16_t *Base64::decode_to_uint16(const char *data, size_t &size)
{
    size_t x_size;
    uint8_t *x = decode_to_uint8(data, x_size);
    uint16_t *y = uint8_to_uint16(x, x_size, size); //this needs to be dynamic
    delete[] x;
    return y;
}

uint8_t *Base64::decode_to_uint8(const char *data, size_t &size)
{
    String decoded = "";
    String data_string = data;

    uint8_t *stage1 = new uint8_t[data_string.length()];
    JustusDevTools::LinkedList<uint8_t> stage2;

    //convert ascii to uint6
    for (int i = 0; i < data_string.length(); i++)
    {
        stage1[i] = fromAscii(data_string[i]);
    }

    //bit shift 4 to 3
    for (int i = 0; i < data_string.length(); i += 4)
    {
        uint8_t a = ((stage1[i] & 0x3F) << 2) | ((stage1[i + 1] & 0x30) >> 4);    //first 6 of 1 and first 2 of 2
        uint8_t b = ((stage1[i + 1] & 0xF) << 4) | ((stage1[i + 2] & 0x3C) >> 2); //last 4 of 2 and first 4 of 3
        uint8_t c = ((stage1[i + 2] & 0x3) << 6) | (stage1[i + 3] & 0x3F);        //last 2 of 3 and first 6 of 4

        stage2.add(a);
        stage2.add(b);
        stage2.add(c);
    }

    delete[] stage1;
    size = stage2.length();
    return stage2.to_array();
}

uint8_t *Base64::uint16_to_uint8(uint16_t *x, size_t size_x, size_t &outputLength)
{
    JustusDevTools::LinkedList<uint8_t> y;
    for (size_t i = 0; i < size_x; i++)
    {
        y.add(x[i] & 0xff);
        y.add(x[i] >> 8);
    }
    outputLength = y.length();
    return y.to_array();
}

uint16_t *Base64::uint8_to_uint16(uint8_t *x, size_t size_x, size_t &outputLength)
{
    JustusDevTools::LinkedList<uint16_t> y;
    for (size_t i = 0; i < size_x; i += 2)
    {
        y.add(((uint16_t)x[i]) | ((uint16_t)x[i + 1] << 8));
    }

    outputLength = y.length();
    return y.to_array();
}

char Base64::toAscii(uint8_t x)
{
    switch (x)
    {
    case 0:
        return 'A';
    case 1:
        return 'B';
    case 2:
        return 'C';
    case 3:
        return 'D';
    case 4:
        return 'E';
    case 5:
        return 'F';
    case 6:
        return 'G';
    case 7:
        return 'H';
    case 8:
        return 'I';
    case 9:
        return 'J';
    case 10:
        return 'K';
    case 11:
        return 'L';
    case 12:
        return 'M';
    case 13:
        return 'N';
    case 14:
        return 'O';
    case 15:
        return 'P';
    case 16:
        return 'Q';
    case 17:
        return 'R';
    case 18:
        return 'S';
    case 19:
        return 'T';
    case 20:
        return 'U';
    case 21:
        return 'V';
    case 22:
        return 'W';
    case 23:
        return 'X';
    case 24:
        return 'Y';
    case 25:
        return 'Z';
    case 26:
        return 'a';
    case 27:
        return 'b';
    case 28:
        return 'c';
    case 29:
        return 'd';
    case 30:
        return 'e';
    case 31:
        return 'f';
    case 32:
        return 'g';
    case 33:
        return 'h';
    case 34:
        return 'i';
    case 35:
        return 'j';
    case 36:
        return 'k';
    case 37:
        return 'l';
    case 38:
        return 'm';
    case 39:
        return 'n';
    case 40:
        return 'o';
    case 41:
        return 'p';
    case 42:
        return 'q';
    case 43:
        return 'r';
    case 44:
        return 's';
    case 45:
        return 't';
    case 46:
        return 'u';
    case 47:
        return 'v';
    case 48:
        return 'w';
    case 49:
        return 'x';
    case 50:
        return 'y';
    case 51:
        return 'z';
    case 52:
        return '0';
    case 53:
        return '1';
    case 54:
        return '2';
    case 55:
        return '3';
    case 56:
        return '4';
    case 57:
        return '5';
    case 58:
        return '6';
    case 59:
        return '7';
    case 60:
        return '8';
    case 61:
        return '9';
    case 62:
        return '+';
    case 63:
        return '/';
    default:
        return ' ';
    }
}
uint8_t Base64::fromAscii(char x)
{
    switch (x)
    {
    case 'A':
        return 0;
    case 'B':
        return 1;
    case 'C':
        return 2;
    case 'D':
        return 3;
    case 'E':
        return 4;
    case 'F':
        return 5;
    case 'G':
        return 6;
    case 'H':
        return 7;
    case 'I':
        return 8;
    case 'J':
        return 9;
    case 'K':
        return 10;
    case 'L':
        return 11;
    case 'M':
        return 12;
    case 'N':
        return 13;
    case 'O':
        return 14;
    case 'P':
        return 15;
    case 'Q':
        return 16;
    case 'R':
        return 17;
    case 'S':
        return 18;
    case 'T':
        return 19;
    case 'U':
        return 20;
    case 'V':
        return 21;
    case 'W':
        return 22;
    case 'X':
        return 23;
    case 'Y':
        return 24;
    case 'Z':
        return 25;
    case 'a':
        return 26;
    case 'b':
        return 27;
    case 'c':
        return 28;
    case 'd':
        return 29;
    case 'e':
        return 30;
    case 'f':
        return 31;
    case 'g':
        return 32;
    case 'h':
        return 33;
    case 'i':
        return 34;
    case 'j':
        return 35;
    case 'k':
        return 36;
    case 'l':
        return 37;
    case 'm':
        return 38;
    case 'n':
        return 39;
    case 'o':
        return 40;
    case 'p':
        return 41;
    case 'q':
        return 42;
    case 'r':
        return 43;
    case 's':
        return 44;
    case 't':
        return 45;
    case 'u':
        return 46;
    case 'v':
        return 47;
    case 'w':
        return 48;
    case 'x':
        return 49;
    case 'y':
        return 50;
    case 'z':
        return 51;
    case '0':
        return 52;
    case '1':
        return 53;
    case '2':
        return 54;
    case '3':
        return 55;
    case '4':
        return 56;
    case '5':
        return 57;
    case '6':
        return 58;
    case '7':
        return 59;
    case '8':
        return 60;
    case '9':
        return 61;
    case '+':
        return 62;
    case '/':
        return 63;
    default:
        return 64;
    }
}
