#include <iostream>
#include <thread>
#include <mutex>
#include <pthread.h>
#include <cmath>
#include <fstream>
#include <stack>
#include <vector>
#include <cstdint>



using namespace std;


//Observer points are used for two things. It forms a gridlike map onto which a linked of list of nearby points can be checked in to observerpoint* next. 
//The purpose of this is to facilitate multithreading.
class observerpoint{
public:
observerpoint* next = nullptr; //this can lead to other points that are "checked in"
observerpoint** shape_array = nullptr; //this can lead to other points in the shape.
int shape_array_size = 0; // this holds the amount of points in the shape

int line_length = 0;
int mag_up = 0;
int mag_side = 0;

//location information
double xlocation = -1.0;
double ylocation = -1.0;
double zlocation = -1.0;

//this is some mutex stuff
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
    //cout << endl << "shape array size" << shape_array_size << endl;
    return;

}

void change_line_length(int temp_line_length){
    // ?minus one because temp_line_length was defined for an array?
    line_length = temp_line_length;
    cout << endl << "line length" << line_length << endl;
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
mutex thisone;
mutex thatone;

// this initializes the 3D space in which the program executes. It is a 3D array of observerpoints
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



//this function is made to change the location information of a particular observer point.
void changepos_xyz(observerpoint* tempxyz, double newposx, double newposy, double newposz){
    
    //all location information is positive. A negative value means uninitialized.
    if((tempxyz -> xlocation) == -1){

        (tempxyz -> pos).lock();
        (tempxyz -> xlocation) = newposx;
        (tempxyz -> ylocation) = newposy;
        (tempxyz -> zlocation) = newposz;
        (tempxyz -> pos).unlock();


        int xbc = static_cast<int>(newposx);
        int ybc = static_cast<int>(newposy);
        int zbc = static_cast<int>(newposz);
        cout << xbc << " ";
        cout << ybc << " ";
        cout << zbc << endl;
        mapply[xbc][ybc][zbc].proximity_checkin(tempxyz); 
        


    }
    //One oberserver point is equal to a value of one in the position. If the change in position to a shape's observerpoint results in a new whole value, it will switch observerpoints from the map.
    //
    //In other words:
    //If x = 12.11 and .23 gets added to it
    //Then there is no call to checkOUT
    //
    //However:
    //If x = 12.11 and .89 gets added to it
    //Then the observerpoint gets moved to the bottom of the linked list coresponding to mapply[13][y][z]
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


};





struct seedingstuff {  
   //size of the created space
  double xspace; 
  double yspace;
  double zspace;
  mapobject* newspace; //reference to the space created
};

struct line_stuff {
    double xvec;
    double yvec;
    double zvec;
    double xlocation_inspace;
    double ylocation_inspace;
    double zlocation_inspace;
    seedingstuff space_info;
};

struct planar_stuff{
    planar_stuff* additionalplane;
    double xpoint1;
    double ypoint1;
    double zpoint1;

    double xpoint2;
    double ypoint2;
    double zpoint2;

    double xpoint3;
    double ypoint3;
    double zpoint3;

    seedingstuff space_info;
};

struct projection_stuff{
    double render_distance_multipliar;
    double window_length;
    double window_height;

    double xdirvec;
    double ydirvec;
    double zdirvec;

    double xpos;
    double ypos;
    double zpos;
    
    double x_base;
    double y_base;
    double z_base;

    seedingstuff space_info;
};

void* liner(void* arg) {
    line_stuff* newarg = static_cast<line_stuff*>(arg);
    string nameof = "lone_line";
    double hypotenuse = sqrt(((newarg -> xvec) * (newarg -> xvec))+((newarg -> yvec) * (newarg -> yvec))+((newarg -> zvec) * (newarg -> zvec)));
    int temphypo = static_cast<int>(hypotenuse);
    double xtemp_connectedpoints[temphypo];
    double ytemp_connectedpoints[temphypo];
    double ztemp_connectedpoints[temphypo];
    int j = 0;
    for(int i = 0; i < hypotenuse; i++){
        double multi = (i/hypotenuse);
        int xtempcheck = (multi * (newarg -> xvec)) + (newarg -> xlocation_inspace);
        int ytempcheck = (multi * (newarg -> yvec)) + (newarg -> ylocation_inspace);
        int ztempcheck = (multi * (newarg -> zvec)) + (newarg -> zlocation_inspace);
        if(j == 0){
            xtemp_connectedpoints[j] = xtempcheck;
            ytemp_connectedpoints[j] = ytempcheck;
            ztemp_connectedpoints[j] = ztempcheck;
            j = j + 1;
        }
        else if((floor(xtemp_connectedpoints[j - 1]) != floor(xtempcheck)) || (floor(ytemp_connectedpoints[j - 1]) != floor(ytempcheck)) || (floor(ztemp_connectedpoints[j - 1]) != floor(ztempcheck))){
            xtemp_connectedpoints[j] = xtempcheck;
            ytemp_connectedpoints[j] = ytempcheck;
            ztemp_connectedpoints[j] = ztempcheck;
            j = j + 1;
        }
    }



    if(((floor(xtemp_connectedpoints[j - 1]) != floor(newarg -> xvec)) || (floor(ytemp_connectedpoints[j - 1]) != floor(newarg -> yvec)) || (floor(ztemp_connectedpoints[j - 1]) != floor(newarg -> zvec)))){
        xtemp_connectedpoints[j] = ((newarg -> xvec) + (newarg -> xlocation_inspace));
        ytemp_connectedpoints[j] = ((newarg -> yvec) + (newarg -> ylocation_inspace));
        ztemp_connectedpoints[j] = ((newarg -> zvec))+ (newarg -> zlocation_inspace);
        j = j + 1;
    }



    observerpoint** connectedpoints = new observerpoint*[j];
        for(int g = 0; g < j; g++){
        connectedpoints[g] = new observerpoint;
    }
    cout << "hello" << endl;

    for(int ed = 0; ed < j; ed++){
        connectedpoints[ed] -> change_shape_array(connectedpoints, j);
        connectedpoints[ed] -> change_line_length(j);

    }

    for(int f = 0; f < j; f++){
        
        observerpoint* temp_observer = connectedpoints[f];

        newarg -> space_info.newspace -> changepos_xyz(temp_observer, xtemp_connectedpoints[f], ytemp_connectedpoints[f], ztemp_connectedpoints[f]);

    }






    return NULL;
}

















observerpoint** line_from_two_pos(double x1, double y1, double z1, double x2, double y2, double z2, mapobject* mappedhere){
    double xvec = x2 - x1;
    double yvec = y2 - y1;
    double zvec = z2 - z1;

    double hypotenuse = sqrt(((xvec) * (xvec))+((yvec) * (yvec))+((zvec) * (zvec)));
    int temphypo = static_cast<int>(hypotenuse);
    double xtemp_connectedpoints[temphypo];
    double ytemp_connectedpoints[temphypo];
    double ztemp_connectedpoints[temphypo];
    int j = 0;
    for(int i = 0; i < hypotenuse; i++){
        double multi = (i/hypotenuse);
        int xtempcheck = (multi * (xvec)) + (x1);
        int ytempcheck = (multi * (yvec)) + (y1);
        int ztempcheck = (multi * (zvec)) + (z1);
        if(j == 0){
            xtemp_connectedpoints[j] = xtempcheck;
            ytemp_connectedpoints[j] = ytempcheck;
            ztemp_connectedpoints[j] = ztempcheck;
            j = j + 1;
        }
        else if((floor(xtemp_connectedpoints[j - 1]) != floor(xtempcheck)) || (floor(ytemp_connectedpoints[j - 1]) != floor(ytempcheck)) || (floor(ztemp_connectedpoints[j - 1]) != floor(ztempcheck))){
            xtemp_connectedpoints[j] = xtempcheck;
            ytemp_connectedpoints[j] = ytempcheck;
            ztemp_connectedpoints[j] = ztempcheck;
            j = j + 1;
        }
    }



    if(((floor(xtemp_connectedpoints[j - 1]) != floor(xvec)) || (floor(ytemp_connectedpoints[j - 1]) != floor(yvec)) || (floor(ztemp_connectedpoints[j - 1]) != floor(zvec)))){
        xtemp_connectedpoints[j] = ((xvec) + (x1));
        ytemp_connectedpoints[j] = ((yvec) + (y1));
        ztemp_connectedpoints[j] = ((zvec))+ (z1);
        j = j + 1;
    }

    
    observerpoint** connectedpoints = new observerpoint*[j];
        for(int g = 0; g < j; g++){
        connectedpoints[g] = new observerpoint;
    }
    cout << "hello" << endl;

    for(int ed = 0; ed < j; ed++){
        connectedpoints[ed] -> change_shape_array(connectedpoints, j);
        connectedpoints[ed] -> change_line_length(j);
    }
cout << "nice";
    for(int f = 0; f < j; f++){
        
        observerpoint* temp_observer = connectedpoints[f];
        mappedhere -> changepos_xyz(temp_observer, xtemp_connectedpoints[f], ytemp_connectedpoints[f], ztemp_connectedpoints[f]);

    }
 
    return connectedpoints;

}


// right now the planes are hollow
observerpoint** planar_recursion(planar_stuff* surface){
    cout << "leftleftleft";
    observerpoint** line1 = line_from_two_pos((surface -> xpoint1), (surface -> ypoint1), (surface -> zpoint1), (surface -> xpoint2), (surface -> ypoint2), (surface -> zpoint2), surface -> space_info.newspace);
    observerpoint** line2 = line_from_two_pos((surface -> xpoint1), (surface -> ypoint1), (surface -> zpoint1), (surface -> xpoint3), (surface -> ypoint3), (surface -> zpoint3), surface -> space_info.newspace);
    observerpoint** line3 = line_from_two_pos((surface -> xpoint2), (surface -> ypoint2), (surface -> zpoint2), (surface -> xpoint3), (surface -> ypoint3), (surface -> zpoint3), surface -> space_info.newspace);
    cout << endl << (line1[0] -> shape_array_size) << endl << endl;

    int wholesize = (line1[0] -> shape_array_size) + (line2[0] -> shape_array_size) + (line3[0] -> shape_array_size);
    observerpoint** planeshape = new observerpoint*[wholesize];

    int counter = 0;
    for(int i = 0; i < (line1[0] -> shape_array_size); i++){
        planeshape[i] = line1[i];
        counter = counter + 1;
    }

    int counter2 = 0;
    for(int i = 0; i < (line2[0] -> shape_array_size); i++){
        planeshape[counter + i] = line2[i];
        counter2 = counter2 + 1;
    }

    int counter3 = 0;
    for(int i = 0; i < (line3[0] -> shape_array_size); i++){
        planeshape[counter + counter2 + i] = line3[i];
        counter3 = counter3 + 1;
    }

    for(int i = 0; i < wholesize; i++){
        planeshape[i] -> change_shape_array(planeshape, wholesize);
        
    }



    if((surface -> additionalplane) != nullptr){
        
        observerpoint** planeshape2 = planar_recursion((surface -> additionalplane));
        int wholesize2 = (planeshape[0] -> shape_array_size) + (planeshape2[0] -> shape_array_size);
        observerpoint** planeshape3 = new observerpoint*[wholesize2];

        int countera = 0;
    for(int i = 0; i < (planeshape[0] -> shape_array_size); i++){
        planeshape3[i] = planeshape[i];
        countera = countera + 1;
    }

    int counterb = 0;
    for(int i = 0; i < (planeshape2[0] -> shape_array_size); i++){
        planeshape3[countera + i] = planeshape2[i];
        counterb = counterb + 1;
    }

        for(int i = 0; i < wholesize2; i++){
        planeshape3[i] -> change_shape_array(planeshape3, wholesize2);
        cout << wholesize2 << "h" << " " << i << endl;
    }
    
        return planeshape3;
        
    }
    else{

        return planeshape;
    }
    
        
    
    

};



void* planar(void* arg) {
    planar_stuff* newarg = static_cast<planar_stuff*>(arg);
    string nameof = "planar";
    planar_recursion(newarg);

    
return NULL;
}

int pointfinder(int renderi, double pos, double vec){

    
    return static_cast<int>(ceil((renderi * vec) + pos));
}


class shaped_objects{
    public:
    observerpoint* pointed_at;
    shaped_objects* thenext;
};



//isinmap((objectsthatmustbemapped3 -> pointed_at -> shape_array[i1]), (exthat))
bool isinmap(observerpoint* isthishere, shaped_objects* tosearchthrough){
    while(true){

        if(isthishere == tosearchthrough -> pointed_at){
            return true;
        }


    if(tosearchthrough -> thenext != nullptr){
    tosearchthrough = tosearchthrough -> thenext;
    }
    else{
        return false;
    }
    }
}

//findpointonplanemultipliar(((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> xlocation), ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> ylocation), ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> zlocation), xvec, yvec, zvec, (newarg -> x_base), (newarg -> y_base), (newarg -> z_base));
double findpointonplanemultipliar(double tempx, double tempy, double tempz, double vecx, double vecy, double vecz, double basex, double basey, double basez){
    
    double hypo = sqrt(((tempx - basex) * (tempx - basex)) + ((tempy - basey) * (tempy - basey)) + ((tempz - basez) * (tempz - basez)));
    double dirvecx = (tempx - basex) / hypo;
    double dirvecy = (tempy - basey) / hypo;
    double dirvecz = (tempz - basez) / hypo;
    
    return (-1 * (((vecx * basex) + (vecy * basey) + (vecz * basez)) / ((dirvecx * vecx) + (dirvecy * vecy) + (dirvecz * vecz))));

}

void* projection_thread(void* arg){
    projection_stuff* newarg = static_cast<projection_stuff*>(arg);
    shaped_objects* thestart = new shaped_objects;
    shaped_objects* theend = thestart;
    
    //find relevent observerpoints
    double xvec = newarg -> xpos - newarg -> x_base;
    double yvec = newarg -> ypos - newarg -> y_base;
    double zvec = newarg -> zpos - newarg -> z_base;

    double search_vec_hypo = sqrt((xvec * xvec) + (yvec * yvec) + (zvec * zvec)); //don't forget squrt
    double search_xvec = (xvec) / search_vec_hypo;
    double search_yvec = (yvec) / search_vec_hypo;
    double search_zvec = (zvec) / search_vec_hypo;

   /*
    double renderx = (search_xvec * (newarg -> render_distance_multipliar));
    double rendery = (search_yvec * (newarg -> render_distance_multipliar));
    double renderz = (search_zvec * (newarg -> render_distance_multipliar));
    double render_hypo = sqrt((renderx * renderx) + (rendery * rendery) + (renderz * renderz));
    //cout << render_hypo;
    int render_hypo2 = static_cast<int>(round(render_hypo));
*/ 

    newarg -> ydirvec = 1;
    newarg -> xdirvec = ((-1 * (yvec * (newarg -> ydirvec))) / (xvec + zvec));
    newarg -> zdirvec = ((-1 * (yvec * (newarg -> ydirvec))) / (xvec + zvec));

    double dirvec = sqrt(((newarg -> ydirvec) * (newarg -> ydirvec)) + ((newarg -> xdirvec) * (newarg -> xdirvec)) + ((newarg -> zdirvec) * (newarg -> zdirvec)));
    newarg -> ydirvec = (newarg -> ydirvec) / dirvec;
    newarg -> xdirvec = (newarg -> xdirvec) / dirvec;
    newarg -> zdirvec = (newarg -> zdirvec) / dirvec;


    double max_up_posx = ((newarg -> xpos) + (newarg -> xdirvec * newarg -> window_height));
    double max_up_posy = ((newarg -> ypos) + (newarg -> ydirvec * newarg -> window_height));
    double max_up_posz = ((newarg -> zpos) + (newarg -> zdirvec * newarg -> window_height));
    double max_up_orvecx = max_up_posx - (newarg -> x_base);
    double max_up_orvecy = max_up_posy - (newarg -> y_base);
    double max_up_orvecz = max_up_posz - (newarg -> z_base);
    double max_up_hypo = sqrt((max_up_orvecx * max_up_orvecx) + (max_up_orvecy * max_up_orvecy) + (max_up_orvecz * max_up_orvecz)); //fixed?
    max_up_orvecx = max_up_orvecx / max_up_hypo;
    max_up_orvecy = max_up_orvecy / max_up_hypo;
    max_up_orvecz = max_up_orvecz / max_up_hypo;
    

    double max_down_posx = (newarg -> xpos) - (newarg -> xdirvec * newarg -> window_height);
    double max_down_posy = (newarg -> ypos) - (newarg -> ydirvec * newarg -> window_height);
    double max_down_posz = (newarg -> zpos) - (newarg -> zdirvec * newarg -> window_height);
    double max_down_orvecx = max_down_posx - (newarg -> x_base);
    double max_down_orvecy = max_down_posy - (newarg -> y_base);
    double max_down_orvecz = max_down_posz - (newarg -> z_base);
    double max_down_hypo = sqrt((max_down_orvecx * max_down_orvecx) + (max_down_orvecy * max_down_orvecy) + (max_down_orvecz * max_down_orvecz));//fixed?
    max_down_orvecx = max_down_orvecx / max_down_hypo;
    max_down_orvecy = max_down_orvecy / max_down_hypo;
    max_down_orvecz = max_down_orvecz / max_down_hypo;

    // xdirvec  ydirvec  zdirvec
    // xvec     yvec     zvec

    double x2dirvec = (newarg -> ydirvec * zvec) - (newarg -> zdirvec * yvec);
    double y2dirvec = (newarg -> zdirvec * xvec) - (newarg -> xdirvec * zvec);
    double z2dirvec = (newarg -> xdirvec * yvec) - (newarg -> ydirvec * xvec);

    double h2dirvec = sqrt((x2dirvec * x2dirvec) + (y2dirvec * y2dirvec) + (z2dirvec * z2dirvec));
    x2dirvec = x2dirvec / h2dirvec;
    y2dirvec = y2dirvec / h2dirvec;
    z2dirvec = z2dirvec / h2dirvec;

    double max_right_posx = (newarg -> xpos) + (x2dirvec * newarg -> window_length);
    double max_right_posy = (newarg -> ypos) + (y2dirvec * newarg -> window_length);
    double max_right_posz = (newarg -> zpos) + (z2dirvec * newarg -> window_length);
    double max_right_orvecx = max_right_posx - (newarg -> x_base);
    double max_right_orvecy = max_right_posy - (newarg -> y_base);
    double max_right_orvecz = max_right_posz - (newarg -> z_base);
    double max_right_hypo = sqrt((max_right_orvecx * max_right_orvecx) + (max_right_orvecy * max_right_orvecy) + (max_right_orvecz * max_right_orvecz));//fixed?
    max_right_orvecx = max_right_orvecx / max_right_hypo;
    max_right_orvecy = max_right_orvecy / max_right_hypo;
    max_right_orvecz = max_right_orvecz / max_right_hypo;

//

    double max_left_posx = (newarg -> xpos) - (x2dirvec * newarg -> window_length);
    double max_left_posy = (newarg -> ypos) - (y2dirvec * newarg -> window_length);
    double max_left_posz = (newarg -> zpos) - (z2dirvec * newarg -> window_length);
    double max_left_orvecx = max_left_posx - (newarg -> x_base);
    double max_left_orvecy = max_left_posy - (newarg -> y_base);
    double max_left_orvecz = max_left_posz - (newarg -> z_base);
    double max_left_hypo = sqrt((max_left_orvecx * max_left_orvecx) + (max_left_orvecy * max_left_orvecy) + (max_left_orvecz * max_left_orvecz));//fixed?
    max_left_orvecx = max_left_orvecx / max_left_hypo;
    max_left_orvecy = max_left_orvecy / max_left_hypo;
    max_left_orvecz = max_left_orvecz / max_left_hypo;

    int xtemp_max_up;
    int ytemp_max_up;
    int ztemp_max_up;
    int xtemp_max_down;
    int ytemp_max_down;
    int ztemp_max_down;
    int xtemp_max_left;
    int ytemp_max_left;
    int ztemp_max_left;
    int xtemp_max_right;
    int ytemp_max_right;
    int ztemp_max_right;

    int i = 0;

    do{

        xtemp_max_up = pointfinder(i, max_up_posx, max_up_orvecx);
        ytemp_max_up = pointfinder(i, max_up_posy, max_up_orvecy);
        ztemp_max_up = pointfinder(i, max_up_posz, max_up_orvecz);
      //  cout << endl << "maxup" << endl << xtemp_max_up << endl << ytemp_max_up << endl << ztemp_max_up;

        xtemp_max_down = pointfinder(i, max_down_posx, max_down_orvecx);
        ytemp_max_down = pointfinder(i, max_down_posy, max_down_orvecy);
        ztemp_max_down = pointfinder(i, max_down_posz, max_down_orvecz);
     //   cout << endl << "maxdown" << endl << xtemp_max_down << endl << ytemp_max_down << endl << ztemp_max_down;

        xtemp_max_left = pointfinder(i, max_left_posx, max_left_orvecx);
        ytemp_max_left = pointfinder(i, max_left_posy, max_left_orvecy);
        ztemp_max_left = pointfinder(i, max_left_posz, max_left_orvecz);
     //   cout << endl << "maxleft" << endl << xtemp_max_left << endl << ytemp_max_left << endl << ztemp_max_left;

        xtemp_max_right = pointfinder(i, max_right_posx, max_right_orvecx);
        ytemp_max_right = pointfinder(i, max_right_posy, max_right_orvecy);
        ztemp_max_right = pointfinder(i, max_right_posz, max_right_orvecz);
     //   cout << endl << "maxright" << endl << xtemp_max_right << endl << ytemp_max_right << endl << ztemp_max_right << endl;
        

        
        double seekervec1_x = (xtemp_max_up - xtemp_max_down);
        double seekervec1_y = (ytemp_max_up - ytemp_max_down);
        double seekervec1_z = (ztemp_max_up - ztemp_max_down);
        double seekervec1_mag = sqrt((seekervec1_x * seekervec1_x) + (seekervec1_y * seekervec1_y) + (seekervec1_z * seekervec1_z));

        double seekervec2_x = (xtemp_max_right - xtemp_max_left);
        double seekervec2_y = (ytemp_max_right - ytemp_max_left);
        double seekervec2_z = (ztemp_max_right - ztemp_max_left);
        double seekervec2_mag = sqrt((seekervec2_x * seekervec2_x) + (seekervec2_y * seekervec2_y) + (seekervec2_z * seekervec2_z));

        
        double xstarterpoint = xtemp_max_left - (seekervec1_x / 2); 
        double ystarterpoint = ytemp_max_left - (seekervec1_y / 2);
        double zstarterpoint = ztemp_max_left - (seekervec1_z / 2);


        int seekervec2_mag_int = static_cast<int>(ceil(seekervec2_mag));
        double seekervec2_x_dir = seekervec2_x / seekervec2_mag;
        double seekervec2_y_dir = seekervec2_y / seekervec2_mag; 
        double seekervec2_z_dir = seekervec2_z / seekervec2_mag;

        int seekervec1_mag_int = static_cast<int>(ceil(seekervec1_mag));
        double seekervec1_x_dir = seekervec1_x / seekervec1_mag;
        double seekervec1_y_dir = seekervec1_y / seekervec1_mag; 
        double seekervec1_z_dir = seekervec1_z / seekervec1_mag;

        for(int j = 0; j < seekervec2_mag_int; j++){
            double seeker_temposx = (seekervec2_x_dir * j) + xstarterpoint;
            double seeker_temposy = (seekervec2_y_dir * j) + ystarterpoint;
            double seeker_temposz = (seekervec2_z_dir * j) + zstarterpoint;
            for(int td = 0; td < seekervec1_mag_int; td++){
                int seeker_temposx2 = static_cast<int>(ceil((seekervec1_x_dir * td) + seeker_temposx));
                int seeker_temposy2 = static_cast<int>(ceil((seekervec1_y_dir * td) + seeker_temposy));
                int seeker_temposz2 = static_cast<int>(ceil((seekervec1_z_dir * td) + seeker_temposz));
                //cout << "d";
                
                // there might be a need to handle mirroring by removing integer values
                observerpoint* checkthis = newarg -> space_info.newspace -> mapply[seeker_temposx2][seeker_temposy2][seeker_temposz2].next;
               // cout << "start " << seeker_temposx2 << " " << seeker_temposy2 << " " << seeker_temposz2 << " mag " << sqrt((seeker_temposx2 * seeker_temposx2) + (seeker_temposy2 * seeker_temposy2) + (seeker_temposz2 * seeker_temposz2)) << endl;
                while(checkthis != nullptr){
                    cout << "bleak";
                    theend -> pointed_at = checkthis;
                    if((checkthis -> next) != nullptr){
                        theend -> thenext = new shaped_objects;
                        theend = theend -> thenext;
                        cout << "Hermin";
                    }
        
                    
                    checkthis = checkthis -> next;
                }

            }
            //cout << endl;
        }
//cout << endl << endl;
    i++;

 }while(i < (newarg -> render_distance_multipliar));
 cout << "twain" << (thestart -> pointed_at -> xlocation);



shaped_objects* exthat;
exthat = thestart;
shaped_objects* objectsthatmustbemapped = new shaped_objects;
shaped_objects* objectsthatmustbemapped_slider = objectsthatmustbemapped;
shaped_objects* objectsthatmustbemapped_slider2;
//observerpoint* observerpoint_slider;
/**/
while(true){
    objectsthatmustbemapped_slider2 = objectsthatmustbemapped;

    //(exthat -> pointed_at -> shape_array[0])
    


    while((exthat -> pointed_at -> shape_array[0] != objectsthatmustbemapped_slider2 -> pointed_at)){
        if((objectsthatmustbemapped_slider2 -> thenext) != nullptr){
            objectsthatmustbemapped_slider2 = objectsthatmustbemapped_slider2 -> thenext;
        }
        else{

            objectsthatmustbemapped_slider2 -> thenext = new shaped_objects;
            objectsthatmustbemapped_slider2 -> thenext -> pointed_at = (exthat -> pointed_at -> shape_array[0]);
            cout << endl << "unique " << objectsthatmustbemapped_slider2 -> thenext -> pointed_at;
            break;
        }
        
        
    }

    if(exthat -> thenext != nullptr){
    exthat = exthat -> thenext;
    }
    else{
        break;
    }
}

objectsthatmustbemapped_slider = objectsthatmustbemapped_slider -> thenext;
//cout << endl << "unique6 " << objectsthatmustbemapped_slider -> thenext -> pointed_at;

#pragma pack(push, 1)


struct BMPFileHeader {
    uint16_t fileType{0x4D42};  
    uint32_t fileSize{30054};   // Size of the file (header + info + pixel data)
    uint16_t reserved1{0};      
    uint16_t reserved2{0};      
    uint32_t offsetData{54};    // Start position of pixel data (54 bytes after file & info headers)
};

struct BMPInfoHeader {
    uint32_t size{40};          // Size of this header (40 bytes)
    int32_t width{100};         // Width of the bitmap in pixels (100)
    int32_t height{100};        // Height of the bitmap in pixels (100)
    uint16_t planes{1};         
    uint16_t bitCount{24};      // Bits per pixel (24 for RGB)
    uint32_t compression{0};    
    uint32_t sizeImage{30000};  // Size of the pixel data (100 * 100 * 3 bytes for RGB)
    int32_t xPixelsPerMeter{0}; 
    int32_t yPixelsPerMeter{0}; 
    uint32_t colorsUsed{0};     
    uint32_t colorsImportant{0};
};
#pragma pack(pop)
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + infoHeader.sizeImage;
    string filename = "output.bmp";
    ofstream file(filename, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(BMPFileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(BMPInfoHeader));
    vector<uint8_t> pixelData(infoHeader.sizeImage, 255); // Initialize with white pixels
    
double x_projection_origin = max_left_posx - ((max_up_posx - max_down_posx) / 2);
double y_projection_origin = max_left_posy - ((max_up_posy - max_down_posy) / 2);
double z_projection_origin = max_left_posz - ((max_up_posz - max_down_posz) / 2);

double vertivecx = (max_up_posx - max_down_posx);
double vertivecy = (max_up_posy - max_down_posy);
double vertivecz = (max_up_posz - max_down_posz);

double horivecx = (max_right_posx - max_left_posx);
double horivecy = (max_right_posy - max_left_posy);
double horivecz = (max_right_posz - max_left_posz);

/*
exthat = thestart;
 while((exthat -> pointed_at) != nullptr){
     
     //finds location of projected point in 3d space
    double t_this = (((xvec * -1) * ((newarg -> x_base) - (newarg -> xpos))) + ((yvec * -1) * ((newarg -> y_base) - (newarg -> ypos))) + ((zvec * -1) * ((newarg -> z_base) - (newarg -> zpos)))) / ((xvec * (exthat -> pointed_at -> xlocation) - (newarg -> x_base)) + (yvec * (exthat -> pointed_at -> ylocation) - (newarg -> y_base)) + (zvec * (exthat -> pointed_at -> zlocation) - (newarg -> z_base)));
    //cout << endl << t_this << endl;
    double xmath_proj_point = (newarg -> x_base) + (t_this * ((exthat -> pointed_at -> xlocation) - (newarg -> x_base)));
    double ymath_proj_point = (newarg -> y_base) + (t_this * ((exthat -> pointed_at -> ylocation) - (newarg -> y_base)));
    double zmath_proj_point = (newarg -> z_base) + (t_this * ((exthat -> pointed_at -> zlocation) - (newarg -> z_base)));
    //cout << endl << "x " << xmath_proj_point << endl;
    //cout << "y " << ymath_proj_point << endl;
    //cout << "z " << zmath_proj_point << endl;

    //projection
    double projmultiply = ((((newarg -> xdirvec) * (xmath_proj_point - (x_projection_origin))) + ((newarg -> ydirvec) * (ymath_proj_point - (y_projection_origin))) + ((newarg -> zdirvec) * (zmath_proj_point - (z_projection_origin)))) / (((newarg -> xdirvec) * (newarg -> xdirvec)) + ((newarg -> ydirvec) * (newarg -> ydirvec)) + ((newarg -> zdirvec) * (newarg -> zdirvec))));
    double xupcom = projmultiply * (newarg -> xdirvec);
    double yupcom = projmultiply * (newarg -> ydirvec);
    double zupcom = projmultiply * (newarg -> zdirvec);
    double xsidecom = (xmath_proj_point - (x_projection_origin)) - xupcom;
    double ysidecom = (ymath_proj_point - (y_projection_origin)) - yupcom;
    double zsidecom = (zmath_proj_point - (z_projection_origin)) - zupcom;

    //2D-fication
    double upmag = sqrt((xupcom * xupcom) + (yupcom * yupcom) + (zupcom * zupcom));
    double sidemag = sqrt((xsidecom * xsidecom) + (ysidecom * ysidecom) + (zsidecom * zsidecom));
    upmag = static_cast<int>(100 * (upmag / sqrt(((max_up_posx - max_down_posx) * (max_up_posx - max_down_posx)) + ((max_up_posy - max_down_posy) * (max_up_posy - max_down_posy)) + ((max_up_posz - max_down_posz) * (max_up_posz - max_down_posz))))); // 100 means the amount of pixles
    sidemag = static_cast<int>(100 * (sidemag / sqrt(((max_right_posx - max_left_posx) * (max_right_posx - max_left_posx)) + ((max_right_posy - max_left_posy) * (max_right_posy - max_left_posy)) + ((max_right_posz - max_left_posz) * (max_right_posz - max_left_posz)))));
    
    (exthat -> pointed_at -> mag_up) = upmag;
    (exthat -> pointed_at -> mag_side) = sidemag;

    //cout << "upmag" << upmag;
    //cout << "sidemag" << sidemag;


    int pixelIndex = (upmag * 100 + sidemag) * 3;
        pixelData[pixelIndex] = 0;      // Red
        pixelData[pixelIndex + 1] = 0;  // Green
        pixelData[pixelIndex + 2] = 0;  // Blue



    if((exthat -> thenext) == nullptr){
        break;
    }
    else{
        exthat = (exthat -> thenext);
    }

 }
*/
cout << endl << endl << " " << exthat -> pointed_at -> shape_array_size;



//cout << endl << "unique7 " << objectsthatmustbemapped_slider -> thenext -> pointed_at;


exthat = thestart;         

        while(true){

            int i1 = 0; 
            //int iforward = 0;
            int iprev = 0;
            int canter = 0; 
            
            
            do{
            
                cout << "canter " << canter;
                if(canter == 2){
                cout << "iprev" << iprev << endl;














                if(isinmap((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]), (exthat)) || isinmap((objectsthatmustbemapped_slider -> pointed_at -> shape_array[iprev]), (exthat))){
                    
                    //find points on plane
                    double mult1 = findpointonplanemultipliar(((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> xlocation), ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> ylocation), ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> zlocation), xvec, yvec, zvec, (newarg -> x_base), (newarg -> y_base), (newarg -> z_base));
                    double tempx13 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> xlocation);
                    double tempy13 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> ylocation);
                    double tempz13 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> zlocation);
                    double hypo13 = sqrt(((tempx13 - (newarg -> x_base)) * (tempx13 - (newarg -> x_base))) + ((tempy13 - (newarg -> y_base)) * (tempy13 - (newarg -> y_base))) + ((tempz13 - (newarg -> z_base)) * (tempz13 - (newarg -> z_base))));
                    double dirvecx13 = (tempx13 - (newarg -> x_base)) / hypo13;
                    double dirvecy13 = (tempy13 - (newarg -> y_base)) / hypo13;
                    double dirvecz13 = (tempz13 - (newarg -> z_base)) / hypo13;
                    double xpointonplane1 = (newarg -> x_base) + (dirvecx13 * mult1);
                    double ypointonplane1 = (newarg -> y_base) + (dirvecy13 * mult1);
                    double zpointonplane1 = (newarg -> z_base) + (dirvecz13 * mult1);
                    

                    double mult2 = findpointonplanemultipliar(((objectsthatmustbemapped_slider -> pointed_at -> shape_array[iprev]) -> xlocation), ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[iprev]) -> ylocation), ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[iprev]) -> zlocation), xvec, yvec, zvec, (newarg -> x_base), (newarg -> y_base), (newarg -> z_base));
                    double tempx14 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[iprev]) -> xlocation);
                    double tempy14 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[iprev]) -> ylocation);
                    double tempz14 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[iprev]) -> zlocation);
                    double hypo14 = sqrt(((tempx14 - (newarg -> x_base)) * (tempx14 - (newarg -> x_base))) + ((tempy14 - (newarg -> y_base)) * (tempy14 - (newarg -> y_base))) + ((tempz14 - (newarg -> z_base)) * (tempz14 - (newarg -> z_base))));
                    double dirvecx14 = (tempx14 - (newarg -> x_base)) / hypo14;
                    double dirvecy14 = (tempy14 - (newarg -> y_base)) / hypo14;
                    double dirvecz14 = (tempz14 - (newarg -> z_base)) / hypo14;
                    double xpointonplane2 = (newarg -> x_base) + (dirvecx14 * mult2);
                    double ypointonplane2 = (newarg -> y_base) + (dirvecy14 * mult2);
                    double zpointonplane2 = (newarg -> z_base) + (dirvecz14 * mult2);
                    

                    double xveconplane = xpointonplane2 - xpointonplane1;
                    double yveconplane = ypointonplane2 - ypointonplane1;
                    double zveconplane = zpointonplane2 - zpointonplane1;

                    double dirveconplane = sqrt((xveconplane * xveconplane) + (yveconplane * yveconplane) + (zveconplane * zveconplane));
                    cout << "dirveconplane" << dirveconplane;
                    double dirxveconplane = xveconplane / dirveconplane;
                    double diryveconplane = yveconplane / dirveconplane;
                    double dirzveconplane = zveconplane / dirveconplane;
                                       
                    
                    //project along the supposed x and suppossed y axes
                    for(int tyu = 0; tyu < static_cast<int>(ceil(dirveconplane) * 100); tyu++){
                        double xupcom = (tyu * dirxveconplane) / 100 + xpointonplane1;
                        double yupcom = (tyu * diryveconplane) / 100 + ypointonplane1;
                        double zupcom = (tyu * dirzveconplane) / 100 + zpointonplane1;

                        double vertimagmult = ((((xupcom - x_projection_origin) * vertivecx) + ((yupcom - y_projection_origin) * vertivecy) + ((zupcom - z_projection_origin) * vertivecz)) / ((vertivecx * vertivecx) + (vertivecy * vertivecy) + (vertivecz * vertivecz)));
                        double vertimagx = vertimagmult * vertivecx;
                        double vertimagy = vertimagmult * vertivecy;
                        double vertimagz = vertimagmult * vertivecz;
                        double vertimag = sqrt((vertimagx * vertimagx) + (vertimagy * vertimagy) + (vertimagz * vertimagz));

                        double horimagmult = ((((xupcom - x_projection_origin) * horivecx) + ((yupcom - y_projection_origin) * horivecy) + ((zupcom - z_projection_origin) * horivecz)) / ((horivecx * horivecx) + (horivecy * horivecy) + (horivecz * horivecz)));
                        double horimagx = horimagmult * horivecx;
                        double horimagy = horimagmult * horivecy;
                        double horimagz = horimagmult * horivecz;
                        double horimag = sqrt((horimagx * horimagx) + (horimagy * horimagy) + (horimagz * horimagz));

                        int vertifinal = static_cast<int>(ceil(100 * (vertimag / sqrt((vertivecx * vertivecx) + (vertivecy * vertivecy) + (vertivecz * vertivecz)))));
                        int horifinal = static_cast<int>(ceil(100 * (horimag / sqrt((horivecx * horivecx) + (horivecy * horivecy) + (horivecz * horivecz)))));
                        //cout << endl << " " << vertimag << endl;
                       //cout << " " << horimag;
                        if(((vertifinal < 100) && (horifinal < 100))){
        
        int pixelIndex = (vertifinal * 100 + horifinal) * 3;
        pixelData[pixelIndex] = 0;      // Red
        pixelData[pixelIndex + 1] = 0;  // Green
        pixelData[pixelIndex + 2] = 0;  // Blue
                        
                        
                        }
                    }

                }
                    
                   


















                    canter = 0;
                    continue;
                }
                if(isinmap((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]), (exthat)) || isinmap((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1 + ((objectsthatmustbemapped_slider -> pointed_at -> line_length) - 1)]), (exthat))){
                    
                    //find points on plane
                    double mult1 = findpointonplanemultipliar(((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> xlocation), ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> ylocation), ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> zlocation), xvec, yvec, zvec, (newarg -> x_base), (newarg -> y_base), (newarg -> z_base));
                    double tempx13 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> xlocation);
                    double tempy13 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> ylocation);
                    double tempz13 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1]) -> zlocation);
                    double hypo13 = sqrt(((tempx13 - (newarg -> x_base)) * (tempx13 - (newarg -> x_base))) + ((tempy13 - (newarg -> y_base)) * (tempy13 - (newarg -> y_base))) + ((tempz13 - (newarg -> z_base)) * (tempz13 - (newarg -> z_base))));
                    double dirvecx13 = (tempx13 - (newarg -> x_base)) / hypo13;
                    double dirvecy13 = (tempy13 - (newarg -> y_base)) / hypo13;
                    double dirvecz13 = (tempz13 - (newarg -> z_base)) / hypo13;
                    double xpointonplane1 = (newarg -> x_base) + (dirvecx13 * mult1);
                    double ypointonplane1 = (newarg -> y_base) + (dirvecy13 * mult1);
                    double zpointonplane1 = (newarg -> z_base) + (dirvecz13 * mult1);
                    

                    double mult2 = findpointonplanemultipliar(((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1 + ((objectsthatmustbemapped_slider -> pointed_at -> line_length) - 1)]) -> xlocation), ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1 + ((objectsthatmustbemapped_slider -> pointed_at -> line_length) - 1)]) -> ylocation), ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1 + ((objectsthatmustbemapped_slider -> pointed_at -> line_length) - 1)]) -> zlocation), xvec, yvec, zvec, (newarg -> x_base), (newarg -> y_base), (newarg -> z_base));
                    double tempx14 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1 + ((objectsthatmustbemapped_slider -> pointed_at -> line_length) - 1)]) -> xlocation);
                    double tempy14 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1 + ((objectsthatmustbemapped_slider -> pointed_at -> line_length) - 1)]) -> ylocation);
                    double tempz14 = ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1 + ((objectsthatmustbemapped_slider -> pointed_at -> line_length) - 1)]) -> zlocation);
                    double hypo14 = sqrt(((tempx14 - (newarg -> x_base)) * (tempx14 - (newarg -> x_base))) + ((tempy14 - (newarg -> y_base)) * (tempy14 - (newarg -> y_base))) + ((tempz14 - (newarg -> z_base)) * (tempz14 - (newarg -> z_base))));
                    double dirvecx14 = (tempx14 - (newarg -> x_base)) / hypo14;
                    double dirvecy14 = (tempy14 - (newarg -> y_base)) / hypo14;
                    double dirvecz14 = (tempz14 - (newarg -> z_base)) / hypo14;
                    double xpointonplane2 = (newarg -> x_base) + (dirvecx14 * mult2);
                    double ypointonplane2 = (newarg -> y_base) + (dirvecy14 * mult2);
                    double zpointonplane2 = (newarg -> z_base) + (dirvecz14 * mult2);
                    

                    double xveconplane = xpointonplane2 - xpointonplane1;
                    double yveconplane = ypointonplane2 - ypointonplane1;
                    double zveconplane = zpointonplane2 - zpointonplane1;

                    double dirveconplane = sqrt((xveconplane * xveconplane) + (yveconplane * yveconplane) + (zveconplane * zveconplane));
                    cout << "dirveconplane" << dirveconplane;
                    double dirxveconplane = xveconplane / dirveconplane;
                    double diryveconplane = yveconplane / dirveconplane;
                    double dirzveconplane = zveconplane / dirveconplane;
                                       
                    
                    //project along the supposed x and suppossed y axes
                    for(int tyu = 0; tyu < static_cast<int>(ceil(dirveconplane) * 100); tyu++){
                        double xupcom = (tyu * dirxveconplane) / 100 + xpointonplane1;
                        double yupcom = (tyu * diryveconplane) / 100 + ypointonplane1;
                        double zupcom = (tyu * dirzveconplane) / 100 + zpointonplane1;

                        double vertimagmult = ((((xupcom - x_projection_origin) * vertivecx) + ((yupcom - y_projection_origin) * vertivecy) + ((zupcom - z_projection_origin) * vertivecz)) / ((vertivecx * vertivecx) + (vertivecy * vertivecy) + (vertivecz * vertivecz)));
                        double vertimagx = vertimagmult * vertivecx;
                        double vertimagy = vertimagmult * vertivecy;
                        double vertimagz = vertimagmult * vertivecz;
                        double vertimag = sqrt((vertimagx * vertimagx) + (vertimagy * vertimagy) + (vertimagz * vertimagz));

                        double horimagmult = ((((xupcom - x_projection_origin) * horivecx) + ((yupcom - y_projection_origin) * horivecy) + ((zupcom - z_projection_origin) * horivecz)) / ((horivecx * horivecx) + (horivecy * horivecy) + (horivecz * horivecz)));
                        double horimagx = horimagmult * horivecx;
                        double horimagy = horimagmult * horivecy;
                        double horimagz = horimagmult * horivecz;
                        double horimag = sqrt((horimagx * horimagx) + (horimagy * horimagy) + (horimagz * horimagz));

                        int vertifinal = static_cast<int>(ceil(100 * (vertimag / sqrt((vertivecx * vertivecx) + (vertivecy * vertivecy) + (vertivecz * vertivecz)))));
                        int horifinal = static_cast<int>(ceil(100 * (horimag / sqrt((horivecx * horivecx) + (horivecy * horivecy) + (horivecz * horivecz)))));
                        //cout << endl << " " << vertimag << endl;
                       //cout << " " << horimag;
                        if(((vertifinal < 100) && (horifinal < 100))){
        
        int pixelIndex = (vertifinal * 100 + horifinal) * 3;
        pixelData[pixelIndex] = 0;      // Red
        pixelData[pixelIndex + 1] = 0;  // Green
        pixelData[pixelIndex + 2] = 0;  // Blue
                        
                        
                        }
                    }

                }
                cout << endl << "i1" << " " << i1 << endl;
                // ?minus 1 because line length is the number that was put into the shape array?
                //iprev = i1;

                if(canter == 0){
                iprev = i1;
                }
                
                i1 = i1 + ((objectsthatmustbemapped_slider -> pointed_at -> shape_array[i1] -> line_length));
                canter = canter + 1; 
            }while(i1 < (objectsthatmustbemapped_slider -> pointed_at -> shape_array_size));





            if((objectsthatmustbemapped_slider -> thenext) == nullptr){
            break;
            }
            else{
            objectsthatmustbemapped_slider = (objectsthatmustbemapped_slider -> thenext);
            }
        }








 file.write(reinterpret_cast<char*>(pixelData.data()), pixelData.size());
 file.close();

