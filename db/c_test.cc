/* Copyright (c) 2011 The LevelDB Authors. All rights reserved.
   Use of this source code is governed by a BSD-style license that can be
   found in the LICENSE file. See the AUTHORS file for names of contributors. */

#include "leveldb/db.h"
#include "leveldb/env.h"
#include "db/version_edit.h"
#include "leveldb/write_batch.h"
#include "db/memtable.h"

#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define ACCOUNT_RANGE 1000000

std::string sha256(const std::string &srcStr) {
    unsigned char mdStr[33] = {0};
    SHA256((const unsigned char *)srcStr.c_str(), srcStr.length(), mdStr);
    return std::string((char *)mdStr, 32);
}


void getNextState(leveldb::Random &rand, std::string &key, std::string &value) {
    std::string srcStr = std::to_string(rand.Next() % ACCOUNT_RANGE);
    key = sha256(srcStr).substr(0, 20);
    value = "value";
}


void generateWriteBatch(int batch_size_, int batch_num_, leveldb::WriteBatch &write_batch_) {
    static leveldb::Random rand(1);
    std::string key;
    std::string value;
    std::string pos;
    for (int i = 0; i < batch_size_; i++) {
        getNextState(rand, key, value);
        pos = std::to_string(i + 10000 * batch_num_);
        write_batch_.Put(key, pos,batch_num_);
    }
    return;
}

leveldb::MemTable* mem_;
struct Epoch{
  int min_height_;
  int max_heigth_;

  //filter:RAMBO
  //node snapshot

  //file list
  std::vector<std::pair<int,leveldb::FileMetaData*>> file_list;

  void AppendFile(leveldb::FileMetaData* file_meta){

  }
};


int main(int argc, char** argv) {
  std::cout<<"1"<<std::endl;
  // leveldb::Env* env=leveldb::Env::Default();
  
  leveldb::DB* db=nullptr;
  leveldb::Options opts;
  opts.create_if_missing = true;
  leveldb::Status s=leveldb::DB::Open(opts,"/tmp/test",&db);
  std::cout<<"2:"<<s.ToString()<<std::endl;
  leveldb::WriteBatch batch1;
  

  std::cout<<"3"<<std::endl;
  std::string db_state;
  for(int i=0;i<10000;++i){
    generateWriteBatch(1000,i,batch1);
    db->Write(leveldb::WriteOptions(),&batch1);
    db->GetProperty("leveldb.stats", &db_state);
    std::cout<<i<<db_state<<std::endl;
  }
  std::cout<<"4:"<<s.ToString()<<std::endl;
  delete db;
  return 0;
}
