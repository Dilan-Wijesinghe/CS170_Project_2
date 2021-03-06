#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <cmath> 
#include <iomanip>
#include <chrono>

using std::cout, std::cin, std::endl, std::vector, std::set;

// Function Declarations
void forward_search(const vector<vector<double>> &data, double data_sz);
void backward_search(const vector<vector<double>> &data, double data_sz); 
double leave_one_out_cross_validation(vector<vector<double> > data, set<int> curr_set, int add_me, double data_sz, bool fwd);
void print_data(const vector<vector<double> > &data);
void print_set(const set<int> &curr_set, const int &feature_to_add);
void print_set_r(const set<int> &curr_set, const int &feature_to_remove);
void print_true_set(const set<int> &curr_set);

int main()
{
    double val;
    double small_sz = 300; // Hardcoded the values of size
    double large_sz = 500;
    int small_fts = 10;
    int large_fts = 100;
    double data_sz;
    int fts;
    int choice;
    std::string file_name;
    cout << "Would you like to use the large or small dataset?\n\t1. Small!\n\t2. Large!\n";
    cin >> choice;
    switch(choice) {
        case 1:
            file_name = "CS170_SMALLtestdata__3.txt"; // Edit for different File
            // file_name = "CS170_small_special_testdata__95.txt"; // - For testing purposes
            data_sz = small_sz;
            fts = small_fts;
            break;
        case 2:
            file_name = "CS170_largetestdata__50.txt"; // Edit for different File
            data_sz = large_sz;
            fts = large_fts;
            break;
    }
    std::ifstream fin(file_name);
    vector<vector<double> > data(data_sz); // We want 300 items (or 500 for large data) 
    std::string line;
    
    for(int i = 0; i < data_sz; i++) {// Parsing to Create a new Data
        std::getline(fin, line);
        std::istringstream parse(line);
        parse >> val;
        data[i].push_back(val); 
        for(int j = 1; j <= fts; j++) { // Get the Features
            parse >> val;
            data[i].push_back(val);   
        } 
    }
   
    cout << "Type the number of the algorithm you want to run\n\t1. Forward Selection\n\t2. Backward Elimination\n";
    cin >> choice;
    switch(choice) {
        case 1: 
            forward_search(data, data_sz);
            break;
        case 2:
            backward_search(data, data_sz);
            break;
        case 3:
            cout << "Testing 'leave-one-out'\n";
            set<int> set_test;
            set_test.insert(6);
            int j = 4;
            cout << leave_one_out_cross_validation(data, set_test, j, data_sz, true) << endl;
            break;
    }
}

// Forward Search Function
void forward_search(const vector<vector<double>> &data, double data_sz)
{
    typedef std::chrono::high_resolution_clock Clock; // Defining Clock
    int num_features = data[0].size();
    bool fwd = true;
    set<int> curr_set; // Set of all features!
    set<int> test_set;
    for(int i = 1; i < num_features; i++) {
        test_set.insert(i); // Test to output all features test
    }
    double accuracy = 0.0;
    double true_best_acc = 0.0;
    set<int> true_set;
    vector<int> feature_to_add_this_level;
    // cout << "This dataset has " << num_features - 1 << " features, with " << data_sz << " instances\n";
    cout << "Running Nearest Neighbor with all " << num_features - 1 << " features I get an accuracy of " << leave_one_out_cross_validation(data, test_set, -1, data_sz, fwd)  * 100 << "%\n";
    cout << "Beginning Search\n";
    auto start = Clock::now();
    
    for(int i = 1; i < num_features; i++) { 
            //cout << "On the " << i << " level of the search tree\n";
            double best_accuracy = 0.0; // The best so far accuracy
            
        for(int j = 1; j < num_features; j++) {
            const bool is_in = curr_set.find(j) != curr_set.end(); // Checking if we already added an elt
            if(!is_in) { 
                // cout << "Considering adding feature " << j << endl;
                accuracy = leave_one_out_cross_validation(data, curr_set, j, data_sz, fwd); // j represents the feature we want to add temporarily
                cout << "\tUsing feature(s) "; print_set(curr_set, j); cout << " accuracy is " << accuracy * 100 << "%\n";
                if(accuracy > best_accuracy) {
                    best_accuracy = accuracy;
                    feature_to_add_this_level.push_back(j); // Inserting any num from [1-10]
                }
            }
        }
        cout << "Feature set "; print_set(curr_set, feature_to_add_this_level[feature_to_add_this_level.size() - 1]); cout << " was best, with accuracy " << best_accuracy * 100 << "%\n";
        curr_set.insert(feature_to_add_this_level[feature_to_add_this_level.size() - 1]); 
        // cout << "On level " << i << " I added feature " << feature_to_add_this_level[feature_to_add_this_level.size() - 1] << " to the current set\n";
        
        if(best_accuracy > true_best_acc) {
            true_set.clear();
            true_best_acc = best_accuracy;
            true_set = curr_set;
        }
        feature_to_add_this_level.clear();
    }
    cout << "Finished Search! The Best Set is "; print_true_set(true_set); cout << " which has an accuracy of " << true_best_acc * 100 << "%\n";
    auto end = Clock::now();
    cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(end-start).count() << " seconds" << endl; 
}

