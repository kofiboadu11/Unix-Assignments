#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main() {
    // Ask user for input file
    string filename;
    string outputfile;
    cout << "Enter input file name: ";
    cin >> filename;
    cout << "Enter output file name: ";
    cin >> outputfile;

    // Open input file
    ifstream input_file(filename);
    if (!input_file) {
        cout << "Error: Unable to open input file!" << endl;
        return 1;
    }

    // Read input string from file
    string input_str;
    input_file >> input_str;

    // Close input file
    input_file.close();

    // Initialize count and timer
    int count = 0;
    auto start_time = high_resolution_clock::now();

    // Search for patterns
    for (int i = 0; i < input_str.length() - 1; i++) {
        if (input_str[i] == '(' && input_str[i+1] == '(') {
            for (int j = i+2; j < input_str.length() - 1; j++) {
                if (input_str[j] == ')' && input_str[j+1] == ')') {
                    count++;
                }
            }
        }
    }
 // Calculate time elapsed
    auto stop_time = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(stop_time - start_time);

    // Write output to file
    ofstream output_file(outputfile);
    output_file << "Time Elapsed: " << duration.count() << " nanoseconds" << endl;
    output_file << "Found Pattern Count: " << count << endl;
    output_file << "Searched Pattern: " << input_str << endl;
    output_file.close();

    cout << "Done!" << endl;

    return 0;
}
