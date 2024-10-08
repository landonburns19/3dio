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


void addtoshapestack(observerpoint* addingtostack[]){
    thisone.lock();
    allshapes.push(addingtostack);
    thisone.unlock();
    
}

//This converts a shape into text information by cycling through the shape's array and referencing the location of its observerpoints.
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

//This is a simple create bmpfile for projecting a single shape from a single angle. It will be expanded to show multiple shapes from any angle and position.
void create_bmp_file(observerpoint* addingtoinitialize[]){


    double object_coordinatesx[((addingtoinitialize[0]) -> shape_array_size)];
    double object_coordinatesy[((addingtoinitialize[0]) -> shape_array_size)];
    double object_coordinatesz[((addingtoinitialize[0]) -> shape_array_size)];

    double mg1x = (((addingtoinitialize[0]) -> xlocation));
    double mg1y = (((addingtoinitialize[0]) -> ylocation));
    double mg1z = (((addingtoinitialize[0]) -> zlocation));
    
    //slider
    double mg3x = (((addingtoinitialize[0]) -> xlocation))/2;
    double mg3y = (((addingtoinitialize[0]) -> ylocation))/2;
    double mg3z = (((addingtoinitialize[0]) -> zlocation))/2;

    object_coordinatesx[0] = mg3x;
    object_coordinatesy[0] = mg3y;
    object_coordinatesz[0] = mg3z;

    for( int iy = 1; iy < (addingtoinitialize[0]) -> shape_array_size; ++iy){
            

            double mg2x = (((addingtoinitialize[iy]) -> xlocation));
            double mg2y = (((addingtoinitialize[iy]) -> ylocation));
            double mg2z = (((addingtoinitialize[iy]) -> zlocation));

            double thisconstant = (mg3x * mg3x) + (mg3y * mg3y) + (mg3z * mg3z);
            double koef = thisconstant / ((mg3x * mg2x) + (mg3y * mg2y) + (mg3z * mg2z));
            
              
            object_coordinatesx[iy] = koef * mg2x;
            object_coordinatesy[iy] = koef * mg2y;
            object_coordinatesz[iy] = koef * mg2z;
            


        } 

    double axis1x = object_coordinatesx[1] - object_coordinatesx[0];
    double axis1y = object_coordinatesy[1] - object_coordinatesy[0];
    double axis1z = object_coordinatesz[1] - object_coordinatesz[0];

//Matrix
// ix      jy       kz
// mg3x    mg3y    mg3z
// axis1x axis1y axis1z


    double ix = (mg3y * axis1z) - (mg3z * axis1y);
    double jy = (mg3x * axis1z) - (mg3z * axis1x);
    double kz = (mg3x * axis1y) - (mg3y * axis1x);

    double axis1multix = sqrt((axis1x * axis1x) + (axis1y * axis1y) + (axis1z * axis1z));
    double axis2multiy = sqrt((ix * ix) + (jy * jy) + (kz * kz));

    double finalx[((addingtoinitialize[0]) -> shape_array_size)];
    double finaly[((addingtoinitialize[0]) -> shape_array_size)];

    finalx[0] = 0;
    finaly[0] = 0;

    for( int ic = 1; ic < (addingtoinitialize[0]) -> shape_array_size; ++ic){

        double icx = object_coordinatesx[ic] - object_coordinatesx[0];
        double icy = object_coordinatesy[ic] - object_coordinatesy[0];
        double icz = object_coordinatesz[ic] - object_coordinatesz[0];

        finalx[ic] = ((axis1x * icx) + (axis1y * icy) + (axis1z * icz)) / axis1multix;
        finaly[ic] = ((ix * icx) + (jy * icy) + (kz * icz)) / axis2multiy;
        cout << "x" << ic << " " << finalx[ic];
        cout << " ";
        cout << "y" << ic << " " << finaly[ic] << endl;

    }
        
        double least_x = 0;
        double least_y = 0;
        double most_x = 0;
        double most_y = 0;
        double most_x_pos = 0;
        double most_y_pos = 0;
        double the_mostest;
        int pixrad = 100;

        for(int ib = 0; ib < (addingtoinitialize[0]) -> shape_array_size; ++ib){
            if(finalx[ib] < least_x){
                least_x = finalx[ib];
                //cout << finalx[ib];
                //cout << least_x;
            }
            if(finalx[ib] > most_x){
                most_x = finalx[ib];
                most_x_pos = ib;
            }
        }
        least_x = abs(least_x);
        most_x = most_x + least_x;

        for( int ia = 0; ia < (addingtoinitialize[0]) -> shape_array_size; ++ia){
            if(finaly[ia] < least_y){
                least_y = finaly[ia];
            }
            if(finaly[ia] > most_y){
                most_y = finaly[ia];
                most_y_pos = ia;
            }
        }

        
        least_y = abs(least_y);
        most_y = most_y + least_y;

        if(most_y > most_x){
            the_mostest = most_y;
        }
        else if(most_x > most_y){
            the_mostest = most_x;
        }
        //algebra stuff
        cout << "the_mostest " << the_mostest << endl;
        cout << "leastx" << least_x << endl;
        cout << "leasty" << least_y << endl;
        cout << "mostx " << most_x << endl;
        cout << "mosty " << most_y << endl;
        cout << "most_x_pos " << most_x_pos << endl;
        cout << "most_y_pos " << most_y_pos << endl;

        for( int ig = 0; ig < (addingtoinitialize[0]) -> shape_array_size; ++ig){
            finalx[ig] = finalx[ig] + least_x;
            finaly[ig] = finaly[ig] + least_y;
            cout << "xfinaltranslated" << finalx[ig] << endl;
            cout << "yfinaltranslated" << finaly[ig] << endl;
        }

        for( int ig = 0; ig < (addingtoinitialize[0]) -> shape_array_size; ++ig){

            cout << finalx[ig] << " ";
            cout << finaly[ig] << endl;
        }

        for( int im = 0; im < (addingtoinitialize[0]) -> shape_array_size; ++im){

            finalx[im] = ceil(100 * (finalx[im] / the_mostest)); 
            finaly[im] = ceil(100 * (finaly[im] / the_mostest));
            //this 99 rule will only work for this object code. When I have multiple objects, I will need to exclude points.
            if(finalx[im] > 99){
                finalx[im] = 99;
            }

            if(finaly[im] > 99){
                finaly[im] = 99;
            }

            cout << " x" << finalx[im];
            cout << " y" << finaly[im] << endl;
        }
            
//be aware that the divider MIGHT be producing in a range of 101 possible numbers BUT its probably not


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
    if (!file) {
        cerr << "Error creating file!" << endl;
        return;
    }

    
    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(BMPFileHeader));

    
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(BMPInfoHeader));

 
    vector<uint8_t> pixelData(infoHeader.sizeImage, 255); // Initialize with white pixels

    for( int ik = 0; ik < (addingtoinitialize[0]) -> shape_array_size; ++ik){
        int pixelIndex = (finaly[ik] * 100 + finalx[ik]) * 3;
        pixelData[pixelIndex] = 0;      // Red
        pixelData[pixelIndex + 1] = 0;  // Green
        pixelData[pixelIndex + 2] = 0;  // Blue
    }





    
    file.write(reinterpret_cast<char*>(pixelData.data()), pixelData.size());

    
    file.close();
    






}

};











//This contains some basic intializtion info for main()
struct seedingstuff { 
  int detail; //Not used
  int oblique_or_passable; //Not used
  double xspace; //size of the created space
  double yspace;
  double zspace;
  double radius;
  mapobject* newspace; //reference to the space created
};


//thread for intilizing a cube shape. In the future there will be more threads for more shapes, and they will perform some state changes (Ex. like kinematics or deletion). All of that will be managed from functions like these.
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

    newarg -> newspace -> create_space_file(connectedpoints, nameof); //This records the records the shapes information
    newarg -> newspace -> create_bmp_file(connectedpoints); //This creates the bmp file. 
    
    return NULL;
}









//This is a messy initialization. It will be streamlined to handle user inputs.

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
    //pthread_create(&thread2, NULL, cuber, (void*)&arg2);
    pthread_join(thread, NULL); 
    //pthread_join(thread2, NULL); 

  

    return 0;
}