// Backward Search Function
// Start with ALL features and see how removing a features helps!
void backward_search(const vector<vector<double>> &data, double data_sz)
{   
    typedef std::chrono::high_resolution_clock Clock; // Defining Clock
    int num_features = data[0].size();
    bool fwd = false;
    set<int> curr_set; // Set with our Features
    for(int i = 1; i < num_features; i++) {
        curr_set.insert(i); // Adding in all of the features, and removing them
    }
    double accuracy = 0.0;
    double true_best_acc = 0.0;
    set<int> true_set;
    vector<int> feature_to_remove_this_level;
    // cout << "This dataset has " << num_features - 1 << " features, with " << data_sz << " instances\n";
    cout << "Running Nearest Neighbor with all " << num_features - 1 << " features I get an accuracy of " << leave_one_out_cross_validation(data, curr_set, -1, data_sz, fwd) * 100 << "%\n";
    cout << "Beginning Search\n";
    auto start = Clock::now();
    
    for(int i = 1; i < num_features; i++) { 
            //cout << "On the " << i << " level of the search tree\n";
            double best_accuracy = 0.0; // The best so far accuracy
            
        for(int j = 1; j < num_features; j++) {
            const bool is_in = curr_set.find(j) != curr_set.end(); // Checking if we already added an elt
            if(is_in) { // If it IS in the set, we want to consider removing it!
                // cout << "Considering removing feature " << j << endl;
                accuracy = leave_one_out_cross_validation(data, curr_set, j, data_sz, fwd); // j represents the feature we want to add temporarily
                cout << "\tUsing feature(s) "; print_set_r(curr_set, j); cout << " accuracy is " << accuracy * 100 << "%\n";
                if(accuracy > best_accuracy) {
                    best_accuracy = accuracy;
                    feature_to_remove_this_level.push_back(j); // Inserting any num from [1-10]
                }
            }
        }
        cout << "Feature set "; print_set_r(curr_set, feature_to_remove_this_level[feature_to_remove_this_level.size() - 1]); cout << " was best, with accuracy " << best_accuracy * 100 << "%\n";
        curr_set.erase(feature_to_remove_this_level[feature_to_remove_this_level.size() - 1]); 
        // cout << "On level " << i << " I added feature " << feature_to_remove_this_level[feature_to_remove_this_level.size() - 1] << " to the current set\n";
        
        if(best_accuracy > true_best_acc) {
            true_set.clear();
            true_best_acc = best_accuracy;
            true_set = curr_set;
        }
        feature_to_remove_this_level.clear();
    }
    cout << "Finished Search! The Best Set is "; print_true_set(true_set); cout << " which has an accuracy of " << true_best_acc * 100 << "%\n";
    auto end = Clock::now();
    cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(end-start).count() << " seconds" << endl; 
}

