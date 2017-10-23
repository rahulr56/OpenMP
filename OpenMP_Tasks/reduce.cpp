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

    void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

int calculateSum(int* arr, int start, int end)
{
    int sum = 0;
    // If there are more threads available, creating nested parallel section
#pragma omp parallel for reduction(+:sum)
    for(int i=start;i<end;i++)
    {
        sum += arr[i];
    }
    return sum;
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

    generateReduceData (arr, n);

    std::chrono::high_resolution_clock::time_point start;

    int sum = 0;
    start = std::chrono::high_resolution_clock::now();
    int *tempSum;

#pragma omp parallel 
    {
        int id = omp_get_thread_num();
        static int val ;
#pragma omp single 
        {
            numThreads = omp_get_num_threads();
            int chunkSize = ceil(static_cast<float>(n/numThreads));

            for(int i=0; i<numThreads;i++)
            {
                int start1  = floor(i*((float)n/numThreads));
                int end1  = floor((i+1)*((float)n/numThreads));
                if(end1>n)
                    end1=n;
#pragma omp task 
                {
                    val = calculateSum(arr, start1, end1);
#pragma omp critical
                    {
                        sum += val;
                    }
                }
            }
        }
#pragma omp taskwait
    }
    std::chrono::high_resolution_clock::time_point end  = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr.precision(10);
    std::cerr<<std::fixed<<elapsed_seconds.count()<<std::endl;

    delete[] arr;

    std::cout<<sum<<std::endl;
    return 0;
}
