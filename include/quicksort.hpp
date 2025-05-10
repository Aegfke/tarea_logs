#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>

#define block = (4096)


class QuickSort {
public:
    QuickSort(const std::string &filename, int arity);
    
    std::vector<uint64_t> read_block_at(int offset, const std::string &file) const;

    void read_blocks_to_ram(int start, const std::string &file);

    std::vector<uint64_t> choose_a_pivots(std::vector<uint64_t> &b, int a) const;

    void sort_block(std::vector<uint64_t> &b);

    void partition(void);

    void write_to_binary_file(const std::string& filename, const std::vector<uint64_t>& sequence);

    void sort_in_memory(void);

    void sort(void);



    private:
        std::string filename;
        std::vector<uint64_t> pivots;
        int arity;
        std::vector<std::vector<uint64_t>> RAM;
        int array_length_in_blocks;


    
};

namespace QuickSortUtils {

    void write_new_subarray(const std::string &filename);
}