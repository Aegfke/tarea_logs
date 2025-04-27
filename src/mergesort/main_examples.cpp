// main.cpp

#include "mergesort.hpp"
#include <cstdlib>
#include <ctime>
#include <cstdio>

int main()
{
    int num_ways = 10;
    int run_size = 1000;

    char input_file[] = "input.txt";
    char output_file[] = "output.txt";

    FILE* in = openFile(input_file, "w");
    srand(time(NULL));

    for (int i = 0; i < num_ways * run_size; i++)
        fprintf(in, "%d ", rand());

    fclose(in);

    externalSort(input_file, output_file, num_ways, run_size);

    return 0;
}
