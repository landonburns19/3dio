#include <iostream>
#include <thread>
#include <mutex>
#include <pthread.h>
#include <cmath>
using namespace std;



class observerpoint{
public:
observerpoint* next = nullptr;
double xlocation = -1.0;
double ylocation = -1.0;
double zlocation = -1.0;

void proximity_checkin(observerpoint* temp){
    mtx.lock();
if(next != nullptr){
    mtx.unlock();
    next -> proximity_checkin(temp); 
    return;
}
else{
    next = temp;
    mtx.unlock();
    return;
}
}

void proximity_checkOUT(observerpoint* temp2){
    mtx.lock();
if(next != temp2){
    mtx.unlock();
    next -> proximity_checkOUT(temp2); 
    return;
}
else if ((next == temp2) && ((next -> next) == nullptr)){
    next = nullptr;
    mtx.unlock();
    return;
}
else if ((next == temp2) && ((next -> next) != nullptr)){
    next = (next -> next);
    mtx.unlock();
    return;
}
else{
    cout << "error in proximity_checkOUT (it's probably not finding temp2)";
    return;
}
}




private:
mutex mtx;
};



class mapobject{

public:
observerpoint ***mapply;
int xmap = 0;
int ymap = 0;
int zmap = 0;

void mapper(int xlx, int yly, int zlz){
    mapply = new observerpoint**[xlx];
    xmap = xlx;
    ymap = yly;
    zmap = zlz;
    
    for (int i = 0; i < xlx; ++i) {
        mapply[i] = new observerpoint*[yly];
        for (int j = 0; j < yly; ++j) {
            mapply[i][j] = new observerpoint[zlz];
        }
    }

    for (int i = 0; i < xlx; ++i) {
        for (int j = 0; j < yly; ++j) {
            for (int l = 0; l < zlz; ++l) {
                double tempxs = (i);
                double tempys = (j);
                double tempzs = (l);
                (mapply[i][j][l].xlocation) = tempxs;
                (mapply[i][j][l].ylocation) = tempys;
                (mapply[i][j][l].zlocation) = tempzs;
            }
        }
    }

}




void changepos_xyz(observerpoint* tempxyz, double newposx, double newposy, double newposz){
    
    
    if((tempxyz -> xlocation) == -1){
        (tempxyz -> xlocation) = newposx;
        (tempxyz -> ylocation) = newposy;
        (tempxyz -> zlocation) = newposz;

        int xbc = static_cast<int>(newposx);
        int ybc = static_cast<int>(newposy);
        int zbc = static_cast<int>(newposz);

        mapply[xbc][ybc][zbc].proximity_checkin(tempxyz); 
    }
   else if((floor(tempxyz -> xlocation) != floor(newposx)) || (floor(tempxyz -> ylocation) != floor(newposy)) || (floor(tempxyz -> zlocation) != floor(newposz))){
        
        
        
        mapply[static_cast<int>(tempxyz -> xlocation)][static_cast<int>(tempxyz -> ylocation)][static_cast<int>(tempxyz -> ylocation)].proximity_checkOUT(tempxyz);

        (tempxyz -> xlocation) = newposx;
        (tempxyz -> ylocation) = newposy;
        (tempxyz -> zlocation) = newposz;

        int xbc = static_cast<int>(newposx);
        int ybc = static_cast<int>(newposy);
        int zbc = static_cast<int>(newposz);

        mapply[xbc][ybc][zbc].proximity_checkin(tempxyz); 
    }
    else{
        (tempxyz -> xlocation) = newposx;
        (tempxyz -> ylocation) = newposy;
        (tempxyz -> zlocation) = newposz;
    }
     
    
    return;
};

};












struct seedingstuff { 
  int detail;
  int oblique_or_passable;
  double xspace;
  double yspace;
  double zspace;
  double radius;
  mapobject* newspace;
};



void* cuber(void* arg) {
   seedingstuff* newarg = static_cast<seedingstuff*>(arg);
    bool exiter = true;
    observerpoint* square1 = new observerpoint;
    observerpoint* square2 = new observerpoint;
    observerpoint* square3 = new observerpoint;
    observerpoint* square4 = new observerpoint;
    observerpoint* square5 = new observerpoint;
    observerpoint* square6 = new observerpoint;
    observerpoint* square7 = new observerpoint;
    observerpoint* square8 = new observerpoint;
    
   
    newarg -> newspace -> changepos_xyz(square1, newarg -> xspace, newarg -> yspace, newarg -> zspace );
    newarg -> newspace -> changepos_xyz(square2, ((newarg -> radius)+(newarg -> xspace)), newarg -> yspace, newarg -> zspace );
    newarg -> newspace -> changepos_xyz(square3, newarg -> xspace, ((newarg -> radius)+(newarg -> yspace)), newarg -> zspace );
    newarg -> newspace -> changepos_xyz(square4, ((newarg -> radius)+(newarg -> xspace)), ((newarg -> radius)+(newarg -> yspace)), newarg -> zspace );
    
    newarg -> newspace -> changepos_xyz(square5, newarg -> xspace, newarg -> yspace, ((newarg -> radius)+(newarg -> zspace)) );
    newarg -> newspace -> changepos_xyz(square6, ((newarg -> radius)+(newarg -> xspace)), newarg -> yspace, ((newarg -> radius)+(newarg -> zspace)));
    newarg -> newspace -> changepos_xyz(square7, newarg -> xspace, ((newarg -> radius)+(newarg -> yspace)), ((newarg -> radius)+(newarg -> zspace)));
    newarg -> newspace -> changepos_xyz(square8, ((newarg -> radius)+(newarg -> xspace)), ((newarg -> radius)+(newarg -> yspace)), ((newarg -> radius)+(newarg -> zspace)) );

    /*
     square1
     square2;
     square3;
     square4;
     square5;
     square6;
     square7;
     square8;

    */
    return NULL;
}











int main() {
    /* mapobject newspace;
    observerpoint testly2;
    newspace.mapper(100, 100, 30);
    observerpoint* testly = new observerpoint;
    testly2.proximity_checkin(testly);
    newspace.changepos_x(testly, 10.1);
    testly2.proximity_checkOUT(testly);
    */

    pthread_t thread;
    seedingstuff arg;
    arg.detail = 9;
    arg.oblique_or_passable = 1;
    arg.xspace = 50;
    arg.yspace = 40;
    arg.zspace = 10;
    arg.radius = 3.0;
    
    int mapx = 100;
    int mapy = 100;
    int mapz = 30;

    arg.newspace = new mapobject;
    arg.newspace -> mapper(mapx, mapy, mapz);
    pthread_create(&thread, NULL, cuber, (void*)&arg);
    pthread_join(thread, NULL); 
    


    return 0;
}