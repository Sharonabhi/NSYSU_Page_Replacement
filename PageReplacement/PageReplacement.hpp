//
//  PageReplacement.hpp
//  AOS_hw1
//
//  Created by Aaron on 2021/10/17.
//

#ifndef PageReplacement_hpp
#define PageReplacement_hpp

#include <iostream>
#include <vector>
#include <random>
#include <unordered_set>
#include <unordered_map>

class GenerTests{
public:
    GenerTests(const int p_dataSize, const int p_refSize = 1200, const double p_dirtyRate = 0.5);
    ~GenerTests(){}
    
    void randomTests(const int refRange = 20, const std::string &fileName = "randomTests.txt");
    void localityTests(const double subRate1 = 1.0 / 10.0, const double subRate2 = 1.0 / 5.0, const int refRange = 10, const std::string &fileName = "localityTests.txt");
    void normalTests(const int mean, const int standardDeviation, const std::string &fileName = "normalTests.txt");
    
private:
    const int dataSize;
    const int refSize;
    const double dirtyRate;
    
    std::random_device rd;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> refDst;
    std::uniform_real_distribution<double> probDst;
};

typedef struct Bits{
    int ref;
    int dirty;
}Bits;

class Performance{
public:
    void reset(){
        pageFaults = 0;
        interrupts = 0;
        writes = 0;
    }
    
    void print(const int n = 1);
    
    int pageFaults;
    int interrupts; // Interrupts include writes, but don't include page faults
    int writes;
};

class PageReplacement{
public:
    PageReplacement(const int p_memSize, const std::string p_fileName);
    ~PageReplacement(){}
    
    inline void setMemSize(const int p_memSize){memSize = p_memSize;}
    void setFile(const std::string p_fileName);
    
    inline int getFileSize(){return pages.size();}
    
    Performance FIFO();
    Performance Optimal();
    Performance ARB(const int interval = 1);
    Performance LIFO();
    Performance NRA(int standardDeviation);
    
private:
    // Data members
    Performance performance;
    int memSize;
    std::string fileName;
    std::vector<std::vector<int>> pages; // Column 0 is a ref and column 1 is a dirty bit.
    
    // Member funciotns
    int predict(const int index, const std::vector<int> &memory); // Find a victim for optimal
    int findMinRef(const std::vector<int> &memory, std::unordered_map<int, Bits> &memBits); // Find a victim for ARB
    void updateARB(const std::vector<int> &memory, std::unordered_map<int, Bits> &memBits, std::unordered_set<int> &memHit); // For ARB
};


#endif /* PageReplacement_hpp */
