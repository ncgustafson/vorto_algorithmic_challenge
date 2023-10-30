using namespace std;

//simple struck to represent locations on a graph
    //ex: a delivery is composed of two coordinate, a pickup and dropoff location
struct coordinate{
    double x;
    double y;
};

//calculate euclidean distance between two graph points
double dist(coordinate a, coordinate b){
    return sqrt(pow(b.x - a.x,2) + pow(b.y - a.y,2));
}

//calculate the distance between a coordinate and 0
double dist_from_zero(coordinate a){
    return sqrt(pow(a.x,2) + pow(a.y,2));
}