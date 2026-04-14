#include <iostream>
#include <string>
#include "leveldb/db.h"

// run: g++ -std=c++17 test_delete.cpp -o test_delete -Iinclude libleveldb.a -lpthread
// then ./test_delete

int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;

    // 1. Open the database
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    if (!status.ok()) {
        std::cerr << "Failed to open database: " << status.ToString() << std::endl;
        return 1;
    }

    // 2. Insert test data
    std::cout << "Inserting test data...\n";
    leveldb::WriteOptions write_options;
    db->Put(write_options, "apple", "red");
    db->Put(write_options, "banana", "yellow");
    db->Put(write_options, "cherry", "red");
    db->Put(write_options, "date", "brown");
    db->Put(write_options, "elderberry", "purple");
    db->Put(write_options, "fig", "green");
    db->Put(write_options, "grape", "purple");

    // 3. Test the Range Delete API
    // Half-open interval ["banana", "fig")
    // This MUST delete: banana, cherry, date, elderberry.
    // This MUST KEEP: apple, fig, grape.
    std::cout << "\nExecuting DeleteRange from 'banana' to 'fig'...\n";
    status = db->DeleteRange(write_options, "bananb", "fig");
    
    if (!status.ok()) {
        std::cerr << "DeleteRange failed: " << status.ToString() << std::endl;
        delete db;
        return 1;
    }

    // 4. Verify the database contents using a standard iterator
    std::cout << "\nScanning remaining database keys:\n";
    std::cout << "---------------------------------\n";
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    
    int count = 0;
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::cout << "Key: " << it->key().ToString() << " => Value: " << it->value().ToString() << "\n";
        count++;
    }
    
    std::cout << "---------------------------------\n";
    std::cout << "Total keys remaining: " << count << " (Expected: 3)\n";

    // 5. Clean up
    delete it;
    delete db;
    return 0;
}