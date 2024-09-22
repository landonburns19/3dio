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
mutex thisone;
mutex thatone;

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
     
    //cout << " " << (allshapes.top())[1] -> xlocation;
    return;
};


void addtoshapestack(observerpoint* addingtostack[]){
    thisone.lock();
    allshapes.push(addingtostack);
    thisone.unlock();
    
}

void create_space_file(observerpoint* addingtoinitialize[], string shapename){
    thatone.lock();
    
    ofstream outfile("initializer.txt", ios::app);
    outfile << "nextobject " << shapename << endl;

      
        for( int iy = 0; iy < (addingtoinitialize[0]) -> shape_array_size; ++iy){
            outfile << " " << (addingtoinitialize[iy]) -> xlocation << " " << (addingtoinitialize[iy]) -> ylocation << " " << (addingtoinitialize[iy]) -> zlocation << endl;
        }
         


    outfile.close();
    thatone.unlock();
}


void create_bmp_file(observerpoint* addingtoinitialize[]){

    double distancer = 2;
    double by = 0;
    double bz= 0;
    double bx= 0;
            double mg3x = (((addingtoinitialize[0]) -> xlocation) - bx);
            double mg3y = (((addingtoinitialize[0]) -> ylocation) - by);
            double mg3z = (((addingtoinitialize[0]) -> zlocation) - bz);
            double mag3 = sqrt((mg3x*mg3x)+(mg3y*mg3y)+(mg3z*mg3z));
    
    

    
    for( int iy = 1; iy < (addingtoinitialize[0]) -> shape_array_size; ++iy){
            double dotproductive = ((((addingtoinitialize[0]) -> xlocation) - bx) * (((addingtoinitialize[iy]) -> xlocation) - bx)) + ((((addingtoinitialize[0]) -> ylocation) - by) * (((addingtoinitialize[iy]) -> ylocation) - by)) + ((((addingtoinitialize[0]) -> zlocation) - bz) * (((addingtoinitialize[iy]) -> zlocation) - bz));

            double mg1x = (((addingtoinitialize[0]) -> xlocation) - bx);
            double mg1y = (((addingtoinitialize[0]) -> ylocation) - by);
            double mg1z = (((addingtoinitialize[0]) -> zlocation) - bz);

            double mg2x = (((addingtoinitialize[iy]) -> xlocation) - bx);
            double mg2y = (((addingtoinitialize[iy]) -> ylocation) - by);
            double mg2z = (((addingtoinitialize[iy]) -> zlocation) - bz);

            double mag1 = sqrt((mg1x*mg1x)+(mg1y*mg1y)+(mg1z*mg1z));
            double mag2 = sqrt((mg2x*mg2x)+(mg2y*mg2y)+(mg2z*mg2z));
            cout << acos(dotproductive/(mag1 * mag2));
            theta1 = acos(dotproductive/(mag1 * mag2));

            double hypot = (cos(theta1) * (mag3/distancer));
            double scaler = hypot/mag2;
            mg4x = mg2x * (scaler);
            mg4y = mg2y * (scaler);
            mg4z = mg2z * (scaler);

            //what would make the base line aligned with an axis? Then apply those constraints to the second


        }
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
    string nameof = "cube";
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
    newarg -> newspace -> create_space_file(connectedpoints, nameof);
    newarg -> newspace -> create_bmp_file(connectedpoints);
    
    return NULL;
}











int main() {


    pthread_t thread;
    seedingstuff arg;
    pthread_t thread2;
    seedingstuff arg2;

    arg.detail = 9;
    arg.oblique_or_passable = 1;
    arg.xspace = 50;
    arg.yspace = 40;
    arg.zspace = 10;
    arg.radius = 3.0;

    arg2.detail = 9;
    arg2.oblique_or_passable = 1;
    arg2.xspace = 20;
    arg2.yspace = 20;
    arg2.zspace = 10;
    arg2.radius = 4.0;
    
    int mapx = 100;
    int mapy = 100;
    int mapz = 30;

    mapobject* space1 = new mapobject;
    space1 -> mapper(mapx, mapy, mapz);
    arg.newspace = space1;
    arg2.newspace = space1;

    ofstream outfile("initializer.txt", ios::app);
    outfile << mapx << " " << mapy << " " << mapz << endl;

    pthread_create(&thread, NULL, cuber, (void*)&arg);
    pthread_create(&thread2, NULL, cuber, (void*)&arg2);
    pthread_join(thread, NULL); 
    pthread_join(thread2, NULL); 

  

    return 0;
}