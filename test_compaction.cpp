#include <iostream>
#include <string>
#include "leveldb/db.h"

/*
    To run,
    rm -rf /tmp/testdb
    g++ -std=c++17 test_compaction.cpp -o test_compaction -Iinclude libleveldb.a -lpthread
    ./test_compaction
*/

int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;

    // Open the database
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    if (!status.ok()) {
        std::cerr << "Failed to open database: " << status.ToString() << std::endl;
        return 1;
    }

    // Insert a massive amount of data to ensure MemTable fills and flushes
    std::cout << "Inserting 100,000 records to stress the engine...\n";
    leveldb::WriteOptions write_options;
    
    for (int i = 0; i < 100000; i++) {
        std::string key = "key_" + std::to_string(i);
        std::string value = "value_which_is_intentionally_long_to_consume_bytes_" + std::to_string(i);
        db->Put(write_options, key, value);
        
        if (i % 25000 == 0 && i != 0) {
            std::cout << "Inserted " << i << " records...\n";
        }
    }
    std::cout << "Data insertion complete.\n";

    // Test the Phase 4 Manual Compaction API
    std::cout << "\nExecuting ForceFullCompaction(). Expecting engine to block...\n";
    
    // Note: Our DBImpl implementation has the printf statements, 
    // so the stats table will print automatically during this call!
    status = db->ForceFullCompaction();
    
    if (!status.ok()) {
        std::cerr << "ForceFullCompaction failed: " << status.ToString() << std::endl;
        delete db;
        return 1;
    }

    std::cout << "Compaction completed and control returned to the foreground thread!\n";

    // Clean up
    delete db;
    return 0;
}