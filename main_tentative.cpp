#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <queue>
#include <random>
#include <ctime>
#define B 4096 // Tamaño de los bloques en bytes
#define M (50 * 1024 * 1024) // Tamaño de la memoria principal en bytes
#define ARITY 70 //aridad optima
extern void quickSort(const std::string& filename, std::string& outfile, uint64_t arr_size, int arity, uint64_t &Ios);
extern void mergeSort(const std::string& filename, uint64_t arr_size, int arity, std::string& output_file, uint64_t& IOs, uint64_t depth = 0);
void generate(std::string input_file,int i){
    std::ofstream w(input_file, std::ios::binary);
    if (!w.is_open()) {
        std::cerr << "Error al abrir " << input_file << "en el main" << std::endl;
        exit(1);
    }
    std::cout << "Generando archivo de entrada..." << std::endl;
    std::mt19937_64 mt(42);
    std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());
    const uint64_t N = i * (M / sizeof(uint64_t));
    std::vector<uint64_t> buffer(B);
    uint64_t escritos = 0;
    while (escritos < N) {
        uint64_t chunk = std::min<uint64_t>(B, N - escritos);
        for (uint64_t i = 0; i < chunk; ++i) {
            buffer[i] = dist(mt);
        }
        w.write(reinterpret_cast<const char*>(buffer.data()), chunk * sizeof(uint64_t));
        escritos += chunk;
    }
    w.close();
    std::cout << "Archivo de entrada generado." << std::endl;
}
int main(){
    int arity=ARITY;
    for(int i=4;i<64;i=i+4){
        double total_time_mergesort=0;
        double total_time_quicksort=0;
        uint64_t total_IOs_mergesort = 0;
        uint64_t total_IOs_quicksort = 0;
        uint64_t arr_size = i * (M / sizeof(uint64_t));
        for(int j=0;j<5;j++){
            std::string input_file =std::to_string(i)+"M_test_seq"+std::to_string(j);
            generate(input_file,i);
            std::string output_file = std::to_string(i)+"M_output" + std::to_string(j) + ".bin";
            uint64_t IOs_mergesort = 0;
            clock_t start_time_mergesort = clock();
            mergeSort(input_file, arr_size, arity, output_file, IOs_mergesort, 0);
            clock_t end_time_mergesort = clock();
            double time_taken_mergesort = ((double)(end_time_mergesort - start_time_mergesort)) / CLOCKS_PER_SEC;
            total_time_mergesort+=time_taken_mergesort;
            total_IOs_mergesort+=IOs_mergesort;
            uint64_t IOs_quicksort = 0 ;
            clock_t start_time_quicksort = clock();
            quickSort(input_file,output_file, arr_size,arity,IOs_quicksort);
            clock_t end_time_quicksort = clock();
            double time_taken_quicksort = ((double)(end_time_quicksort - start_time_quicksort)) / CLOCKS_PER_SEC;
            total_time_quicksort+=time_taken_quicksort;
            total_IOs_quicksort+=IOs_quicksort;
            std::remove(input_file.c_str());
            std::remove(output_file.c_str());
        }
        double promtotal_time_mergesort=total_time_mergesort/5;
        uint64_t promtotal_IOs_mergesort=total_IOs_mergesort/5;
        double promtotal_time_quicksort=total_time_quicksort/5;
        uint64_t promtotal_IOs_quicksort=total_IOs_quicksort/5;
        std::cout << "Mergesort:Tiempo promedio de archivos de tamaño" << i <<"M es: "<<promtotal_time_mergesort << std::endl;
        std::cout << "Mergesort:IOs promedio de archivos de tamaño: " << i <<"M es: "<<promtotal_IOs_mergesort  << std::endl;
        std::cout << "Quicksort:Tiempo promedio de archivos de tamaño" << i <<"M es: "<<promtotal_time_quicksort << std::endl;
        std::cout << "Quicksort:IOs promedio de archivos de tamaño: " << i <<"M es: "<<promtotal_IOs_quicksort  << std::endl;

    }
    return 1;
}