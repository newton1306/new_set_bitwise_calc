#include "set_calculator.h"

void print_menu() {
    printf("\n--- Set Calculator (Bitwise, Large, Fast) ---\n");
    printf("1. Input set from keyboard\n");
    printf("2. Input set from file\n");
    printf("3. Show all sets\n");
    printf("4. Bitwise operation\n");
    printf("5. Save all sets to file\n");
    printf("6. Exit\n");
    printf("Select menu: ");
}

int main() {
    // State ถูกเก็บไว้ใน Main แทน Global Variable
    Set sets[MAX_SETS];
    int nsets = 0;

    while (1) {
        print_menu();
        int ch;
        char dummy[8];
        
        if (scanf("%d", &ch) != 1) { 
            fgets(dummy, sizeof(dummy), stdin); 
            continue; 
        }
        getchar(); // Consume newline

        switch (ch) {
            case 1: 
                input_set_keyboard(sets, &nsets); 
                break;
            case 2: {
                char fname[128];
                printf("Enter filename: ");
                fgets(fname, sizeof(fname), stdin); 
                fname[strcspn(fname, "\n")] = 0;
                input_set_file(fname, sets, &nsets);
                break;
            }
            case 3: 
                show_all_sets(sets, nsets); 
                break;
            case 4: 
                do_bitwise_op(sets, &nsets); 
                break;
            case 5: 
                save_results_file(sets, nsets); 
                break;
            case 6: 
                printf("Bye!\n"); 
                return 0;
            default: 
                printf("Invalid menu.\n");
        }
    }
    return 0;
}