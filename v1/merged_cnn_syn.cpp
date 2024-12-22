// _____________________________________AP_FIXED PRIMITIVES ONLY______________________________________________

#include <iostream>
#include <stdlib.h>
#include "param_def.h"

// #define VITIS_C_SIM
// #define NORMAL_SIM
// #define PRINT
#define VITIS_SYN

#ifdef VITIS_C_SIM
    #include <ap_fixed.h>
    #include "merged_cnn_syn.h"
#endif

#ifdef NORMAL_SIM
    typedef float data_t;
#endif

#ifdef VITIS_SYN
    #include <ap_fixed.h>
    #include "merged_cnn_syn.h"
#endif


// all primitives work with ap_fixed, arithmetic operations are well defined
void display_mat(data_t* output, int rows, int cols){
    for (int i=0; i<rows; i++){
        for (int j=0; j<cols; j++){
            #ifdef VITIS_C_SIM
                std::cout << output[i * cols + j].to_string(10).c_str() << " " << " ";
            #endif
            #ifdef NORMAL_SIM
                std::cout << output[i * cols + j] << " " << " ";
            #endif
        }
        std::cout << "\n";
    }
}

data_t dot_prod(data_t* vec1, data_t* vec2, int len){
    data_t sum=0;
    for (int i=0; i<len; i++){
        sum += vec1[i] * vec2[i];
    }
    return sum;
}

void ReLU (data_t* mat, int matrows, int matcols, int matdepth,
           data_t* res, int resrows, int rescols, int resdepth)
{
    if (matrows != resrows || matcols != rescols || matdepth != resdepth) {std::cout << "RELU DIMEN ERROR\n";return;}

    rel_i: for (int i=0; i<matdepth; i++){
        rel_j: for (int j=0; j<matrows; j++){
            rel_k: for (int k=0; k<matcols; k++){
                res[i*matrows*matcols + j*matcols + k] = (mat[i*matrows*matcols + j*matcols + k] > 0) ? mat[i*matrows*matcols + j*matcols + k] : data_t(0);
            }
        }
    }
}

void ReLU_conv(data_t* mat, int mat_N, int mat_C, int mat_H, int mat_W,
               data_t* res, int res_N, int res_C, int res_H, int res_W)
{
    c_rel_N: for (int i=0; i<mat_N; i++){
        c_rel_C: for (int j=0; j<mat_C; j++){
            c_rel_H: for (int k=0; k<mat_H; k++){
                c_rel_W: for (int l=0; l<mat_W; l++){
                    res[i*mat_C*mat_H*mat_W + j*mat_H*mat_W + k*mat_W + l] = (mat[i*mat_C*mat_H*mat_W + j*mat_H*mat_W + k*mat_W + l] > 0) ? mat[i*mat_C*mat_H*mat_W + j*mat_H*mat_W + k*mat_W + l] : data_t(0);
                }
            }
        }
    }
}

void mat_mat_mul(data_t* mat1, int mat1rows, int mat1cols, 
                 data_t* mat2, int mat2rows, int mat2cols,
                 data_t* res, int resrows, int rescols)
{
    if (mat1cols != mat2rows) {std::cout << "MATMUL DIMENSION ERROR\n";return;}

    mm_i: for (int i=0; i<mat1rows; i++){
        mm_j: for (int j=0; j<mat2cols; j++){
            res[i*rescols + j] = 0;
            mm_k: for (int k=0; k<mat1cols; k++){
                res[i*rescols + j] += mat1[i*mat1cols + k] * mat2[k*mat2cols + j];
            }
        }
    }
}

void vec_add (data_t* vec1, int vec1len, 
              data_t* vec2, int vec2len, 
              data_t* res, int reslen)
{
    // if (vec1len != vec2len) {std::cout << "ADD VEC DIMEN ERROR"; return;}
    vadd: for (int i=0; i<vec1len; i++){
        res[i] = vec1[i] + vec2[i];
    }
}

void dense(data_t* input_vec, int inlen,
           data_t* weight_mat, int weightrows, int weightcols,
           data_t* bias_vec, int biaslen,
           data_t* output_vec, int output_len)
{
    if (weightcols != inlen || output_len != weightrows || output_len != biaslen) {std::cout << "DENSE DIMEN ERROR\n";return;} 
    
    mat_mat_mul(weight_mat, weightrows, weightcols, input_vec, inlen, 1, output_vec, output_len, 1);
    // INPLACE ADD OP, SIMUL READ WRITE
    vec_add(output_vec, output_len, bias_vec, biaslen, output_vec, output_len);
}

void full_dense(data_t* input, int in_N, int in_H,
                data_t* weight_mat, int weight_H, int weight_W,
                data_t* bias_vec, int bias_N,
                data_t* out, int out_N, int out_H)
{
    full_den_N: for (int in_id=0; in_id<in_N; in_id++){

        data_t* current_in = input + in_id*in_H;
        data_t* current_res_vec = out + in_id*out_H;

        dense(current_in, in_H, 
              weight_mat, weight_H, weight_W,
              bias_vec, bias_N, 
              current_res_vec, out_H);

    }
}



