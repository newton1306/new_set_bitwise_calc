#include "set_calculator.h"

// --- Utility Functions ---

void trim(char* s) {
    int l = 0, r = strlen(s) - 1;
    while (isspace((unsigned char)s[l])) l++;
    while (r >= l && isspace((unsigned char)s[r])) r--;
    if (l > 0) memmove(s, s + l, r - l + 1);
    s[r - l + 1] = 0;
}

void set_bit(unsigned char* bits, int i) {
    bits[i/8] |= (1 << (i % 8));
}

int get_bit(const unsigned char* bits, int i) {
    return (bits[i/8] >> (i % 8)) & 1;
}

void clear_bits(unsigned char* bits) {
    memset(bits, 0, BIT_ARR_LEN);
}

void set_to_bits(unsigned char* bits, int* arr, int n) {
    clear_bits(bits);
    for (int i = 0; i < n; ++i) {
        if (arr[i] >= 0 && arr[i] < UNIVERSAL_SET_SIZE) set_bit(bits, arr[i]);
    }
}

void bits_to_set(const unsigned char* bits, int* arr, int* n) {
    int idx = 0;
    for (int i = 0; i < UNIVERSAL_SET_SIZE; ++i)
        if (get_bit(bits, i)) arr[idx++] = i;
    *n = idx;
}

// --- Display Functions ---

void print_bits(const unsigned char* bits) {
    printf("[bits: ");
    int maxbit = UNIVERSAL_SET_SIZE < 64 ? UNIVERSAL_SET_SIZE : 64;
    for (int i = maxbit-1; i >= 0; --i)
        printf("%d", get_bit(bits, i));
    if (UNIVERSAL_SET_SIZE > 64) printf(" ...");
    printf("]");
}

void print_set(const Set* s) {
    int arr[UNIVERSAL_SET_SIZE], n = 0;
    bits_to_set(s->bits, arr, &n);
    printf("%s = {", s->name);
    int max_print = n < 20 ? n : 20;
    for (int i = 0; i < max_print; ++i)
        printf(i == max_print-1 ? "%d" : "%d,", arr[i]);
    if (n > 20) printf(", ... (%d items)", n);
    printf("} "); 
    print_bits(s->bits); 
    printf("\n");
}

void show_all_sets(const Set* sets, int nsets) {
    if (nsets == 0) { printf("No sets stored.\n"); return; }
    for (int i = 0; i < nsets; ++i) print_set(&sets[i]);
}

// --- Set Management & Logic ---

int find_set(const Set* sets, int nsets, const char* name) {
    for (int i = 0; i < nsets; ++i)
        if (strcmp(sets[i].name, name) == 0) return i;
    return -1;
}

// ฟังก์ชันกลางสำหรับเพิ่มข้อมูลลงใน Array (แยก logic การเก็บข้อมูลออกมา)
void add_or_update_set(Set* sets, int* nsets, const char* name, int* arr, int n) {
    int idx = find_set(sets, *nsets, name);
    if (idx != -1) {
        // Update existing
        set_to_bits(sets[idx].bits, arr, n);
        // Only print if updating explicitly (optional)
    } else if (*nsets < MAX_SETS) {
        // Add new
        strcpy(sets[*nsets].name, name);
        set_to_bits(sets[*nsets].bits, arr, n);
        (*nsets)++;
    } else {
        printf("Set storage full! Cannot add %s\n", name);
    }
}

// ฟังก์ชันกลางสำหรับแกะ String (Parsing) ลด Code ซ้ำ
void parse_and_add_set(char* line, Set* sets, int* nsets) {
    char name[MAX_NAME];
    int arr[UNIVERSAL_SET_SIZE], n = 0, x;
    
    char *eq = strchr(line, '=');
    if (!eq) return;

    strncpy(name, line, eq - line);
    name[eq - line] = '\0';
    trim(name);

    char* b = strchr(line, '{');
    char* e = strchr(line, '}');
    if (!b || !e) return;
    
    b++;
    while (sscanf(b, "%d", &x) == 1) {
        arr[n++] = x;
        b = strchr(b, ',');
        if (!b || b > e) break;
        b++;
    }

    int prev_idx = find_set(sets, *nsets, name);
    add_or_update_set(sets, nsets, name, arr, n);
    
    if (prev_idx == -1 && find_set(sets, *nsets, name) != -1) {
         printf("Stored set %s\n", name);
    } else if (prev_idx != -1) {
         printf("Set \"%s\" updated.\n", name);
    }
}

void input_set_keyboard(Set* sets, int* nsets) {
    char line[20000];
    printf("Enter set [Eg. A = {1,2,4000}]: ");
    fgets(line, sizeof(line), stdin);
    if (!strchr(line, '=') || !strchr(line, '{') || !strchr(line, '}')) {
        printf("Invalid format.\n");
        return;
    }
    parse_and_add_set(line, sets, nsets);
}

