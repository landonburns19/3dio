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


//always assume 1x3 in x y z order
//does not find direction vector
double* orthogonal_vec_finder(double* vec1, double* vec2){

    double* vec_orth = new double[3];
    vec_orth[0] = (vec1[1] * vec2[2]) - (vec1[2] * vec2[1]);
    vec_orth[1] = (vec1[0] * vec2[2]) - (vec1[2] * vec2[0]);
    vec_orth[2] = (vec1[0] * vec2[1]) - (vec1[1] * vec2[0]);

    //for(int i = 0 ; i < 3; i++){
    //    cout << endl << "vec_orth[]" << i << " " << vec_orth[i];
   // }


    return vec_orth;
};


double* finddirvecfromtwopoints(double* pointf, double* pointi){

    double* dirvec = new double[3];
    for(int i1 = 0; i1 < 3; i1++){
        dirvec[i1] = pointf[i1] - pointi[i1];
    }

    double hypo = sqrt((dirvec[0] * dirvec[0]) + (dirvec[1] * dirvec[1]) + (dirvec[2] * dirvec[2]));
    for(int i2 = 0; i2 < 3; i2++){
        dirvec[i2] = dirvec[i2] / hypo;
    }


    return dirvec;
}

class linkedlist_of_uniqueobjects_and_their_points{
    public:
    linkedlist_of_uniqueobjects_and_their_points* nextobject; //if objectarray pointer does NOT match the shape_array pointer go to nextobject
    linkedlist_of_uniqueobjects_and_their_points* nextobserverpoint; //if objectarray pointer DOES match shape_array pointer but foundpoint is filled go to nextobserverpoint
    observerpoint** objectarray;
    observerpoint* foundpoint; // if empty fill 

};










