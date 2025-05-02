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
double external_merge_sort(const char* input, const char* output, long N, int arity) {
    // Buffer for reading and writing data (block size)

    // Disk access and time tracking
    clock_t start_time = clock();
    int num_runs = createInitialRunsBlockBased(input);
    mergeFilesarity(output, num_runs,arity);
    // Final statistics
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    cout << "Execution Time: " << time_taken << " seconds" << endl;
    return time_taken;
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
int ternary_search(int i,int j,int arr[],const char* input, const char* output,int N){
    int nelm=j-i;
    if(nelm<4){
        double to=external_merge_sort(input, output, N, arr[i]);
        int min=arr[i];
        int k=i+1;
        while(k<j){
            double tk=external_merge_sort(input, output, N, arr[k]);
            if(tk<to){
                to=tk;
                min=arr[k];
            }
            k++;
        }
        return min;
    }
    int mid1 = i + (j - i) / 3;
    int mid2 = j - (j - i) / 3;
    double t1=external_merge_sort(input, output, N, arr[mid1]);
    double t2=external_merge_sort(input, output, N, arr[mid2]);
    if(t1>t2){
        return ternary_search(mid1,j,arr,input,output,N);
    }
    return ternary_search(i,mid1,arr,input, output,N);
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
    long N = 60 * 1024 * 1024;// Example size (60MB)
    int b=4096/sizeof(uint64_t);
    int start=2;
    int end=b;
    int size=end-start+1;
    int arr[size];
    for (int i=start;i<size;i++){
        arr[i-start]=i;
    } 
    int arity = ternary_search(0,size-1,arr,input,output,N);  // Arity of the merge (set to 4 here)

    // Perform external merge sort
    cout << "Optimal arity: " << arity << endl;
    // Close files

    return 0;
}
