// ___________________________________________________________________________
// RUN THIS ONLY IN VIVADO AS TESTBENCH SIM 
// ___________________________________________________________________________

#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;
#include "ap_fixed.h"

#include "param_def.h"
#include "c_test_weights.cpp"
#include "c_test_imgs.cpp"

typedef ap_fixed<INPUT_DATA_WIDTH, IN_INTG_WIDTH, IN_QMODE, IN_OMODE> data_t;

// TEMPORARY BUFFERS
data_t fp_CONV_1_WEIGHTS[CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W];
data_t fp_CONV_1_BIAS[CONV_1_BIAS_N]; 
data_t fp_CONV_2_WEIGHTS[CONV_2_WEIGHTS_N * CONV_2_WEIGHTS_C * CONV_2_WEIGHTS_H * CONV_2_WEIGHTS_W];
data_t fp_CONV_2_BIAS[CONV_2_BIAS_N];
data_t fp_DENSE_3_WEIGHTS[DENSE_3_WEIGHTS_H * DENSE_3_WEIGHTS_W];
data_t fp_DENSE_3_BIAS[DENSE_3_BIAS_N];
data_t fp_test_imgs[BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH];

int main(){

	// // printing methods
	// data_t val = -0.2162136435508728;
	// cout << val.to_string().c_str() << en    dl;

    // _________________________________ACTUAL IMPLICIT CONVERSION______________________________________________________
    for (int i=0; i < CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W; i++){
        fp_CONV_1_WEIGHTS[i] = CONV_1_WEIGHTS[i];

        // test by printing first 10 vals of weights - works
        // std::cout << fp_CONV_1_WEIGHTS[i].to_string(10).c_str() << std::endl;
    }

    for (int i=0; i < CONV_1_BIAS_N; i++){
        fp_CONV_1_BIAS[i] = CONV_1_BIAS[i];
    }

    for (int i=0; i < CONV_2_WEIGHTS_N * CONV_2_WEIGHTS_C * CONV_2_WEIGHTS_H * CONV_2_WEIGHTS_W; i++){
        fp_CONV_2_WEIGHTS[i] = CONV_2_WEIGHTS[i];
    }

    for (int i=0; i< CONV_2_BIAS_N; i++){
        fp_CONV_2_BIAS[i] = CONV_2_BIAS[i];
    }

    for (int i=0; i < DENSE_3_WEIGH TS_H * DENSE_3_WEIGHTS_W; i++){
        fp_DENSE_3_WEIGHTS[i] = DENSE_3_WEIGHTS[i];
    }

    for (int i=0; i< DENSE_3_BIAS_N; i++){
        fp_DENSE_3_BIAS[i] = DENSE_3_BIAS[i];
    }

    // ____________________________________DATA CONVERSION_____________________________________________________
    for (int i=0; i < BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH; i++){
        fp_test_imgs[i] = test_images[i];
    }

    // __________________WRITE WEIGHTS - the final output file is in solution1/csim/build/_____________________
    ofstream file;


    file.open("fp_CONV_1_WEIGHTS.txt");
    for (int i=0; i<CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W; i++){
        file << fp_CONV_1_WEIGHTS[i].to_string(10).c_str() << endl;
    }
    file.close();
    file.clear();

    file.open("fp_CONV_1_BIAS.txt");
    for (int i=0; i<CONV_1_BIAS_N; i++){
        file << fp_CONV_1_BIAS[i].to_string(10).c_str() << endl;
    }
    file.close();
    file.clear();

    file.open("fp_CONV_2_WEIGHTS.txt");
    for (int i=0; i<CONV_2_WEIGHTS_N * CONV_2_WEIGHTS_C * CONV_2_WEIGHTS_H * CONV_2_WEIGHTS_W; i++){
        file << fp_CONV_2_WEIGHTS[i].to_string(10).c_str() << endl;
    }
    file.close();
    file.clear();

    file.open("fp_CONV_2_BIAS.txt");
    for (int i=0; i<CONV_2_BIAS_N; i++){
        file << fp_CONV_2_BIAS[i].to_string(10).c_str() << endl;
    }
    file.close();
    file.clear();

    file.open("fp_DENSE_3_WEIGHTS.txt");
    for (int i=0; i<DENSE_3_WEIGHTS_H * DENSE_3_WEIGHTS_W; i++){
        file << fp_DENSE_3_WEIGHTS[i].to_string(10).c_str() << endl;
    }
    file.close();
    file.clear();


    file.open("fp_DENSE_3_BIAS.txt");
    for (int i=0; i<DENSE_3_BIAS_N; i++){
        file << fp_DENSE_3_BIAS[i].to_string(10).c_str() << endl;
        // file << fp_DENSE_3_BIAS[i].range(INPUT_DATA_WIDTH-1,0) << endl;
    }
    file.close();
    file.clear();

    file.open("fp_TEST_IMGS.txt");
    for (int i=0; i<BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH; i++){
        file << fp_test_imgs[i].to_string(10).c_str() << endl;
    }
    file.close();
    file.clear();
}


