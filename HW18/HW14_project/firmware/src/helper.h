#ifndef HELPER_H__
#define HELPER_H__

void read_all(unsigned char * array, int address, int number);
void print_to_LCD(char * message, unsigned short color, unsigned short background, int x, int y);
void display_character(char character, unsigned short color, unsigned short background, int x, int y);
void write_to_slave(int register_address, int number);
int sign(int x);
void convert(unsigned char * array, int * acc_gyr);

#endif
