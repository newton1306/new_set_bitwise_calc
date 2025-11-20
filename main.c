#include "set_calculator.h"

void print_menu() {
    ui_clear_screen(); // ล้างหน้าจอทุกครั้งที่โชว์เมนู
    printf("\n");
    printf("  +--------------------------------------+\n");
    printf("  |      SET BITWISE CALCULATOR PRO      |\n");
    printf("  +--------------------------------------+\n");
    printf("  |  1. Input set (Keyboard)             |\n");
    printf("  |  2. Input set (File)                 |\n");
    printf("  |  3. Show all sets                    |\n");
    printf("  |  4. Calculate (Bitwise Ops)          |\n");
    printf("  |  5. Save results to file             |\n");
    printf("  |  6. Exit                             |\n");
    printf("  +--------------------------------------+\n");
    printf("  Select menu [1-6] > ");
}

int main() {
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
                ui_clear_screen(); // ล้างหน้าจอก่อนเข้าฟังก์ชัน
                input_set_keyboard(sets, &nsets); 
                ui_pause(); // หยุดรอก่อนกลับเมนู
                break;
            case 2: {
                ui_clear_screen();
                ui_print_header("Import from File");
                char fname[128];
                printf("  Filename > ");
                fgets(fname, sizeof(fname), stdin); 
                fname[strcspn(fname, "\n")] = 0;
                input_set_file(fname, sets, &nsets);
                ui_pause();
                break;
            }
            case 3: 
                ui_clear_screen();
                show_all_sets(sets, nsets); 
                ui_pause();
                break;
            case 4: 
                // ใน do_bitwise_op มี clear_screen เองแล้ว
                do_bitwise_op(sets, &nsets); 
                ui_pause();
                break;
            case 5: 
                ui_clear_screen();
                save_results_file(sets, nsets); 
                ui_pause();
                break;
            case 6: 
                printf("\n  Bye Bye! :)\n"); 
                return 0;
            default: 
                printf("\n  [!] Invalid menu.\n");
                ui_pause();
        }
    }
    return 0;
}