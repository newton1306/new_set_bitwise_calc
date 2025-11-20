#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define UNIVERSAL_SET_SIZE 8 // U = {0,1,2,3,4,5,6,7} ไซส์ 8 (ปรับได้ให้เยอะไขึ้น)
#define MAX_SETS 10 // จำนวนเซ็ทที่อยู่ในระบบ เคสนีคือ 10 ตัว (A,B,C,...,ตัวที่10)
#define MAX_NAME 16 // ชื่อเซ็ทแต่ละตัว (เช่น GEAR888 = {1,2,3,4,5,6})
#define WORD_SIZE 8 // bits per unsigned char
#define BIT_ARR_LEN ((UNIVERSAL_SET_SIZE + WORD_SIZE-1)/WORD_SIZE)

typedef struct {
    char name[MAX_NAME];
    unsigned char bits[BIT_ARR_LEN];
} Set;

Set sets[MAX_SETS];
int nsets = 0;

// Prototypes ครบทุกฟังก์ชันเพื่อป้องกัน warning!
void trim(char* s);
void set_bit(unsigned char* bits, int i);
int get_bit(const unsigned char* bits, int i);
void clear_bits(unsigned char* bits);
void set_to_bits(unsigned char* bits, int* arr, int n);
void bits_to_set(const unsigned char* bits, int* arr, int* n);
void print_bits(const unsigned char* bits);
void print_set(const Set* s);
void show_all_sets();
void input_set_keyboard();
void input_set_file(const char* fname);
int find_set(const char* name);
void bitwise_operation(char op, const Set* a, const Set* b, unsigned char* res);
void bitwise_not(const Set* a, unsigned char* res);
void do_bitwise_op();
void save_results_file();
void menu();

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
    printf("} "); print_bits(s->bits); printf("\n");
}

void show_all_sets() {
    if (nsets == 0) { printf("No sets stored.\n"); return; }
    for (int i = 0; i < nsets; ++i) print_set(&sets[i]);
}

int find_set(const char* name) {
    for (int i = 0; i < nsets; ++i)
        if (strcmp(sets[i].name, name) == 0) return i;
    return -1;
}

void input_set_keyboard() {
    char line[20000];
    char name[MAX_NAME];
    int arr[UNIVERSAL_SET_SIZE], n = 0, x;
    printf("Enter set [Eg. A = {1,2,4000}]: ");
    fgets(line, sizeof(line), stdin);
    char *eq = strchr(line, '=');
    if (!eq) { printf("Invalid format.\n"); return; }
    strncpy(name, line, eq - line);
    name[eq - line] = '\0';
    trim(name);

    char* b = strchr(line, '{');
    char* e = strchr(line, '}');
    if (!b || !e) { printf("Invalid format.\n"); return; }
    b++;
    while (sscanf(b, "%d", &x) == 1) {
        arr[n++] = x;
        b = strchr(b, ',');
        if (!b || b > e) break;
        b++;
    }

    int idx = find_set(name);
    if (idx != -1) {
        printf("Set \"%s\" already exists; overwritten.\n", name);
        set_to_bits(sets[idx].bits, arr, n);
    } else if (nsets < MAX_SETS) {
        strcpy(sets[nsets].name, name);
        set_to_bits(sets[nsets].bits, arr, n);
        nsets++;
        printf("Stored set %s\n", name);
    } else {
        printf("Set storage full\n");
    }
}

void input_set_file(const char* fname) {
    FILE* f = fopen(fname, "r");
    if (!f) { printf("Cannot open file.\n"); return; }
    char line[20000];
    while (fgets(line, sizeof(line), f)) {
        char name[MAX_NAME];
        int arr[UNIVERSAL_SET_SIZE], n = 0, x;
        char *eq = strchr(line, '=');
        if (!eq) continue;
        strncpy(name, line, eq - line);
        name[eq - line] = '\0';
        trim(name);
        char* b = strchr(line, '{');
        char* e = strchr(line, '}');
        if (!b || !e) continue;
        b++;
        while (sscanf(b, "%d", &x) == 1) {
            arr[n++] = x;
            b = strchr(b, ',');
            if (!b || b > e) break;
            b++;
        }
        int idx = find_set(name);
        if (idx != -1) set_to_bits(sets[idx].bits, arr, n);
        else if (nsets < MAX_SETS) {
            strcpy(sets[nsets].name, name);
            set_to_bits(sets[nsets].bits, arr, n);
            nsets++;
        }
    }
    fclose(f);
    printf("Done importing from %s\n", fname);
}

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

