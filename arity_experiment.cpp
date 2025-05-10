#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <random>
#include <cstring>
#define BLOCK_SIZE 4096  // Size of each block in bytes
#define ARITY 4  // Example arity, can be set based on user input
#define M (1 * 1024 * 1024)  // 1MB = 1024 * 1024 bytes, the RAM size, deberia ser 50 MB
using namespace std;
extern void mergeSort(const std::string& filename, int arr_size, int arity);
// Function to perform external merge sort
double external_merge_sort(const std::string& input, int N, int arity) {
    // El reloj de la vida comienza
    clock_t start_time = clock();
    mergeSort(input,N,0);
    // El reloj de la vida termina
    clock_t end_time = clock();
    //pasamos el valor a un tiempo legible
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    cout << "Execution Time: " << time_taken << " seconds" << endl;
    return time_taken;
}
//funcion para generar sequencia al azar
std::vector<uint64_t> generate_sequence(int num_elements) {
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
    outfile.close();
}
//busqueda ternaria para encontrar aridad ideal
int ternary_search(int i,int j,int arr[],const std::string& input,int N){
    int nelm=j-i;
    if(nelm<4){
        // nelemento suficientemente chico
        double to=external_merge_sort(input, N, arr[i]);
        int min=arr[i];
        int k=i+1;
        while(k<j){
            double tk=external_merge_sort(input, N, arr[k]);
            if(tk<to){
                // cambiamos el minimo
                to=tk;
                min=arr[k];
            }
            k++;
        }
        return min;
    }
    // tomamos el elemento en 1/3 y 2/3
    int mid1 = i + (j - i) / 3;
    int mid2 = j - (j - i) / 3;
    // sacamos los tiempos con la aridad mid1 y mid2
    double t1=external_merge_sort(input, N, arr[mid1]);
    double t2=external_merge_sort(input, N, arr[mid2]);
    if(t1>t2){
        return ternary_search(mid1,j,arr,input,N);
    }
    return ternary_search(i,mid1,arr,input,N);
}

int main() {
    
    cout << ">>> external_merge_sort started" << endl;

    int nelementos=60*M/sizeof(uint64_t);
    std::vector<uint64_t> sequence = generate_sequence(nelementos);
   
    std::string input = "input.bin";
    write_to_binary_file(input,sequence);
    // Set N (total data size) and arity (number of subarrays to merge at once)
    
    int b=4096/sizeof(uint64_t);
    int start=2;
    int end=b;
    int size = end - start + 1;
    int arr[size];
    for (int i = 0; i < size; ++i) {
        arr[i] = start + i;
    } 
    int arity = ternary_search(0,size-1,arr,input,nelementos); // Arity of the merge (set to 4 here)

    // Perform external merge sort
    cout << "Optimal arity: " << arity << endl;

    return 0;
}