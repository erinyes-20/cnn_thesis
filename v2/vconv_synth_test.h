#ifndef MERGE_VCONV
#define MERGE_VCONV

    #include <iostream>
    #include <stdlib.h>
    #include "bare_param_def.h"

//     #define VITIS_C_SIM
//    #define NORMAL_SIM
     #define VITIS_SYN

    #ifdef VITIS_C_SIM
        #include <ap_fixed.h>
        typedef ap_fixed<INPUT_DATA_WIDTH, IN_INTG_WIDTH, IN_QMODE, IN_OMODE> data_t;
    #endif

    #ifdef NORMAL_SIM
        typedef float data_t;
    #endif

    #ifdef VITIS_SYN
        #include <ap_fixed.h>
        typedef ap_fixed<INPUT_DATA_WIDTH, IN_INTG_WIDTH, IN_QMODE, IN_OMODE> data_t;
    #endif


void vconv(data_t INPUT_MAT[IMG_WIDTH * IMG_HEIGHT],
           data_t FILTER[CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W],
           data_t filter_bias,
           data_t RES[CONV_1_RES_H * CONV_1_RES_W]);
           
#endif
