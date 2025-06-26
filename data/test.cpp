#include <iostream>
#include <fstream>
using namespace std;

int main() {
    ofstream outfile("int1000data.txt"); 
    for (int i = 1; i <= 1000; ++i) {
        outfile << i << endl; 
    }
    outfile.close(); 
}
