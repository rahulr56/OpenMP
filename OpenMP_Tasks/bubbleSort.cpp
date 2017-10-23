#include <omp.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <ratio> 
#include <ctime> 
#include <chrono>
#include <iostream>
#include <algorithm>


#ifdef __cplusplus
extern "C" {
#endif

    void generateMergeSortData (int* arr, size_t n);
    void checkMergeSortResult (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

void bubbleSort(int *arr, int n, int numThreads)
{
#pragma omp parallel 
    {
        //std::cout<<"thread : "<<omp_get_thread_num()<<std::endl;
#pragma omp for
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n-1;++j)
            {

                if(arr[j]>arr[j+1])
                {
                    int temp = arr[j];
                    arr[j] = arr[j+1];
                    arr[j+1] = temp;
                }
            }
        }
    }
}


void bubbleSortOddEven(int*arr, int n)
{
    bool sorted = true;
    for(int i=0; i < n; ++i)
    {
        int k = (i%2==0)?0:1; 
#pragma omp parallel for shared(k)
        for(int j=k; j<n-1; j+=2 )
        {
           if (arr[j] > arr[j+1]) 
           {
               int temp = arr[j];
               arr[j] = arr[j+1];
               arr[j+1] = temp;
               sorted = false;
           }
        }
           if(sorted)
               break;
    }
}

void genArray(int *arr, int n)
{
    for (int i=0;i<n;i++)
        arr[i] = rand()%100;
}

int main(int argc, char* argv[])
{
    if(argc<3)
    {
        std::cout<<"Usage : "<<argv[0]<<"<size of array> <numThreads>"<<std::endl;
        exit(-1);
    }
    
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


    int n = atoi(argv[1]);
    int numThreads = atoi(argv[2]);

    int *arr = new int[n]; 
    // genArray(arr, n);
    generateMergeSortData (arr, n);

    omp_set_num_threads(numThreads); 
/* 
    // Basic form of bubble Sort  
    std::chrono::time_point<std::chrono::system_clock> start1 = std::chrono::system_clock::now(); 
    bubbleSort(arr,n, numThreads );
    std::chrono::time_point<std::chrono::system_clock> end1 = std::chrono::system_clock::now(); 
    std::chrono::duration<double> elapsed_seconds = end1 - start1;
    std::cerr<<std::fixed;
    std::cerr << elapsed_seconds.count() << std::endl;

    // Sequential bubble sort
    std::chrono::time_point<std::chrono::system_clock> start_seq = std::chrono::system_clock::now(); 
    omp_set_num_threads(1);
    bubbleSort(arr,n, 1 );
    std::chrono::time_point<std::chrono::system_clock> end_seq = std::chrono::system_clock::now(); 
    std::chrono::duration<double> elapsed_seconds_seq = end_seq - start_seq;

    std::cerr <<numThreads<<"\t"<<std::fixed<<elapsed_seconds_seq.count() <<"\t";
*/ 

//    std::cout<<"bubbleSortOddEven sort : \n";
    omp_set_num_threads(numThreads);
    // start timer 
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now(); 
    
    // sort the array elements 
    bubbleSortOddEven(arr, n);

    // stop time 
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now(); 

    // Evaluate time taken
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cerr<<std::fixed<<numThreads<<" "<< n<<" "<<elapsed_seconds.count() << std::endl;

    checkMergeSortResult (arr, atoi(argv[1]));

    delete[] arr;

    return 0;
}
