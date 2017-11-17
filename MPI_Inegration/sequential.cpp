#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>


float sequential_integrate (int argc, char* argv[]);


extern "C" {

    float f1(float x, int intensity);
    float f2(float x, int intensity);
    float f3(float x, int intensity);
    float f4(float x, int intensity);

}

float (*funcPtr) (float,int);

int main (int argc, char* argv[]) {

    if (argc < 6) {
        std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
        return -1;
    }
    int a = atoi(argv[2]);
    int n = atoi(argv[4]);
    int intensity = atoi(argv[5]);
    float multiplier = (atoi(argv[3]) - a) / (float)n;
    double integrate = 0.0;

    switch(atoi(argv[1]))
    {
        case 1:
            funcPtr = &f1;
            break;
        case 2:
            funcPtr = &f2;
            break;
        case 3:
            funcPtr = &f3;
            break;
        case 4:
            funcPtr = &f4;
            break;
        default:
            std::cerr<<"Invalid function number provided\n";
            exit(-1);
    }


    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    for (int x = 0; x < n; ++x)
    {
       integrate += ((double)funcPtr(a + (x + 0.5) * multiplier, intensity) * multiplier);
    }

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;

    std::cout<<integrate<<std::endl;

    std::cerr<<elapsed_seconds.count()<<std::endl;

    return 0;
}
