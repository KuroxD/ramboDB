#ifndef _RamboConstruction_
#define _RamboConstruction_
#include <iomanip>
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include "MyBloom.h"
#include "MurmurHash3.h"
#include "utils.h"
#include "constants.h"
#include "bitArray.h"
#include <set>
#include <iterator>
#include <bitset>
#include <unordered_set>
#include <boost/dynamic_bitset.hpp>
#include <tbb/tbb.h>
// vector<uint> hashfunc( void* key, int len, int R, int B){
// }

class RAMBO {
public:

    RAMBO(int n, int r1, int b1, int K,int bias_);

    std::vector<uint> hashfunc(std::string key, int len);

    void insertion(std::string setID, std::vector<std::string> keys);

    std::set<int> takeunion(std::set<int> set1, std::set<int> set2);

    std::set<int> takeIntrsec(std::set<int> *setArray);

    std::vector<std::string> getdata(std::string filenameSet);

    void serializeRAMBO(std::string dir);

    void deserializeRAMBO(std::vector<std::string> dir);

    void insertion_pairs(std::vector<std::pair<std::string, std::string>> &data_key_number);

    void createMetaRambo_single(int value);

    void insertion_pair(std::pair<std::string, std::string> pair1);

    void merge_another_rambo(RAMBO &b);

    boost::dynamic_bitset<> query_bias(std::string query_key, int len, int bias);
    std::set<int> query_bias_set(std::string query_key, int len);


    int R;
    int B;
    int n;
    float p;
    int range;
    int k;
    float FPR;
    int K1;
    int bias;
    BloomFiler **Rambo_array;
    //std::vector<int> *metaRambo;
    std::unordered_set<int> *metaRambo;
    

    
};

#endif
