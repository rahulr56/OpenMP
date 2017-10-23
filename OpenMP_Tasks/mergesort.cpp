#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <chrono>
#include <ratio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>


#ifdef __cplusplus
extern "C" {
#endif

    void generateMergeSortData (int* arr, size_t n);
    void checkMergeSortResult (int* arr, size_t n);


#ifdef __cplusplus
}
#endif

int binarySearch(int *arr, int l, int r, int x)
{
    if (r >= l)
    {
        int mid = l + (r - l)/2;

        if (arr[mid] > x) 
        {
            return mid;
        }
        return binarySearch(arr, mid+1, r, x);
    }
}

void merge (int* arr, int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
 
    /* create temp arrays */
    int *L = new int[n1];
    int *R = new int[n2];
 
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            ++i;
        }
        else
        {
            arr[k] = R[j];
            ++j;
        }
        ++k;
    }
 
    while (i < n1)
    {
        arr[k] = L[i];
        ++i;
        ++k;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        ++j;
        ++k;
    }
}

void mergesort(int *arr, int l, int r)
{
    if(l < r)
    {
        int mid = l +(r-l )/2;
        mergesort(arr, l, mid);
        mergesort(arr, mid+1, r);
        merge(arr, l, mid,r);
    }
}


int main (int argc, char* argv[]) 
{

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
    if (argc < 3) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
        return -1;
    }

    int n = atoi(argv[1]);
    int nbthreads = atoi(argv[2]);
    int granularity = 2;
    int * arr = new int [atoi(argv[1])];
    omp_set_num_threads(nbthreads);

    generateMergeSortData (arr, atoi(argv[1]));

    int count = 1; 
    int (*tempArr)[nbthreads];
    int treeHeight = log(n)/log(2) +1;

    std::chrono::high_resolution_clock::time_point start  = std::chrono::high_resolution_clock::now();
    while (treeHeight)
    {
#pragma omp parallel 
        {
#pragma omp single firstprivate(granularity)
            for (int i=0; i<n ;i+=granularity)
            {
                int aStart = i; // *granularity;
                int aEnd = i + granularity;
                if(aEnd >= n)
                {
                    aEnd = n-1;
                }
#pragma omp task
                mergesort(arr, aStart, aEnd);
            }
#pragma omp taskwait
        }
        --treeHeight;
        granularity = pow(2,++count);
#pragma omp barrier
    }
    std::chrono::high_resolution_clock::time_point end  = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr.precision(10);
    std::cerr<<std::fixed<<elapsed_seconds.count()<<std::endl;

    checkMergeSortResult (arr, atoi(argv[1]));

    delete[] arr;

    return 0;
}