//amount_to_add to is renderdistance divided by some double(representing the desired search detail)
void recursivefinder(double* top_point, double* bottom_point, double* left_point, double* right_point, double* top_dir, double* bottom_dir, double* left_dir, double* right_dir, double rendermulti, double amount_to_add, double counterfinder, projection_stuff* thisarg, linkedlist_of_uniqueobjects_and_their_points* start_at_there){

    
    double* starting_point = new double[3];
    for(int s1 = 0; s1 < 3; s1++){
        starting_point[s1] = left_point[s1] - ((top_point[s1] - bottom_point[s1]) / 2);
    }
    
    


    
    double from_down_to_up_vec = sqrt(((top_point[0] - bottom_point[0]) * (top_point[0] - bottom_point[0])) + ((top_point[1] - bottom_point[1]) * (top_point[1] - bottom_point[1])) + ((top_point[2] - bottom_point[2]) * (top_point[2] - bottom_point[2])));
    double* detailed_updown_vec = new double[3];
    for(int s8 = 0; s8 < 3; s8++){
        detailed_updown_vec[s8] = (top_point[s8] - bottom_point[s8]) / from_down_to_up_vec;
    }

    double from_left_to_right_vec = sqrt(((right_point[0] - left_point[0]) * (right_point[0] - left_point[0])) + ((right_point[1] - left_point[1]) * (right_point[1] - left_point[1])) + ((right_point[2] - left_point[2]) * (right_point[2] - left_point[2])));
    double* detailed_rightleft_vec = new double[3];
    for(int s9 = 0; s9 < 3; s9++){
        detailed_rightleft_vec[s9] = (right_point[s9] - left_point[s9]) / from_left_to_right_vec;
    }
    
    //cout << "from_left_to_right_vec" << from_left_to_right_vec << endl;
    //cout << "from_down_to_up_vec" << from_down_to_up_vec << " "; 
    int s2 = 0;
    double* sliding_point = new double[3];
    sliding_point[0] = starting_point[0];
    sliding_point[1] = starting_point[1];
    sliding_point[2] = starting_point[2];
    double* sliding_point2 = new double[3];
    sliding_point2[0] = starting_point[0];
    sliding_point2[1] = starting_point[1];
    sliding_point2[2] = starting_point[2];

    


        while((amount_to_add * s2) <= ceil(from_down_to_up_vec)){
            int s7 = 0;
            while((amount_to_add * s7) <= ceil(from_left_to_right_vec)){
                
                if(((static_cast<int>(floor(sliding_point2[0]))) > 99) || ((static_cast<int>(floor(sliding_point2[1]))) > 99) || ((static_cast<int>(floor(sliding_point2[2]))) > 99)){ 
                    break;
                    }
                observerpoint& searchthis = thisarg -> space_info.newspace -> mapply[static_cast<int>(floor(sliding_point2[0]))][static_cast<int>(floor(sliding_point2[1]))][static_cast<int>(floor(sliding_point2[2]))];
                //cout << " " << searchthis.xlocation << " " << searchthis.ylocation << " " << searchthis.zlocation << endl;
                
                observerpoint* thenextone = searchthis.next;
                while(thenextone != nullptr){
                    linkedlist_of_uniqueobjects_and_their_points* slider_at_there = start_at_there;
                    while(slider_at_there != nullptr){
                        
                        if(((slider_at_there -> nextobject) == nullptr) && ((slider_at_there -> nextobserverpoint) == nullptr)){
                            (slider_at_there -> objectarray) = (thenextone -> shape_array);
                            (slider_at_there -> foundpoint) = thenextone;
                            (slider_at_there -> nextobject) = new linkedlist_of_uniqueobjects_and_their_points;
                            (slider_at_there -> nextobserverpoint) = new linkedlist_of_uniqueobjects_and_their_points;

                            cout << "saved a new one     ";
                            slider_at_there = nullptr;
                        }
                        else if(((slider_at_there -> objectarray) != (thenextone -> shape_array)) && ((slider_at_there -> nextobject) != nullptr)){
                            slider_at_there = slider_at_there -> nextobject;
                            cout << "moved to next object    ";
                        }
                        else if(((slider_at_there -> objectarray) == (thenextone -> shape_array)) && (slider_at_there -> nextobserverpoint != nullptr)){
                            if((slider_at_there -> nextobserverpoint -> foundpoint) == thenextone){ break;}
                            slider_at_there = slider_at_there -> nextobserverpoint;
                            cout << "moved to next point    ";
                        }
                        
                    }


                    thenextone = thenextone -> next;
                }

                
                
                s7 = s7 + 1;
                sliding_point2[0] = sliding_point[0]  + ((amount_to_add * s7) * detailed_rightleft_vec[0]);
                sliding_point2[1] = sliding_point[1]  + ((amount_to_add * s7) * detailed_rightleft_vec[1]);
                sliding_point2[2] = sliding_point[2]  + ((amount_to_add * s7) * detailed_rightleft_vec[2]);
                //cout << " " << sqrt((((amount_to_add * s7) * detailed_rightleft_vec[0]) * ((amount_to_add * s7) * detailed_rightleft_vec[0])) + (((amount_to_add * s7) * detailed_rightleft_vec[1]) * ((amount_to_add * s7) * detailed_rightleft_vec[1])) + (((amount_to_add * s7) * detailed_rightleft_vec[2]) * ((amount_to_add * s7) * detailed_rightleft_vec[2])));
            }


            //cout << endl;
            s2 = s2 + 1;
            sliding_point[0] = starting_point[0] + ((amount_to_add * s2) * detailed_updown_vec[0]);
            sliding_point[1] = starting_point[1] + ((amount_to_add * s2) * detailed_updown_vec[1]);
            sliding_point[2] = starting_point[2] + ((amount_to_add * s2) * detailed_updown_vec[2]);
            //cout << sqrt((((amount_to_add * s2) * detailed_updown_vec[0]) * ((amount_to_add * s2) * detailed_updown_vec[0])) + (((amount_to_add * s2) * detailed_updown_vec[1]) * ((amount_to_add * s2) * detailed_updown_vec[1])) + (((amount_to_add * s2) * detailed_updown_vec[2]) * ((amount_to_add * s2) * detailed_updown_vec[2])));
        }
        


    counterfinder = counterfinder + 1;

    double* top_point2 = new double[3];
    for(int s3 = 0; s3 < 3; s3++){
        top_point2[s3] = (top_dir[s3] * amount_to_add * counterfinder) + top_point[s3];
    }
    double* bottom_point2 = new double[3];
    for(int s4 = 0; s4 < 3; s4++){
        bottom_point2[s4]= (bottom_dir[s4] * amount_to_add * counterfinder) + bottom_point[s4];
    }
    double* left_point2 = new double[3];
    for(int s5 = 0; s5 < 3; s5++){
        left_point2[s5] = (left_dir[s5] * amount_to_add * counterfinder) + left_point[s5];
    }
    double* right_point2 = new double[3];
    for(int s6 = 0; s6 < 3; s6++){
        right_point2[s6] = (right_dir[s6] * amount_to_add * counterfinder) + right_point[s6];
    }
    
    if((amount_to_add * counterfinder) < rendermulti){
       // cout << endl;
    recursivefinder(top_point2, bottom_point2, left_point2, right_point2, top_dir, bottom_dir, left_dir, right_dir, rendermulti, (amount_to_add * pow(.999, counterfinder)), counterfinder, thisarg, start_at_there);
    }

    return;
}









 void binarysearch(linkedlist_of_uniqueobjects_and_their_points* starthere2){
    
    if(((starthere2 -> nextobject) == nullptr) && ((starthere2 -> nextobserverpoint) == nullptr)){
        cout << "thismany";
         return;
    }
    else{
        cout << (starthere2 -> objectarray) << " " << (starthere2 -> foundpoint) << endl;
        binarysearch(starthere2 -> nextobject);
        binarysearch(starthere2 -> nextobserverpoint);
    }
 }




