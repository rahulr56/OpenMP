#include <omp.h>
#include <stdio.h>
#include <iostream>
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

        // If the element is present at the middle itself
        /*
           if (arr[mid] == x)  
           return mid;
           */
        // If element is smaller than mid, then it can only be present
        // in left subarray
        if (arr[mid] > x) 
        {
            return mid;
        }
        //return binarySearch(arr, l, mid-1, x);

        // Else the element can only be present in right subarray
        return binarySearch(arr, mid+1, r, x);
    }

    // We reach here when element is not present in array
    return -1;
}

int* merge(int* arr, int l, int m, int r) 
{
    int i=l;
    int j = m, k=0;
    int *temp = new int[r-l];
#pragma omp critical
    {
        for (int i=l ;i<r;i++)
            std::cout<<"+++"<<arr[i]<<"\t";
        std::cout<<std::endl;
    }
    while(i<m  && j<r)
    {
        if(arr[i]< arr[j])
        {
            temp[k] = arr[i];
            ++i;
        }
        else 
        {
            temp[k] = arr[j];
            ++j;
        }
        ++k;
    }

    while(i<m)
    {
        temp[k] = arr[i];
        ++i; ++k;
    }
    while(j<r)
    {
        temp[k] = arr[j];
        ++j; ++k;
    }
    memcpy(arr+l, temp, (r-l)*sizeof(int));
#pragma omp critical
    {
        for (int i=0 ;i<(r-l);i++)
            std::cout<<"_____"<<arr[i]<<"\t";
        std::cout<<std::endl;
    }
}

void mergesort(int *arr, int l, int r)
{
    /*
#pragma omp critical 
    {
        std::cout<<std::endl;
        for(int i=l;i<r;i++)
        {
            std::cout<<">>>>>"<<arr[i]<<"\t"; 
        }
        std::cout<<std::endl;
    }
    */
    
    if(l < r)
    {
        int mid = floor((l + r)/2);
//        int mid = (l +(r-1)/2);
//        std::cout<<"P1 -> low : "<<l<<"\t mid "<<mid<<std::endl;
        mergesort(arr, l, mid);
//        std::cout<<"P1 -> mid+1 : "<<mid +1<<"\t End "<<r<<std::endl;
        mergesort(arr, mid+1, r);
        merge(arr, l, mid,r);
    }
}


void mergesortTask(int* arr, int size, int *result)
{
    int *temp = new int[size];
    memcpy(temp, arr, size*sizeof(int));
/*
#pragma omp critical 
    {
        std::cout<<"Size : " <<size<<std::endl; 
        for(int i=0;i<size;i++)
        {
            temp[i] = arr[i];
            std::cout<<temp[i]<<" :  "<<arr[i]<<std::endl;
        }
    }
    
    std::cout<<std::endl;
    
    */
    mergesort(temp, 0, size);
    result = temp;
}

int main (int argc, char* argv[]) {

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
    omp_set_num_threads(atoi(argv[2]));

    int n = atoi(argv[1]);
    int nbthreads = atoi(argv[2]);
    int granularity = (int)ceil((float)n/(float)nbthreads);
    int * arr = new int [atoi(argv[1])];

    std::cout<<"Granularity " <<granularity<<std::endl;

    generateMergeSortData (arr, atoi(argv[1]));

    std::cout<<"Input array :\n";
    for (int i=0;i<n;i++)
        std::cout<<arr[i]<<"\t";

    std::cout<<std::endl;

    int count = 0; 
    int (*tempArr)[nbthreads];
#pragma omp parallel 
    {
#pragma omp single firstprivate(granularity)
        for (int i=0; i<nbthreads;i++)
        {
            int aStart = i*granularity;
            if(i == nbthreads - 1 )
                granularity = n - count;
#pragma omp task
            mergesortTask(&arr[aStart], granularity, tempArr[i]);
            count +=granularity;
        }
#pragma omp taskwait
    }

    checkMergeSortResult (arr, atoi(argv[1]));

    delete[] arr;

    return 0;
}
