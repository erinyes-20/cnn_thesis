#ifndef MERGE_H
#define MERGE_H

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


    void bare_model(data_t* test_images, 
            data_t* preds, 
            data_t* fp_conv_1_weights, 
            data_t* fp_conv_1_bias,  
            data_t* fp_dense_2_weights, 
            data_t* fp_dense_2_bias);
           
#endif
