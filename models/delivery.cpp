#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "coordinate.cpp"


using namespace std;

//a delivery is used to hold the data of an individual delivery
    //ex: the data read in from the files is stored in a vector of deliveries
class delivery{
    public:
    coordinate pickup;
    coordinate dropoff;
    double cost;
    bool taken;

    delivery(coordinate pickup_in, coordinate dropoff_in){
        pickup = pickup_in;
        dropoff = dropoff_in;
        taken = false;
        cost = dist(pickup_in, dropoff_in);
    }

    delivery(){}
};