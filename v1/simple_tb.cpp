
#include <iostream>
#include <direct.h>
#include <fstream>

//   #define VITIS_C_SIM
//  #define NORMAL_SIM
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

int main(){
	// ap_fixed<1,1> v = 1.25;
	// std::cout << (ap_fixed<1,1>(0)).to_string(10).c_str() << std::endl;

	// ____________________________________________READ IN TXT FILES_________________________________________________
	data_t fp_CONV_1_WEIGHTS[CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W];
	data_t fp_CONV_1_BIAS[CONV_1_BIAS_N]; 
	data_t fp_CONV_2_WEIGHTS[CONV_2_WEIGHTS_N * CONV_2_WEIGHTS_C * CONV_2_WEIGHTS_H * CONV_2_WEIGHTS_W];
	data_t fp_CONV_2_BIAS[CONV_2_BIAS_N];
	data_t fp_DENSE_3_WEIGHTS[DENSE_3_WEIGHTS_H * DENSE_3_WEIGHTS_W];
	data_t fp_DENSE_3_BIAS[DENSE_3_BIAS_N];
	data_t fp_test_imgs[BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH];

	std::ifstream file;

    // const int size = 1024;
    // char buffer[size];
    // Checking working directory for proper addresses
    // if (getcwd(buffer, size) != NULL) {
    // // print the current working directory
    //     std::cout << "Current working directory: " << buffer << std::endl;
    // } 
    // else {  
    //     // If _getcwd returns NULL, print an error message
    //     std::cerr << "Error getting current working directory" << std::endl;
    // }

	data_t temp;
	std::string dir = "C:\\Users\\DELL\\Desktop\\HLS_VITI\\c_cnn_hls\\SRC_c_cnn\\fp_data\\";
	std::string conv_1_weights = dir + "fp_CONV_1_WEIGHTS.txt";
	std::string conv_1_bias = dir + "fp_CONV_1_BIAS.txt";
	std::string conv_2_weights = dir + "fp_CONV_2_WEIGHTS.txt";
	std::string conv_2_bias = dir + "fp_CONV_2_BIAS.txt";
	std::string dense_weights = dir + "fp_DENSE_3_WEIGHTS.txt";
	std::string dense_bias = dir + "fp_DENSE_3_BIAS.txt";
	std::string test_imgs = dir + "fp_TEST_IMGS.txt";
	

	file.open(conv_1_weights);
    for (int i=0; i<CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W; i++){
		file >> temp;
        fp_CONV_1_WEIGHTS[i] = temp;
    }
    file.close();
    file.clear();

    file.open(conv_1_bias);
    for (int i=0; i<CONV_1_BIAS_N; i++){
		file >> temp;
        fp_CONV_1_BIAS[i] = temp;
    }
    file.close();
    file.clear();

    file.open(conv_2_weights);
    for (int i=0; i<CONV_2_WEIGHTS_N * CONV_2_WEIGHTS_C * CONV_2_WEIGHTS_H * CONV_2_WEIGHTS_W; i++){
		file >> temp;
        fp_CONV_2_WEIGHTS[i] = temp;
    }
    file.close();
    file.clear();

	file.open(conv_2_bias);
    for (int i=0; i<CONV_2_BIAS_N; i++){
		file >> temp;
        fp_CONV_2_BIAS[i] = temp;
    }
    file.close();
    file.clear();

	file.open(dense_weights);
    for (int i=0; i<DENSE_3_WEIGHTS_H * DENSE_3_WEIGHTS_W; i++){
		file >> temp;
        fp_DENSE_3_WEIGHTS[i] = temp;
    }
    file.close();
    file.clear();

	file.open(dense_bias);
    for (int i=0; i<DENSE_3_BIAS_N; i++){
		file >> temp;
        fp_DENSE_3_BIAS[i] = temp;
//		 std::cout << temp.to_string(10).c_str() << std::endl;
    }
    file.close();
    file.clear();

	file.open(test_imgs);
    for (int i=0; i<BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH; i++){
		file >> temp;
        fp_test_imgs[i] = temp;
    }
    file.close();
    file.clear();

	
	//________________________________________________RUN MODEL___________________________________________________________
	data_t res[BATCH_SIZE];
	full_model(fp_test_imgs, res, fp_CONV_1_WEIGHTS, fp_CONV_1_BIAS, fp_CONV_2_WEIGHTS, fp_CONV_2_BIAS,
	            fp_DENSE_3_WEIGHTS, fp_DENSE_3_BIAS);


//	std::cout << "PRINTING RESULTS\n" << std::endl;
	std::ofstream out_file;
    out_file.open(dir+"RES.txt");
	for (int i=0; i<BATCH_SIZE; i++){
        out_file << res[i].to_string(10).c_str() << "\n";
//		std::cout << res[i].to_string(10).c_str() << std::endl;
	}
    out_file.close();

}
