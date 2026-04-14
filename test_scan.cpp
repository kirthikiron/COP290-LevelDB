#include <iostream>
#include <vector>
#include <string>
#include "leveldb/db.h"

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

    // 2. Insert some test data (Alphabetical)
    leveldb::WriteOptions write_options;
    db->Put(write_options, "apple", "red");
    db->Put(write_options, "banana", "yellow");
    db->Put(write_options, "cherry", "red");
    db->Put(write_options, "date", "brown");
    db->Put(write_options, "elderberry", "purple");

    // 3. Test the new Range Scan API!
    // We want a half-open interval ["b", "e")
    // This should grab banana, cherry, and date, but STOP before elderberry.
    leveldb::ReadOptions read_options;
    std::vector<std::pair<std::string, std::string>> results;

    std::cout << "Scanning from 'apple' to 'date'...\n";
    status = db->Scan(read_options, "apple", "date", &results);

    // 4. Print the results
    if (status.ok()) {
        std::cout << "Scan successful! Found " << results.size() << " items:\n";
        for (const auto& pair : results) {
            std::cout << "Key: " << pair.first << " => Value: " << pair.second << "\n";
        }
    } else {
        std::cerr << "Scan failed: " << status.ToString() << std::endl;
    }

    // 5. Clean up
    delete db;
    return 0;
}