//
//  PageReplacement.cpp
//  AOS_hw1
//
//  Created by Aaron on 2021/10/17.
//

#include "PageReplacement.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <stack>
using namespace std;

void printVec(const vector<int> &vec) {
    for (const auto v : vec) {
        cout << v << " ";
    }
    cout << endl;
}

void printStack(stack<int> s) {
    if (s.size() != 0){
        do{
            cout << s.top() << " ";
            s.pop();
        } while(s.size());
    }
    cout << endl;
}

void printQueue(queue<int> q){
    if (q.size() != 0){
        do{
            cout << q.front() << " ";
            q.pop();
        } while(q.size());
    }
    cout << endl;
}

/* Performance */
void Performance::print(const int n){
    switch (n) {
        case 1:
            cout << "Page faults: " << pageFaults << endl;
            cout << "Interrupts: " << interrupts << endl;
            cout << "Writes: " << writes << endl;
            break;
            
        case 2:
            cout << pageFaults << " " << interrupts << " " << writes << endl;
            break;
    }
}

/* GenerTests */

GenerTests::GenerTests(const int p_dataSize, const int p_refSize, const double p_dirtyRate)
:dataSize(p_dataSize),
 refSize(p_refSize),
 dirtyRate(p_dirtyRate),
 generator(rd()),
 refDst(1, refSize),
 probDst(0, 1)
{}

void GenerTests::randomTests(const int refRange, const string &fileName){
    uniform_int_distribution<int> rangeDst(1, refRange);
    
    int p_dataSize = dataSize;
    ofstream file(fileName);
    do{
        const int refHead = refDst(generator);
        const int range = min({rangeDst(generator), p_dataSize, refSize - refHead});
        int dirtyBit = 0;
        for (int i = 0; i < range; ++i) {
            dirtyBit =  probDst(generator) <= dirtyRate ? 1 : 0;
            file << refHead + i << " " << dirtyBit << endl;
        }
        p_dataSize -= range;
    } while (p_dataSize);
    file.close();
}


void GenerTests::localityTests(const double subRate1, const double subRate2, const int refRange, const string &fileName) {
    int p_dataSize = dataSize;
    uniform_int_distribution<int> rangeDst(1, refRange);
    uniform_int_distribution<int> subDst(dataSize * subRate1, dataSize * subRate2);
    
    ofstream file(fileName);
    do{
        const int refHead = refDst(generator);
        const int range = rangeDst(generator);
        uniform_int_distribution<int> localDst(refHead, min(refHead + range, refSize));
        const int subSize = min(subDst(generator), p_dataSize);
        int dirtyBit = 0, ref = 0;
        for (int i = 0; i < subSize; ++i) {
            dirtyBit = probDst(generator) <= dirtyRate ? 1 : 0;
            ref = localDst(generator);
            file << ref << " " << dirtyBit << endl;
        }
        p_dataSize -= subSize;
    } while(p_dataSize);
    file.close();
}

void GenerTests::normalTests(const int mean, const int standardDeviation, const string &fileName){
    normal_distribution<float> refDst(mean, standardDeviation);
    
    int p_dataSize = dataSize;
    ofstream file(fileName);
    while (p_dataSize--) {
        int ref = 0;
        while (ref < 1 || ref > refSize) {ref = refDst(generator);}
        const int dirtyBit = probDst(generator) <= dirtyRate ? 1 : 0;
        file << ref << " " << dirtyBit << endl;
    }
    file.close();
}

/* PageReplacement */

PageReplacement::PageReplacement(const int p_memSize, const string p_fileName)
:memSize(p_memSize)
{
    setFile(p_fileName);
}

void PageReplacement::setFile(const std::string p_fileName) {
    if (fileName != p_fileName) {
        fileName = p_fileName;
        
        // 1. Open a file and check if it is opened.
        ifstream file(fileName);
        if (!file) {cerr << "File don't be opened." << endl;}
        
        // 2. Pages store all content of athe file.
        int ref, dirtyBit;
        pages.clear();
        // Column 0 is a ref and column 1 is a dirty bit.
        while (file >> ref >> dirtyBit) {pages.push_back({ref, dirtyBit});}
        file.close();
    }
}

