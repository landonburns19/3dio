
#include <iostream>
#include <thread>
#include <mutex>
#include <pthread.h>
using namespace std;

void* acube(void* arg) {
    int* num = (int*) arg;
    printf("Hello from the thread! Argument passed: %d\n", *num);
    return NULL;
}


struct seedingstuff { 
  int detail;
  int oblique_or_passable;
  int xspace;
  int yspace;
  int zspace;
};



class threadsafeclass {
public:

double ***mapply;
void mapper(int xlx, int yly, int zlz){
    double ***mapply = new double**[xlx];
    for (int i = 0; i < xlx; ++i) {
        mapply[i] = new double*[yly];
        for (int j = 0; j < yly; ++j) {
            mapply[i][j] = new double[zlz];
        }
    }
    
}



void checkin(){

mtx.lock();

mtx.unlock();
return;
}



private:
mutex mtx;
};



int main() {
    
    pthread_t thread;
    seedingstuff arg;
    arg.detail = 9;
    arg.oblique_or_passable = 1;
    arg.xspace = 30;
    arg.yspace = 30;
    arg.zspace = 30;
    threadsafeclass space1;
    space1.mapper(arg.xspace, arg.yspace, arg.zspace);
    
    
     int result = pthread_create(&thread, NULL, acube, (void*)&arg);

    if (result != 0) {
        fprintf(stderr, "Error creating thread: %d\n", result);
        return EXIT_FAILURE;
    }

    result = pthread_join(thread, NULL);

    if (result != 0) {
        fprintf(stderr, "Error joining thread: %d\n", result);
        return EXIT_FAILURE;
    }


    return 0;
}