#include <omp.h>
#include <iostream>

using namespace std;
#define NUM_THREADS 2
static long num_steps =100000;
double step;

int main()
{
    int i, nthreads;
    double pi,sum[NUM_THREADS];
    step = 1.0 / (double)num_steps;
    omp_set_num_threads(NUM_THREADS);
#pragma omp parallel
    {
        int i, id, nthrds; 
        double x;
        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();
        if (id == 0) nthreads = nthrds;
        // Round robin distribution
        for (i=id, sum[id]=0.0 ; i<num_steps;i= i +nthrds )
        {
            x=(i+0.5)*step;
            sum[id] += 4.0/(1.0+x*x);
        }
    }
    for (i=0;i<nthreads;i++)
        pi += step * sum[i];
    cout<<"Time taken : "<<omp_get_wtime()<<endl;
    cout<<"Pi : "<<pi<<endl;
}
