#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <cstdio>
#include <cstring>

#include "../include/quicksort.hpp"

#define block 4096
#define M (1024 * 1024)




QuickSort::QuickSort(const std::string &filename, int arity) {
    this->filename;
    this->arity;
}

std::vector<uint64_t> QuickSort::read_block_at(int offset) const {
    uint64_t num_size = block / sizeof(uint64_t);
    std::vector<uint64_t> ret(num_size);

    std::ifstream in(filename, std::ios::binary);

    if (!in.is_open()) {
        std::cerr << "Error al abrir el archivo de lectura" << filename
                  << std::endl;
        std::exit(1);
    }

    std::streampos file_offset = offset * block;
    in.seekg(file_offset);
    in.read(reinterpret_cast<char *>(&ret), block);

    if (!in) {
        std::cerr << "Error al leer el el bloque en posicion" << offset << std::endl;
        std::exit(1);
    }


    return ret; // Tal vez lo mejor es escribir esto en buffer
}

void QuickSort::read_blocks_to_ram(int start, int end) {

    for (size_t i = 0; i < RAM.size(); i++) {

        RAM.push_back(read_block_at(start + i));
    }
}

std::vector<uint64_t> QuickSort::choose_a_pivots(std::vector<uint64_t> &b, int a) const {

    std::vector<uint64_t> pivots(a);

    for (int i = 0; i < a; i++)
    {
        int index = rand() % b.size();
        pivots[i] = b[index];
    }

    return pivots;

}

void QuickSort::sort_block(std::vector<uint64_t> &b) {

    std::sort(b.begin(), b.end());
}


void QuickSort::partition(void) {

    // Crear a archivos binarios y guardar en vector

    std::vector<std::string> files;

    for (int i = 0; i < arity; i++) {

        // Name of each file

        std::string nombre = filename + "-" + std::to_string(i) + ".bin";

        std::ofstream subarray(nombre, std::ios::binary);

        if(!subarray) {
            std::cerr << "Error al abrir el archivo" << nombre << std::endl;
            std::exit(1);
        }

        files.emplace_back(subarray);

    }

    // Recursion en el archivo

    //Traer desde el ultimo bloque leido a tamaño ram bloque de memoria

    int filenamesize;

    for (int i = 0; i < filenamesize; i++) {

        read_blocks_to_ram(i, i*block);

        for (uint64_t x : RAM) {
            
            auto sub = std::lower_bound(pivots.begin(), pivots.end(), x)
        }
    }









}

namespace QuickSortUtils {
    
    void write_new_subarray(const std::string &filename) {

        std::ofstream out(filename, std::ios::binary);
        if(!out.is_open()) {
            std::cerr << "Error al abrir archivo: " << filename
                      << std::endl;
            std::exit(1);
        }
        out.close();
    }
}






