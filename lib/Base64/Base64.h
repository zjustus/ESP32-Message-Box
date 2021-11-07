#pragma once
#include <Arduino.h>
#include <LinkedList.h>

class Base64
{
public:
   /** 
     * Encodes uint16_t array to Base64 String
     * @param x the arry to pass
     * @param size_x the size of the array to encode
     **/
   static String encode(uint16_t *x, size_t size_x);

   /**
     * Encodes uint8_t array to Base64 String
     * @param y the array to apss
     * @param size_y the size of the array
     **/
   static String encode(uint8_t *y, size_t size_y);

   /** 
     * Decodes Base64 string to uint16_t array
     * @param data the string of data to decode
     * @param size the length of the array that will be returned
     **/
   static uint16_t *decode_to_uint16(const char *data, size_t &size);

   /** 
     * Decodes Base64 string to uint8_t array
     * @param data the string of data to decode
     * @param size the length of the array that will be returned
     **/
   static uint8_t *decode_to_uint8(const char *data, size_t &size);

private:
   /**
     * This Converts uint16 var arrays to uint8 arrays of twice the length
     * @param x The array to convert
     * @param size_x The length of the array passed
     * @param outputLength The length of the array returned
     **/
   static uint8_t *uint16_to_uint8(uint16_t *x, size_t size_x, size_t &outputLength);

   /**
     * This Converts uint8 var arrays to uint16 arrays of half the length
     * @param x The array to convert
     * @param size_x The length of the array passed
     * @param outputLength The length of the array returned
     **/
   static uint16_t *uint8_to_uint16(uint8_t *x, size_t size_x, size_t &outputLength);

   /**
     * Returns a charector of the base64 equivelent of the number passed
     * @param x A 6bit number to convert to ASCII 
     **/
   static char toAscii(uint8_t x);

   /**
     * Returns a 6 bit number equivelent to the Base64 charector passed
     * @param x A Base64 ASCII charector
     **/
   static uint8_t fromAscii(char x);
};