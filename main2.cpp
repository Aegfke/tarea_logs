#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <queue>
#include <random>

#define B 4096 // Tamaño de los bloques en bytes
#define M (50 * 1024 * 1024) // Tamaño de la memoria principal en bytes

// Comparador para min-heap en priority_queue
struct MinHeapComparator {
    bool operator()(const std::pair<uint64_t, int>& a, const std::pair<uint64_t, int>& b) const {
        return a.first > b.first; // min-heap: el menor arriba
    }
};


// Función para ordenar un archivo en memoria RAM
void ram_mergeSort(const std::string& filename, uint64_t arr_size, std::string& output_filename, uint64_t& IOs) {
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
        IOs++;
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
        IOs++;
        escritos += chunk;
    }
    out.close();
}

// En merge_files, reemplaza el heap:
void merge_files(const std::vector<std::string>& input_files, const std::string& output_file, uint64_t& IOs) {
    // Abrir los archivos de entrada
    int k = static_cast<int>(input_files.size());
    std::vector<std::ifstream> inputs(k);
    for (int i = 0; i < k; ++i) {
        inputs[i].open(input_files[i], std::ios::binary);
        if (!inputs[i].is_open()) {
            std::cerr << "No se pudo abrir " << input_files[i] << std::endl;
            exit(1);
        }
    }
    // Crear o abrir el archivo de salida
    std::ofstream out(output_file, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "No se pudo abrir " << output_file << std::endl;
        exit(1);
    }
    // Crear un heap de tamaño k para almacenar los elementos mínimos de cada archivo
    // y un buffer para cada archivo de entrada
    std::vector<std::vector<uint64_t>> buffers(k, std::vector<uint64_t>(B));
    std::vector<uint64_t> pos(k, 0);
    std::vector<uint64_t> sizes(k, 0);

    // Inicializador del heap
    std::priority_queue<
        std::pair<uint64_t, int>,
        std::vector<std::pair<uint64_t, int>>,
        MinHeapComparator
    > min_heap;

    // Leer el primer bloque de cada archivo y agregar el primer elemento al heap
    for (int i = 0; i < k; ++i) {
        inputs[i].read(reinterpret_cast<char*>(buffers[i].data()), B * sizeof(uint64_t));
        IOs++;
        sizes[i] = inputs[i].gcount() / sizeof(uint64_t);
        pos[i] = 0;
        if (sizes[i] > 0) {
            min_heap.emplace(buffers[i][0], i);
            pos[i]++;
        }
    }

    // Buffer de salida de tamaño B para escribir
    std::vector<uint64_t> out_buffer(B);
    uint64_t out_count = 0;

    while (!min_heap.empty()) {
        // Obtener el valor mínimo y el indice del archivo
        auto min_node = min_heap.top();
        min_heap.pop();
        uint64_t val = min_node.first;
        int idx = min_node.second;
        out_buffer[out_count++] = val;

        // Si el buffer de salida está lleno, escribirlo
        if (out_count == B) {
            out.write(reinterpret_cast<const char*>(out_buffer.data()), B * sizeof(uint64_t));
            IOs++;
            out_count = 0;
        }
        // Si aún quedan elementos por leer del archivo, lo introducimos al heap
        if (pos[idx] < sizes[idx]) {
            min_heap.emplace(buffers[idx][pos[idx]], idx);
            pos[idx]++;
        }
        else {
            // Recargar buffer si quedan datos en el archivo
            inputs[idx].read(reinterpret_cast<char*>(buffers[idx].data()), B * sizeof(uint64_t));
            IOs++;
            sizes[idx] = inputs[idx].gcount() / sizeof(uint64_t);
            pos[idx] = 0;
            if (sizes[idx] > 0) {
                min_heap.emplace(buffers[idx][0], idx);
                pos[idx]++;
            }
            // Si no hay más elementos en el archivo, no se hace nada (ese archivo ya terminó)
        }
    }

    // Escribir los elementos restantes del buffer de salida
    if (out_count > 0) {
        out.write(reinterpret_cast<const char*>(out_buffer.data()), out_count * sizeof(uint64_t));
        IOs++;
    }
    // Cerrar los archivos de entrada y salida
    for (int i = 0; i < k; ++i) inputs[i].close();
    out.close();
}
void mergeSort(const std::string& filename, uint64_t arr_size, int arity, std::string& output_file, uint64_t& IOs, uint64_t depth = 0) {// Merge de los subarchivos en un nuevo archivo de salida
    // Si el tamaño del archivo es menor o igual a M, lo ordenamos en memoria
    if (arr_size <= M) {
        ram_mergeSort(filename, arr_size, output_file, IOs);
        return;
    }

    // Dividir el archivo en "arity" subarchivos
    std::vector<std::string> subfiles(arity);
    // Vector para almacenar el tamaño de cada subarchivo
    std::vector<uint64_t> sizes(arity, arr_size / arity);
    // Repartir el sobrante entre los primeros "arr_size % arity" subarchivos
    for (int i = 0; i < arr_size % arity; ++i) sizes[i]++;

    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error al abrir " << filename << " en mergeSort" << std::endl;
        exit(1);
    }
    // Crear los subarchivos y escribir en ellos
    for (int i = 0; i < arity; ++i) {
        // Crear subarchivos
        subfiles[i] = filename + "_part" + std::to_string(i) + + "_d" + std::to_string(depth) +  ".bin";
        std::ofstream out(subfiles[i], std::ios::binary);
        // La cantidad de enteros a leer
        uint64_t to_read = sizes[i];
        // Buffer para ayudar a lectura y escritura
        std::vector<uint64_t> buffer(B);
        while (to_read > 0) {
            uint64_t chunk = std::min<uint64_t>(B, to_read);
            in.read(reinterpret_cast<char*>(buffer.data()), chunk * sizeof(uint64_t));
            // Actualizar I/Os
            IOs++;
            out.write(reinterpret_cast<const char*>(buffer.data()), chunk * sizeof(uint64_t));
            // Actualizar I/Os
            IOs++;
            to_read -= chunk;
        }
        out.close();
    }
    in.close();

    // Ordenar recursivamente cada subarchivo
    std::vector<std::string> temp_outputs(arity);
    for (int i = 0; i < arity; ++i) {
        temp_outputs[i] = output_file + "_part" + std::to_string(i) + "_d" + std::to_string(depth) + ".bin";
        mergeSort(subfiles[i], sizes[i], arity, temp_outputs[i], IOs ,depth + 1);
    }
    // Fusionar los subarchivos ordenados en un nuevo archivo de salida
    merge_files(temp_outputs, output_file, IOs);

    // Eliminar los archivos temporales
    for (const auto& f: subfiles) std::remove(f.c_str());
    for (const auto& f: temp_outputs) std::remove(f.c_str());
}

