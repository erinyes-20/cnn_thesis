#include <iostream>
#include "my_primitives.cpp"

typedef float DTYPE;

// import test img, label files in form of arrays
#include "c_test_imgs.h"    // test_images in NCH*W
#include "c_test_labels.h"   // test_labels in N


// import weight matrices as arrays with proper dimensions
#include "c_test_weights.h"


// use primitives to setup model pipeline and send weights, data along them
void model(DTYPE* input, int in_N, int in_C, int in_H, int in_W,
           DTYPE* conv_1_res, int conv_1_res_N, int conv_1_res_C, int conv_1_res_H, int conv_1_res_W,
           DTYPE* conv_2_res, int conv_2_res_N, int conv_2_res_C, int conv_2_res_H, int conv_2_res_W,
           DTYPE* output, int out_N, int out_H)
{
    // insert network arch here

    full_conv(input, in_N, in_C, in_H, in_W, 
              CONV_1_WEIGHTS, CONV_1_WEIGHTS_N, CONV_1_WEIGHTS_C, CONV_1_WEIGHTS_H, CONV_1_WEIGHTS_W,
              CONV_1_BIAS, CONV_1_BIAS_N, 
              conv_1_res, conv_1_res_N, conv_1_res_C, conv_1_res_H, conv_1_res_W);

    ReLU_conv(conv_1_res, conv_1_res_N, conv_1_res_C, conv_1_res_H, conv_1_res_W,
              conv_1_res, conv_1_res_N, conv_1_res_C, conv_1_res_H, conv_1_res_W);

    full_conv(conv_1_res, conv_1_res_N, conv_1_res_C, conv_1_res_H, conv_1_res_W,
              CONV_2_WEIGHTS, CONV_2_WEIGHTS_N, CONV_2_WEIGHTS_C, CONV_2_WEIGHTS_H, CONV_2_WEIGHTS_W,
              CONV_2_BIAS, CONV_2_BIAS_N, 
              conv_2_res, conv_2_res_N, conv_2_res_C, conv_2_res_H, conv_2_res_W );

    ReLU_conv(conv_2_res, conv_2_res_N, conv_2_res_C, conv_2_res_H, conv_2_res_W,
              conv_2_res, conv_2_res_N, conv_2_res_C, conv_2_res_H, conv_2_res_W);
    
    full_dense(conv_2_res, conv_2_res_N, conv_2_res_C*conv_2_res_H*conv_2_res_W, 
          DENSE_3_WEIGHTS, DENSE_3_WEIGHTS_H, DENSE_3_WEIGHTS_W,
          DENSE_3_BIAS, DENSE_3_BIAS_N,
          output, out_N, out_H);   

}


// comparison function to check model output vs labels file
int main(){

    // calc intermediate dimensions and malloc arrays
    DTYPE conv_1_res [10 * 32 * 24 * 24];
    DTYPE conv_2_res [10 * 64 * 20 * 20];
    DTYPE output [10*10];
    DTYPE pred [10];

    // pass to model
    model(test_images, 10, 1, 28, 28,
          conv_1_res, 10, 32, 24, 24, 
          conv_2_res, 10, 64, 20, 20,
          output, 10, 10);

    // max over output probs
    for (int i=0; i<10; i++){
        DTYPE pred_val_prob = INT_MIN;

        for (int pred_num=0; pred_num<10; pred_num++){
            if (output[i*10 + pred_num] > pred_val_prob){                
                pred_val_prob = output[i*10 + pred_num];
                pred[i] = pred_num;
            }
        }
    }

    // check output vs test_labels
    display_mat(pred, 10, 1);

}


