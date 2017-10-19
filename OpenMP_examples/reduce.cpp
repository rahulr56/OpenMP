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


int main (int argc, char* argv[]) {

    if (argc < 5) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <chunkSize>"<<std::endl;
        return -1;
    }
    omp_set_num_threads(atoi(argv[2]));
    std::string staticStr ("static");
    std::string dynamicStr ("dynamic");
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

    int chunkSize = atoi(argv[4]);

    int * arr = new int [atoi(argv[1])];

    generateReduceData (arr, atoi(argv[1]));

    std::chrono::high_resolution_clock::time_point start;

    //write code here
    int sum = arr[0];
    if(staticStr.compare(argv[3])==0){
        omp_set_schedule(omp_sched_static, chunkSize);
    }
    else if(dynamicStr.compare(argv[3])==0){
        omp_set_schedule(omp_sched_dynamic, chunkSize);
    }
    else 
    {
        omp_set_schedule(omp_sched_guided, chunkSize);
    }
    start = std::chrono::high_resolution_clock::now();
#pragma omp parallel reduction(+:sum)
    {
//        std::cout<<"Num threads : "<<omp_get_num_threads()<<std::endl;
#pragma omp for schedule(runtime)
        for(int i=1; i < atoi(argv[1]); i++ )
            sum += arr[i];
    }
    std::chrono::high_resolution_clock::time_point end  = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<<elapsed_seconds.count()<<std::endl;

    delete[] arr;

    std::cout<<sum<<std::endl;
    return 0;
}