// PCONV performs planar conv between 1 kernel and 1 input matrix (depth of both = 1)
void pconv(data_t* input_mat, int inrows, int incols, 
           data_t* kernel, int kernelrows, int kernelcols, 
           data_t* res, int resrows, int rescols )
{
    if ( ((inrows - kernelrows)/1 + 1 ) != resrows ) {std::cout << "PCONV ROW DIMEN ERROR\n";return;}
    if ( ((incols - kernelcols)/1 + 1 ) != rescols ) {std::cout << "PCONV COL DIMEN ERROR\n";return;}

    for (int i=0; i<resrows; i++){
        for (int j=0; j<rescols; j++){

            res[i*rescols + j] = 0;  

            // traveling over kernel           
            for (int k=0; k<kernelrows; k++){
                for (int l=0; l<kernelcols; l++){
                    res[i*rescols + j] += kernel[k*kernelcols + l] * input_mat[k*incols + l + j + i*incols];
                }
            }
        }
    }
}

// Volume convolution
void vconv(data_t* input_mat, int in_C, int in_H, int in_W,
           data_t* filter, int filter_C, int filter_H, int filter_W, 
           data_t filter_bias,
           data_t* res, int res_H, int res_W)
{
#pragma HLS ARRAY_PARTITION type=complete variable=filter

    if ( ((in_H - filter_H)/1 + 1 ) != res_H ) {std::cout << "VCONV ROW DIMEN ERROR\n";return;}
    if ( ((in_W - filter_W)/1 + 1 ) != res_W ) {std::cout << "VCONV COL DIMEN ERROR\n";return;}
    if ( in_C != filter_C ) {std::cout << "VCONV DEPTH MISMATCH\n";return;}

    vcon_res_H: for (int i=0; i<res_H; i++){
        vcon_res_W: for (int j=0; j<res_W; j++){
            
            // initialize with bias and accum
            res[i*res_W + j] = filter_bias;
            
            vcon_fil_C: for (int k=0; k<filter_C; k++){
                vcon_fil_H: for (int l=0; l<filter_H; l++){
                    vcon_fil_W: for (int m=0; m<filter_W; m++){
                        res[i*res_W + j] += filter[k*filter_H*filter_W + l*filter_W + m] * input_mat[k*in_W*in_H + (l+i)*in_W + m + j];
                    }
                }
            }
        }
    }
}


void full_conv(data_t* input_mat, int in_N, int in_C, int in_H, int in_W,
              data_t* filter, int filter_N, int filter_C, int filter_H, int filter_W,
              data_t* filter_biases, int filter_biases_N,
              data_t* res, int res_N, int res_C, int res_H, int res_W)
{   
    if (res_C != filter_N) {std::cout << "TOTAL FILTER NUMBER OUTPUT CHANNELS MISMATCH\n";}

    fconv_N: for (int img_id=0; img_id<in_N; img_id++){
        
        // isolating next image and output
        data_t* current_img = input_mat + img_id*in_C*in_H*in_W;
        data_t* current_res = res + img_id*res_C*res_H*res_W;

        fconv_fil_N: for (int fil_num=0; fil_num<filter_N; fil_num++){
            // isolating next filter and res framewha

            data_t* current_filter = filter + fil_num*filter_C*filter_H*filter_W;
            data_t* current_res_frame = current_res + fil_num*res_H*res_W;
            data_t current_bias = filter_biases[fil_num];

            // convolving img with filter (vconv)
            vconv(current_img, in_C, in_H, in_W, current_filter, filter_C, filter_H, filter_W, current_bias, current_res_frame, res_H, res_W);

        }         
    }
}

void predict_class(data_t* output_probs, int out_H, int out_W,
                   data_t* preds, int preds_N)
{
    #ifdef PRINT
        // display probs
        display_mat(output_probs, out_H, out_W);
    #endif
    
    // max over output probs
    pclass_H: for (int i=0; i<preds_N; i++){
        data_t pred_val_prob = INT_MIN;

        pclass_W: for (int pred_num=0; pred_num < out_W; pred_num++){
            if (output_probs[i*10 + pred_num] > pred_val_prob){                
                pred_val_prob = output_probs[i*10 + pred_num];
                preds[i] = pred_num;
            }
        }
    }
}

//___________________________________________END OF PRIMITIVE___________________________________________________

// ___________________________________________CNN_SYN BEGINS__________________________________________________________

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
           data_t* fp_dense_bias)
           
