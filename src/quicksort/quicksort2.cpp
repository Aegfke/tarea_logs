#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <random>

#define B 4096
#define M (1024 * 1024)
using namespace std;

std::vector<std::vector<u_int64_t>> buffer;


struct MinHeapNode {
    int value;
    int index;
};

class MinHeap {
    public:
        std::vector<MinHeapNode> arr;
        int size;
        int capacity;

        // Función típica de swap
        void swap(int i, int j) {
            MinHeapNode temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        };
        
        // Reorganiza el árbol para mantener la propiedad del min heap
        void heapify(int i) {
            int smallest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < size && arr[left].value < arr[smallest].value) {
                smallest = left;
            }
            if (right < size && arr[right].value < arr[smallest].value) {
                smallest = right;
            }
            if (smallest != i) {
                swap(i, smallest);
                heapify(smallest);
            }
        };
        
        // Constructor de la clase a partir de un vector
        MinHeap(std::vector<int> arr) {
            for (int i = 0; i < arr.size(); i++) {
                MinHeapNode node;
                node.value = arr[i];
                node.index = i;
                this->arr.push_back(node);
            }
            this->size = arr.size();
            this->capacity = arr.size();

            int i = (size - 1) / 2;
            while (i >= 0) {
                heapify(i);
                i--;
            }
        };
};
// Write a run to a temporary file
//void writeRun(std::vector<int> arr, int size, int run_num) 
//{   // creamos el archivo para la run
    //std::ofstream out("run"+std::to_string(run_num),std::ios::binary);
    //out.write(reinterpret_cast<const char *>(&arr),sizeof(arr));
    //cerramos
    //out.close();
//}


//------------------------------------------------------

// Función que toma el primer bloque de cada arreglo creado y retorna un vector con todos los bloques
std::vector<int> merge_files_to_m(int a, std::vector<char*> filenames, int block_position,  std::vector<int> llenar ) {

    // Ciclo para abrir multiples archivos
    for (int i = 0; i<a; i++) {

        std::ifstream in(filenames[i], std::ios::binary);

        if (!in.is_open()) {
            std::cerr << "Error al abrir el archivo de lectura" << filenames[i]
                      << std::endl;
            std::exit(1);
        }

        std::streampos file_offset = block_position * B;

        in.seekg(file_offset);

        in.read(reinterpret_cast<char *>(&llenar), B/sizeof(int)); // Llevar a memoria el bloque leido
    }

    return;
}


// MergeSort para memoria externa
void QuickSort(const std::string &filename, int arr_size, int arity) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error al abrir el archivo de lectura" << filename << std::endl;
        std::exit(1);
    }
       
    // Si el tamaño del arreglo a ordenar es menor o igual al tamaño de la memoria principal, se ordena en memoria principal
    if (arr_size <= M) {
        std::vector<int> A(arr_size);
        for (int i = 0; i < arr_size; i++) {
            in.read(reinterpret_cast<char *>(&A[i]), sizeof(int));
        }
        in.close();

        sort(A.begin(), A.end()); // Ordenar el arreglo en memoria principal

        std::ofstream out(filename, std::ios::binary);
        if (!out.is_open()) {
            std::cerr << "Error al abrir el archivo de escritura" << filename
                    << std::endl;
            std::exit(1);
        }
        for (int i = 0; i < arr_size; i++) {
            out.write(reinterpret_cast<const char *>(&A[i]), sizeof(int));
        }
        out.close();
        return;
    }
    
    // Si el tamaño del arreglo no es menor o igual al de memoria principal, se divide en arity bloques
    else {

        int file_length = arr_size/arity;

        // Dividir el archivo principal en "arity" archivos
        for (int i = 0; i<arity; i++) {
            
            std::string current_file = "run" + std::to_string(i) + ".bin";
            //filenames.push_back(current_file);
            std::ofstream out(current_file, std::ios::binary);

            // Error si no se logro abrir el archivo i 
            if (!out.is_open()) {
                std::cerr << "Error al abrir archivo para escritura: " << current_file
                          << std::endl;
                std::exit(1);
            }
            
            // Escribir tamaño_input/a bloques en cada uno

            // Guardar los archivos temporalmente
            std::vector<int> buffer;

            // Leo desde el archivo main a buffer
            in.read(reinterpret_cast<char *>(&buffer), file_length); //No se si es solo b o b/sizeof(int)

            // Desde buffer a archivo i
            out.write(reinterpret_cast<const char *>(&buffer), file_length); //No se si es solo b o b/sizeof(int)
            out.close();
        }

        // Llamar recursivamente en cada seccion
        for (int i=0;i<arity;i++){
            
            std::string new_filename = "run" + std::to_string(i) + ".bin";
            mergeSort(new_filename,file_length,arity);
        }
        // Paso de merge a cada subarreglo

        // Crear arity vectores donde se guarda los bloques del archivo i
        std::vector<int> current_int(arity, 0);

        // bloques que tendremos en memoria
        std::vector<std::vector<int>> blocks_in_memory(arity, std::vector<int>(B / sizeof(int)));

        // El vector archivo i esta vacio?
        std::vector<bool> stream_is_empty(arity, false);

        for (int i = 0; i < arity; i++) {

            std::string current_filename = "run" + std::to_string(i) + ".bin";
            std::ifstream in(current_filename, std::ios::binary);

            in.seekg(0);

            std::vector<int> temp;
            for (int j=0;j<B;j++){
                int elemento;
                in.read(reinterpret_cast<char*>(&elemento), sizeof(int));
                temp.push_back(elemento);
                current_int[i]++;
            }
            blocks_in_memory.push_back(temp);
        }

        

        



        // Cada subarreglo mide N/A, en bloque es N/A/B?

        // usar minheap con los i vectores creados

        // escribir en el output
        std::ofstream output(filename,std::ios::binary);
        int info=0;//cambiar por la info que se va a guardar
        output.write(reinterpret_cast<const char *>(&info),sizeof(info));
        output.close();
    }
}
std::vector<int> generate_sequence(int num_elements) {
    std::vector<int> sequence(num_elements);
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int> dis;

    for (auto& elem : sequence) {
        elem = dis(gen);
    }
    return sequence;
}
void write_to_binary_file(const std::string& filename, const std::vector<int>& sequence) {
    std::ofstream outfile(filename, std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(sequence.data()), sequence.size() * sizeof(int));
}
int main(){
    cout << ">>> mergesort started" << endl;
    std::ofstream out("test.bin",std::ios::binary);
    std::vector<int> seq=generate_sequence(4*M);
    out.write(reinterpret_cast<const char *>(&seq),sizeof(seq));
    out.close();
    mergeSort("test.bin",4*M,10);
    cout << ">>> mergesort ended" << endl;
    return 0;
}