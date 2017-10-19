#include <omp.h>
#include <iostream>

using namespace std;
#define NUM_THREADS 2
static long num_steps =100000;
double step;

int main()
{
    int i;
    double pi,sum=0.0;
    step = 1.0 / (double)num_steps;
    omp_set_num_threads(NUM_THREADS);
#pragma omp parallel 
    {
        double x =0.0;
#pragma omp for reduction(+:sum)
        for (i=0;i<num_steps ; i++)
        {
            x=(i+0.5)*step;
            sum += 4.0/(1.0+x*x);
        }
    }
    pi = sum * step;
    cout<<"Time taken : "<<omp_get_wtime()<<endl;
    cout<<"Pi : "<<pi<<endl;
}
