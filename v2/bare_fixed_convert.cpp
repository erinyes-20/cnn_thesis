// ___________________________________________________________________________________
// RUN THIS ONLY IN VIVADO AS TESTBENCH SIM AND INCLUDE ONLY PARAM_DEF FILE IN SOURCES
// ___________________________________________________________________________________

#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;
#include "ap_fixed.h"

#include "bare_param_def.h"

// these files provide the WEIGHTS
#include "bare_weights.cpp"
#include "c_test_imgs.cpp"

typedef ap_fixed<INPUT_DATA_WIDTH, IN_INTG_WIDTH, IN_QMODE, IN_OMODE> data_t;

// TEMPORARY BUFFERS
data_t fp_CONV_1_WEIGHTS[CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W];
data_t fp_CONV_1_BIAS[CONV_1_BIAS_N]; 

data_t fp_DENSE_2_WEIGHTS[DENSE_2_WEIGHTS_H * DENSE_2_WEIGHTS_W];
data_t fp_DENSE_2_BIAS[DENSE_2_BIAS_N];

data_t fp_TEST_IMGS[BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH];

int main(){

	// // printing methods
	// data_t val = -0.2162136435508728;
	// cout << val.to_string().c_str() << endl;

    // _________________________________ACTUAL IMPLICIT CONVERSION______________________________________________________
    for (int i=0; i < CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W; i++){
        fp_CONV_1_WEIGHTS[i] = CONV_1_WEIGHTS[i];

        // test by printing first 10 vals of weights - works
        //  std::cout << fp_CONV_1_WEIGHTS[i].to_string(10).c_str() << std::endl;
    }

    for (int i=0; i < CONV_1_BIAS_N; i++){
        fp_CONV_1_BIAS[i] = CONV_1_BIAS[i];
    }


    for (int i=0; i < DENSE_2_WEIGHTS_H * DENSE_2_WEIGHTS_W; i++){
        fp_DENSE_2_WEIGHTS[i] = DENSE_2_WEIGHTS[i];
    }

    for (int i=0; i< DENSE_2_BIAS_N; i++){
        fp_DENSE_2_BIAS[i] = DENSE_2_BIAS[i];
    }

    // ____________________________________DATA CONVERSION_____________________________________________________
    for (int i=0; i < BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH; i++){
        fp_TEST_IMGS[i] = test_images[i];
    }

    // __________________WRITE WEIGHTS - the final output file is in solution1/csim/build/ by default, can be changed_____________________
    ofstream file;

    std::string dir = "C:\\Users\\DELL\\Desktop\\HLS_VITI\\bare_cnn_hls\\SRC_bare_cnn\\fp_data\\";
	std::string conv_1_weights = dir + "fp_CONV_1_WEIGHTS.txt";
	std::string conv_1_bias = dir + "fp_CONV_1_BIAS.txt";

	std::string dense_2_weights = dir + "fp_DENSE_2_WEIGHTS.txt";
	std::string dense_2_bias = dir + "fp_DENSE_2_BIAS.txt";

	std::string test_imgs_dir = dir + "fp_TEST_IMGS.txt";


    file.open(conv_1_weights);
    for (int i=0; i<CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W; i++){
        file << fp_CONV_1_WEIGHTS[i].to_string(10).c_str() << endl;
    }
    file.close();
    file.clear();

    file.open(conv_1_bias);
    for (int i=0; i<CONV_1_BIAS_N; i++){
        file << fp_CONV_1_BIAS[i].to_string(10).c_str() << endl;
    }
    file.close();
    file.clear();


    file.open(dense_2_weights);
    for (int i=0; i<DENSE_2_WEIGHTS_H * DENSE_2_WEIGHTS_W; i++){
        file << fp_DENSE_2_WEIGHTS[i].to_string(10).c_str() << endl;
    }
    file.close();
    file.clear();


    file.open(dense_2_bias);
    for (int i=0; i<DENSE_2_BIAS_N; i++){
        file << fp_DENSE_2_BIAS[i].to_string(10).c_str() << endl;
        // file << fp_DENSE_3_BIAS[i].range(INPUT_DATA_WIDTH-1,0) << endl;
    }
    file.close();
    file.clear();

    file.open(test_imgs_dir);
    for (int i=0; i<BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH; i++){
         file << fp_TEST_IMGS[i].to_string(10).c_str() << endl;
    }
    file.close();
    file.clear();
}


