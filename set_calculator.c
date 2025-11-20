#include "set_calculator.h"

// --- UI Helper Functions (ส่วนใหม่เพื่อความสวยงาม) ---

void ui_clear_screen() {
    // ใช้ system("cls") สำหรับ Windows และ system("clear") สำหรับ Linux/Mac
    // ถ้าไม่อยากใช้ system() สามารถใช้ลูป print \n หลายๆ ทีแทนได้
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void ui_pause() {
    printf("\n[Press Enter to continue...]");
    char c;
    while ((c = getchar()) != '\n' && c != EOF); // เคลียร์ buffer ถ้ามี
    getchar(); // รอรับ Enter จริงๆ
}

void ui_print_header(const char* title) {
    printf("\n========================================\n");
    printf("  %s\n", title);
    printf("========================================\n");
}

void ui_print_divider() {
    printf("----------------------------------------\n");
}

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

// --- Display Functions (ปรับปรุงใหม่) ---

void print_bits(const unsigned char* bits) {
    printf(" [BITS: ");
    int maxbit = UNIVERSAL_SET_SIZE < 64 ? UNIVERSAL_SET_SIZE : 64;
    for (int i = maxbit-1; i >= 0; --i)
        printf("%d", get_bit(bits, i));
    if (UNIVERSAL_SET_SIZE > 64) printf("...");
    printf("]");
}

void print_set(const Set* s) {
    int arr[UNIVERSAL_SET_SIZE], n = 0;
    bits_to_set(s->bits, arr, &n);
    
    // ใช้ %-12s เพื่อจองพื้นที่ชื่อ 12 ตัวอักษร ให้ชื่อตรงกันเป็นแนวตั้ง
    printf("  %-12s = { ", s->name);
    
    int max_print = n < 15 ? n : 15; // ลดจำนวนที่โชว์ลงนิดหน่อยกันบรรทัดล้น
    for (int i = 0; i < max_print; ++i)
        printf(i == max_print-1 ? "%d" : "%d, ", arr[i]);
        
    if (n > 15) printf(", ...(%d)", n);
    else if (n == 0) printf("Empty");
    
    printf(" }");
    
    // ถ้าอยากโชว์ Bits ด้วย ให้เอาคอมเมนต์ออก
    // print_bits(s->bits); 
    
    printf("\n");
}

void show_all_sets(const Set* sets, int nsets) {
    ui_print_header("List of All Sets");
    if (nsets == 0) { printf("  (No sets stored yet)\n"); return; }
    for (int i = 0; i < nsets; ++i) print_set(&sets[i]);
    ui_print_divider();
}

// --- Set Management ---

int find_set(const Set* sets, int nsets, const char* name) {
    for (int i = 0; i < nsets; ++i)
        if (strcmp(sets[i].name, name) == 0) return i;
    return -1;
}

void add_or_update_set(Set* sets, int* nsets, const char* name, int* arr, int n) {
    int idx = find_set(sets, *nsets, name);
    if (idx != -1) {
        set_to_bits(sets[idx].bits, arr, n);
    } else if (*nsets < MAX_SETS) {
        strcpy(sets[*nsets].name, name);
        set_to_bits(sets[*nsets].bits, arr, n);
        (*nsets)++;
    } else {
        printf("  [!] Error: Storage full. Cannot add '%s'\n", name);
    }
}

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
         printf("  [+] New set stored: %s\n", name);
    } else if (prev_idx != -1) {
         printf("  [*] Set updated: %s\n", name);
    }
}

void input_set_keyboard(Set* sets, int* nsets) {
    ui_print_header("Input Set from Keyboard");
    char line[20000];
    printf("  Format: Name = {1, 2, 3}\n");
    printf("  Enter set > ");
    fgets(line, sizeof(line), stdin);
    if (!strchr(line, '=') || !strchr(line, '{') || !strchr(line, '}')) {
        printf("  [!] Invalid format.\n");
        return;
    }
    parse_and_add_set(line, sets, nsets);
}

void input_set_file(const char* fname, Set* sets, int* nsets) {
    FILE* f = fopen(fname, "r");
    if (!f) { printf("  [!] Cannot open file '%s'\n", fname); return; }
    char line[20000];
    printf("  Reading from '%s'...\n", fname);
    while (fgets(line, sizeof(line), f)) {
        parse_and_add_set(line, sets, nsets);
    }
    fclose(f);
    printf("  [Done] Import finished.\n");
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
    ui_clear_screen(); // เคลียร์หน้าจอก่อนเริ่มคำนวณ
    show_all_sets(sets, *nsets);
    
    if (*nsets == 0) return;

    char a[MAX_NAME], b[MAX_NAME], op;
    ui_print_divider();
    printf("  [Calculation Mode]\n");
    
    printf("  1st Set Name > ");
    fgets(a, sizeof(a), stdin); a[strcspn(a, "\n")] = 0; trim(a);
    int ia = find_set(sets, *nsets, a);
    if (ia == -1) { printf("  [!] Set not found.\n"); return; }

    printf("  Operation (&, |, ^, -, ~) > ");
    scanf(" %c", &op); getchar();
    
    unsigned char res[BIT_ARR_LEN];
    Set temp; 
    strcpy(temp.name, "RESULT");

    if (op == '~') {
        bitwise_not(&sets[ia], res);
        printf("\n  Result: ~%s\n", sets[ia].name);
    } else {
        printf("  2nd Set Name > ");
        fgets(b, sizeof(b), stdin); b[strcspn(b, "\n")] = 0; trim(b);
        int ib = find_set(sets, *nsets, b);
        if (ib == -1) { printf("  [!] Set not found.\n"); return; }
        
        bitwise_operation(op, &sets[ia], &sets[ib], res);
        printf("\n  Result: %s %c %s\n", sets[ia].name, op, sets[ib].name);
    }
    
    memcpy(temp.bits, res, BIT_ARR_LEN);
    ui_print_divider();
    print_set(&temp); // โชว์ผลลัพธ์
    ui_print_divider();

    // Logic การบันทึก
    if (*nsets < MAX_SETS) {
        char save_choice;
        printf("  Save this result? (y/n) > ");
        scanf(" %c", &save_choice); getchar();
        if (save_choice == 'y' || save_choice == 'Y') {
            char new_name[MAX_NAME];
            printf("  New Set Name > ");
            fgets(new_name, sizeof(new_name), stdin);
            new_name[strcspn(new_name, "\n")] = 0; trim(new_name);
            
            int arr[UNIVERSAL_SET_SIZE], n;
            bits_to_set(res, arr, &n);
            add_or_update_set(sets, nsets, new_name, arr, n);
        }
    } else {
        printf("  [!] Storage full, cannot save.\n");
    }
}

void save_results_file(const Set* sets, int nsets) {
    ui_print_header("Save to File");
    char fname[128];
    printf("  Enter filename > ");
    fgets(fname, sizeof(fname), stdin); fname[strcspn(fname, "\n")] = 0;
    
    FILE* f = fopen(fname, "w");
    if (!f) { printf("  [!] Could not open file.\n"); return; }
    
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
    printf("  [Done] Results saved to '%s'\n", fname);
}