// First in first out (FIFO) algorithm
Performance PageReplacement::FIFO(){
    performance.reset();
    queue<int> memory;
    unordered_set<int> memSet;
    unordered_map<int, Bits> memBits;

    // 1. Execute FIFO algorithm
    for (const auto &p : pages) {
        // Column 0 is a ref and column 1 is a dirty bit.
        const int ref = p[0];
        const int dirtyBit = p[1];
        if (memSet.find(ref) == memSet.end()) {
            if (memory.size() < memSize) {
                // 1.1 A memory isn't full and ref isn't found in the memory.
                // 1.1.1 Add a new ref into the memory.
                memory.push(ref);
                memSet.insert(ref);
                memBits[ref] = {1, dirtyBit};
            } else {
                // 1.2 A memory is full and ref isn't found in the memory.
                // 1.2.1 Choose and Remove a victim from the memory.
                const int victim = memory.front();
                memory.pop();
                memSet.erase(victim);
                if (memBits[victim].dirty == 1) { // Write back into the disk.
                    ++performance.interrupts;
                    ++performance.writes;
                }
                memBits[victim] = {0, 0};
                // 1.2.2 Add a new ref into the memory.
                memBits[victim].ref = 0;
                memory.push(ref);
                memSet.insert(ref);
                memBits[ref] = {1, dirtyBit};
            }
            ++performance.pageFaults;
        } else if (memBits[ref].dirty == 0 && dirtyBit == 1) { // Writes
            memBits[ref].dirty = dirtyBit;
        }
//        printQueue(memory);
    }
    return performance;
}

// Optimal algorithm
Performance PageReplacement::Optimal(){
    performance.reset();
    vector<int> memory;
    unordered_map<int, Bits> memBits;
    
    // 1. Execute optimal algorithm.
    for (int i = 0; i < pages.size(); ++i) {
        // Column 0 is a ref and column 1 is a dirty bit.
        const int ref = pages[i][0];
        const int dirtyBit = pages[i][1];
        if (find(memory.begin(), memory.end(), ref) == memory.end()) {
            if (memory.size() < memSize) {
                // 1.1 A memory isn't full and ref isn't found in the memory.
                // Add a new ref into the memory.
                memory.push_back(ref);
                memBits[ref] = {1, dirtyBit};
            } else {
                // 1.2 A memory is full and ref isn't found in the memory.
                // Choose and Remove a victim from the memory.
                const int j = predict(i + 1, memory);
                const int victim = memory[j];
                memory[j] = ref;
                memBits[ref] = {1, dirtyBit};
                if (memBits[victim].dirty == 1) {
                    ++performance.writes;
                    ++performance.interrupts;
                }
                memBits[victim] = {0, 0};
            }
            ++performance.pageFaults;
        } else if (memBits[ref].dirty == 0 && dirtyBit == 1) {
            memBits[ref].dirty = dirtyBit;
        }
//        printVec(memory);
    }
    return performance;
}

// Find a victim for optimal
int PageReplacement::predict(const int index, const std::vector<int> &memory){
    int pre = -1, farthest = index;
    for (int i = 0; i < memory.size(); ++i) {
        int j;
        // Store the index (j) of pages which are going to be used recently in future
        for (j = index; j < pages.size(); ++j) {
            if (memory[i] == pages[j][0]) {
                if (j > farthest) {
                    farthest = j;
                    pre = i;
                }
                break;
            }
        }
        // If a page is never used in future, return it.
        if (j == pages.size()) {return i;}
    }
    // If all of the frames were not in future, return any of them, we return 0.
    // Otherwise we return pre.
    return pre == -1 ? 0 : pre;
}

// Additional-reference-bits (ARB) algorithm
Performance PageReplacement::ARB(const int interval) {
    performance.reset();
    int count = 0;
    vector<int> memory;
    unordered_map<int, Bits> memBits;
    unordered_set<int> memHits;
    
    // 1. Excute Additional-reference-bits (ARB)
    for (const auto &p : pages) {
        int isInterrupt = 0;
        // Column 0 is a ref and column 1 is a dirty bit.
        const int ref = p[0];
        const int dirtyBit = p[1];
        if (find(memory.begin(), memory.end(), ref) == memory.end()) {
            if (memory.size() < memSize) {
                // 1.1 A memory isn't full and ref isn't found in the memory.
                // Add a new ref into the memory.
                memory.push_back(ref);
                memBits[ref] = {128, dirtyBit};
            } else {
                // 1.2 A memory is full and ref isn't found in the memory.
                // Choose and Remove a victim from the memory.
                const int j = findMinRef(memory, memBits);
                const int victim = memory[j];
                memory[j] = ref;
                memBits[ref] = {128, dirtyBit};
                if (memBits[victim].dirty == 1) {
                    ++performance.writes;
                    ++performance.interrupts;
                    isInterrupt = 1;
                }
                memBits[victim] = {0, 0};
            }
            ++performance.pageFaults;
        } else {
            memHits.insert(ref);
            if (memBits[ref].dirty == 0 && dirtyBit == 1) {memBits[ref].dirty = dirtyBit;}
        }
//        printVec(memory);
        
        // 1.2 Update the refBit of all pages in the memory.
        if (++count == interval) {
            count = 0;
            updateARB(memory ,memBits, memHits);
            if (!isInterrupt) {++performance.interrupts;}
        }
    }
    return performance;
}

