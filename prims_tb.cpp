#include <iostream>
#include "my_primitives.cpp"


int main(){
    
    // TESTING MAT_MAT_MUL
    // DTYPE mat1[3*3] = { 1,2,3,
    //                     4,5,6,
    //                     7,8,9 };

    // DTYPE mat2[3*3] = { 2,0,0,
    //                     0,2,0,
    //                     0,0,2 };
    
    // DTYPE output[3*3] = {0};

    // mat_mat_mul(mat1, 3, 3, mat2, 3, 3, output, 3, 3);
    // display_mat(output,3,3);


    // // TESTING PCONV
    // DTYPE input[5*5] = { 1,2,3,4,5,
    //                      6,7,8,9,10,
    //                      11,12,13,14,15,
    //                      16,17,18,19,20, 
    //                      21,22,23,24,25 };

    // // conv is just sum of diagonal entries
    // DTYPE weight[3*3] = { 1,0,0,
    //                       0,1,0,
    //                       0,0,1 };
    
    // DTYPE output[3*3] = {0};

    // pconv(input, 5, 5, weight, 3, 3, output, 3, 3);
    // display_mat(output,3,3);


     // // // TESTING RELU    
    // DTYPE input[3*3] = { 1,-2,3,
    //                      45,1,-32,
    //                      -3,4,34 };
    
    // DTYPE output[3*3];

    // ReLU(input, 3, 3, 1, output, 3, 3, 1);
    // display_mat(output,3,3);


      // TESTING DENSE
    // DTYPE input [5] = {1,2,3,4,5};
    // DTYPE weights[2*5] = {1,1,0,0,0,
    //                       0,0,0,1,1};
    // DTYPE bias[2] = {1,1};
    // DTYPE output[2];

    // dense(input, 5, weights, 2, 5, bias, 2, output, 2);
    // display_mat(output,2,1);


   //  // TESTING VCONV
    // DTYPE input[2*5*5] = {
       
    //         1,1,1,1,1,
    //         1,1,1,1,1,
    //         1,1,1,1,1,
    //         1,1,1,1,1,
    //         1,1,1,1,1,        

    //         2,2,2,2,2,
    //         2,2,2,2,2,
    //         2,2,2,2,2,
    //         2,2,2,2,2,
    //         2,2,2,2,2,
        
    // };

    // DTYPE filter[2*3*3] = {
        
    //         1,0,0,
    //         1,0,0,
    //         1,0,0,       

    //         0,0,1,
    //         0,0,1,
    //         0,0,1,
        
    // };

    // DTYPE bias = 2;
    // DTYPE output[3*3];
    // vconv(input, 2, 5, 5, filter, 2, 3, 3, bias, output, 3, 3);
    // display_mat(output, 3, 3); 
}