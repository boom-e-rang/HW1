#ifndef IMU_HELPER_H__
#define IMU_HELPER_H__

void read_all(unsigned char * array, int address, int number);
void write_to_slave(int register_address, int number);
int sign(int x);
void convert_acc(unsigned char * array, int * acc_all);

#endif
