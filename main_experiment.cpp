#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>

#define BLOCK_SIZE 4096  // Size of each block in bytes
#define ARITY 4  // Example arity, can be set based on user input

using namespace std;

// Function to perform external merge sort
void external_merge_sort(ifstream &input, ofstream &output, long N, int arity) {
    // Buffer for reading and writing data (block size)
    vector<char> buffer(BLOCK_SIZE);
    
    // Disk access and time tracking
    int read_accesses = 0;
    int write_accesses = 0;
    clock_t start_time = clock();
    
    // Step 1: Read the file into blocks of size B (BLOCK_SIZE)
    long blocks_to_read = N / BLOCK_SIZE;
    
    // Read blocks and process data (simplified here, real merge logic is needed)
    for (long i = 0; i < blocks_to_read; i++) {
        input.read(buffer.data(), BLOCK_SIZE);
        size_t read_size = input.gcount();
        if (read_size < BLOCK_SIZE) {
            // Process last block
            break;
        }
        read_accesses++;
        // Perform in-memory sorting on the buffer (e.g., sorting a small chunk of data)
    }

    // Step 2: Implement the merge step (merge "arity" number of subarrays)
    // For simplicity, assume we're merging arity subarrays in the external merge sort
    
    // Merge blocks using the given arity
    // Here you'd need a more complex merging function (using priority queues, for example)

    // For simplicity, let's assume merging is completed and we're writing back to disk

    // Step 3: Write sorted blocks to disk
    output.write(buffer.data(), BLOCK_SIZE);
    write_accesses++;

    // Final statistics
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    cout << "Execution Time: " << time_taken << " seconds" << endl;
    cout << "Disk Reads: " << read_accesses << endl;
    cout << "Disk Writes: " << write_accesses << endl;
}

int main() {
    // Open input and output binary files
    ifstream input("input.bin", ios::binary);
    ofstream output("output.bin", ios::binary);

    if (!input || !output) {
        cerr << "Error opening files!" << endl;
        return 1;
    }

    // Set N (total data size) and arity (number of subarrays to merge at once)
    long N = 60 * 1024 * 1024;  // Example size (60MB)
    int arity = ARITY;  // Arity of the merge (set to 4 here)

    // Perform external merge sort
    external_merge_sort(input, output, N, arity);

    // Close files
    input.close();
    output.close();

    return 0;
}