void* projection_thread(void* arg){
    projection_stuff* newarg = static_cast<projection_stuff*>(arg);

    //plane vector
    double xvec = newarg -> xpos - newarg -> x_base;
    double yvec = newarg -> ypos - newarg -> y_base;
    double zvec = newarg -> zpos - newarg -> z_base;
    //plane unit vector
    double vec_hypo = sqrt((xvec * xvec) + (yvec * yvec) + (zvec * zvec));
    double* vec_unit = new double[3];
        vec_unit[0] = xvec / vec_hypo;
        vec_unit[1] = yvec / vec_hypo;
        vec_unit[2] = zvec / vec_hypo;
    

    //most_up_vector
    double ymost_up_vector = 1;
    double xmost_up_vector = ((-1 * (yvec * (ymost_up_vector))) / (xvec + zvec));
    double zmost_up_vector = ((-1 * (yvec * (ymost_up_vector))) / (xvec + zvec));
    //most_up unit vector
    double most_up_hypo = sqrt(((ymost_up_vector) * (ymost_up_vector)) + ((xmost_up_vector) * (xmost_up_vector)) + ((zmost_up_vector) * (zmost_up_vector)));
        double* most_up_dir = new double[3];
        most_up_dir[0] = (xmost_up_vector) / most_up_hypo;
        most_up_dir[1] = (ymost_up_vector) / most_up_hypo;
        most_up_dir[2] = (zmost_up_vector) / most_up_hypo;
        //cout << "most_up_dir[0]" << most_up_dir[0];
        //cout << "most_up_dir[1]" << most_up_dir[1];
        //cout << "most_up_dir[2]" << most_up_dir[2];
    
    //side direction vector
    double* side_dir_vec = orthogonal_vec_finder(most_up_dir, vec_unit);
    


    

    //setting up height and length
    double* top_vec = new double[3];
    for(int j1 = 0; j1 < 3; j1++){
        top_vec[j1] = most_up_dir[j1] * (newarg -> window_height);
        //cout << endl << "top_vec[j1]" << j1 << " " << top_vec[j1];
    }
    double* side_vec = new double[3];
    for(int j2 = 0; j2 < 3; j2++){
        side_vec[j2] = side_dir_vec[j2] * (newarg -> window_length);
        //cout << endl << "side_vec[j2]" << j2 << " " << side_vec[j2];
    }
    
    




    //loading point on plain and base point
    double* point_on_plane = new double[3];
    point_on_plane[0] = newarg -> xpos;
    point_on_plane[1] = newarg -> ypos;
    point_on_plane[2] = newarg -> zpos;
    double* base_point = new double[3];
    base_point[0] = newarg -> x_base;
    base_point[1] = newarg -> y_base;
    base_point[2] = newarg -> z_base;


    //setting up top bottom left right points and direction vectors
    double* top_point = new double[3];
    for(int j3 = 0; j3 < 3; j3++){
        top_point[j3] = point_on_plane[j3] + top_vec[j3];
        //cout << endl << "top_point[j3]" << j3 << " " << top_point[j3];

    }
    double* bottom_point = new double[3];
    for(int j5 = 0; j5 < 3; j5++){
        bottom_point[j5] = point_on_plane[j5] - top_vec[j5];
        //cout << endl << "bottom_point[j5]" << j5 << " " << bottom_point[j5];
    }
    double* left_point = new double[3];
    for(int j6 = 0; j6 < 3; j6++){
        left_point[j6] = point_on_plane[j6] - side_vec[j6];
        //cout << endl << "left_point[j6]" << j6 << " " << left_point[j6];
    }
    //cout << endl;
    double* right_point = new double[3];
    for(int j7 = 0; j7 < 3; j7++){
        right_point[j7] = point_on_plane[j7] + side_vec[j7];
        //cout << "right_point[j7]" << j7 << " " << right_point[j7] << endl;
    }

    double* top_dir = new double[3];
    top_dir = finddirvecfromtwopoints(top_point, base_point);
    double* bottom_dir = new double[3];
    bottom_dir = finddirvecfromtwopoints(bottom_point, base_point);
    double* left_dir = new double[3];
    left_dir = finddirvecfromtwopoints(left_point, base_point);
    double* right_dir = new double[3];
    right_dir = finddirvecfromtwopoints(right_point, base_point);


    //This searchdetail controls the how hard the system searches for grids. Its here because I haven't found out how solve this mathmatically yet. (a lower value  means more searching)
    double searchdetail = (newarg -> render_distance_multipliar);
    double amountadd = searchdetail / (newarg -> render_distance_multipliar);
    int counterfinder0 = 0;
    linkedlist_of_uniqueobjects_and_their_points* starthere = new linkedlist_of_uniqueobjects_and_their_points;
    recursivefinder(top_point, bottom_point, left_point, right_point, top_dir, bottom_dir, left_dir, right_dir, (newarg -> render_distance_multipliar), amountadd, counterfinder0, newarg, starthere);
    binarysearch(starthere);
    
    

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
     plane2.xpoint1 = 35;
     plane2.ypoint1 = 35;
     plane2.zpoint1 = 35;

     plane2.xpoint2 = 40;
     plane2.ypoint2 = 30;
     plane2.zpoint2 = 40;

     plane2.xpoint3 = 35;
     plane2.ypoint3 = 30;
     plane2.zpoint3 = 35;

     plane2.additionalplane = new planar_stuff;     
     plane2.additionalplane -> space_info = space1_seeder;
     plane2.additionalplane -> xpoint1 = 60;
     plane2.additionalplane -> ypoint1 = 38;
     plane2.additionalplane -> zpoint1 = 55;

     plane2.additionalplane -> xpoint2 = 32;
     plane2.additionalplane -> ypoint2 = 32;
     plane2.additionalplane -> zpoint2 = 32;

     plane2.additionalplane -> xpoint3 = 38;
     plane2.additionalplane -> ypoint3 = 32;
     plane2.additionalplane -> zpoint3 = 32;

    pthread_t thread3;
    planar_stuff plane1;
    plane1.space_info = space1_seeder;
     plane1.xpoint1 = 30;
     plane1.ypoint1 = 35;
     plane1.zpoint1 = 35;

     plane1.xpoint2 = 40;
     plane1.ypoint2 = 30;
     plane1.zpoint2 = 40;

     plane1.xpoint3 = 35;
     plane1.ypoint3 = 30;
     plane1.zpoint3 = 35;


  
     plane1.additionalplane = new planar_stuff;     
     plane1.additionalplane -> space_info = space1_seeder;
     plane1.additionalplane -> xpoint1 = 60;
     plane1.additionalplane -> ypoint1 = 38;
     plane1.additionalplane -> zpoint1 = 55;

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
    viewpoint1.render_distance_multipliar = 10;

    viewpoint1.xpos = 50 - 11;
    viewpoint1.ypos = 28 - 11;
    viewpoint1.zpos = 45 - 11;

    viewpoint1.x_base = 21 - 11;
    viewpoint1.y_base = 15 - 11;
    viewpoint1.z_base = 27 - 11;
    viewpoint1.window_length = 10;
    viewpoint1.window_height = 10;




    
    pthread_create(&thread, NULL, liner, (void*)&line1);
    pthread_create(&thread2, NULL, liner, (void*)&line2);
    pthread_create(&thread3, NULL, planar, (void*)&plane1);
    pthread_create(&thread5, NULL, planar, (void*)&plane2);
    
    pthread_join(thread, NULL); 
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL); 
    pthread_join(thread5, NULL);

    pthread_create(&thread4, NULL, projection_thread, (void*)&viewpoint1);
    pthread_join(thread4, NULL);  

//projection_thread(&viewpoint1);

    return 0;
}


















