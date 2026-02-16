#include "./terminal.h"

struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
} __attribute__((packed));

struct gdt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gp;

void gdt_flush(unsigned int);

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install() {
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (unsigned int)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment (0x08)
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment (0x10)

    asm volatile("lgdt %0" : : "m"(gp));
    
    asm volatile(
        "ljmp $0x08, $.1\n\t"
        ".1:\n\t"
        "mov $0x10, %ax\n\t"
        "mov %ax, %ds\n\t"
        "mov %ax, %es\n\t"
        "mov %ax, %fs\n\t"
        "mov %ax, %gs\n\t"
        "mov %ax, %ss\n\t"
    );
}

struct idt_entry {
    unsigned short base_low;
    unsigned short sel;        
    unsigned char  always0;    
    unsigned char  flags;      
    unsigned short base_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct interrupt_frame;

struct idt_entry idt[256];
struct idt_ptr idtp; 

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
void pic_remap() {
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28); 
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0x0);  outb(0xA1, 0x0);
}

__attribute__((interrupt, target("general-regs-only")))
void default_handler(struct interrupt_frame* frame) {
    (void)frame;
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}


char kbd_us[128] = {

0, 27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */

'9', '0', '-', '=', '\b', /* Backspace */

'\t', /* Tab */

'q', 'w', 'e', 'r', /* 19 */

't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */

0, /* 29 - Control */

'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */

'\'', '`', 0, /* Left shift */

'\\', 'z', 'x', 'c', 'v', 'b', 'n', /* 49 */

'm', ',', '.', '/', 0, /* Right shift */

'*',

0, /* Alt */

' ', /* Space bar */

0, /* Caps lock */

};

char buffer[5];
size_t index = 0;

// q
// l
// i
// w
// c
void do_cmd(){
    if(buffer[0] == 'l'){
        set_fg(VGA_COLOR_BLUE);
        safe_print_string("there is no filesystem or even userspace, but..");
        endl();
        safe_print_string("this project made by Assembly, C and GRUB.");
        endl();
        safe_print_string("Idc about making OS for x86 so I used gemini to make the gdt, idt and intrrupts stuff");
        endl();
        safe_print_string("https://wiki.osdev.org/Bare_Bones what I used to make this!");
        endl();
        safe_print_string("this project will be on github, next time I will work with RISC, ARM or even micro controllers.");
        endl();
        safe_print_string("Too much fun and knowladge in this journy! I really like it!");
        endl();
        safe_print_string("but do not be sad... the files is:");
        endl();
        set_fg(VGA_COLOR_RED);
        safe_print_string("boot.s   build.sh   grub.cfg   linker.ld   global.h  terminal.h   vga.h");
        endl();
        set_fg(VGA_COLOR_BLACK);
        safe_print_string("also the tool-chain (cross-compiler).");
        set_fg(VGA_COLOR_WHITE);

    }
    if(buffer[0] == 'i'){
        safe_print_string("This is a very dump i686 OS (32bit) made with C and Assembly by someone finally understod the pointers.");
    }
    if(buffer[0] == 'q'){
        print_string("System Halted. It is now safe to turn off your computer.");
        asm volatile("cli"); 
        while(1) {
            asm volatile("hlt"); 
        }
    }
    if(buffer[0] == 'w'){
        safe_print_string("Sorry, there is no editor or filesystem yet, BRO YOU ARE PLAYING WITH THE KERNEL! no userspace lamo.");
    }
    if(buffer[0] == 'c'){
        clear();
    }
    endl();
    safe_print_string("-$:> ");
}

__attribute__((interrupt, target("general-regs-only")))
void keyboard_handler(struct interrupt_frame* frame) {
    (void)frame;
    uint8_t scancode = inb(0x60);
    if (!(scancode & 0x80)) {
        char letter = kbd_us[scancode];
        if (letter > 0) {
            char str[2] = {letter, 0};
            if(letter == '\n'){
                endl();
                do_cmd();
            } else if( letter == '\b'){
            backspace();
            }else {
                buffer[0] = letter;
                index++; // I need one letter for each command
                if (index == 5){
                    endl();
                    do_cmd();
                }
                safe_print_string(str);
            }
        }
    }
    outb(0x20, 0x20);
}

// 5. دوال الإعداد (Setup)

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_load() {
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (unsigned long)default_handler, 0x08, 0x8E);
    }

    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;
    asm volatile("lidt %0" : : "m"(idtp));
}

void kernel_main(void){
    terminal_init();
    
gdt_install(); 
    idt_load();    
    
    idt_set_gate(33, (unsigned long)keyboard_handler, 0x08, 0x8E);
    pic_remap();   

    set_fg(VGA_COLOR_WHITE);
    fill_row(0, '-', VGA_COLOR_BLUE);
    set_bg(VGA_COLOR_BLUE);
    print_string(" Terminal ");
    print_string_centre(" Hello World OS ");

    char time[6];
    get_time(time);
    print_at_end(time);

    set_bg(VGA_COLOR_LIGHT_BLUE);
    fill_row(24, ' ', VGA_COLOR_BLUE);
    print_string("-$:> ");

    asm volatile("sti");
    while(1) {
        asm volatile("hlt"); 
    }
}