# read pynq overlay docs and 4 pp4fpga labs for axi recap  
from pynq import Overlay
from pynq import allocate
import numpy as np

# setup hwh and bit files
ol = Overlay('design_1.bit')
cnn_ip = ol.bare_model_0 

BATCH_SIZE = 5
NUM_CHANNELS = 1
IMG_HEIGHT = 28
IMG_WIDTH = 28

CONV_1_WEIGHTS_N = 8
CONV_1_WEIGHTS_C = 1
CONV_1_WEIGHTS_H = 5
CONV_1_WEIGHTS_W = 5
CONV_1_BIAS_N = 8

CONV_1_RES_H = 24
CONV_1_RES_W = 24

POOL_H = 2
POOL_W = 2
POOL_STRIDE = 2

POOL_RES_H = 12
POOL_RES_W = 12

DENSE_2_WEIGHTS_H = 10
DENSE_2_WEIGHTS_W = 1152
DENSE_2_BIAS_N = 10 

# allocate np arrays with weights and test_images
test_space = allocate(shape=(BATCH_SIZE * NUM_CHANNELS * IMG_HEIGHT * IMG_WIDTH,), dtype=np.int16)
pred_space = allocate(shape=(BATCH_SIZE,), dtype=np.int16)
conv_1_weight_space = allocate(shape=(CONV_1_WEIGHTS_N * CONV_1_WEIGHTS_C * CONV_1_WEIGHTS_H * CONV_1_WEIGHTS_W,), dtype=np.int16)
conv_1_bias_space = allocate(shape=(CONV_1_BIAS_N,), dtype=np.int16)
dense_weight_space = allocate(shape=(DENSE_2_WEIGHTS_H*DENSE_2_WEIGHTS_W,), dtype=np.int16)
dense_bias_space = allocate(shape=(DENSE_2_BIAS_N,), dtype=np.int16)


def mul_256(inp):   
    val = int(float(inp) * 256) % (1<<16)
    return val

def fill_space(dir, addr_space):
    new, inp_vals = [], []
    file = open(dir, 'r')
    inp_vals = file.read().split()
    new = [mul_256(x) for x in inp_vals]
    file.close()
    np.copyto(addr_space, new)
    

# copy values to PS Memory
fill_space("./fp_data/fp_TEST_IMGS.txt", test_space)
fill_space("./fp_data/fp_CONV_1_BIAS.txt", conv_1_bias_space)
fill_space("./fp_data/fp_CONV_1_WEIGHTS.txt", conv_1_weight_space)
fill_space("./fp_data/fp_DENSE_2_BIAS.txt", dense_bias_space)
fill_space("./fp_data/fp_DENSE_2_WEIGHTS.txt", dense_weight_space)


# trigger module control registers
cnn_ip.write(0x10, test_space.physical_address)
cnn_ip.write(0x18, pred_space.physical_address)
cnn_ip.write(0x20, conv_1_weight_space.physical_address)
cnn_ip.write(0x28, conv_1_bias_space.physical_address)
cnn_ip.write(0x30, dense_weight_space.physical_address)
cnn_ip.write(0x38, dense_bias_space.physical_address)

# trigger module
cnn_ip.write(0x0, 0x1)

cnt =0
while (cnn_ip.read(0) >> 1) & 1 != 0:
    cnt+=1


# copy written res to new array for display
res = np.zeros((BATCH_SIZE))
np.copyto(res, pred_space)
print("________FINAL RESULT IN {cnt} CYCLES__________\n")
print(res)