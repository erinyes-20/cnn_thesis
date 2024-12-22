#ifndef PARAM_DEF
#define PARAM_DEF

	#define BATCH_SIZE 5 
	#define NUM_CHANNELS 1 
	#define IMG_HEIGHT 28 
	#define IMG_WIDTH 28 

	#define CONV_1_WEIGHTS_N 8 
	#define CONV_1_WEIGHTS_C 1 
	#define CONV_1_WEIGHTS_H 5 
	#define CONV_1_WEIGHTS_W 5 
	#define CONV_1_BIAS_N 8 

	#define CONV_1_RES_H 24
	#define CONV_1_RES_W 24

	#define POOL_RES_H 12
	#define POOL_RES_W 12

	#define DENSE_2_WEIGHTS_H 10 
	#define DENSE_2_WEIGHTS_W 1152 
	#define DENSE_2_BIAS_N 10 

	// ap_fixed params
    #define INPUT_DATA_WIDTH 16
    #define IN_INTG_WIDTH 8
    #define IN_QMODE AP_TRN
    #define IN_OMODE AP_WRAP

#endif