// Find a victim for ARB
int PageReplacement::findMinRef(const vector<int> &memory, unordered_map<int, Bits> &memBits) {
    int min = 256;
    int minIndex = 0;
    for (int i = 0; i < memory.size(); ++i) {
        if (memBits[memory[i]].ref < min) {
            minIndex = i;
            min = memBits[memory[i]].ref;
        }
    }
    return minIndex;
}

void PageReplacement::updateARB(const vector<int> &memory, unordered_map<int, Bits> &memBits, unordered_set<int> &memHits) {
    // 1. Shfit right the refBit of all pages in the memory by 1 bit.
    for (const auto m : memory) {memBits[m].ref >>= 1;}
    
    // 2. If pages in the memory are referenced, their refBit ^ 1000 0000(2).
    for (const auto &h : memHits) {memBits[h].ref &= 128;}
    memHits.clear();
}

// Last in First out
Performance PageReplacement::LIFO() {
    performance.reset();
    stack<int> memory;
    unordered_set<int> memSet;
    unordered_map<int, Bits> memBits;
    
    // 1. Execute LIFO algorithm
    for (const auto &p : pages) {
        // Column 0 is a ref and column 1 is a dirty bit.
        const int ref = p[0];
        const int dirtyBit = p[1];
        if (memSet.find(ref) == memSet.end()) {
            if (memory.size() < memSize) {
                // 1.1 A memory isn't full and ref isn't found in the memory.
                // Add a new ref into the memory.
                memory.push(ref);
                memSet.insert(ref);
                memBits[ref] = {1, dirtyBit};
                ++performance.pageFaults;
            } else {
                // 1.2 A memory is full and ref isn't found in the memory.
                // 1.2.1 Choose and Remove a victim from the memory.
                const int victim = memory.top();
                memory.pop();
                memSet.erase(victim);
                if (memBits[victim].dirty == 1) { // Write back into the disk.
                    ++performance.interrupts;
                    ++performance.writes;
                }
                memBits[victim] = {0, 0};
                // 1.2.2 Add a new ref into the memory.
                memBits[victim].ref = 0;
                memory.push(ref);
                memSet.insert(ref);
                memBits[ref] = {1, dirtyBit};
                ++performance.pageFaults;
            }
        }else if (memBits[ref].dirty == 0 && dirtyBit == 1) { // Writes
            memBits[ref].dirty = dirtyBit;
        }
//        printStack(memory);
    }
    return performance;
}

// Normal random accumulation (NRA)
Performance PageReplacement::NRA(int standardDeviation){
    performance.reset();
    if (standardDeviation < 1) {standardDeviation = 1;}
    vector<int> memory;
    unordered_map<int, Bits> memBits;
    random_device rd;
    default_random_engine generator(rd());
    normal_distribution<float> victimDst(memSize, standardDeviation);
    
    // 1. Execute NRA algorithm
    for (const auto &p : pages) {
        const int ref = p[0];
        const int dirtyBit = p[1];
        if (find(memory.begin(), memory.end(), ref) == memory.end()) {
            if (memory.size() < memSize) {
                // 1.1 A memory isn't full and ref isn't found in the memory.
                // 1.1.1 Add a new ref into the memory.
                memory.push_back(ref);
                memBits[ref] = {1, dirtyBit};
            } else {
                // 1.2 A memory is full and ref isn't found in the memory.
                // 1.2.1 Choose and Remove a victim from the memory.
                int j = -1;
                while (1) {
                    while (j < 0 || j > memSize - 1) {
                        j = victimDst(generator);
                        if (j > memSize - 1) {j = 2 * memSize - j;}
                    }
                    if (memBits[memory[j]].ref >= 1) { // sencond chance
                        --memBits[memory[j]].ref;
                        continue;
                    } else {
                        break;
                    }
                }
                
                const int victim = memory[j];
                memory.erase(memory.begin() + j);
                memory.push_back(ref);
                memBits[ref] = {1, dirtyBit};
                if (memBits[victim].dirty == 1) { // Write back into the disk.
                    ++performance.interrupts;
                    ++performance.writes;
                }
                memBits[victim] = {0, 0};
            }
            ++performance.pageFaults;
        } else {
            ++memBits[ref].ref;
            if (memBits[ref].dirty == 0 && dirtyBit == 1) {memBits[ref].dirty = dirtyBit;}
        }
//        printVec(memory);
    }
    return performance;
}
