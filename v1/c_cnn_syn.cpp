// ___________________________________________CNN_SYN BEGINS__________________________________________________________
#include "my_primitives.cpp" 
// primitives are defined to work with ap_fixed data, hence the typedefs defined there
// all global constants are defined so that vitis can calculate the latencies of loops

// import weights, test img, labels in form of arrays
// don't, this slows down compiler, use weights in separate file
//#include "c_test_imgs.h"    // test_images in NCHW
//#include "c_test_labels.h"   // test_labels in N


// model parameters will also form the INTERFACE to your module
void full_model(data_t* test_images, 
           data_t* preds, 
           data_t* fp_conv_1_weights, 
           data_t* fp_conv_1_bias, 
           data_t* fp_conv_2_weights, 
           data_t* fp_conv_2_bias, 
           data_t* fp_dense_weights, 
           data_t* fp_dense_bias,
           )
           
{
    // _______________________INPUT LOOPS TO LOAD INTERNAL WEIGHTS, DATA ARRAYS____________________________
    data_t fp_CONV_1_WEIGHTS[CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W];
    data_t fp_CONV_1_BIAS[CONV_1_BIAS_N]; 
    data_t fp_CONV_2_WEIGHTS[CONV_2_WEIGHTS_N * CONV_2_WEIGHTS_C * CONV_2_WEIGHTS_H * CONV_2_WEIGHTS_W];
    data_t fp_CONV_2_BIAS[CONV_2_BIAS_N];
    data_t fp_DENSE_3_WEIGHTS[DENSE_3_WEIGHTS_H * DENSE_3_WEIGHTS_W];
    data_t fp_DENSE_3_BIAS[DENSE_3_BIAS_N];
    data_t fp_test_imgs[BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH];

    d_conv1_w_loop: for (int i=0; i < CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W; i++){
        fp_CONV_1_WEIGHTS[i] = fp_conv_1_weights[i];
    }

    d_conv1_b_loop: for (int i=0; i < CONV_1_BIAS_N; i++){
        fp_CONV_1_BIAS[i] = fp_conv_1_bias[i];
    }

    d_conv2_w_loop: for (int i=0; i < CONV_2_WEIGHTS_N * CONV_2_WEIGHTS_C * CONV_2_WEIGHTS_H * CONV_2_WEIGHTS_W; i++){
        fp_CONV_2_WEIGHTS[i] = fp_conv_2_weights[i];
    }

    d_conv2_b_loop: for (int i=0; i< CONV_2_BIAS_N; i++){
        fp_CONV_2_BIAS[i] = fp_conv_2_bias[i];
    }

    d_dens_w_loop: for (int i=0; i < DENSE_3_WEIGHTS_H * DENSE_3_WEIGHTS_W; i++){
        fp_DENSE_3_WEIGHTS[i] = fp_dense_weights[i];
    }

    d_dens_b_loop: for (int i=0; i< DENSE_3_BIAS_N; i++){
        fp_DENSE_3_BIAS[i] = fp_dense_bias[i];
    }

    // ___________________________________DATA PULL____________________________________________________________
    d_test_data_loop: for (int i=0; i < BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH; i++){
        fp_test_imgs[i] = test_images[i];
    }

    // _____________________________________FULL_MODEL__________________________________________________________
    data_t conv_1_res [BATCH_SIZE * CONV_1_WEIGHTS_N * (IMG_HEIGHT-CONV_1_WEIGHTS_H+1) * (IMG_WIDTH-CONV_1_WEIGHTS_W+1)];
    data_t conv_2_res [BATCH_SIZE * CONV_2_WEIGHTS_N * (IMG_HEIGHT-CONV_1_WEIGHTS_H+1-CONV_2_WEIGHTS_H+1) * (IMG_WIDTH-CONV_1_WEIGHTS_W+1-CONV_2_WEIGHTS_W+1)];
    data_t output_probs [BATCH_SIZE * DENSE_3_WEIGHTS_H];
    data_t preds_val [BATCH_SIZE];

    full_conv(test_images, BATCH_SIZE, NUM_CHANNELS, IMG_HEIGHT, IMG_WIDTH,
              fp_CONV_1_WEIGHTS, CONV_1_WEIGHTS_N, CONV_1_WEIGHTS_C, CONV_1_WEIGHTS_H, CONV_1_WEIGHTS_W,
              fp_CONV_1_BIAS, CONV_1_BIAS_N, 
              conv_1_res, BATCH_SIZE, CONV_1_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1));

    ReLU_conv(conv_1_res, BATCH_SIZE, CONV_1_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1),
              conv_1_res, BATCH_SIZE, CONV_1_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1));

    full_conv(conv_1_res, BATCH_SIZE, CONV_1_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1),
              fp_CONV_2_WEIGHTS, CONV_2_WEIGHTS_N, CONV_2_WEIGHTS_C, CONV_2_WEIGHTS_H, CONV_2_WEIGHTS_W,
              fp_CONV_2_BIAS, CONV_2_BIAS_N, 
              conv_2_res, BATCH_SIZE, CONV_2_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1-CONV_2_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1-CONV_2_WEIGHTS_W+1) );

    ReLU_conv(conv_2_res, BATCH_SIZE, CONV_2_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1-CONV_2_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1-CONV_2_WEIGHTS_W+1),
              conv_2_res, BATCH_SIZE, CONV_2_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1-CONV_2_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1-CONV_2_WEIGHTS_W+1));
    
    full_dense(conv_2_res, BATCH_SIZE, CONV_2_WEIGHTS_N * (IMG_HEIGHT-CONV_1_WEIGHTS_H+1-CONV_2_WEIGHTS_H+1) * (IMG_WIDTH-CONV_1_WEIGHTS_W+1-CONV_2_WEIGHTS_W+1), 
               fp_DENSE_3_WEIGHTS, DENSE_3_WEIGHTS_H, DENSE_3_WEIGHTS_W,
               fp_DENSE_3_BIAS, DENSE_3_BIAS_N,
               output_probs, BATCH_SIZE, DENSE_3_WEIGHTS_H);   


    predict_class(output_probs, BATCH_SIZE, DENSE_3_WEIGHTS_H, preds_val, BATCH_SIZE);

    // _______________________________WRITE predicted classes to output__________________________________________
    for (int i=0; i<BATCH_SIZE; i++){
        preds[i] = preds_val[i];
    }
}

