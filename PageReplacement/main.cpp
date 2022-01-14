//
//  main.cpp
//  AOS_hw1
//
//  Created by Aaron on 2021/10/17.
//

#include <iostream>
#include <fstream>
#include "PageReplacement.hpp"
using namespace std;

int main(int argc, const char * argv[]) {
    // 0. Generate three different tests.
    const int dataSize = 300000;
    const int refSize = 1200;
    const double dirtyRate = 0.5;
    const double subRate1 = 1.0 / 300.0;
    const double subRate2 = 1.0 / 120.0;
    const int refRange = 100;
    const double setSize = 30;
    
    GenerTests tests(dataSize, refSize, dirtyRate);
    tests.randomTests(20);
    tests.localityTests(subRate1, subRate2, refRange);
    tests.normalTests(refSize / 2, refSize / setSize);
    
    // 1. Execute five different algoritms on each tests.
    vector<int> memSize = {30, 60, 90, 120, 150};
    vector<string> fileName = {"randomTests.txt", "localityTests.txt", "normalTests.txt"};
    vector<string> algoName = {"FIFO", "Optimal", "ARB", "LIFO", "NRA"};
    
    Performance performance;
    PageReplacement pageReplacement(memSize[0], fileName[2]);
//    cout << "The size of data: " << pageReplacement.getFileSize() << endl;
    for (int i = 0; i < fileName.size(); ++i) {
        pageReplacement.setFile(fileName[i]);
//        cout << "The file: " << fileName[i] << endl;
        for (int j = 0; j < memSize.size(); ++j) {
//            cout << "The size of memory: " << memSize[j] << endl;
            pageReplacement.setMemSize(memSize[j]);
            performance = pageReplacement.FIFO();
//            performance.print(2);
            cout << algoName[0] << "," << memSize[j] << "," <<  performance.pageFaults << "," << performance.interrupts << "," << performance.writes << endl;

            performance = pageReplacement.Optimal();
//            performance.print(2);
            cout << algoName[1] << "," << memSize[j] << "," <<  performance.pageFaults << "," << performance.interrupts << "," << performance.writes << endl;
            
            performance = pageReplacement.ARB(setSize);
//            performance.print(2);
            cout << algoName[2] << "," << memSize[j] << "," <<  performance.pageFaults << "," << performance.interrupts << "," << performance.writes << endl;
            
            performance = pageReplacement.LIFO();
//            performance.print(2);
            cout << algoName[3] << "," << memSize[j] << "," <<  performance.pageFaults << "," << performance.interrupts << "," << performance.writes << endl;
            
            performance = pageReplacement.NRA(memSize[j] / setSize);
//            performance.print(2);
            cout << algoName[4] << "," << memSize[j] << "," <<  performance.pageFaults << "," << performance.interrupts << "," << performance.writes << endl;
        }
    }
    return 0;
}
