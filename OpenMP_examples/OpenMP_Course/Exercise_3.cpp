#include <omp.h>
#include <iostream>

using namespace std;
#define NUM_THREADS 2
static long num_steps =100000;
double step;

int main()
{
    int i, nthreads;
    double pi;
    step = 1.0 / (double)num_steps;
    omp_set_num_threads(NUM_THREADS);
#pragma omp parallel
    {
        int i, id, nthrds; 
        double x, sum;
        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();
        if (id == 0) nthreads = nthrds;
        // Round robin distribution
        for (i=id  ; i<num_steps;i= i +nthrds )
        {
            x=(i+0.5)*step;
            sum += 4.0/(1.0+x*x);
        }
# pragma omp critical 
        pi += step * sum;
/*
 * # pragma omp critical 
        pi += step * sum;
// Same as critical but, less overhead in this case as it is only updation
*/
    }
    cout<<"Time taken : "<<omp_get_wtime()<<endl;
    cout<<"Pi : "<<pi<<endl;
}



// No need for a develoer to know the cache line
