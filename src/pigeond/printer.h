// printer.h
// A module for printing bitmaps on the thermal printer
#ifndef _PRINTER_H
#define _PRINTER_H

//Print a string on the printer
//Warning very easy to break the program with special characters, stick to letters and numbers
void printString(char *input);

//Transform a string into a pdf417 formatted barcode and print it
void printPDF417Barcode(char *input);

//Transform a string into a 2d data matrix and print it
void printDataMatrix(char* input);

//Prints a small bitmap (the adafruit logo), useful for testing printer settings
void printTestBitmap();

#endif
