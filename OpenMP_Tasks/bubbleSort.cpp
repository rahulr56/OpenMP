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
#include <math.h>
#include <unistd.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

    void generateMergeSortData (int* arr, size_t n);
    void checkMergeSortResult (int* arr, size_t n);

#ifdef __cplusplus
}
#endif


void Bubble_sort_OMP(
      int*  a  /* in/out */,
      int  n    /* in     */,
      int  num_threads) {
   int list_length, block_count, i, temp;
   int block_size, lower, upper, block;

   block_count = num_threads;
   omp_lock_t m[block_count];
   block_size = n/block_count;

   for(i=0;i<block_count;i++) omp_init_lock(&m[i]);

   omp_set_num_threads(num_threads);
   
   #pragma omp parallel private(list_length, block, lower, upper, i, temp)
   {
       for(list_length=block_size;list_length>=1;list_length--){
           for(block=0;block<block_count;block++){
               lower=block_size*block;
               if(block==block_count-1) upper = n;
               else upper = block_size*(block+1);
               if(block==0) omp_set_lock(&m[0]);

               for(i=lower;i<upper-1;i++){
                   if(a[i]>a[i+1]){
                       temp=a[i];
                       a[i]=a[i+1];
                       a[i+1]=temp;
                   }
               }
               if(block==block_count-1){
                   omp_unset_lock(&m[block]);
                   break;
               }

               omp_set_lock(&m[block+1]);
               if(a[i]>a[i+1]){
                   temp=a[i];
                   a[i]=a[i+1];
                   a[i+1]=temp;
               }
               omp_unset_lock(&m[block]);
           }
        }
    }
}

int main (int argc, char* argv[]) 
{
    if (argc < 3) 
    {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
        return -1;
    }

    int n = atoi(argv[1]);
    int numThreads = atoi(argv[2]); 
    if (n< numThreads)
        numThreads = n;
    omp_set_num_threads(numThreads);

    //forces openmp to create the threads beforehand
#pragma omp parallel
    {
        int fd = open (argv[0], O_RDONLY);
        if (fd != -1) {
            close (fd);
        }
        else {
            std::cerr<<"something is amiss"<<std::endl;
        }
    }

    int * arr = new int [n];

    generateMergeSortData (arr, atoi(argv[1]));

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    Bubble_sort_OMP(arr, atoi(argv[1]), atoi(argv[2])); 
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end-start;
    std::cerr.precision(10);
    std::cerr <<std::fixed<< elapsed.count() << std::endl;

    checkMergeSortResult (arr, atoi(argv[1]));

}
