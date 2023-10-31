#include <iostream>
#include <fstream>
#include <limits.h>
#include <algorithm>
#include <string>
#include "models/delivery.cpp"
#include "models/route.cpp"

using namespace std;

//reads and stores data from a text file
vector<delivery> read_data(string filename){
    vector<delivery> res;
    ifstream MyReadFile(filename);
    string s;
    while (getline (MyReadFile, s)) {
        if(s[0] == 'l' || s.size() == 0){
            continue;
        }        
        double pickup_x = stod(s.substr(s.find(' ') + 2, s.find(',') - 2));        
        double pickup_y = stod(s.substr(s.find(',') + 1, s.find(')') - s.find(',') - 2));
        coordinate pickup_coordinate = {pickup_x, pickup_y};
        int p_loc = s.find(')') + 3;
        double dropoff_x = stod(s.substr(p_loc, s.find(',', p_loc) - p_loc));
        double dropoff_y = stod(s.substr(s.find(',', p_loc) + 1, s.size() - s.find(',', p_loc) - 3));
        coordinate dropoff_coordinate = {dropoff_x, dropoff_y};        
        delivery new_delivery = delivery(pickup_coordinate, dropoff_coordinate);
        res.push_back(new_delivery);
    }

    /*for(int i = 0; i < (int)res.size(); ++i){
        res[i].print();
        cout << i + 1 << " " << res[i].cost << endl;
    }*/
    return res;
}

//prints the routes by index, as specified in the scope
void print_routes(vector<route> routes){
    for(int i = 0; i < (int)routes.size(); ++i){
        routes[i].print();
    }
}

//calculate the cost of a route
double calculate_routes_cost(vector<route> routes){
    //500 for each driver
    double running_cost = 500*routes.size();
    for(int i = 0; i < (int)routes.size(); ++i){
        running_cost += routes[i].cost;
    }
    return running_cost;
}

//finds the closest deliviery from a specific point on a graph
//will return -1 if no deliveries are available, indicating that all the routes have been built
    //used to build the routes
int closest_delivery(coordinate a, vector<delivery> d){
    int res = -1;
    double min_dist = INT_MAX;
    for(int i = 0; i < (int)d.size(); ++i){
        if(dist(a, d[i].pickup) < min_dist && !d[i].taken){
            res = i;
            min_dist = dist(a, d[i].pickup);
        }
    }
    return res;
}

//builds the routes using a  sequential nearest neighbor heuristic
double solve(vector<delivery> deliveries, vector<route> &routes){
    int num_drivers = 1;
    coordinate start = {0,0};
    double min_cost = INT_MAX;
    vector<route> route_res;
    while(num_drivers < (int)deliveries.size()){
        vector<delivery> d = deliveries;
        vector<route> routes;
        bool completed = false;
        for(int i = 0; i < num_drivers; ++i){
            route new_route;
            int min_delivery_dist_index = closest_delivery(start, d);
            if(min_delivery_dist_index == -1){
                    completed = true;
                    break;
            }
            new_route.add(min_delivery_dist_index, d[min_delivery_dist_index]);
            d[min_delivery_dist_index].taken = true;            
            while(true){
                int min_delivery_dist_index = closest_delivery(new_route.end_location(), d);
                if(min_delivery_dist_index == -1){
                    completed = true;
                    break;
                }
                //if the new route doesn't violate the 12 hr rule (720 mins)
                if(new_route.cost + 
                dist(new_route.end_location(), d[min_delivery_dist_index].pickup) + dist(d[min_delivery_dist_index].pickup, d[min_delivery_dist_index].dropoff) +
                dist_from_zero(d[min_delivery_dist_index].dropoff) - dist(new_route.end_location(), start) < 720){
                    new_route.add(min_delivery_dist_index, d[min_delivery_dist_index]);
                    d[min_delivery_dist_index].taken = true;
                }
                else{
                    break;
                }
            }
            routes.push_back(new_route);
        }       
        if(completed){
            double cost = calculate_routes_cost(routes);           
            if(cost < min_cost){
                min_cost = cost;
                route_res = routes;
            }
        }
        ++num_drivers;
    }
    routes = route_res;
    return min_cost;
}

//intra relocate improvement heuristic
double intra_relocate_routes(vector<route> &routes){
    double running_cost = 0;
    for(int i = 0; i < (int)routes.size(); ++i){
        running_cost += routes[i].relocate_optimization();
    }
    return running_cost + 500*(int)routes.size();
}

//inter relocate improvement heuristic
double inter_relocate_routes(vector<route> &routes, double cost){
    while(true){
        double biggest_change = 0;
        vector<route> min_route = routes;
        for(int i = 0; i < (int)routes.size(); ++i){
            if(min_route[i].deliveries.size() == 0){
                continue;
            }
            for(int j = 0; j < (int)routes[i].deliveries.size(); ++j){
                for(int k = i + 1; k < (int)routes.size(); ++k){
                    double old_cost = routes[i].cost + routes[k].cost;
                    for(int l = 0; l < (int)routes[k].deliveries.size(); ++l){
                        //test swapping routes[i].deliveries[j], routes[k].deliveries[l]
                        double swap1 = routes[i].swap_cost(j, routes[k].deliveries[l]);
                        double swap2 = routes[k].swap_cost(l, routes[i].deliveries[j]);
                         
                        if(swap1 < 720 && swap2 < 720 && old_cost - (swap1 + swap2) > biggest_change){
                            //good swap!                            
                            biggest_change = old_cost - (swap1 + swap2);
                            min_route = routes;
                            min_route[i].swap_delivery(j, routes[k].deliveries[l], routes[k].deliveries_by_index[l]);
                            min_route[k].swap_delivery(l, routes[i].deliveries[j], routes[i].deliveries_by_index[j]);
                        }                        
                    }
                }
            }
        }
        routes = min_route;
        double new_cost = calculate_routes_cost(routes);
        if(cost == new_cost || new_cost > cost){
           break;
        }
        cost = new_cost;
    }
    return cost;
}

int main(int argc,  char *argv[]){
    if(argc != 2){
        cout << "IMPROPER INPUT FORMAT" << endl;\
        cout << "   Please format as such: ./main.o <filepath>" << endl;
        return 0;
    }
    string filename = argv[1];
    vector<route> routes;
    vector<delivery> deliveries = read_data(filename);
    solve(deliveries, routes);        
    double cost = intra_relocate_routes(routes);
    cost = inter_relocate_routes(routes, cost);
    print_routes(routes);    
    return 0;
}
