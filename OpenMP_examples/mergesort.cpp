#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <algorithm>


#ifdef __cplusplus
extern "C" {
#endif

    void generateMergeSortData (int* arr, size_t n);
    void checkMergeSortResult (int* arr, size_t n);


#ifdef __cplusplus
}
#endif
struct List {
    int val;
    List *next;
};

struct data {
    List *head;
    int size;
};

void merge(struct data *array1,struct data *array2) 
{
    int size = array1->size + array2->size;
    struct List *head = NULL, *temp = NULL, *curr = NULL;
    struct List *startA1 = array1->head, *startA2 = array2->head;
    int k=0;
    while (startA1 != NULL && startA2 != NULL)
    {
        if(startA1->val <= startA2->val) 
        {
            temp = startA1;
            startA1 = startA1->next;
        }
        else 
        {
            temp = startA2;
            startA2 = startA2->next;
        }
        if(head == NULL)
            head = temp; 
        else 
            curr->next = temp; 
        curr = temp;
    }
    while(startA1 != NULL)
    {
        temp = startA1; 
        startA1 = startA1->next; 
        curr->next = temp;
        curr = temp;
    }
    while(startA2 != NULL)
    {
        temp = startA2; 
        startA2 = startA2->next; 
        curr->next = temp;
        curr = temp;
    }
    array1->head = head;
    array1->size = size;
}

int main (int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nthreads>"<<std::endl; 
        return -1;
    }
    omp_set_num_threads(atoi(argv[2]));
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

    int * arr = new int [atoi(argv[1])];

    generateMergeSortData (arr, atoi(argv[1]));
    int n = atoi(argv[1]);
    struct data *threadData = new data[n];
    int height = (int) ceil(log(n) / log(2)) + 1;

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
#pragma omp parallel for
    for(int i=0; i<n; i++)
    {
        threadData[i].size = 1;
        threadData[i].head = new List {arr[i], NULL};
    }

    for (int i=1; i<height; i++) 
    {
#pragma omp parallel
        {
            int numLeafNodes = ceil(float(n) / pow(2, i - 1)) - 1;
            int numTopLevelNodes = ceil(float(n) / pow(2, i));
#pragma omp for
            for(int k=0; k < numTopLevelNodes; k++) {
                int last = numLeafNodes - k, first = k;
                if(first != last)
                    merge(&threadData[first], &threadData[last]);
            }
#pragma omp barrier
/*#pragma omp master
            {;
            }*/
        }
    }
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end-start;
    std::cerr.precision(10);
    std::cerr <<std::fixed<< elapsed.count() << std::endl;
    List *iter = threadData[0].head;
    for(int i=0; i<n; i++)
    {
        arr[i] = iter->val;
        iter = iter->next;
    }

    checkMergeSortResult (arr, atoi(argv[1]));

    delete[] arr;
    delete[] threadData;

    return 0;
}