/*
int checker(const std::string& filename, uint64_t arr_size) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error al abrir " << filename << " en el checker" << std::endl;
        exit(1);
    }
    // Leer el archivo en bloques de tamaño B
    std::vector<uint64_t> buffer(B);
    uint64_t leidos = 0;
    uint64_t ultimo = 0;
    while (leidos < arr_size) {
        uint64_t chunk = std::min<uint64_t>(B, arr_size - leidos);
        in.read(reinterpret_cast<char*>(buffer.data()), chunk * sizeof(uint64_t));
        // Actualizar I/Os
        leidos += chunk;
        // Verificar si el bloque está ordenado
        for (uint64_t i = 1; i < chunk; ++i) {
            if (i == 1) {
                if (ultimo > buffer[i]) {
                    in.close();
                    return 0;
                }
            }
            if (buffer[i] < buffer[i - 1]) {
                in.close();
                return 0;
            }
            if (i == chunk - 1) {
                ultimo = buffer[i];
            }
        }
        buffer.clear();
        buffer.resize(B);
    }
    in.close();

    return 1;
}
*/
uint64_t ternary_search2(std::string input_file,std::string output_file, int l, int r,std::vector<int>aritys,uint64_t arr_size) {
int nelm=r-l;
    if(nelm<4){
        // nelemento suficientemente chico
        uint64_t IO0=0;
        mergeSort(input_file, arr_size, aritys[l], output_file, IO0, 0);
        std::cout << "IOS:"<<IO0 << std::endl;
        int min=l;
        int k=l+1;
        while(k<r){
            uint64_t IOk=0;
            mergeSort(input_file, arr_size, aritys[k], output_file, IOk, 0);
            std::cout << "IOS:"<<IOk << std::endl;
            if(IOk<IO0){
                // cambiamos el minimo
                IO0=IOk;
                min=k;
            }
            k++;
        }
        return min;
    }
    // tomamos el elemento en 1/3 y 2/3
    int mid1 = l + (r - l) / 3;
    int mid2 = r - (r - l) / 3;
    // sacamos los tiempos con la aridad mid1 y mid2
    uint64_t IO1=0;
    uint64_t IO2=0;
    mergeSort(input_file, arr_size, aritys[mid1], output_file, IO1, 0);
    mergeSort(input_file, arr_size, aritys[mid2], output_file, IO2, 0);
    std::cout << "IOS:"<<IO1 << std::endl;
    std::cout << "IOS:"<<IO2 << std::endl;
    if(IO1>IO2){
        return ternary_search2(input_file,output_file, mid1,r,aritys,arr_size);
    }else{
        return ternary_search2(input_file,output_file,l, mid1,aritys,arr_size);
    }
}
/*int main() {
    std::string input_file = "input.bin";
    uint64_t arr_size = 60 * (M / sizeof(uint64_t));
    std::vector<int> aritys;
    int b=B/sizeof(uint64_t);
    for (int i = 2; i<b; i ++) {
        aritys.push_back(i);
    }
    // Generar un archivo de entrada con números aleatorios
    std::ofstream w(input_file, std::ios::binary);
    if (!w.is_open()) {
        std::cerr << "Error al abrir " << input_file << "en el main" << std::endl;
        exit(1);
    }
    // Generar números aleatorios enteros y escribirlos en bloques de tamaño B
    std::cout << "Generando archivo de entrada..." << std::endl;
    std::mt19937_64 mt(42);
    std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());
    const uint64_t N = 60 * (M / sizeof(uint64_t));
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
    std::cout << "Comenzando ordenamiento para aridades entre [2, B/8]..." << std::endl;
    //std::vector<uint64_t> IOs_per_arity(aritys.size());
    std::string output_file = "output.bin";
    /*for (int i = 0; i < aritys.size(); ++i) {
        std::string output_file = "output_arity_" + std::to_string(aritys[i]) + ".bin";
        uint64_t IOs = 0;
        mergeSort(input_file, arr_size, aritys[i], output_file, IOs, 0);
        IOs_per_arity[i] = IOs;
        std::cout << "Aridad: " << aritys[i] << ", I/Os: " << IOs << std::endl;
        std::cout << "Verificando archivo de salida..." << std::endl;
        /*
        if (checker(output_file, arr_size)) {
            std::cout << "El archivo de salida está ordenado." << std::endl;
        } else {
            std::cerr << "El archivo de salida no está ordenado." << std::endl;
        }*
        std::remove(output_file.c_str());
    }*
    // Elegir la mejor aridad con busqueda ternaria
    uint64_t best_arity = ternary_search2(input_file,output_file, 0, aritys.size() - 1,aritys,arr_size);
    std::cout << "La mejor aridad es: " << aritys[best_arity] << std::endl;
    std::remove(input_file.c_str());
    std::remove(output_file.c_str());
    return 1;
}*/