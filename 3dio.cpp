
#include <iostream>
#include <thread>
#include <mutex>
#include <pthread.h>
using namespace std;


struct seedingstuff { 
  int detail;
  int oblique_or_passable;
  int xspace;
  int yspace;
  int zspace;
  double radius;
};








void* cuber(void* arg) {
   seedingstuff* newarg = static_cast<seedingstuff*>(arg);
    cout << newarg -> radius;

    

    
    return NULL;
}










class spaceobject {
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

// mapply[5][5][5] = 5.5;

}


};


class observerpoint{
public:
observerpoint* next;
double xlocation;
double ylocation;
double zlocation;

void proximity_checkin(observerpoint* temp){
    mtx.lock();
if(next != nullptr){
    mtx.unlock();
    next -> proximity_checkin(temp); 
}
else{
    next = temp;
    mtx.unlock();
    return;
}
}

private:
mutex mtx;
};





int main() {
    
    pthread_t thread;
    seedingstuff arg;
    arg.detail = 9;
    arg.oblique_or_passable = 1;
    arg.xspace = 100;
    arg.yspace = 100;
    arg.zspace = 10;
    arg.radius = 3.0;
    spaceobject space1;
    space1.mapper(arg.xspace, arg.yspace, arg.zspace);
    
    
     int result = pthread_create(&thread, NULL, cuber, (void*)&arg);

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