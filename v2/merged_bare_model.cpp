// _____________________________________AP_FIXED PRIMITIVES ONLY______________________________________________

#include <iostream>
#include <stdlib.h>
#include "bare_param_def.h"

// #define VITIS_C_SIM
//#define NORMAL_SIM
// #define PRINT
 #define VITIS_SYN

#ifdef VITIS_C_SIM
    #include <ap_fixed.h>
    #include "merged_bare_model.h"
#endif

#ifdef NORMAL_SIM
    typedef float data_t;
#endif

#ifdef VITIS_SYN
    #include <ap_fixed.h>
    #include "merged_bare_model.h"
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



// Volume convolution
void vconv(data_t* input_mat, int in_C, int in_H, int in_W,
           data_t* filter, int filter_C, int filter_H, int filter_W, 
           data_t filter_bias,
           data_t* res, int res_H, int res_W)
{
    if ( ((in_H - filter_H)/1 + 1 ) != res_H ) {std::cout << "VCONV ROW DIMEN ERROR\n";return;}
    if ( ((in_W - filter_W)/1 + 1 ) != res_W ) {std::cout << "VCONV COL DIMEN ERROR\n";return;}
    if ( in_C != filter_C ) {std::cout << "VCONV DEPTH MISMATCH\n";return;}

    // ______TIPS FOR LARGE SYNTH FILES__________
    // USE CONSTANTS FOR RES DIMENSIONS
    // SYNTHESIZE 1 VCONV CALL
    // SET LARGE LATENCY - NOPE
    // MANUAL HIERARCHICAL MODULE-AT-A-TIME SYNTHESIS TESTS
    // FOCUS ON THE POWER AND EFFECIENCY METRICS

    vcon_res_H: for (int i=0; i<res_H; i++){
        vcon_res_W: for (int j=0; j<res_W; j++){
            
            // initialize with bias and accum
            res[i*res_W + j] = filter_bias;
            
            vcon_fil_C: for (int k=0; k<filter_C; k++){

                vcon_fil_H: for (int l=0; l<filter_H; l++){
                    vcon_fil_W: for (int m=0; m<filter_W; m++){
#pragma HLS PIPELINE
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
            // isolating next filter and res frame

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


data_t max(data_t a, data_t b, data_t c, data_t d){
    if (a>=b && a>=c && a>=d) {return a;}
    if (b>=a && b>=c && b>=d) {return b;}
    if (c>=a && c>=b && c>=d) {return c;}
    if (d>=a && d>=b && d>=c) {return d;}
    else {return 0;}
}


// Note that this max_pool is fixed for window size (2,2) and stride 2
// Might change this later if critical
void max_pool(data_t* input_mat, int in_N, int in_C, int in_H, int in_W,
              data_t* res, int res_N, int res_C, int res_H, int res_W)
{  
    if (res_N != in_N || res_C != in_C || res_H != (in_H)/2 || res_W != (in_W)/2) {std::cout << "POOL DIMEN ERROR" << std::endl;}

    pool_N: for (int i=0; i<res_N; i++){
        pool_C: for (int j=0; j<res_C; j++){
            
            pool_H: for (int k=0; k<res_H; k++){
                pool_W: for (int l=0; l<res_W; l++){

                    // here k,l are multiplied by 2 due to stride
                    res[i*res_C*res_H*res_W + j*res_H*res_W + k*res_W + l] = max(input_mat[i*in_C*in_H*in_W + j*in_H*in_W + (2*k)*in_W + 2*l],
                                                                                 input_mat[i*in_C*in_H*in_W + j*in_H*in_W + (2*k)*in_W + 2*l + 1],
                                                                                 input_mat[i*in_C*in_H*in_W + j*in_H*in_W + (2*k + 1)*in_W + 2*l],
                                                                                 input_mat[i*in_C*in_H*in_W + j*in_H*in_W + (2*k + 1)*in_W + 2*l + 1]);
                }
            }
        }
    }
}


// ___________________________________________CNN_SYN BEGINS__________________________________________________________
void bare_model ( data_t* test_images, 
	 data_t* preds, 
	 data_t* fp_conv_1_weights, 
	 data_t* fp_conv_1_bias, 
	 data_t* fp_dense_2_weights, 
	 data_t* fp_dense_2_bias
	 ) 

{

// read the Managing Interface Synth section to handle burst settings
#pragma HLS INTERFACE mode=m_axi bundle=master_handle port=test_images offset=slave

#pragma HLS INTERFACE mode=m_axi bundle=master_handle port=preds offset=slave

#pragma HLS INTERFACE mode=m_axi bundle=master_handle port=fp_conv_1_weights offset=slave

#pragma HLS INTERFACE mode=m_axi bundle=master_handle port=fp_conv_1_bias offset=slave

#pragma HLS INTERFACE mode=m_axi bundle=master_handle port=fp_dense_2_weights offset=slave

#pragma HLS INTERFACE mode=m_axi bundle=master_handle port=fp_dense_2_bias offset=slave

#pragma HLS INTERFACE mode=ap_ctrl_none port=return

#pragma HLS INTERFACE mode=s_axilite port=test_images bundle=crm_handle
#pragma HLS INTERFACE mode=s_axilite port=preds bundle=crm_handle
#pragma HLS INTERFACE mode=s_axilite port=fp_conv_1_weights bundle=crm_handle
#pragma HLS INTERFACE mode=s_axilite port=fp_conv_1_bias bundle=crm_handle
#pragma HLS INTERFACE mode=s_axilite port=fp_dense_2_weights bundle=crm_handle
#pragma HLS INTERFACE mode=s_axilite port=fp_dense_2_bias bundle=crm_handle


//_________________________________PULL_DATA___________________________
	data_t fp_CONV_1_WEIGHTS [CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W];
	p_conv1_w_loop: for (int i=0; i < CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W; i++){
		fp_CONV_1_WEIGHTS[i] = fp_conv_1_weights[i];
	}

	data_t fp_CONV_1_BIAS [CONV_1_BIAS_N];
	p_conv1_b_loop: for (int i=0; i < CONV_1_BIAS_N; i++){
		fp_CONV_1_BIAS[i] = fp_conv_1_bias[i];
	}

	data_t fp_DENSE_2_WEIGHTS [DENSE_2_WEIGHTS_H * DENSE_2_WEIGHTS_W];
	p_dense2_w_loop: for (int i=0; i < DENSE_2_WEIGHTS_H * DENSE_2_WEIGHTS_W; i++){
		fp_DENSE_2_WEIGHTS[i] = fp_dense_2_weights[i];
	}

	data_t fp_DENSE_2_BIAS [DENSE_2_BIAS_N];
	p_dense2_b_loop: for (int i=0; i < DENSE_2_BIAS_N; i++){
		fp_DENSE_2_BIAS[i] = fp_dense_2_bias[i];
	}

	data_t fp_TEST_IMGS [BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH];
	p_test_data_loop: for (int i=0; i < BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH; i++){
		fp_TEST_IMGS[i] = test_images[i];
	}

// ______________________________FULL_MODEL_______________________________________

    data_t conv_res [BATCH_SIZE * CONV_1_WEIGHTS_N * (IMG_HEIGHT-CONV_1_WEIGHTS_H+1) * (IMG_WIDTH-CONV_1_WEIGHTS_W+1)];
    data_t pool_res [BATCH_SIZE * CONV_1_WEIGHTS_N * ((IMG_HEIGHT-CONV_1_WEIGHTS_H+1)/2) * ((IMG_WIDTH-CONV_1_WEIGHTS_W+1)/2)];
    data_t dense_res [BATCH_SIZE * DENSE_2_WEIGHTS_H];
    data_t preds_val [BATCH_SIZE];

    // pass actual constants pre computed
    // int RES = IMG_HEIGHT-CONV_1_WEIGHTS_H+1;

    full_conv(fp_TEST_IMGS, BATCH_SIZE, NUM_CHANNELS, IMG_HEIGHT, IMG_WIDTH, 
               fp_CONV_1_WEIGHTS, CONV_1_WEIGHTS_N, CONV_1_WEIGHTS_C, CONV_1_WEIGHTS_H, CONV_1_WEIGHTS_W,
               fp_CONV_1_BIAS, CONV_1_BIAS_N, 
               conv_res, BATCH_SIZE, CONV_1_WEIGHTS_N, CONV_1_RES_H, CONV_1_RES_W);

    ReLU_conv(conv_res, BATCH_SIZE, CONV_1_WEIGHTS_N, CONV_1_RES_H, CONV_1_RES_W,
              conv_res, BATCH_SIZE, CONV_1_WEIGHTS_N, CONV_1_RES_H, CONV_1_RES_W);

    max_pool(conv_res, BATCH_SIZE, CONV_1_WEIGHTS_N, CONV_1_RES_H, CONV_1_RES_W,
             pool_res, BATCH_SIZE, CONV_1_WEIGHTS_N, POOL_RES_H, POOL_RES_W);

    full_dense(pool_res, BATCH_SIZE, DENSE_2_WEIGHTS_W,
                fp_DENSE_2_WEIGHTS, DENSE_2_WEIGHTS_H, DENSE_2_WEIGHTS_W, 
                fp_DENSE_2_BIAS, DENSE_2_BIAS_N,
                dense_res, BATCH_SIZE, DENSE_2_WEIGHTS_H);

    predict_class(dense_res, BATCH_SIZE, DENSE_2_WEIGHTS_H,
                    preds_val, BATCH_SIZE);

// __________________________WRITE predicted classes to output__________________________________________
    final_write: for (int i=0; i<BATCH_SIZE; i++){
        preds[i] = preds_val[i];
    }

}
