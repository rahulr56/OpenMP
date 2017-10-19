#include <chrono>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#ifdef __cplusplus
extern "C" {
#endif

  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
  
  
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {

  
  if (argc < 2) {
    std::cerr<<"Usage: "<<argv[0]<<" <n>"<<std::endl;
    return -1;
  }


  int n = atoi(argv[1]);
  
  int * arr = new int [n];

  generatePrefixSumData (arr, n);
  

  //write code here
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  int* newarr = new int [n+1];

  newarr[0] = 0;

  for (int i=0; i<n; ++i) {
    newarr[i+1] = newarr[i] + arr[i];
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;

  checkPrefixSumResult(newarr, n);
  
  delete[] arr;
  delete[] newarr;

  return 0;
}
