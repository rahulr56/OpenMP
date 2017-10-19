#include <omp.h>
#include <iostream>

using namespace std;

int main()
{
#pragma omp parallel
    {
    int id = omp_get_thread_num();
    cout<<"Hello"<<id<<" world "<<id<<endl;
    }
}
