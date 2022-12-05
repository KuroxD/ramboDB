//
// Created by dase on 11/23/22.
//

#include "MyRambo.h"

//todo 后续将main函数里的东西整合成类使用
MyRamboApi::MyRamboApi(int n_perSet_, int R_all_, int B_all_, int all_start_, int delta_range_) {
    n_perSet = n_perSet_; //num of items(judge how to build bloomfilter)
    R_all = R_all_;
    B_all = B_all_;
    all_start = all_start_;
    delta_range = delta_range_;
    start = all_start_;
    end = delta_range_;
    pool = new ThreadPool(64);
    RAMBO temprambo = RAMBO(n_perSet, R_all, B_all, end-start+1,start-1);
    rambo_vector.emplace_back(temprambo);
}

MyRamboApi::~MyRamboApi() {
    delete pool;

}
void MyRamboApi::myrambo_insert(std::string key,int file_num){
    while(file_num>=end){ //如果需要一个新的Rambo
        start+=delta_range;
        end+=delta_range;
        RAMBO temprambo = RAMBO(n_perSet, R_all, B_all, end-start+1,start-1);
        rambo_vector.emplace_back(temprambo);
    }

    rambo_vector[file_num/delta_range-all_start/delta_range].createMetaRambo_single(file_num);
    rambo_vector[file_num/delta_range-all_start/delta_range].insertion_pair(std::make_pair(key, std::to_string(file_num)));


}


std::vector<int> MyRamboApi::myrambo_search(std::string key){
        std::vector<int> answer;

        for(int i=0;i<rambo_vector.size();i++){
            
            //int bias = all_start+i*delta_range-1;
            int bias = rambo_vector[i].bias;
            
            boost::dynamic_bitset<> MemVec = rambo_vector[i].query_bias(key, key.size(), bias);
            for (size_t i = MemVec.find_first(); i != boost::dynamic_bitset<>::npos;) {
                answer.emplace_back(i+bias);
                i = MemVec.find_next(i);
            }

        }
        return answer;
};


std::vector<std::vector<int>> MyRamboApi::myrambo_search_threadpool(std::string key){

        std::vector< std::future<std::vector<int>> > results;
        for(int i = 0; i < rambo_vector.size(); ++i) {
            results.emplace_back(
                pool->enqueue([&](int num) { 
                    std::vector<int> answer;
                    int bias = rambo_vector[num].bias;
                    boost::dynamic_bitset<> MemVec = rambo_vector[num].query_bias(key, key.size(), bias);
                    for (size_t i = MemVec.find_first(); i != boost::dynamic_bitset<>::npos;) {
                        answer.emplace_back(i+bias);
                        i = MemVec.find_next(i);
                    }      
                    return answer; 
                }, i)
            );
        }
        std::vector<std::vector<int>> answers;
        for(auto &x:results){
            answers.emplace_back(x.get());
        }

        return answers;
};


tbb::concurrent_vector<tbb::concurrent_vector<int>> MyRamboApi::myrambo_search_tbb(std::string key){

        tbb::concurrent_vector<tbb::concurrent_vector<int>> answers(rambo_vector.size());


        tbb::parallel_for((size_t)0,(size_t)rambo_vector.size(),[&](size_t i){
            tbb::concurrent_vector<int> answer;
            int bias = rambo_vector[i].bias;
            boost::dynamic_bitset<> MemVec = rambo_vector[i].query_bias(key, key.size(), bias);
            for (size_t i = MemVec.find_first(); i != boost::dynamic_bitset<>::npos;) {
                answer.emplace_back(i+bias);
                i = MemVec.find_next(i);
            }
            answers[i] = answer;
        });

        return answers;
};


void MyRamboApi::merge_two_neighbor_rambo(int r1){
    if(!(r1>=0&&r1<rambo_vector.size())){
        return;
    }
    rambo_vector[r1].merge_another_rambo(rambo_vector[r1+1]);
    rambo_vector.erase(rambo_vector.begin() + r1 + 1);

};