#include <iostream>
#include <chrono>

__global__
void computePolynomial(float*array, float* poly, int degree, int n)
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  if( i < n )
  {
    float result = 0.0;
    float xtothepowerof = 1.0;
    for ( int x = 0; x < degree; ++x)
    {
      result += xtothepowerof * poly[x];
      xtothepowerof *= array[i];
    }
    array[i] = result;
  }
}
__global__
void polynomial_expansion (float* poly, int degree,
			   int n, float* array) {
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  if( i < n )
  {
    float result = 0.0;
    float xtothepowerof = 1.0;
    for ( int x = 0; x < degree; ++x)
    {
      result += xtothepowerof * poly[x];
      xtothepowerof *= array[i];
    }
    array[i] = result;
  }
}

int main (int argc, char* argv[]) 
{
  if (argc < 3) 
  {
     std::cerr<<"usage: "<<argv[0]<<" n degree"<<std::endl;
     return -1;
  }

  int n = atoi(argv[1]); 
  int degree = atoi(argv[2]);
  int nbiter = 1;

  float* array = new float[n];
  float* poly = new float[degree+1];
  for (int i=0; i<n; ++i)
    array[i] = 1.;

  for (int i=0; i<degree+1; ++i)
    poly[i] = 1.;

  float *gpuArray, *gpuPoly;

  cudaMallocManaged(&gpuArray, n*sizeof(float));
  cudaMallocManaged(&gpuPoly, (degree+1)*sizeof(float));
  
  cudaMemCpy(gpuArray, array, n*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemCpy(gpuPoly, poly, n*sizeof(float), cudaMemcpyHostToDevice);

  polynomial_expansion<<<(n+255)/256, 256>>>(gpuPoly, degree, n, gpuArray);
  cudaMemCpy(array, gpuArray, n*sizeof(float), cudaMemcpyDeviceToHost);

  cudaFree(gpuArray);
  cudaFree(gpuPoly);

  std::chrono::time_point<std::chrono::system_clock> begin, end;
  begin = std::chrono::system_clock::now();
  
  for (int iter = 0; iter<nbiter; ++iter)
    polynomial_expansion (poly, degree, n, array);
  cudaDeviceSychronze();
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> totaltime = (end-begin)/nbiter;

  std::cerr<<array[0]<<std::endl;
  std::cout<<n<<" "<<degree<<" "<<totaltime.count()<<std::endl;

  delete[] array;
  delete[] poly;

  return 0;
}
