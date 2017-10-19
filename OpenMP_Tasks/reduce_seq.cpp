#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <ctime>
#include <ratio>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

    void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

int calculateSum(int* arr, int start, int end)
{
    int sum = 0;
    for(int i=start;i<end;i++)
        sum += arr[i];
    return sum;
}


int main (int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr<<"Usage: "<<argv[0]<<" <n>"<<std::endl;
        return -1;
    }
    int n = atoi(argv[1]);
    int * arr = new int [n];

    generateReduceData (arr, n);

    int sum = 0;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    for (int i=0;i<n;i++) 
        sum+=arr[i];
    std::chrono::high_resolution_clock::time_point end  = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr.precision(10);
    std::cerr<<std::fixed<<elapsed_seconds.count()<<std::endl;

    delete[] arr;

    std::cout<<sum<<std::endl;
    return 0;
}
