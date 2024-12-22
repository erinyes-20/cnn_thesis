//___________________________PRIMTIVE________________________________

#include <iostream>
#include <stdlib.h>
#include "bare_param_def.h"

// #define VITIS_C_SIM
//#define NORMAL_SIM
// #define PRINT
 #define VITIS_SYN

#ifdef VITIS_C_SIM
    #include <ap_fixed.h>
    #include "vconv_synth_test.h"
#endif

#ifdef NORMAL_SIM
    typedef float data_t;
#endif

#ifdef VITIS_SYN
    #include <ap_fixed.h>
    #include "vconv_synth_test.h"
#endif

// Volume convolution
void vconv(data_t INPUT_MAT[IMG_WIDTH * IMG_HEIGHT],
           data_t FILTER[CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W],
           data_t filter_bias,
           data_t RES[CONV_1_RES_H * CONV_1_RES_W])
{

    // ___________________PULL_DATA_________________________________
    data_t input_mat[IMG_WIDTH * IMG_HEIGHT];
    data_t filter [CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W];
    data_t res [CONV_1_RES_H * CONV_1_RES_W];

    in_pull: for (int i=0; i<IMG_HEIGHT * IMG_WIDTH; i++){
        input_mat[i] = INPUT_MAT[i];
    }

    filter_pull: for (int i=0; i<CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W; i++){
        filter[i] = FILTER[i];    
    }

    res_init: for (int i=0; i<CONV_1_RES_H * CONV_1_RES_W; i++){
         res[i] = filter_bias;
    }
    
    data_t sum;

    // _____________________ACTUAL COMPUTE_____________________________
    vcon_res_H: for (int i=0; i<CONV_1_RES_H; i++){ 
        vcon_res_W: for (int j=0; j<CONV_1_RES_W; j++){

            sum = 0;
            
            vcon_fil_C: for (int k=0; k<CONV_1_WEIGHTS_C; k++){
                vcon_fil_H: for (int l=0; l<CONV_1_WEIGHTS_H; l++){                    
                    vcon_fil_W: for (int m=0; m<CONV_1_WEIGHTS_W; m++){
                        sum += filter[k*CONV_1_WEIGHTS_H*CONV_1_WEIGHTS_W + l*CONV_1_WEIGHTS_W + m] * input_mat[k*IMG_HEIGHT*IMG_WIDTH + (l+i)*IMG_WIDTH + m + j];
                    }
                }
            }

            res[i*CONV_1_RES_W + j] = sum;
        }
    }

    // __________________WRITE TO RES______________________________
    res_write: for (int i=0; i<CONV_1_RES_H * CONV_1_RES_W; i++){
        RES[i] = res[i];
    }
}




