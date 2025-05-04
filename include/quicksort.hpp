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
    
    std::vector<uint64_t> read_block_at(int offset) const;

    void read_blocks_to_ram(int start, int end);


    std::vector<uint64_t> choose_a_pivots(std::vector<uint64_t> &b, int a) const;

    void sort_block(std::vector<uint64_t> &b);

    void partition(void) const;



    private:
        std::string filename;
        std::vector<uint64_t> pivots;
        int arity;
        std::vector<std::vector<uint64_t>> RAM;
    
};

namespace QuickSortUtils {

    void write_new_subarray(const std::string &filename);
}