{
    // _______________________INPUT LOOPS TO LOAD INTERNAL WEIGHTS, DATA ARRAYS____________________________
    data_t fp_CONV_1_WEIGHTS[CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W];
    data_t fp_CONV_1_BIAS[CONV_1_BIAS_N]; 
    data_t fp_CONV_2_WEIGHTS[CONV_2_WEIGHTS_N * CONV_2_WEIGHTS_C * CONV_2_WEIGHTS_H * CONV_2_WEIGHTS_W];
    data_t fp_CONV_2_BIAS[CONV_2_BIAS_N];
    data_t fp_DENSE_3_WEIGHTS[DENSE_3_WEIGHTS_H * DENSE_3_WEIGHTS_W];
    data_t fp_DENSE_3_BIAS[DENSE_3_BIAS_N];
    data_t fp_test_imgs[BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH];

    p_conv1_w_loop: for (int i=0; i < CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W; i++){
        fp_CONV_1_WEIGHTS[i] = fp_conv_1_weights[i];
    }

    p_conv1_b_loop: for (int i=0; i < CONV_1_BIAS_N; i++){
        fp_CONV_1_BIAS[i] = fp_conv_1_bias[i];
    }

    p_conv2_w_loop: for (int i=0; i < CONV_2_WEIGHTS_N * CONV_2_WEIGHTS_C * CONV_2_WEIGHTS_H * CONV_2_WEIGHTS_W; i++){
        fp_CONV_2_WEIGHTS[i] = fp_conv_2_weights[i];
    }

    p_conv2_b_loop: for (int i=0; i< CONV_2_BIAS_N; i++){
        fp_CONV_2_BIAS[i] = fp_conv_2_bias[i];
    }

    p_dens_w_loop: for (int i=0; i < DENSE_3_WEIGHTS_H * DENSE_3_WEIGHTS_W; i++){
        fp_DENSE_3_WEIGHTS[i] = fp_dense_weights[i];
    }

    p_dens_b_loop: for (int i=0; i< DENSE_3_BIAS_N; i++){
        fp_DENSE_3_BIAS[i] = fp_dense_bias[i];
    }

    // ___________________________________DATA PULL____________________________________________________________
    p_test_data_loop: for (int i=0; i < BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH; i++){
        fp_test_imgs[i] = test_images[i];
    }

    // _____________________________________FULL_MODEL_______________________________________________________
    data_t conv_1_res [BATCH_SIZE * CONV_1_WEIGHTS_N * (IMG_HEIGHT-CONV_1_WEIGHTS_H+1) * (IMG_WIDTH-CONV_1_WEIGHTS_W+1)];
    data_t conv_2_res [BATCH_SIZE * CONV_2_WEIGHTS_N * (IMG_HEIGHT-CONV_1_WEIGHTS_H+1-CONV_2_WEIGHTS_H+1) * (IMG_WIDTH-CONV_1_WEIGHTS_W+1-CONV_2_WEIGHTS_W+1)];
    data_t output_probs [BATCH_SIZE * DENSE_3_WEIGHTS_H];
    data_t preds_val [BATCH_SIZE];

    c1: full_conv(test_images, BATCH_SIZE, NUM_CHANNELS, IMG_HEIGHT, IMG_WIDTH,
              fp_CONV_1_WEIGHTS, CONV_1_WEIGHTS_N, CONV_1_WEIGHTS_C, CONV_1_WEIGHTS_H, CONV_1_WEIGHTS_W,
              fp_CONV_1_BIAS, CONV_1_BIAS_N, 
              conv_1_res, BATCH_SIZE, CONV_1_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1));

    r1: ReLU_conv(conv_1_res, BATCH_SIZE, CONV_1_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1),
              conv_1_res, BATCH_SIZE, CONV_1_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1));

    c2: full_conv(conv_1_res, BATCH_SIZE, CONV_1_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1),
              fp_CONV_2_WEIGHTS, CONV_2_WEIGHTS_N, CONV_2_WEIGHTS_C, CONV_2_WEIGHTS_H, CONV_2_WEIGHTS_W,
              fp_CONV_2_BIAS, CONV_2_BIAS_N, 
              conv_2_res, BATCH_SIZE, CONV_2_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1-CONV_2_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1-CONV_2_WEIGHTS_W+1) );

    r2: ReLU_conv(conv_2_res, BATCH_SIZE, CONV_2_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1-CONV_2_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1-CONV_2_WEIGHTS_W+1),
              conv_2_res, BATCH_SIZE, CONV_2_WEIGHTS_N, (IMG_HEIGHT-CONV_1_WEIGHTS_H+1-CONV_2_WEIGHTS_H+1), (IMG_WIDTH-CONV_1_WEIGHTS_W+1-CONV_2_WEIGHTS_W+1));
    
    d3: full_dense(conv_2_res, BATCH_SIZE, CONV_2_WEIGHTS_N * (IMG_HEIGHT-CONV_1_WEIGHTS_H+1-CONV_2_WEIGHTS_H+1) * (IMG_WIDTH-CONV_1_WEIGHTS_W+1-CONV_2_WEIGHTS_W+1),
               fp_DENSE_3_WEIGHTS, DENSE_3_WEIGHTS_H, DENSE_3_WEIGHTS_W,
               fp_DENSE_3_BIAS, DENSE_3_BIAS_N,
               output_probs, BATCH_SIZE, DENSE_3_WEIGHTS_H);   


    predict_class(output_probs, BATCH_SIZE, DENSE_3_WEIGHTS_H, preds_val, BATCH_SIZE);

    // _______________________________WRITE predicted classes to output__________________________________________
    final_write: for (int i=0; i<BATCH_SIZE; i++){
        preds[i] = preds_val[i];
    }
}



