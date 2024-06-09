#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <numeric>
#include <algorithm>
#include "LineInfo.h"

using namespace std;

int main(int argc, char *argv[]) {
    try {
        if (argc != 3) {
            throw LineInfo("Wrong arguments\nUsage:\ncalculate <sum|average|greatest|least> filename.txt\nfilename - a file that contains integers, 1 integer per line\nExample:\ncalculate sum filerandom.txt", __FILE__, __LINE__);
        }
        
        string command = argv[1];
        if (command != "sum" && command != "average" && command != "greatest" && command != "least") {
            throw LineInfo("Invalid Command: " + command + "\nUsage:\ncalculate <sum|average|greatest|least> filename.txt\nfilename - a file that contains integers, 1 integer per line\nExample:\ncalculate sum filerandom.txt", __FILE__, __LINE__);
        }
        
        ifstream infile(argv[2]);
        if (!infile) {
            throw LineInfo("Failed to open file: " + string(argv[2]), __FILE__, __LINE__);
        }
        
        istream_iterator<int> input_iterator(infile);
        istream_iterator<int> end_of_stream;
        vector<int> nums(input_iterator, end_of_stream);
        
        int result = 0;
        if (command == "sum") {
            result = accumulate(nums.begin(), nums.end(), 0);
        } else if (command == "average") {
            result = accumulate(nums.begin(), nums.end(), 0) / nums.size();
        } else if (command == "greatest") {
            result = *max_element(nums.begin(), nums.end());
        } else if (command == "least") {
            result = *min_element(nums.begin(), nums.end());
        }
        
        cout << result << endl;
        exit(EXIT_SUCCESS);
    } catch ( exception& e) {
    cerr << e.what() << endl;
    exit(EXIT_FAILURE);
}
}