// Function leave_one_out_cross_validation - Computes our Accuracy using k-fold (or k-fold adjacent) calculation.
// Also calculates nearest neighbor with Euclidean Distance
// Want to intentionally make a local copy of data - so we can 0 out some items
double leave_one_out_cross_validation(vector<vector<double> > data_temp, set<int> curr_set, int add_me, double data_sz, bool fwd)
{
    // Zeroing out Values here
    int num_features = data_temp[0].size();
    if(add_me != -1) {
        for(int i = 0; i < data_sz; i++) { 
            for(int j = 1; j < num_features; j++) { // Get the 10 Features
                const bool is_in = curr_set.find(j) != curr_set.end(); // Only want to add the features in the set!
                if(!fwd) {
                    if(!is_in || j == add_me) { // Remove the jth feature
                        data_temp[i][j] = 0;  
                    }
                } else {
                    if(!(is_in || j == add_me)) { 
                        data_temp[i][j] = 0;  
                    }
                }
            }
        } // This is Working as Intended
    }
    // Double Checking if the Values are actually Zeroed out - - 
    // print_data(data_temp);

    double num_correctly_classified = 0;
    double obj_to_classify_label;
    double nn_label;
    int nn_loc;
    double d_sum;
    double dist;

    for(int i = 0; i < data_sz; i++) {
            double obj_to_classify; // Iterates through Features 1 - 10 
            obj_to_classify_label = data_temp[i][0]; 
            // cout << "Looping over i, at the location: " << i + 1 << endl;
            // cout << "Object " << i + 1 << " is in class " << obj_to_classify_label << endl;

            double nn_dist = std::numeric_limits<double>::max(); // Distance to Nearest Neighbor
            nn_loc = std::numeric_limits<int>::max(); // Location of Nearest Neighbor
            nn_label = 0; // Will end up being either 1 or 2

            for(int k = 0; k < data_sz; k++) {
                if(k != i) {
                    d_sum = 0;
                    //cout << "Ask if " << i + 1 << " is the nearest neighbor with " << k + 1 << endl;
                    for(int n = 1; n < num_features; n++) { // Euclidean Distance - Occurs num_feature many times 
                        obj_to_classify = data_temp[i][n];
                        d_sum = d_sum + pow(obj_to_classify - data_temp[k][n], 2.0); 
                    }
                    dist = sqrt(d_sum);
                    if(dist < nn_dist) {
                        // cout << dist << " At Location: " << k << endl;
                        nn_dist = dist;
                        nn_loc = k;
                        nn_label = data_temp[k][0]; // Will return the Number associated with the Class Label
                        // cout << "Label is: " << nn_label << " And data_temp is: " << data_temp[k][0] << endl;
                    }  
                }
            }
          
        if(obj_to_classify_label == nn_label) { num_correctly_classified++; } 
        // cout << "Object " << i + 1 << " is class " << obj_to_classify_label << endl;
        // cout << "Its N.N. is " << nn_loc << " which is in class " << nn_label << endl;
    }
    // cout << "Number of Correctly Classified is: " << num_correctly_classified << endl;
    return num_correctly_classified / data_sz;
}
// Accuracy = Number of correct classifications / number of instances in our database
// K = number of rows (300 in small data set)


// Helper Functions for Printing
void print_data(const vector<vector<double> > &data) // Helper Function for Printing out Data
{
    for(int i = 0; i < data.size(); i++) {
            cout << "-- Feature " << i + 1 << " --" << endl;
        for(int j = 0; j < data[i].size(); j++) {
            cout << std::setprecision(8) << data[i][j] << " ";
        }
        cout << endl;
    }  
}

void print_set(const set<int> &curr_set, const int &feature_to_add)
{
    cout << "{ " << feature_to_add << " "; 
    for(auto const&i: curr_set) {
        cout << i << " ";
    }
    cout << "}";
}

void print_true_set(const set<int> &curr_set)
{
   cout << "{ ";
   for(auto const&i: curr_set) {
        cout << i << " ";
   } 
   cout << "}";
}

void print_set_r(const set<int> &curr_set, const int &feature_to_remove)
{
    cout << "{ ";
    for(auto const&i: curr_set) {
        if(i == feature_to_remove) { continue; }
        else { cout << i << " ";}
    }
    cout << "}";
}