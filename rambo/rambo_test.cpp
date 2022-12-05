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
#include "MyRambo.h"
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
using namespace std;


int main(int argc, char **argv) {


    //test myrambo MyRamboApi(int n_perSet_, int R_all_, int B_all, int all_start_, int delta_range)

    //MyRamboApi myrambo(3000000,4,7,0,40);
    MyRamboApi myrambo(1000000,3,4,0,10);
    std::unordered_map<string, set<int>> key_set;

    //read
    fstream f("rambo_data_10000000.txt");
    int file_target = 500000;
    string line;
    int current_key_num = 0;
    int file_num = 0;
    int now_insert_num = 0;
    cout<<"start insert lines"<<endl;


    //makeint test_num = 0;
    //insert
    while(getline(f,line)){
        std::vector<string> KeySets = line2array(line, ';');
        int block_number = atoi(KeySets[1].c_str());



        now_insert_num++;
        if(now_insert_num%100000==0){
            cout<<"having insert "<<now_insert_num<<" keys!"<<endl;
        }

        key_set[KeySets[0]].insert(file_num);

        myrambo.myrambo_insert(KeySets[0],file_num);

        current_key_num++;
        if(current_key_num==file_target){
            file_num++;
            current_key_num = 0;
        }

        // test_num++;
        // if(test_num==1000000){
        //     break;
        // }
    }
    file_num++;
    current_key_num = 0;

    cout<<"rambo num:"<<myrambo.rambo_vector.size()<<endl;

    //merge
    myrambo.merge_two_neighbor_rambo(0);

    cout<<"rambo num:"<<myrambo.rambo_vector.size()<<endl;
    //query
    timeval t_start, t_end;

    double search_time = 0;

    ofstream out(argv[1]);
    double fp = 0;
    int now_key_num = 0;

    gettimeofday( &t_start, NULL);
    for(auto &x:key_set){
        out<<x.first<<":"<<endl;
        int true_num = x.second.size();
        
        for(auto &y:x.second){
            out<<y<<",";
        }

        out<<endl;
        out<<"======================================================"<<endl;
        
        vector<int> guess = myrambo.myrambo_search(x.first);
        int guess_num = guess.size();
        for(auto &y:guess){
            out<<y<<",";
        }
        out<<endl;
        if(guess_num<true_num){
            cout<<"wrong: "<<x.first<<endl;
            cout<<endl;
        }



        out<<((guess_num-true_num)*0.1)/(guess_num*0.1)<<endl;

        fp+=((guess_num-true_num)*0.1)/(guess_num*0.1);

        now_key_num++;
        if(now_key_num%10000==0){
            cout<<"having searched "<<now_key_num<<" keys!"<<endl;
        }

    }
    gettimeofday( &t_end, NULL);

    double delta_t = (t_end.tv_sec-t_start.tv_sec) + 
                (t_end.tv_usec-t_start.tv_usec)/1000000.0;
    search_time += delta_t;

    cout<<"use for cycle:"<<endl;
    cout<<"key set size:"<<key_set.size()<<endl;
    cout<<"average fp:"<<(fp*1.0)/(key_set.size())<<endl;
    out<<"use for cycle:"<<endl;
    out<<"key set size:"<<key_set.size()<<endl;
    out<<"average fp:"<<(fp*1.0)/(key_set.size())<<endl;
    out<<"rambo search time:"<<search_time<<endl;



    // // tbb query //threadpool query
    // timeval t_start2, t_end2;
    // double search_time2 = 0;
    // double fp2 = 0;
    // int now_key_num2 = 0;
    // gettimeofday( &t_start2, NULL);
    // for(auto &x:key_set){
    //     //out<<x.first<<":"<<endl;
    //     int true_num = x.second.size();
        
    //     // for(auto &y:x.second){
    //     //     out<<y<<",";
    //     // }

    //     // out<<endl;
    //     // out<<"======================================================"<<endl;
        
    //     //tbb::concurrent_vector<tbb::concurrent_vector<int>> guess = myrambo.myrambo_search_tbb(x.first);

    //     auto guess = myrambo.myrambo_search_threadpool(x.first);
    //     int guess_num = 0;
    //     for(int k=0;k<guess.size();k++){
    //         guess_num+=guess[k].size();
    //     }
    //     // for(auto &y:guess){
    //     //     out<<y<<",";
    //     // }
    //     // out<<endl;
    //     if(guess_num<true_num){
    //         cout<<"wrong: "<<x.first<<endl;
    //         cout<<endl;
    //     }



    //     //out<<((guess_num-true_num)*0.1)/(guess_num*0.1)<<endl;

    //     fp2+=((guess_num-true_num)*0.1)/(guess_num*0.1);

    //     now_key_num2++;
    //     if(now_key_num2%10000==0){
    //         cout<<"having searched "<<now_key_num2<<" keys!"<<endl;
    //     }

    // }
    // gettimeofday( &t_end2, NULL);

    // double delta_t2 = (t_end2.tv_sec-t_start2.tv_sec) + 
    //         (t_end2.tv_usec-t_start2.tv_usec)/1000000.0;
    // search_time2 += delta_t2;

    // cout<<"use for tbb:"<<endl;
    // cout<<"key set size:"<<key_set.size()<<endl;
    // cout<<"average fp:"<<(fp2*1.0)/(key_set.size())<<endl;
    // out<<"use for tbb:"<<endl;
    // out<<"key set size:"<<key_set.size()<<endl;
    // out<<"average fp:"<<(fp2*1.0)/(key_set.size())<<endl;
    // out<<"rambo search time:"<<search_time2<<endl;



// all test in main
//     int n_perSet = 3000000; //num of items(judge how to build bloomfilter)
//     int R_all = 4;  //（参数修改R！！！！！！！！！！！）
//     int B_all = 7; // （参数修改B！！！！！！！！！！！！！！！！）

//     //int K = Ki; // total number of sets
//     int K = 0;

//     double insert_raw = 0;
//     double insert_rambo = 0;
//     double search_time = 0;
//     double search_time2 = 0;

//     //test blockchain data

//     //fstream f("/home/z/rambo/ramboDB/test_modified.txt"); //（读取的文件！！！！！！！！！！！！！！！！！！！！！！！！）
//     fstream f("rambo_data_20000000.txt");
//     //fstream f("xaa");
//     //unordered_set<string> key_set;
//     std::unordered_map<string, unordered_set<int>> key_set;

//     //参数修改
//     int delta_range = 20; //每个Rambo负责file数    （参数修改位置file数！！！！！！！！！！！！！！）
//     int all_start = 0;//表示file开始号
//     int start = 0; //当前Rambo开始file
//     int end = 20;  // 当前Rambo结束file（deltarange修改后对应修改end！！！！！！！！！！！！！！）
//     const int file_target = 500000;//keys per file （参数修改：每个文件的key数量！！！！！！！！！！！！）
//     string line;

//     if((end-start)!=delta_range){
//         cout<<"delta range should equal to (end-start)"<<endl;
//     }

//     int file_num = 0;
//     int current_key_num = 0;




// //all operation 
//     //std::map<string,vector<int>> file_cache;


//     //int bias = start - 1;
//     //K = end - bias;

//     //创建Rambo
//     //cout<<"start build myrambo"<<endl;
//     //RAMBO myRambo(n_perSet, R_all, B_all, K);

//     ofstream f_write;


//     vector<RAMBO> rambo_vector;
//     RAMBO temprambo = RAMBO(n_perSet, R_all, B_all, end-start+1,start-1);
//     rambo_vector.emplace_back(temprambo); //一开始就一个Rambo
    
//     int now_insert_num = 0;
//     cout<<"start insert lines"<<endl;

//     //文件获取每行插入
//     while(getline(f,line)){
//         std::vector<string> KeySets = line2array(line, ';');
//         int block_number = atoi(KeySets[1].c_str());



//         now_insert_num++;
//         if(now_insert_num%100000==0){
//             cout<<"having insert "<<now_insert_num<<" keys!"<<endl;
//         }
//         while(file_num>=end){ //如果需要一个新的Rambo
//             start+=delta_range;
//             end+=delta_range;
//             RAMBO temprambo = RAMBO(n_perSet, R_all, B_all, end-start+1,start-1);
//             rambo_vector.emplace_back(temprambo);
//         }
//         //f_write<<KeySets[0]<<" "<<KeySets[1]<<endl;

//         //file_cache[KeySets[0]].emplace_back(block_number);

//         std::clock_t insert1 = std::clock();
//         key_set[KeySets[0]].insert(file_num);

//         std::clock_t insert2 = std::clock();

//         rambo_vector[file_num/delta_range-all_start/delta_range].createMetaRambo_single(file_num);
//         rambo_vector[file_num/delta_range-all_start/delta_range].insertion_pair(make_pair(KeySets[0], to_string(file_num)));

//         // myRambo.createMetaRambo_single(atoi(KeySets[1].c_str()));

//         //myRambo.insertion_pair(make_pair(KeySets[0], KeySets[1]));
//         std::clock_t insert3 = std::clock();

//         insert_raw += double(insert2-insert1)/CLOCKS_PER_SEC;
//         insert_rambo += double(insert3-insert2)/CLOCKS_PER_SEC;


//         current_key_num++;
//         if(current_key_num==file_target){

//             f_write.open("file/"+to_string(file_num)+".txt");
//             // for(auto &x:file_cache){
//             //     f_write<<x.first<<";";
//             //     for(auto &y:x.second){
//             //         f_write<<y<<",";
//             //     }
//             //     f_write<<endl;
//             // }

//             f_write.close();
//             //file_cache.clear();
//             file_num++;
//             current_key_num = 0;
//         }
//     }

//     //final file
//     f_write.open("file/"+to_string(file_num)+".txt");
//     // for(auto &x:file_cache){
//     //     f_write<<x.first<<";";
//     //     for(auto &y:x.second){
//     //         f_write<<y<<",";
//     //     }
//     //     f_write<<endl;
//     // }

//     f_write.close();
//     //file_cache.clear();
//     file_num++;
//     current_key_num = 0;



// // f_write.open("dpsy.txt");
// // for(auto &x:key_set){
// //     f_write<<x.first<<";";
// //     for(auto &y:x.second){
// //         f_write<<y<<",";
// //     }
// //     f_write<<endl;
// // }

// //     int t = 1;
// //     for(auto &rambo1:rambo_vector){

// //         rambo1.serializeRAMBO("rambo_save_"+to_string(delta_range)+"/"+to_string(t));
// //         t++;
// //     }



//     cout<<"start query"<<endl;
//     cout<<rambo_vector.size()<<endl;
//     //ofstream out("1110answer.txt");
    
    
    
//     ofstream out(argv[1]);
//     //out<<"bias = "<<bias<<endl;
//     out<<key_set.size()<<endl;
    
//     double fp = 0;
//     int now_key_num = 0;

//     int test_num = 0;
//     //std::clock_t search1 = std::clock();
//     timeval t_start, t_end;
//     gettimeofday( &t_start, NULL);

//     if(argv[2][0]=='0'){ //0表示查询返回的是bias bitset
//         out<<"using bitset"<<endl;
//         for (auto &x: key_set) {

            
//             test_num++;
//             if(test_num == 1000000) break;

//             int true_num = x.second.size();
//             int guess_num = 0;


            
//             // //boost::dynamic_bitset<> MemVec = myRambo.query_bias(x, x.size(), bias);
//             out<<x.first<<endl;


// //using tbb!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//             //using bitset
//             /*
//             out<<"guess answer :"<<endl;
//              */
//             // tbb::concurrent_vector<int> parallel_answer;

//             // tbb::parallel_for_each(rambo_vector.begin(), rambo_vector.end(), [&](RAMBO &r){
//             //     //std::cout << r.i << std::endl; 
//             //     int bias = r.bias;
//             //     boost::dynamic_bitset<> MemVec = r.query_bias(x.first, x.first.size(), bias);
//             //     parallel_answer.emplace_back(MemVec.count());

//             // });

//             // for(auto x:parallel_answer){
//             //     guess_num+=x;
//             // }

// //using for cycle!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//             for(int i=0;i<rambo_vector.size();i++){
                
//                 //int bias = all_start+i*delta_range-1;
//                 int bias = rambo_vector[i].bias;
                
//                 boost::dynamic_bitset<> MemVec = rambo_vector[i].query_bias(x.first, x.first.size(), bias);
//                 guess_num += MemVec.count();

// //
// //                for (size_t i = MemVec.find_first(); i != boost::dynamic_bitset<>::npos;) {
// //                    out << i+bias << " ";
// //                    //guess_num++;
// //                    i = MemVec.find_next(i);
// //                }
// //                out<<endl;
//             }

//             now_key_num++;
//             if(now_key_num%10000==0){
//                 cout<<"having searched "<<now_key_num<<" keys!"<<endl;
//             }
// /*
//             out<<"true answer:"<<endl;
//             //out<<true_num<<endl;
//             for(auto &y:x.second){
//                 out << y << " ";
//             }
//             out<<endl;
// */
//             if(guess_num<true_num){
//                 cout<<"wrong: "<<x.first<<endl;
//                 cout<<endl;
//             }



//             out<<((guess_num-true_num)*0.1)/(guess_num*0.1)<<endl;

//             fp+=((guess_num-true_num)*0.1)/(guess_num*0.1);


//         }
//     }else if(argv[2][0]=='1'){ //1表示查询返回的是std::set
//         /*
//         out<<"using set"<<endl;
//         //int a = 0;
//         for (auto &x: key_set) {
//             // a++;
//             // if(a==200000){
//             //     break;
//             // }
//             int true_num = x.second.size();
//             int guess_num = 0;
//             out<<x.first<<endl;

//             //using set
//             for(int i=0;i<rambo_vector.size();i++){

//                 set<int> MemVec = rambo_vector[i].query_bias_set(x.first, x.first.size());
//                 guess_num += MemVec.size();

//             }

//             now_key_num++;
//             if(now_key_num%10000==0){
//                 cout<<"having searched "<<now_key_num<<" keys!"<<endl;
//             }


            
            
//             if(guess_num<true_num){
//                 out<<"wrong: "<<x.first<<endl;
//                 out<<endl;
//             }
            

//             out<<((guess_num-true_num)*0.1)/(guess_num*0.1)<<endl;

//             fp+=((guess_num-true_num)*0.1)/(guess_num*0.1);


//         }
//          */


//     }else{
//         cout<<"argv[2] should be 0(use bitset) or 1(use std::set)"<<endl;
//     }
    

//     //std::clock_t search2 = std::clock();
//     gettimeofday( &t_end, NULL);



//     std::clock_t search3 = std::clock();
//     int b = 0;
//     for (auto &x: key_set) {
//       b++;
//       if(b==1000000){
//           break;
//       }
//       auto answer = x.second;
//     }

//     std::clock_t search4 = std::clock();


//     //search_time += double(search2-search1)/CLOCKS_PER_SEC;

//     double delta_t = (t_end.tv_sec-t_start.tv_sec) + 
//                 (t_end.tv_usec-t_start.tv_usec)/1000000.0;

//     search_time += delta_t;
//     search_time2 += double(search4-search3)/CLOCKS_PER_SEC;
//     out<<"key set size:"<<key_set.size()<<endl;
//     out<<"average fp:"<<(fp*1.0)/(key_set.size())<<endl;
//     out<<"raw map insert time :"<<insert_raw<<endl;
//     out<<"rambo insert time:"<<insert_rambo<<endl;
//     out<<"raw map search time:"<<search_time2<<endl;
//     out<<"rambo search time:"<<search_time<<endl;

//     f.close();
//     f_write.close();

    return 0;
}
