#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <random>

#include "ternary_search.hpp"

#define B 4096
#define M (50 * 1024 * 1024)

std::vector<uint64_t> read_block_at(int offset, const std::string& file, uint64_t &Ios) {
    uint64_t num_size = B / sizeof(uint64_t);
    std::vector<uint64_t> ret(num_size);

    std::ifstream in(file, std::ios::binary);

    if (!in.is_open()) {
        std::cerr << "Error al abrir el archivo de lectura" << file
                  << std::endl;
        std::exit(1);
    }

    std::streampos file_offset = offset * B;
    in.seekg(file_offset);

    in.read(reinterpret_cast<char *>(ret.data()), B);
    Ios++;

    if (!in) {
        std::cerr << "Error al leer el bloque en posicion" << offset << std::endl;
        std::exit(1);
    }

    return ret;
}


void ram_mergeSort(const std::string& filename, uint64_t arr_size, std::string& output_filename, uint64_t& Ios) {
    // Leer el archivo en bloques de tamaño B
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error al abrir " << filename << std::endl;
        exit(1);
    }
    // Lo guardamos en un vector de enteros
    std::vector<uint64_t> A(arr_size);
    uint64_t leidos = 0;
    while (leidos < arr_size) {
        uint64_t chunk = std::min<uint64_t>(B, arr_size - leidos);
        in.read(reinterpret_cast<char*>(A.data() + leidos), chunk * sizeof(uint64_t));
        // Actualizar I/Os
        Ios++;
        leidos += chunk;
    }
    in.close();
    // Ordenar el vector en memoria
    std::sort(A.begin(), A.end());
    std::ofstream out(output_filename, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error al abrir " << output_filename << std::endl;
        exit(1);
    }
    uint64_t escritos = 0;
    // Escribir el vector ordenado en nuestro archivo output
    while (escritos < arr_size) {
        uint64_t chunk = std::min<uint64_t>(B, arr_size - escritos);
        out.write(reinterpret_cast<const char*>(A.data() + escritos), chunk * sizeof(uint64_t));
        // Actualizar I/Os
        Ios++;
        escritos += chunk;
    }
    out.close();
}
std::vector<uint64_t> choose_pivots(const std::vector<uint64_t> &block, int pivots) {

    std::vector<uint64_t> ret(pivots);

    for (int i = 0; i < pivots; i++) {

        int index = rand() % block.size();
        ret[i] = block[index];
    }

    return ret;
}

std::vector<uint64_t> divide_input(std::vector<std::string> &outfiles,const std::string &inputfile, int arr_size,const std::vector<uint64_t> &pivots, uint64_t &Ios) {

    // Abrir los a subarreglos
    int k = outfiles.size();
    std::vector<std::ofstream> outputs(k);
    for (int i = 0; i < k; ++i) {
        outputs[i].open(outfiles[i], std::ios::binary);
        if (!outputs[i].is_open()) {
            std::cerr << "No se pudo abrir " << outfiles[i] << std::endl;
            exit(1);
        }
    }

    //Abrir input del algoritmo
    std::ifstream in(inputfile, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "No se pudo abrir " << inputfile << std::endl;
        exit(1);
    }

    std::vector<std::vector<uint64_t>> buffers(k, std::vector<uint64_t>(B/sizeof(uint64_t)));
    std::vector<uint64_t> temp(B/sizeof(uint64_t));
    std::vector<int> sizes(k, 0);


    std::vector<uint64_t> subfile_size(k);

    while (in.peek()!= EOF) {
        // Leo un bloque
        in.read(reinterpret_cast<char*>(temp.data()), B);
        Ios++;
        size_t bytes_leidos = static_cast<size_t>(in.gcount());

        if (bytes_leidos == 0) {
        break;
        }

        size_t temp_size = bytes_leidos / sizeof(uint64_t);

        // Falta cambiar este bloque para escriba en bloques
        for (size_t i=0; i < temp_size; i++){

            auto place = std::lower_bound(pivots.begin(), pivots.end(), temp[i]);
            int partition_index = std::distance(pivots.begin(), place);

            uint64_t n = temp[i];

            subfile_size[partition_index]++;

            buffers[partition_index][sizes[partition_index]] = n;
            sizes[partition_index]++;                      

            if (sizes[partition_index] == (B/sizeof(uint64_t))) {

                outputs[partition_index].write(
                    reinterpret_cast<const char*>(buffers[partition_index].data()), B);
                    Ios++;
                

                sizes[partition_index] = 0;

            }
        }
    }
    // Escribir datos restantes de los buffers de salida
    for (int i = 0; i < k; i++) {
        if (sizes[i] != 0) {
            outputs[i].write(reinterpret_cast<const char*>(buffers[i].data()), sizes[i] * sizeof(uint64_t));
            Ios++;
        }
    }
    // Cerrar archivos
    for (int i = 0; i < k; ++i) {
        outputs[i].close();
    }

    return subfile_size;
}

