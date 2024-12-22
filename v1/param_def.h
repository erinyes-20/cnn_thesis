#ifndef PARAM_DEF
#define PARAM_DEF

    #define CONV_1_WEIGHTS_N 8 
    #define CONV_1_WEIGHTS_C 1 
    #define CONV_1_WEIGHTS_H 5 
    #define CONV_1_WEIGHTS_W 5
    #define CONV_1_BIAS_N 8 

    #define CONV_2_WEIGHTS_N 16 
    #define CONV_2_WEIGHTS_C 8 
    #define CONV_2_WEIGHTS_H 5 
    #define CONV_2_WEIGHTS_W 5 
    #define CONV_2_BIAS_N 16
     
    #define DENSE_3_WEIGHTS_H 10 
    #define DENSE_3_WEIGHTS_W 6400 
    #define DENSE_3_BIAS_N 10 

    #define BATCH_SIZE 10
    #define NUM_CHANNELS 1 
    #define IMG_HEIGHT 28 
    #define IMG_WIDTH 28 

    // ap_fixed params
    #define INPUT_DATA_WIDTH 32
    #define IN_INTG_WIDTH 16
    #define IN_QMODE AP_TRN
    #define IN_OMODE AP_WRAP


#endif
