#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>

#define BLOCK_SIZE (4096) // 4KB block size

// Function to read a single block of data and convert it to a vector of uint64_t
std::vector<uint64_t> read_block(std::ifstream& infile, size_t block_size_bytes) {
    size_t num_elements = block_size_bytes / sizeof(uint64_t);
    std::vector<uint64_t> block(num_elements);

    infile.read(reinterpret_cast<char*>(block.data()), block_size_bytes);

    // Resize in case less data was read (for example, near end of file)
    block.resize(infile.gcount() / sizeof(uint64_t));

    return block;
}

// Function to read multiple blocks starting from an optional position
bool read_blocks(std::ifstream& infile,
                 size_t block_size_bytes,
                 size_t num_blocks,
                 std::vector<std::vector<uint64_t>>& blocks_in_ram,
                 std::streampos& end_position,
                 std::streampos start_position = -1)
{
    blocks_in_ram.clear();

    // If a specific start position is provided, seek to it
    if (start_position != -1) {
        infile.clear(); // Clear EOF or error flags before seeking
        infile.seekg(start_position);
        if (!infile) {
            std::cerr << "Failed to seek to start_position\n";
            return false;
        }
    }

    // Read num_blocks blocks into blocks_in_ram
    for (size_t i = 0; i < num_blocks; ++i) {
        if (infile.eof()) {
            return false; // End of file reached
        }
        auto block = read_block(infile, block_size_bytes);
        if (block.empty()) {
            return false; // Nothing more to read
        }
        blocks_in_ram.push_back(std::move(block));
    }

    end_position = infile.tellg(); // Save the position after reading
    return true;
}

int main() {
    const std::string filename = "sequence_4M_1.bin";

    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return 1;
    }

    size_t num_blocks_to_read = 5;
    std::vector<std::vector<uint64_t>> blocks_in_ram;
    std::streampos start_position = 0;
    std::streampos end_position = 0;

    // --- First read ---
    if (read_blocks(infile, BLOCK_SIZE, num_blocks_to_read, blocks_in_ram, end_position, start_position)) {
        std::cout << "First read done, read " << blocks_in_ram.size() << " blocks.\n";
        std::cout << "Stopped at position (bytes): " << end_position << "\n";
    } else {
        std::cout << "First read: reached end of file or failed.\n";
    }

    blocks_in_ram.clear();

    // --- Second read, continuing from where we stopped ---
    if (read_blocks(infile, BLOCK_SIZE, num_blocks_to_read, blocks_in_ram, end_position, end_position)) {
        std::cout << "Second read done, read " << blocks_in_ram.size() << " blocks.\n";
        std::cout << "Stopped at position (bytes): " << end_position << "\n";
    } else {
        std::cout << "Second read: reached end of file or failed.\n";
    }

    infile.close();
    return 0;
}
