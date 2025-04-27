#ifndef MERGESORT_HPP
#define MERGESORT_HPP

#include <cstdint>
#include <cstdio>

// Tamaño de bloque fijo
constexpr int64_t BLOCK_SIZE = 4096;

// Funciones principales
void generarArregloBinario(const char* filename, int64_t N);
void externalMergeSort(const char* input, const char* output, int64_t N, int a);
int buscarMejorAridad(const char* input, int64_t N);

// Funciones de bloque
void leerBloque(FILE* file, void* buffer);
void escribirBloque(FILE* file, void* buffer);

#endif // MERGESORT_HPP