void input_set_file(const char* fname, Set* sets, int* nsets) {
    FILE* f = fopen(fname, "r");
    if (!f) { printf("Cannot open file.\n"); return; }
    char line[20000];
    while (fgets(line, sizeof(line), f)) {
        parse_and_add_set(line, sets, nsets);
    }
    fclose(f);
    printf("Done importing from %s\n", fname);
}

// --- Operations ---

void bitwise_operation(char op, const Set* a, const Set* b, unsigned char* res) {
    for (int i = 0; i < BIT_ARR_LEN; ++i) {
        if      (op == '&') res[i] = a->bits[i] & b->bits[i];
        else if (op == '|') res[i] = a->bits[i] | b->bits[i];
        else if (op == '^') res[i] = a->bits[i] ^ b->bits[i];
        else if (op == '-') res[i] = a->bits[i] & ~b->bits[i];
    }
}

void bitwise_not(const Set* a, unsigned char* res) {
    for (int i = 0; i < BIT_ARR_LEN; ++i)
        res[i] = ~a->bits[i];
    int excess = (WORD_SIZE * BIT_ARR_LEN) - UNIVERSAL_SET_SIZE;
    if (excess) res[BIT_ARR_LEN-1] &= ((1<<(WORD_SIZE-excess))-1);
}

void do_bitwise_op(Set* sets, int* nsets) {
    char a[MAX_NAME], b[MAX_NAME], op;
    show_all_sets(sets, *nsets);
    
    printf("Select first set: ");
    fgets(a, sizeof(a), stdin); a[strcspn(a, "\n")] = 0; trim(a);
    int ia = find_set(sets, *nsets, a);
    if (ia == -1) { printf("Set not found.\n"); return; }

    printf("Enter operation (& AND | OR ^ XOR - SUB ~ NOT): ");
    scanf(" %c", &op); getchar();
    
    unsigned char res[BIT_ARR_LEN];
    Set temp; 
    strcpy(temp.name, "Result");

    if (op == '~') {
        bitwise_not(&sets[ia], res);
        printf("~%s = ", sets[ia].name);
    } else {
        printf("Select second set: ");
        fgets(b, sizeof(b), stdin); b[strcspn(b, "\n")] = 0; trim(b);
        int ib = find_set(sets, *nsets, b);
        if (ib == -1) { printf("Set not found.\n"); return; }
        
        bitwise_operation(op, &sets[ia], &sets[ib], res);
        printf("%s %c %s = ", sets[ia].name, op, sets[ib].name);
    }
    
    memcpy(temp.bits, res, BIT_ARR_LEN);
    print_set(&temp);

    // Save Result Logic
    if (*nsets < MAX_SETS) {
        char save_choice;
        printf("Save this result as a new set? (y/n): ");
        scanf(" %c", &save_choice); getchar();
        if (save_choice == 'y' || save_choice == 'Y') {
            char new_name[MAX_NAME];
            printf("Enter name for new set: ");
            fgets(new_name, sizeof(new_name), stdin);
            new_name[strcspn(new_name, "\n")] = 0; trim(new_name);
            
            int arr[UNIVERSAL_SET_SIZE], n;
            bits_to_set(res, arr, &n);
            add_or_update_set(sets, nsets, new_name, arr, n);
            printf("Saved result as %s\n", new_name);
        }
    } else {
        printf("Cannot save new set: set storage full!\n");
    }
}

void save_results_file(const Set* sets, int nsets) {
    char fname[128];
    printf("Enter filename to save results: ");
    fgets(fname, sizeof(fname), stdin); fname[strcspn(fname, "\n")] = 0;
    
    FILE* f = fopen(fname, "w");
    if (!f) { printf("Could not open file.\n"); return; }
    
    for (int i = 0; i < nsets; ++i) {
        int arr[UNIVERSAL_SET_SIZE], n;
        bits_to_set(sets[i].bits, arr, &n);
        fprintf(f, "%s = {", sets[i].name);
        int max_print = n < 20 ? n : 20;
        for (int k = 0; k < max_print; ++k)
            fprintf(f, k == max_print-1 ? "%d" : "%d,", arr[k]);
        if (n > 20) fprintf(f, ", ... (%d items)", n);
        
        fprintf(f, "} [bits: ");
        int maxbit = UNIVERSAL_SET_SIZE < 64 ? UNIVERSAL_SET_SIZE : 64;
        for (int m = maxbit-1; m >= 0; --m)
            fprintf(f, "%d", get_bit(sets[i].bits, m));
        if (UNIVERSAL_SET_SIZE > 64) fprintf(f, " ...");
        fprintf(f, "]\n");
    }
    fclose(f);
    printf("Results saved to %s\n", fname);
}