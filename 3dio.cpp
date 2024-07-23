
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

};


int main() {
    
    pthread_t thread;
    seedingstuff arg;
    arg.detail = 9;
    arg.oblique_or_passable = 1;
     int result = pthread_create(&thread, NULL, asquare, (void*)&arg);

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