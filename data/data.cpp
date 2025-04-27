#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

#define M (1 * 1024 * 1024)  // 1MB = 1024 * 1024 bytes, the RAM size, should be 50 * ...

// B should be block size, in my case is 4096

// Generate a sequence of random 64-bit integers and shuffle it
std::vector<uint64_t> generate_sequence(size_t num_elements) {
    std::vector<uint64_t> sequence(num_elements);
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    for (auto& elem : sequence) {
        elem = dis(gen);
    }

    std::shuffle(sequence.begin(), sequence.end(), gen);
    return sequence;
}

// Write the sequence to a binary file
void write_to_binary_file(const std::string& filename, const std::vector<uint64_t>& sequence) {
    std::ofstream outfile(filename, std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(sequence.data()), sequence.size() * sizeof(uint64_t));
}

int main() {
    // Sizes: {4M, 8M, ..., 60M} and 5 sequences per size
    for (size_t size : {4}) { //8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60
        size_t num_elements = size * M / sizeof(uint64_t);  // Number of 64-bit integers
        for (int i = 0; i < 5; ++i) {  // Generate 5 sequences per size
            std::vector<uint64_t> sequence = generate_sequence(num_elements);
            write_to_binary_file("sequence_" + std::to_string(size) + "M_" + std::to_string(i+1) + ".bin", sequence);
            std::cout << "Generated and saved sequence of size " << size << "M, sequence " << i+1 << std::endl;
        }
    }

    return 0;
}