return NULL;
}



int main() {

    //space1
    //The space is always a rectangular prism. This means that the observerpoints scale linearly. The limit on github codespaces is around 40,000,000!
    //If this becomes a design problem later on, this is fixable by making smaller spaces and loading them around the viewpoint as needed. 

    int mapx = 100;
    int mapy = 100;
    int mapz = 100;
    cout << mapx * mapy * mapz << " observerpoints" << endl;
    mapobject* space1 = new mapobject;
    seedingstuff space1_seeder;
    space1_seeder.xspace = mapx;
    space1_seeder.yspace = mapy;
    space1_seeder.zspace = mapz;
    space1 -> mapper(mapx, mapy, mapz);
    space1_seeder.newspace = space1;
    

    //lines
    pthread_t thread;
    line_stuff line1;
    line1.space_info = space1_seeder;
    line1.xlocation_inspace = 32;
    line1.ylocation_inspace = 32;
    line1.zlocation_inspace = 54;
    line1.xvec = 5.1;
    line1.yvec = 1;
    line1.zvec = 7;

    pthread_t thread2;
    line_stuff line2;
    line2.space_info = space1_seeder;
    line2.xlocation_inspace = 33;
    line2.ylocation_inspace = 40;
    line2.zlocation_inspace = 32;
    line2.xvec = -2;
    line2.yvec = 6;
    line2.zvec = 8;

    pthread_t thread5;
    planar_stuff plane2;
    plane2.space_info = space1_seeder;
     plane2.xpoint1 = 32;
     plane2.ypoint1 = 32;
     plane2.zpoint1 = 38;

     plane2.xpoint2 = 38;
     plane2.ypoint2 = 32;
     plane2.zpoint2 = 38;

     plane2.xpoint3 = 35;
     plane2.ypoint3 = 29;
     plane2.zpoint3 = 35;

     plane2.additionalplane = new planar_stuff;     
     plane2.additionalplane -> space_info = space1_seeder;
     plane2.additionalplane -> xpoint1 = 35;
     plane2.additionalplane -> ypoint1 = 35;
     plane2.additionalplane -> zpoint1 = 35;

     plane2.additionalplane -> xpoint2 = 32;
     plane2.additionalplane -> ypoint2 = 32;
     plane2.additionalplane -> zpoint2 = 32;

     plane2.additionalplane -> xpoint3 = 38;
     plane2.additionalplane -> ypoint3 = 32;
     plane2.additionalplane -> zpoint3 = 32;

    pthread_t thread3;
    planar_stuff plane1;
    plane1.space_info = space1_seeder;
     plane1.xpoint1 = 32;
     plane1.ypoint1 = 32;
     plane1.zpoint1 = 38;

     plane1.xpoint2 = 40;
     plane1.ypoint2 = 40;
     plane1.zpoint2 = 40;

     plane1.xpoint3 = 20;
     plane1.ypoint3 = 23;
     plane1.zpoint3 = 20;


  
     plane1.additionalplane = new planar_stuff;     
     plane1.additionalplane -> space_info = space1_seeder;
     plane1.additionalplane -> xpoint1 = 40;
     plane1.additionalplane -> ypoint1 = 40;
     plane1.additionalplane -> zpoint1 = 35;

     plane1.additionalplane -> xpoint2 = 32;
     plane1.additionalplane -> ypoint2 = 32;
     plane1.additionalplane -> zpoint2 = 32;

     plane1.additionalplane -> xpoint3 = 38;
     plane1.additionalplane -> ypoint3 = 32;
     plane1.additionalplane -> zpoint3 = 32;


    //projection thread stuff
    pthread_t thread4;
    projection_stuff viewpoint1;
    viewpoint1.space_info = space1_seeder;
    viewpoint1.render_distance_multipliar = 20;

    viewpoint1.xpos = 28;
    viewpoint1.ypos = 28;
    viewpoint1.zpos = 28;

    viewpoint1.x_base = 23;
    viewpoint1.y_base = 23;
    viewpoint1.z_base = 23;
    viewpoint1.window_length = 7;
    viewpoint1.window_height = 7;




    
    //pthread_create(&thread, NULL, liner, (void*)&line1);
    //pthread_create(&thread2, NULL, liner, (void*)&line2);
    pthread_create(&thread3, NULL, planar, (void*)&plane1);
    //pthread_create(&thread5, NULL, planar, (void*)&plane2);
    
    //pthread_join(thread, NULL); 
    //pthread_join(thread2, NULL);
    pthread_join(thread3, NULL); 
    //pthread_join(thread5, NULL);

    pthread_create(&thread4, NULL, projection_thread, (void*)&viewpoint1);
    pthread_join(thread4, NULL);  

//projection_thread(&viewpoint1);

    return 0;
}


















