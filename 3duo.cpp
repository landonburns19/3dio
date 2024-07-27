#include <iostream>
#include <thread>
#include <mutex>
#include <pthread.h>
#include <cmath>
#include <fstream>
#include <stack>
using namespace std;



class observerpoint{
public:
observerpoint* next = nullptr;
observerpoint** shape_array = nullptr;
int shape_array_size = 0;
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

void change_shape_array(observerpoint* temp_shape_array[], int tempsize){

    shape_array = temp_shape_array;
    shape_array_size = tempsize;
    return;

}



mutex pos;
private:
mutex mtx;
};



class mapobject{

public:
observerpoint ***mapply;
int xmap = 0;
int ymap = 0;
int zmap = 0;
stack<observerpoint**> allshapes;

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

        (tempxyz -> pos).lock();
        (tempxyz -> xlocation) = newposx;
        (tempxyz -> ylocation) = newposy;
        (tempxyz -> zlocation) = newposz;
        (tempxyz -> pos).unlock();


        int xbc = static_cast<int>(newposx);
        int ybc = static_cast<int>(newposy);
        int zbc = static_cast<int>(newposz);
        
        mapply[xbc][ybc][zbc].proximity_checkin(tempxyz); 



    }
   else if((floor(tempxyz -> xlocation) != floor(newposx)) || (floor(tempxyz -> ylocation) != floor(newposy)) || (floor(tempxyz -> zlocation) != floor(newposz))){

        int xth = static_cast<int>(tempxyz->xlocation);
        int yth = static_cast<int>(tempxyz->ylocation);
        int zth = static_cast<int>(tempxyz->zlocation);

        mapply[xth][yth][zth].proximity_checkOUT(tempxyz);

        (tempxyz -> pos).lock();
        (tempxyz -> xlocation) = newposx;
        (tempxyz -> ylocation) = newposy;
        (tempxyz -> zlocation) = newposz;
        (tempxyz -> pos).unlock();

        int xbc = static_cast<int>(newposx);
        int ybc = static_cast<int>(newposy);
        int zbc = static_cast<int>(newposz);

        mapply[xbc][ybc][zbc].proximity_checkin(tempxyz); 
    }
    else{
        (tempxyz -> pos).lock();
        (tempxyz -> xlocation) = newposx;
        (tempxyz -> ylocation) = newposy;
        (tempxyz -> zlocation) = newposz;
        (tempxyz -> pos).unlock();
    }
     
    
    return;
};


void addtoshapestack(observerpoint* addingtostack[]){
    allshapes.push(addingtostack);
}

void create_space_file(){
    ofstream outfile("initializer.txt");

    outfile << xmap << " " << ymap << " " << zmap << endl;

    while (!allshapes.empty()) {
        outfile << "next object" << endl;
        for( int iy = 0; iy < (((allshapes.top())[0]) -> shape_array_size); ++iy){
            
            outfile << " " << (((allshapes.top())[iy]) -> xlocation) << " " << (((allshapes.top())[iy]) -> ylocation) << " " << (((allshapes.top())[iy]) -> zlocation) << endl;
        }
        allshapes.pop();
    }


// this code doesn't scale efficiently. change it so that looks for unique shape_array 's 
/*
    for (int i = 0; i < xmap; ++i) {
        for (int j = 0; j < ymap; ++j) {
            for (int l = 0; l < zmap; ++l) {
                
                if(mapply[i][j][l].next != nullptr){
                    outfile << "list" << endl << i << " " << j << " " << l << endl;
                    observerpoint* checkly = mapply[i][j][l].next;
                    while(checkly != nullptr){
                    outfile << checkly << endl;
                        for(int al = 0; al < checkly -> shape_array_size; al++){
                           outfile << (checkly -> shape_array)[al] << " ";
                        }
                        outfile << endl;
                        checkly = checkly -> next;
                    }
                }
                
            }
        }
    }

    */
    outfile.close();
}

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

    observerpoint* connectedpoints[8];

    connectedpoints[0] = square1;
    connectedpoints[1] = square2;
    connectedpoints[2] = square3;
    connectedpoints[3] = square4;
    connectedpoints[4] = square5;
    connectedpoints[5] = square6;
    connectedpoints[6] = square7;
    connectedpoints[7] = square8;
    newarg -> newspace -> addtoshapestack(connectedpoints);

    square1->change_shape_array(connectedpoints, 8);
    square2->change_shape_array(connectedpoints, 8);
    square3->change_shape_array(connectedpoints, 8);
    square4->change_shape_array(connectedpoints, 8);
    square5->change_shape_array(connectedpoints, 8);
    square6->change_shape_array(connectedpoints, 8);
    square7->change_shape_array(connectedpoints, 8);
    square8->change_shape_array(connectedpoints, 8);
   
    newarg -> newspace -> changepos_xyz(square1, newarg -> xspace, newarg -> yspace, newarg -> zspace);
    newarg -> newspace -> changepos_xyz(square2, ((newarg -> radius)+(newarg -> xspace)), newarg -> yspace, newarg -> zspace );
    newarg -> newspace -> changepos_xyz(square3, newarg -> xspace, ((newarg -> radius)+(newarg -> yspace)), newarg -> zspace );
    newarg -> newspace -> changepos_xyz(square4, ((newarg -> radius)+(newarg -> xspace)), ((newarg -> radius)+(newarg -> yspace)), newarg -> zspace );
    
    newarg -> newspace -> changepos_xyz(square5, newarg -> xspace, newarg -> yspace, ((newarg -> radius)+(newarg -> zspace)) );
    newarg -> newspace -> changepos_xyz(square6, ((newarg -> radius)+(newarg -> xspace)), newarg -> yspace, ((newarg -> radius)+(newarg -> zspace)));
    newarg -> newspace -> changepos_xyz(square7, newarg -> xspace, ((newarg -> radius)+(newarg -> yspace)), ((newarg -> radius)+(newarg -> zspace)));
    newarg -> newspace -> changepos_xyz(square8, ((newarg -> radius)+(newarg -> xspace)), ((newarg -> radius)+(newarg -> yspace)), ((newarg -> radius)+(newarg -> zspace)) );
    newarg -> newspace -> create_space_file();

    
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