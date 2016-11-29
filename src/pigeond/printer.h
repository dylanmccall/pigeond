// printer.h
// A module for printing bitmaps on the thermal printer
#ifndef _PRINTER_H
#define _PRINTER_H

//Print a string on the printer
//Warning very easy to break the program with special characters, stick to letters and numbers
void printer_printString(int fileDescriptor, const char *input);

//Transform a string into a pdf417 formatted barcode and print it
//Needs work to get working again. Left in as it would not be hard to update
//void printPDF417Barcode(char *input);

//Transform a string into a 2d data matrix and print it
//Needs work to get working again. Left in as it would not be hard to update
//void printDataMatrix(const unsigned char* input, int size);

//Prints a small bitmap (the adafruit logo), useful for testing printer settings
void printer_printTestBitmap(int fileDescriptor);

void printer_printQRCode(int fileDescriptor, const unsigned char* input, int size);

#endif
