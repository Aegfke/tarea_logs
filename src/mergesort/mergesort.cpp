#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <cstdio>
#include <cstring>
using namespace std;

typedef long long ll;

#define BLOCK_SIZE (4096)   // Size of a block in elements
#define RAM_LIMIT (1 * 1024 * 1024) // 1M elements in RAM

struct MinHeapNode 
{ 
    ll element;
    int file_index;
};

void swap(MinHeapNode* a, MinHeapNode* b) 
{ 
    MinHeapNode temp = *a; 
    *a = *b; 
    *b = temp; 
} 

class MinHeap 
{ 
    MinHeapNode* heap_arr; 
    int heap_size; 

public: 
    
    MinHeapNode getMin() { return heap_arr[0]; }
    
    void replaceMin(MinHeapNode x) 
    { 
        heap_arr[0] = x; 
        MinHeapify(0); 
    }

    void MinHeapify(int i)
    { 
        int l = 2*i + 1; 
        int r = 2*i + 2; 
        int smallest = i; 
        if (l < heap_size && heap_arr[l].element < heap_arr[i].element) 
            smallest = l; 
        if (r < heap_size && heap_arr[r].element < heap_arr[smallest].element) 
            smallest = r; 
        if (smallest != i) 
        { 
            swap(&heap_arr[i], &heap_arr[smallest]); 
            MinHeapify(smallest); 
        } 
    }

    MinHeap(MinHeapNode a[], int size)
    { 
        heap_arr = a;
        heap_size = size; 
        int i = (heap_size - 1)/2; 
        while (i >= 0) 
        { 
            MinHeapify(i); 
            i--; 
        } 
    }
};

void merge(ll arr[], ll left, ll mid, ll right) 
{ 
    ll n1 = mid - left + 1;
    ll n2 = right - mid;
    
    vector<ll> L(arr + left, arr + mid + 1);
    vector<ll> R(arr + mid + 1, arr + right + 1);
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2)
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    
    while (i < n1)
        arr[k++] = L[i++];
    
    while (j < n2)
        arr[k++] = R[j++];
}

void mergeSort(ll arr[], ll l, ll r) 
{ 
    if (l < r) 
    { 
        ll m = l + (r - l) / 2; 
        mergeSort(arr, l, m); 
        mergeSort(arr, m + 1, r); 
        merge(arr, l, m, r); 
    } 
}  

FILE* openFile(const char* filename, const char* mode) 
{
    FILE* fp = fopen(filename, mode);
    if (!fp) 
    {
        perror("Error while opening file");
        exit(EXIT_FAILURE);
    }
    return fp;
}

// Write a run to a temporary file
void writeRun(ll arr[], int size, int run_num) 
{
    char filename[20];
    sprintf(filename, "run%d.bin", run_num);
    FILE* out = openFile(filename, "wb");
    fwrite(arr, sizeof(ll), size, out);
    fclose(out);
}

// Read next number from run file
bool readNext(FILE* in, ll& val) 
{
    return fread(&val, sizeof(ll), 1, in) == 1;
}

