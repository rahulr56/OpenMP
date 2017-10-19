#include <omp.h>
#include <iostream>

using namespace std;
#define NUM_THREADS 2
#define PAD 8       
// Would be the size of the Cache line  in 64 bit L1 cache line size. yes, you should know that before dealing with PAD. Else, use synchronization
static long num_steps =100000;
double step;

int main()
{
    int i, nthreads;
    double pi,sum[NUM_THREADS][PAD];
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
        for (i=id, sum[id][0]=0.0 ; i<num_steps;i= i +nthrds )
        {
            x=(i+0.5)*step;
            sum[id][0] += 4.0/(1.0+x*x);
        }
    }
    for (i=0;i<nthreads;i++)
        pi += step * sum[i][0];
    cout<<"Time taken : "<<omp_get_wtime()<<endl;
    cout<<"Pi : "<<pi<<endl;
}
