#ifndef SET_CALCULATOR_H
#define SET_CALCULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define UNIVERSAL_SET_SIZE 8
#define MAX_SETS 10
#define MAX_NAME 16
#define WORD_SIZE 8
#define BIT_ARR_LEN ((UNIVERSAL_SET_SIZE + WORD_SIZE-1)/WORD_SIZE)

typedef struct {
    char name[MAX_NAME];
    unsigned char bits[BIT_ARR_LEN];
} Set;

// Bitwise & Utility Functions
void trim(char* s);
void set_bit(unsigned char* bits, int i);
int get_bit(const unsigned char* bits, int i);
void clear_bits(unsigned char* bits);
void set_to_bits(unsigned char* bits, int* arr, int n);
void bits_to_set(const unsigned char* bits, int* arr, int* n);

// Display Functions
void print_bits(const unsigned char* bits);
void print_set(const Set* s);
void show_all_sets(const Set* sets, int nsets);

// Set Management Functions
int find_set(const Set* sets, int nsets, const char* name);
void add_or_update_set(Set* sets, int* nsets, const char* name, int* arr, int n);
void parse_and_add_set(char* line, Set* sets, int* nsets); // ฟังก์ชันใหม่ช่วยลดโค้ดซ้ำ

// Input/Output Functions
void input_set_keyboard(Set* sets, int* nsets);
void input_set_file(const char* fname, Set* sets, int* nsets);
void save_results_file(const Set* sets, int nsets);

// Operation Functions
void bitwise_operation(char op, const Set* a, const Set* b, unsigned char* res);
void bitwise_not(const Set* a, unsigned char* res);
void do_bitwise_op(Set* sets, int* nsets);

#endif