void do_bitwise_op() {
    char a[MAX_NAME], b[MAX_NAME], op;
    show_all_sets();
    printf("Select first set: ");
    fgets(a, sizeof(a), stdin); a[strcspn(a, "\n")] = 0; trim(a);
    int ia = find_set(a);
    if (ia == -1) { printf("Set not found.\n"); return; }
    printf("Enter operation (& AND | OR ^ XOR - SUB ~ NOT): ");
    scanf(" %c", &op); getchar();
    unsigned char res[BIT_ARR_LEN];
    if (op == '~') {
        bitwise_not(&sets[ia], res);
        printf("~%s = ", sets[ia].name);
        Set temp; strcpy(temp.name, "Result"); memcpy(temp.bits, res, BIT_ARR_LEN);
        print_set(&temp);
        if (nsets < MAX_SETS) {
            char save_choice;
            printf("Save this result as a new set? (y/n): ");
            scanf(" %c", &save_choice); getchar();
            if (save_choice == 'y' || save_choice == 'Y') {
                char new_name[MAX_NAME];
                printf("Enter name for new set: ");
                fgets(new_name, sizeof(new_name), stdin);
                new_name[strcspn(new_name, "\n")] = 0; trim(new_name);
                if (find_set(new_name) != -1) {
                    printf("That name already exists, not saved.\n");
                } else {
                    strcpy(sets[nsets].name, new_name);
                    memcpy(sets[nsets].bits, res, BIT_ARR_LEN);
                    nsets++;
                    printf("Saved as set %s\n", new_name);
                }
            }
        } else {
            printf("Cannot save new set: set storage full!\n");
        }

    } else {
        printf("Select second set: ");
        fgets(b, sizeof(b), stdin); b[strcspn(b, "\n")] = 0; trim(b);
        int ib = find_set(b);
        if (ib == -1) { printf("Set not found.\n"); return; }
        bitwise_operation(op, &sets[ia], &sets[ib], res);
        printf("%s %c %s = ", sets[ia].name, op, sets[ib].name);
        Set temp; strcpy(temp.name, "Result"); memcpy(temp.bits, res, BIT_ARR_LEN);
        print_set(&temp);
        if (nsets < MAX_SETS) {
            char save_choice;
            printf("Save this result as a new set? (y/n): ");
            scanf(" %c", &save_choice); getchar();
            if (save_choice == 'y' || save_choice == 'Y') {
                char new_name[MAX_NAME];
                printf("Enter name for new set: ");
                fgets(new_name, sizeof(new_name), stdin);
                new_name[strcspn(new_name, "\n")] = 0; trim(new_name);
                if (find_set(new_name) != -1) {
                    printf("That name already exists, not saved.\n");
                } else {
                    strcpy(sets[nsets].name, new_name);
                    memcpy(sets[nsets].bits, res, BIT_ARR_LEN);
                    nsets++;
                    printf("Saved as set %s\n", new_name);
                }
            }
        } else {
            printf("Cannot save new set: set storage full!\n");
        }
    }
}

void save_results_file() {
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

void menu() {
    printf("\n--- Set Calculator (Bitwise, Large, Fast) ---\n");
    printf("1. Input set from keyboard\n");
    printf("2. Input set from file\n");
    printf("3. Show all sets\n");
    printf("4. Bitwise operation\n");
    printf("5. Save all sets to file\n");
    printf("6. Exit\n");
}

int main() {
    while (1) {
        menu();
        printf("Select menu: ");
        int ch;
        char dummy[8];
        if (scanf("%d", &ch) != 1) { fgets(dummy, sizeof(dummy), stdin); continue; }
        getchar();
        if (ch == 1) input_set_keyboard();
        else if (ch == 2) {
            char fname[128];
            printf("Enter filename: ");
            fgets(fname, sizeof(fname), stdin); fname[strcspn(fname, "\n")] = 0;
            input_set_file(fname);
        }
        else if (ch == 3) show_all_sets();
        else if (ch == 4) do_bitwise_op();
        else if (ch == 5) save_results_file();
        else if (ch == 6) { printf("Bye!\n"); break; }
        else printf("Invalid menu.\n");
    }
    return 0;
}
