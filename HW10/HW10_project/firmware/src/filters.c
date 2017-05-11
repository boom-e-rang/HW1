
#include <xc.h> 
#include "filters.h"
#include "i2c_master_noint.h"
#include "ILI9163C.h"

#define MAX_LENGTH 15
#define A 0.8
#define B 0.2

int q_index=0;
int q[MAX_LENGTH];
int q2[MAX_LENGTH];

int previous_value=0;

int MAF(int acc_z, int iteration) {
    
    int i=0;
    int ave = acc_z;
    if (iteration==1) {

        for (i=0; i<MAX_LENGTH; i++) {
            q[i]=acc_z;
        }
        q_index=0;
    
    } else {
    
        q[q_index]=acc_z;
        q_index++;
        if (q_index==(MAX_LENGTH-1)) {
            q_index=0;
        }

        int sum=0;
        for (i=0; i<MAX_LENGTH; i++) {
            sum=sum+q[i];
        }
        ave = sum/MAX_LENGTH;
        
    }
    
    return(ave);
    
}

int IIR(int acc_z, int iteration) {
    
    int new_average=acc_z;
    
    if (iteration==1) {
        
        previous_value=acc_z;
        
    } else {
        
        new_average = A*previous_value + B*acc_z;
        previous_value=new_average;
        
    }
    
    return(new_average);
}

int FIR(int acc_z, int iteration) {
    
    float weights[] = {-0.0026, -0.0067, -0.0114, 0.0000, 0.0483, 0.1319, 0.2153, 0.2505, 0.2153, 0.1319, 0.0483, 0.0000, -0.0114, -0.0067, -0.0026};
    
    int i=0;
    
    if (iteration==1) {

        for (i=0; i<MAX_LENGTH; i++) {
            q2[i]=acc_z;
        }
    
    } else {
        
        for (i=0; i<(MAX_LENGTH-1); i++) {
            q2[i] = q2[i+1];
        }
        q2[MAX_LENGTH-1]= acc_z;

    }
    
    int sum=0;
    for (i=0; i<MAX_LENGTH; i++) {
        sum=sum + ((int) (weights[i] * ((float) q2[i])));
    }
    
    // sum=sum/MAX_LENGTH;
    
    return(sum);
    
}