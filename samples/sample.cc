// samples in https://github.com/google/leveldb/blob/main/doc/index.md

#include <iostream>
#include <cassert>
#include "leveldb/db.h"
#include "leveldb/write_batch.h"

int main() {
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status s = leveldb::DB::Open(options, "/tmp/testdb", &db);
  if (!s.ok()) {
    std::cerr << s.ToString() << std::endl;
    return -1;
  }

  // put
  std::string key1{"user1"};
  if (s.ok()) s = db->Put(leveldb::WriteOptions(), key1, "zzz");

  // get
  std::string value;
  if (s.ok()) s = db->Get(leveldb::ReadOptions(), key1, &value);
  std::cout << "key=" << key1 << ", value=" << value << "\n";

  // use WriteBatch to atomically apply a set of updates:
  std::string key2{"user2"};
  if (s.ok()) {
    leveldb::WriteBatch batch;
    batch.Delete(key1);
    batch.Put(key2, value);
    s = db->Write(leveldb::WriteOptions(), &batch);
  }

  // synchronous writes (default is asynchronous)
  leveldb::WriteOptions write_options;
  write_options.sync = true;
  db->Put(write_options, "user3", "yyy");

  // iteration
  leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    assert(it->status().ok()); // Check for any errors found during the scan
    std::cout << it->key().ToString() << ": " << it->value().ToString() << "\n";
  }
  delete it;

  // close db
  if (db) delete db;
  return 0;
}