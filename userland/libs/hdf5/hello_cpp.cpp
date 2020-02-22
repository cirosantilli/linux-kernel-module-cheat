// Adapted from: https://support.hdfgroup.org/HDF5/doc/cpplus_RM/create_8cpp-example.html

#include <iostream>
#include <string>

#include <H5Cpp.h>

using namespace H5;

const H5std_string FILE_NAME("hello_cpp.tmp.h5");
const H5std_string DATASET_NAME("IntArray");
const int NX = 5;
const int NY = 6;
const int RANK = 2;

int main (void) {
    // 0 1 2 3 4 5
    // 1 2 3 4 5 6
    // 2 3 4 5 6 7
    // 3 4 5 6 7 8
    // 4 5 6 7 8 9
    int i, j;
    int data[NX][NY];
    for (j = 0; j < NX; j++) {
        for (i = 0; i < NY; i++)
        data[j][i] = i + j;
    }

    //Create a new file using H5F_ACC_TRUNC access,
    //default file creation properties, and default file
    //access properties.
    H5File file(FILE_NAME, H5F_ACC_TRUNC);

    // Define the size of the array and create the data space for fixed
    // size dataset.
    hsize_t dimsf[2];
    dimsf[0] = NX;
    dimsf[1] = NY;
    DataSpace dataspace(RANK, dimsf);

    // Define datatype for the data in the file.
    // We will store little endian INT numbers.
    IntType datatype(PredType::NATIVE_INT);
    datatype.setOrder(H5T_ORDER_LE);

    // Create a new dataset within the file using defined dataspace and
    // datatype and default dataset creation properties.
    DataSet dataset = file.createDataSet(DATASET_NAME, datatype, dataspace);

    // Write the data to the dataset using default memory space, file
    // space, and transfer properties.
    dataset.write(data, PredType::NATIVE_INT);
    return 0;
}