// Merge all runs into the final sorted file
void mergeFiles(const char* output_file, int num_runs) 
{
    FILE* in[num_runs];
    for (int i = 0; i < num_runs; i++) 
    {
        char filename[20];
        sprintf(filename, "run%d.bin", i);
        in[i] = openFile(filename, "rb");
    }

    FILE* out = openFile(output_file, "wb");

    MinHeapNode* heap_arr = new MinHeapNode[num_runs];

    int i;
    for (i = 0; i < num_runs; i++) 
    {
        ll val;
        if (readNext(in[i], val))
            heap_arr[i] = {val, i};
        else
            heap_arr[i] = {LLONG_MAX, i}; // Empty run
    }

    MinHeap hp(heap_arr, num_runs);

    while (true) 
    {
        MinHeapNode root = hp.getMin();
        if (root.element == LLONG_MAX)
            break;

        fwrite(&root.element, sizeof(ll), 1, out);

        ll val;
        if (readNext(in[root.file_index], val))
            root.element = val;
        else
            root.element = LLONG_MAX;

        hp.replaceMin(root);
    }

    for (i = 0; i < num_runs; i++)
        fclose(in[i]);
    fclose(out);

    delete[] heap_arr;
}
void mergeKFiles(int start_idx, int k, int pass, const char* output_file) 
{
    FILE* in[k];
    for (int i = 0; i < k; i++) 
    {
        char filename[30];
        if (pass == 0)
            sprintf(filename, "run%d.bin", start_idx + i);
        else
            sprintf(filename, "pass%d_run%d.bin", pass, start_idx + i);

        in[i] = openFile(filename, "rb");
    }

    FILE* out = openFile(output_file, "wb");

    MinHeapNode* heap_arr = new MinHeapNode[k];

    for (int i = 0; i < k; i++) 
    {
        ll val;
        if (readNext(in[i], val))
            heap_arr[i] = {val, i};
        else
            heap_arr[i] = {LLONG_MAX, i}; // Run vacío
    }

    MinHeap hp(heap_arr, k);

    while (true) 
    {
        MinHeapNode root = hp.getMin();
        if (root.element == LLONG_MAX)
            break;

        fwrite(&root.element, sizeof(ll), 1, out);

        ll val;
        if (readNext(in[root.file_index], val))
            root.element = val;
        else
            root.element = LLONG_MAX;

        hp.replaceMin(root);
    }

    for (int i = 0; i < k; i++)
        fclose(in[i]);
    fclose(out);

    delete[] heap_arr;
}
void mergeFilesarity(const char* output_file, int num_runs, int arity) 
{
    int current_run = 0;
    int pass = 0;

    // Repetir la fusión en varias etapas si hay más runs que aridad
    while (num_runs > arity) 
    {
        int new_run_num = 0;

        for (int i = 0; i < num_runs; i += arity) 
        {
            int group_size = std::min(arity, num_runs - i);

            // Crear nombre para archivo temporal de salida
            char merged_filename[30];
            sprintf(merged_filename, "pass%d_run%d.bin", pass + 1, new_run_num);

            // Mezclar los group_size archivos a uno nuevo
            mergeKFiles(i, group_size, pass, merged_filename);

            new_run_num++;
        }

        num_runs = new_run_num;
        pass++;
    }

    // Fusión final: num_runs <= arity
    mergeKFiles(0, num_runs, pass, output_file);
}
// --- NEW FUNCTION: read binary file by blocks ---
vector<ll> read_block(FILE* file, size_t block_elements, size_t& start_position) 
{
    vector<ll> block(block_elements);
    fseek(file, start_position * sizeof(ll), SEEK_SET);
    size_t read_count = fread(block.data(), sizeof(ll), block_elements, file);
    block.resize(read_count);  // shrink if less than requested
    start_position += read_count;
    return block;
}

// --- MAIN FUNCTION to create initial runs ---
int createInitialRunsBlockBased(const char* input_file) 
{
    FILE* in = openFile(input_file, "rb");

    vector<ll> ram_buffer;
    ram_buffer.reserve(RAM_LIMIT);

    size_t start_position = 0;
    int run_num = 0;

    while (true) 
    {
        vector<ll> block = read_block(in, BLOCK_SIZE, start_position);
        if (block.empty())
            break;

        // Add block to RAM buffer
        ram_buffer.insert(ram_buffer.end(), block.begin(), block.end());

        // If buffer full, sort and write
        if (ram_buffer.size() >= RAM_LIMIT) 
        {
            mergeSort(ram_buffer.data(), 0, ram_buffer.size() - 1);
            writeRun(ram_buffer.data(), ram_buffer.size(), run_num++);
            ram_buffer.clear();
        }
    }

    // After reading all blocks, if any data remains
    if (!ram_buffer.empty()) 
    {
        mergeSort(ram_buffer.data(), 0, ram_buffer.size() - 1);
        writeRun(ram_buffer.data(), ram_buffer.size(), run_num++);
        ram_buffer.clear();
    }

    fclose(in);
    return run_num;
}

// --- MAIN PROGRAM ---
/*int main() 
{
    const char* input_file = "sequence_4M_1.bin";
    const char* output_file = "sorted_output.bin";

    int num_runs = createInitialRunsBlockBased(input_file);
    mergeFiles(output_file, num_runs);

    cout << "External Merge Sort Completed. Output file: " << output_file << endl;
    return 0;
}*/