void merge_files(std::vector<std::string> arr_files, std::string &outfile, int arity, uint64_t &Ios) {

    std::vector<std::ifstream> arr_streams(arity);

    // Abrimos los a subarreglos
    for (int i = 0; i < arity; i++) {
        arr_streams[i].open(arr_files[i], std::ios::binary);
        if (!arr_streams[i].is_open()) {
            std::cerr << "No se pudo abrir " << arr_files[i] << std::endl;
            exit(1);
        }
    }
    // Abrir archivo de salida
    std::ofstream out(outfile, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "No se pudo abrir " << outfile<< std::endl;
        exit(1);
    }

    std::vector<uint64_t> buffer(B / sizeof(uint64_t));

    for (int i = 0; i < arity; ++i) {
        while (arr_streams[i].read(reinterpret_cast<char*>(buffer.data()), B)) {
            out.write(reinterpret_cast<char*>(buffer.data()), B);
            Ios++;
        }

        // Manejo del último bloque parcial (si lo hay)
        std::streamsize bytes_read = arr_streams[i].gcount();
        if (bytes_read > 0) {
            out.write(reinterpret_cast<char*>(buffer.data()), bytes_read);
            Ios++;
        }

        arr_streams[i].close();
    }

    out.close();
}


void quickSort(const std::string& filename, std::string& outfile, uint64_t arr_size, int arity, uint64_t &Ios) {

    srand(0);

    if (arr_size < M) {
        ram_mergeSort(filename, arr_size, outfile, Ios);
        return;
    }

    else {

        // Tomar un bloque aleatorio de memoria
        int random_block = rand() % (arr_size/B);
        std::vector<uint64_t> block = read_block_at(random_block, filename, Ios);

        // Elegir a-1 pivotes
        std::vector<uint64_t> pivots = choose_pivots(block, arity-1);

        // ordenar el bloque en memoria
        sort(pivots.begin(), pivots.end());
        
        // Arreglo de los filenames de los subarreglos
        std::vector<std::string> file_strings(arity);

        std::vector<std::string> temp_outputs(arity);
        // particionar en a subarreglos
        for (int i = 0; i<arity; i++) {

            file_strings[i] = filename + "-" + std::to_string(i) + ".bin";
            temp_outputs[i] = outfile + "-" + std::to_string(i) + ".bin";
        }

        // escribimos elementos en cada uno de los subarreglos según los pivotes
        std::vector<uint64_t> files_size = divide_input(file_strings, filename, arr_size, pivots, Ios);

        // Aplicar quicksort a cada subarreglo
        for (int i = 0; i<arity; i++) {
            quickSort(file_strings[i], temp_outputs[i], files_size[i], arity, Ios);// CAMBIAR ARR_SIZE
        }


        // Escribir todo en el output

        merge_files(temp_outputs, outfile, arity, Ios);

        // Eliminar los archivos de salida
        for (const auto& f: file_strings) std::remove(f.c_str());
        for (const auto& f: temp_outputs) std::remove(f.c_str());

    }
}