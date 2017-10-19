#include <iostream>
#include <chrono>
#include <ctime>
#include <ratio>

#include <omp.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

    float f1(float x, int intensity);
    float f2(float x, int intensity);
    float f3(float x, int intensity);
    float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

// Function pointer to define the function to be called from the library
float (*funcPtr) (float,int);

int main (int argc, char* argv[]) {
    if (argc < 8) {
        std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <chunkSize>"<<std::endl;
        return -1;
    }

    std::string staticStr ("static");
    std::string dynamicStr ("dynamic");

#pragma omp_set_num_threads(atoi(argv[6]))
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

    int a = atoi(argv[2]);
    int n = atoi(argv[4]);
    int intensity = atoi(argv[5]);
    int chunkSize = atoi(argv[8]);
    float multiplier = (atoi(argv[3]) - a) / (float)n;

    if (staticStr.compare(argv[7]))
    {
        omp_set_schedule(omp_sched_static, chunkSize);
    }
    else if (dynamicStr.compare(argv[7]))
    {
        omp_set_schedule(omp_sched_dynamic, chunkSize);
    }
    else{
        omp_set_schedule(omp_sched_guided, chunkSize);
    }

    switch(atoi(argv[1]))
    {
        case 1:
            funcPtr = &f1;
            break;
        case 2: 
            funcPtr = &f2;
            break;
        case 3: 
            funcPtr = &f3;
            break;
        case 4: 
            funcPtr = &f4;
            break;
        default:
            std::cerr<<"Invalid function number provided\n";
            exit(-1);
    }

    std::chrono::high_resolution_clock::time_point start  = std::chrono::high_resolution_clock::now();
    double sum = 0;
#pragma omp parallel
    {
#pragma omp for schedule(guided, chunkSize) reduction(+:sum)
        for (int i=0; i< n ;i++)
        {
            double val =  (double)funcPtr(a + (i + 0.5) * multiplier, intensity) * multiplier;
#pragma omp critical
            {
                sum += val;
            }
        }
    }
    std::chrono::high_resolution_clock::time_point end  = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout<<sum<<std::endl;
    std::cerr<<elapsed_seconds.count()<<std::endl;
    return 0;
}
