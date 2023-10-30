#include <vector>

//a route refers to a specific route that one driver will make
    //ex: [1,2,3] or [1] (by delivery index)
class route{
    public:
    vector<delivery> deliveries;
    vector<int> deliveries_by_index;
    double cost;

    //calculates cost of the route
    double compute_cost(){
        if(deliveries.size() == 0){
            return 0;
        }
        double running_cost = dist_from_zero(this->begin_location());
        for(int i = 0; i < (int)deliveries.size(); ++i){
            running_cost += deliveries[i].cost;
        }
        running_cost += dist_from_zero(this->end_location());
        return running_cost;
    }

    //adds a delivery to the route
    void add(int i, delivery a){
        deliveries.push_back(a);
        deliveries_by_index.push_back(i + 1);
        cost = compute_cost();
    }

    //returns the beginning location of a route
    coordinate begin_location(){
        return deliveries[0].pickup;
    }

    //returns the end location of a route
    coordinate end_location(){
        return deliveries.back().dropoff;
    }

    //prints a route
    void print(){
        cout << "[";
        for(int i = 0; i < (int)deliveries_by_index.size(); ++i){
            cout << deliveries_by_index[i];
            if(i != (int)deliveries_by_index.size() - 1){
                cout << ",";
            }
        }
        cout << "]" << endl;
    }

    //intra relocation heuristic
    double relocate_optimization(){
        vector<delivery> min_route = deliveries;
        double min_cost = cost;
        vector<int> min_indices = deliveries_by_index;

        for(int i = 0; i < (int)deliveries.size(); ++i){
            for(int j = i + 1; j < (int)deliveries.size(); ++j){
                vector<delivery> d = deliveries;
                swap(deliveries[i], deliveries[j]);
                double new_cost = compute_cost();
                if(new_cost < min_cost){
                    swap(deliveries_by_index[i], deliveries_by_index[j]);
                    vector<int> temp_indices = deliveries_by_index;
                    swap(temp_indices[i], temp_indices[j]);
                    min_indices = temp_indices;
                    min_cost = new_cost;
                }
                deliveries = d;                
            }
        }

        cost = min_cost;
        deliveries = min_route;
        deliveries_by_index = min_indices;
        return cost;
    }

    //returns the cost of a swap, used in the inter relocation heuristic
    double swap_cost(int delivery_index, delivery d){
        delivery old_d = deliveries[delivery_index];
        double test_cost = cost - old_d.cost + d.cost;
        if(delivery_index == 0){
            test_cost -= dist_from_zero(old_d.pickup);
            test_cost += dist_from_zero(d.pickup);
        }
        else if(delivery_index == (int)deliveries.size() - 1){
            test_cost -= dist_from_zero(old_d.dropoff);
            test_cost += dist_from_zero(d.dropoff);
        }
        return test_cost;
    }

    //performs a swap of deliveries
    void swap_delivery(int delivery_index, delivery d, int index){
        deliveries[delivery_index] = d;
        deliveries_by_index[delivery_index] = index;
        cost = compute_cost();
    }
};