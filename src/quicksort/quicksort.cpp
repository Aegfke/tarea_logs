#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define block 4096
#define M (1024 * 1024)
using namespace std;

std::vector<std::vector<uint64_t>> main_ram;

QuickSort::QuickSort(const std::string &filename, int arity) {
    this->filename = filename;

    std::vector<uint64_t> pivots;

    this->arity = arity;

    this-> RAM = main_ram;

    std::ifstream input(filename,std::ios::binary);
    uint64_t* myNumbers;
    input.read(reinterpret_cast<char*>(&myNumbers),sizeof(uint64_t*));
    int getArrayLength = sizeof(myNumbers) / sizeof(uint64_t);

    this-> array_length_in_blocks = getArrayLength/block;
    //--------------
}

std::vector<uint64_t> QuickSort::read_block_at(int offset, const std::string& file) const {
    uint64_t num_size = block / sizeof(uint64_t);
    std::vector<uint64_t> ret(num_size);

    std::ifstream in(file, std::ios::binary);

    if (!in.is_open()) {
        std::cerr << "Error al abrir el archivo de lectura" << file
                  << std::endl;
        std::exit(1);
    }

    std::streampos file_offset = offset * block;
    in.seekg(file_offset);
    //in.read(reinterpret_cast<char *>(&ret), block);

    in.read(reinterpret_cast<char *>(ret.data()), block);

    if (!in) {
        std::cerr << "Error al leer el bloque en posicion" << offset << std::endl;
        std::exit(1);
    }


    return ret; // Tal vez lo mejor es escribir esto en buffer
}

void QuickSort::read_blocks_to_ram(int start, const std::string& file) {

    RAM.clear();

    size_t ram_in_blocks = M/block;

    for (size_t i = 0; i < ram_in_blocks; i++) {

        RAM.push_back(read_block_at(start + i, file));
    }
}

std::vector<uint64_t> QuickSort::choose_a_pivots(std::vector<uint64_t> &b, int a) const {

    std::vector<uint64_t> pivots(a);

    for (int i = 0; i < a; i++)
    {
        int index = rand() % b.size();
        pivots[i] = b[index];
    }
    std::sort(pivots.begin(), pivots.end());
    return pivots;

}

void QuickSort::sort_block(std::vector<uint64_t> &b) {

    std::sort(b.begin(), b.end());
}

void QuickSort::write_to_binary_file(const std::string& filename, const std::vector<uint64_t>& sequence){
    std::ofstream outfile(filename, std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(sequence.data()), sequence.size() * sizeof(uint64_t));
}

void QuickSort::sort_in_memory(void) {

    read_blocks_to_ram(0, filename);

    std::vector<uint64_t> output;

    size_t total_elements = 0;
    for (const auto& vector_interno : RAM) {
        total_elements += vector_interno.size();
    }
    output.reserve(total_elements); // Optimización opcional

    for (const auto& vector_interno : RAM) {
        // Inserta los elementos del vector_interno al final de unico_vector
        output.insert(output.end(), vector_interno.begin(), vector_interno.end());
    }
}

void QuickSort::partition(void) {

    // Crear a archivos binarios y guardar en vector

    std::vector<std::string> file_names;
    std::vector<std::ofstream> files_ofstream;
    for (int i = 0; i <= arity; i++) {

        // Name of each file

        std::string nombre = filename + "-" + std::to_string(i) + ".bin";

        std::ofstream subarray(nombre, std::ios::binary);

        if(!subarray) {
            std::cerr << "Error al abrir el archivo" << nombre << std::endl;
            std::exit(1);
        }
        file_names.emplace_back(subarray);
        files_ofstream.emplace_back(subarray);

    }

    // Recursion en el archivo

    //Traer desde el ultimo bloque leido a tamaño ram bloque de memoria

    for (int i = 0; i < array_length_in_blocks; i += (M - block)) {

        // Traer bloques a memoria
        read_blocks_to_ram(i, filename);

        // 
        for (int b = 0; b < RAM.size(); b++) {
            for (int x = 0; x < block; x++) {

                
                auto place = std::lower_bound(this->pivots.begin(), this->pivots.end(), RAM[b][x]);
                int partition_index = std::distance(pivots.begin(), place);

                uint64_t n = RAM[b][x];

                files_ofstream[partition_index].write(reinterpret_cast<const char*>(&n), sizeof(uint64_t));

            }
        }




    }

    //Cerrar archivos abiertos
    for (int i = 0; i <= arity; i++) {
        files_ofstream[i].close();

         // Aplicar recursivamente Quicksort a los binarios generados
         QuickSort(file_names[i], arity).sort();//CAMBIAR N IN BLOCKS
    }

    


    // Escribir cada uno de los binarios generados de salida en filename-output.bin

    std::string out_name = filename + "-output.bin";


    std::ofstream output(out_name,std::ios::binary);
    for (int i = 0; i <= arity; i++){

        // Escribir en output
        std::ifstream in(file_names[i] + "-output.bin",std::ios::binary);
        uint64_t* myNumbers;
        in.read(reinterpret_cast<char*>(&myNumbers),sizeof(uint64_t*));
        output.write(reinterpret_cast<const char*>(&myNumbers),sizeof(uint64_t*));

        // Eliminar archivo base y salida

        int first_remove = std::remove(file_names[i].c_str());

        std::string output_name = file_names[i] + "-output.bin";
        int second_remove = std::remove(out_name.c_str());



    }

}


void QuickSort::sort(void) {

    //Si N < M Ordenamos en memoria y terminamos

    if (array_length_in_blocks < M) {

    }


    else {

        // Traemos un bloque aleatorio y lo traemos a memoria
        std::vector<uint64_t> random_block = read_block_at(rand() % array_length_in_blocks, filename);

        // Elegimos a pivotes aleatorios
        pivots = choose_a_pivots(random_block,arity);

        // Ordenamos los bloques
        sort_block(pivots);

        partition();

    }

    // Creamos a archivos binarios2

    // Escribimos en el archivo correspondiente segun pivotes2

    // Aplicamos quicksort a cada archivo2

    // Una vez terminan escribimos todo de estos archivos en orden en el output1

    // Borramos los archivos binarios creados entregados1
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

int main() {

    const std::string filename = "sequence_4M_1.bin";

    const int arity = 5;

    // Creamos el objeto Quicksort

    QuickSort main_obj = QuickSort(filename, arity);

    main_obj.sort();



    return 0;


}






