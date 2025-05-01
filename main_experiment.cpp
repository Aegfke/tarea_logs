#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <random>
#define BLOCK_SIZE 4096  // Size of each block in bytes
#define ARITY 4  // Example arity, can be set based on user input
#define M (1 * 1024 * 1024)  // 1MB = 1024 * 1024 bytes, the RAM size, deberia ser 50 MB
using namespace std;
extern int createInitialRunsBlockBased(const char* input_file);
extern void mergeFilesarity(const char* output_file, int num_runs, int arity);
// Function to perform external merge sort
void external_merge_sort(const char* input, const char* output, long N, int arity) {
    // Buffer for reading and writing data (block size)

    // Disk access and time tracking
    clock_t start_time = clock();
    int num_runs = createInitialRunsBlockBased(input);
    mergeFilesarity(output, num_runs,arity);
    // Final statistics
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    cout << "Execution Time: " << time_taken << " seconds" << endl;
}
std::vector<uint64_t> generate_sequence(size_t num_elements) {
    std::vector<uint64_t> sequence(num_elements);
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    for (auto& elem : sequence) {
        elem = dis(gen);
    }
    return sequence;
}
void write_to_binary_file(const std::string& filename, const std::vector<uint64_t>& sequence) {
    std::ofstream outfile(filename, std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(sequence.data()), sequence.size() * sizeof(uint64_t));
}
//binary search of the n element in the array between i and j positions
int binary_search(int n,int i,int j,int arr[]){
    int mid=i+(j-i)/2;
    if(arr[mid]==n){
        return arr[mid];
    }
    if (arr[mid]<n){
        return binary_search(n,mid+1,j,arr);
    }
    if(arr[mid]>n){
        return binary_search(n,i,mid-1,arr); 
    }
    return -1;
}
int main() {
    // Open input and output binary files
    cout << ">>> external_merge_sort started" << endl;

    size_t nelementos=60*M/sizeof(uint64_t);
    std::vector<uint64_t> sequence = generate_sequence(nelementos);
    write_to_binary_file("input.bin", sequence);
    const char* input = "input.bin";
    const char* output = "output.bin";
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

    return 0;
}
