#include <iostream>

typedef float DTYPE;

void display_mat(DTYPE* output, int rows, int cols){
    for (int i=0; i<rows; i++){
        for (int j=0; j<cols; j++){
            std::cout << output[i * cols + j] << " " << " ";
        }
        std::cout << "\n";
    }
}

DTYPE dot_prod(DTYPE* vec1, DTYPE* vec2, int len){
    DTYPE sum=0;
    for (int i=0; i<len; i++){
        sum += vec1[i] * vec2[i];
    }
    return sum;
}

void ReLU (DTYPE* mat, int matrows, int matcols, int matdepth,
           DTYPE* res, int resrows, int rescols, int resdepth)
{
    if (matrows != resrows || matcols != rescols || matdepth != resdepth) {std::cout << "RELU DIMEN ERROR";return;}

    for (int i=0; i<matdepth; i++){
        for (int j=0; j<matrows; j++){
            for (int k=0; k<matcols; k++){
                res[i*matrows*matcols + j*matcols + k] = (mat[i*matrows*matcols + j*matcols + k] > 0) ? mat[i*matrows*matcols + j*matcols + k] : 0;
            }
        }
    }
}

void ReLU_conv(DTYPE* mat, int mat_N, int mat_C, int mat_H, int mat_W,
               DTYPE* res, int res_N, int res_C, int res_H, int res_W)
{
    for (int i=0; i<mat_N; i++){
        for (int j=0; j<mat_C; j++){
            for (int k=0; k<mat_H; k++){
                for (int l=0; l<mat_W; l++){
                    res[i*mat_C*mat_H*mat_W + j*mat_H*mat_W + k*mat_W + l] = (mat[i*mat_C*mat_H*mat_W + j*mat_H*mat_W + k*mat_W + l] > 0) ? mat[i*mat_C*mat_H*mat_W + j*mat_H*mat_W + k*mat_W + l] : 0;
                }
            }
        }
    }
}

void mat_mat_mul(DTYPE* mat1, int mat1rows, int mat1cols, 
                 DTYPE* mat2, int mat2rows, int mat2cols,
                 DTYPE* res, int resrows, int rescols)
{
    if (mat1cols != mat2rows) {std::cout << "MATMUL DIMENSION ERROR";return;}

    for (int i=0; i<mat1rows; i++){
        for (int j=0; j<mat2cols; j++){
            res[i*rescols + j] = 0;
            for (int k=0; k<mat1cols; k++){
                res[i*rescols + j] += mat1[i*mat1cols + k] * mat2[k*mat2cols + j];
            }
        }
    }
}

void vec_add (DTYPE* vec1, int vec1len, 
              DTYPE* vec2, int vec2len, 
              DTYPE* res, int reslen)
{
    // if (vec1len != vec2len) {std::cout << "ADD VEC DIMEN ERROR"; return;}
    for (int i=0; i<vec1len; i++){
        res[i] = vec1[i] + vec2[i];
    }
}

void dense(DTYPE* input_vec, int inlen,
           DTYPE* weight_mat, int weightrows, int weightcols,
           DTYPE* bias_vec, int biaslen,
           DTYPE* output_vec, int output_len)
{
    if (weightcols != inlen || output_len != weightrows || output_len != biaslen) {std::cout << "DENSE DIMEN ERROR";return;} 
    
    mat_mat_mul(weight_mat, weightrows, weightcols, input_vec, inlen, 1, output_vec, output_len, 1);
    // INPLACE ADD OP, SIMUL READ WRITE
    vec_add(output_vec, output_len, bias_vec, biaslen, output_vec, output_len);
}

void full_dense(DTYPE* input, int in_N, int in_H,
                DTYPE* weight_mat, int weight_H, int weight_W,
                DTYPE* bias_vec, int bias_N,
                DTYPE* out, int out_N, int out_H)
{
    for (int in_id=0; in_id<in_N; in_id++){

        DTYPE* current_in = input + in_id*in_H;
        DTYPE* current_res_vec = out + in_id*out_H;

        dense(current_in, in_H, 
              weight_mat, weight_H, weight_W,
              bias_vec, bias_N, 
              current_res_vec, out_H);

    }
}



// PCONV performs planar conv between 1 kernel and 1 input matrix (depth of both = 1)
void pconv(DTYPE* input_mat, int inrows, int incols, 
           DTYPE* kernel, int kernelrows, int kernelcols, 
           DTYPE* res, int resrows, int rescols )
{
    if ( ((inrows - kernelrows)/1 + 1 ) != resrows ) {std::cout << "PCONV ROW DIMEN ERROR";return;}
    if ( ((incols - kernelcols)/1 + 1 ) != rescols ) {std::cout << "PCONV COL DIMEN ERROR";return;}

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
void vconv(DTYPE* input_mat, int in_C, int in_H, int in_W,
           DTYPE* filter, int filter_C, int filter_H, int filter_W, 
           DTYPE filter_bias,
           DTYPE* res, int res_H, int res_W)
{
    if ( ((in_H - filter_H)/1 + 1 ) != res_H ) {std::cout << "VCONV ROW DIMEN ERROR";return;}
    if ( ((in_W - filter_W)/1 + 1 ) != res_W ) {std::cout << "VCONV COL DIMEN ERROR";return;}
    if ( in_C != filter_C ) {std::cout << "VCONV DEPTH MISMATCH";return;}

    for (int i=0; i<res_H; i++){
        for (int j=0; j<res_W; j++){
            
            // initialize with bias and accum
            res[i*res_W + j] = filter_bias;
            for (int k=0; k<filter_C; k++){
                for (int l=0; l<filter_H; l++){
                    for (int m=0; m<filter_W; m++){
                        res[i*res_W + j] += filter[k*filter_H*filter_W + l*filter_W + m] * input_mat[k*in_W*in_H + (l+i)*in_W + m + j];
                    }
                }
            }
        }
    }
}


void full_conv(DTYPE* input_mat, int in_N, int in_C, int in_H, int in_W,
              DTYPE* filter, int filter_N, int filter_C, int filter_H, int filter_W,
              DTYPE* filter_biases, int filter_biases_N,
              DTYPE* res, int res_N, int res_C, int res_H, int res_W)
{   
    if (res_C != filter_N) {std::cout << "TOTAL FILTER NUMBER OUTPUT CHANNELS MISMATCH";}

    for (int img_id=0; img_id<in_N; img_id++){
        
        // isolating next image and output
        DTYPE* current_img = input_mat + img_id*in_C*in_H*in_W;
        DTYPE* current_res = res + img_id*res_C*res_H*res_W;

        for (int fil_num=0; fil_num<filter_N; fil_num++){
            // isolating next filter and res framewha

            DTYPE* current_filter = filter + fil_num*filter_C*filter_H*filter_W;
            DTYPE* current_res_frame = current_res + fil_num*res_H*res_W;
            DTYPE current_bias = filter_biases[fil_num];

            // convolving img with filter (vconv)
            vconv(current_img, in_C, in_H, in_W, current_filter, filter_C, filter_H, filter_W, current_bias, current_res_frame, res_H, res_W);

        }         
    }
}

// int main(){


// }


