//
// Created by dase on 11/23/22.
//

#ifndef RAMBO_BLOCKRANGESEARCH_MYRAMBO_H
#define RAMBO_BLOCKRANGESEARCH_MYRAMBO_H

#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <chrono>
#include "MyBloom.h"
#include "MurmurHash3.h"
#include "Rambo_construction.h"
#include "utils.h"
#include "constants.h"
#include "bitArray.h"
#include <ctime>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <future>
#include <tbb/tbb.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ThreadPool.h"
class MyRamboApi{
public:
    MyRamboApi(int n_perSet_,int R_all_,int B_all,int all_start_,int delta_range);

    ~MyRamboApi();

    void myrambo_insert(std::string key,int file_num);

    std::vector<int> myrambo_search(std::string key);

    void merge_two_neighbor_rambo(int r1); //r1表示Rambo_vector下标,r1试图与r1+1号Rambo合

    std::vector<std::vector<int>> myrambo_search_threadpool(std::string key);

    tbb::concurrent_vector<tbb::concurrent_vector<int>> myrambo_search_tbb(std::string key);



    ThreadPool* pool;
    std::vector<RAMBO> rambo_vector;
    int n_perSet;
    int R_all;
    int B_all;
    int all_start;
    int start;
    int end;
    int delta_range;
};






#endif //RAMBO_BLOCKRANGESEARCH_MYRAMBO_H
