# hello_world_os
not an OS, it is just a simple kernel with C

source:
https://wiki.osdev.org/Bare_Bones

# 🚀 Hello-World-OS (x86 i686)

A minimalist, 32-bit monolithic kernel-mode terminal built from scratch using **C** and **x86 Assembly**. This project demonstrates the fundamental bridge between high-level logic and raw hardware, bypassing standard libraries to interact directly with the CPU and VGA hardware.

## 🛠 Project Overview

This OS follows the **Multiboot Standard**, allowing it to be loaded by the GRUB bootloader. It initializes a Protected Mode environment, sets up critical x86 data structures (GDT/IDT), remaps the PIC for hardware interrupts, and provides a functional terminal interface.

### Key Features:

* **Custom Bootloader Entry:** Written in Assembly to transition from the bootloader to C.
* **Global Descriptor Table (GDT):** Defines memory segments and privilege levels.
* **Interrupt Descriptor Table (IDT):** Handles hardware interrupts (specifically the Keyboard).
* **VGA Text Mode Driver:** Direct memory access (DMA) to `0xB8000` for color text rendering.
* **Hardware RTC Integration:** Reads the Real-Time Clock directly from CMOS ports.
* **Mini-Shell:** A command-line interface that interprets single-character hardware commands.

---

## 📂 File Structure & Logic

| File | Responsibility |
| --- | --- |
| **`boot.s`** | The entry point. Sets up the Stack Pointer (`esp`) and provides the Multiboot Header. |
| **`kernel.c`** | The "Brain." Contains GDT/IDT setup, Interrupt Handlers, and the Command Logic. |
| **`terminal.h`** | Manages screen state, scrolling logic, and the hardware cursor. |
| **`vga.h`** | Defines color constants and bit-masking for VGA text entries. |
| **`global.h`** | Core utilities: String length, CMOS time retrieval, and I/O port functions (`outb`/`inb`). |
| **`linker.ld`** | Defines the binary layout, ensuring the Multiboot header is at the very beginning. |

---

## ⚙️ How the Hardware "Thinks"

### 1. The Boot Sequence

The CPU starts in 32-bit Protected Mode (thanks to GRUB). `boot.s` allocates 16KB of stack space and calls `kernel_main`. Without this assembly stub, C code cannot run because there would be no place to store local variables or return addresses.

### 2. Memory Protection (GDT)

We define three segments:

1. **Null Segment:** Required by x86 architecture.
2. **Code Segment:** Covers 4GB of memory with execution rights.
3. **Data Segment:** Covers 4GB of memory with read/write rights.

### 3. Talking to the Keyboard (IDT & PIC)

When you press a key, the keyboard sends an IRQ1 signal. We remap the **Programmable Interrupt Controller (PIC)** so that IRQs don't conflict with CPU exceptions. We then point entry 33 in the IDT to our `keyboard_handler`.

---

## ⌨️ Integrated Commands

The OS includes a built-in command interpreter within the kernel:

* `i`: **Information** - Displays OS build details.
* `l`: **List** - Displays a simulated list of project files.
* `c`: **Clear** - Clears the VGA buffer.
* `w`: **Write** - Shows a placeholder for future filesystem implementation.
* `q`: **Quit/Halt** - Disables interrupts and halts the CPU safely.

---

## 🚀 Building and Running

### Prerequisites

You need an `i686-elf` cross-compiler to ensure the code isn't contaminated by your host OS (Windows/Linux) headers.

1. **Compile & Assemble:**
```bash
i686-elf-as boot.s -o boot.o
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

```


2. **Link:**
```bash
i686-elf-gcc -T linker.ld -o myos -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

```


3. **Create ISO (Grub):**
```bash
mkdir -p isodir/boot/grub
cp myos isodir/boot/myos
cp grub.cfg isodir/boot/grub/grub.cfg
grub2-mkrescue -o myos.iso isodir

```


4. **Emulate:**
```bash
qemu-system-i386 -cdrom myos.iso

```



---

## 🧠 Final Thoughts for Newbies

Operating System development is the ultimate "final boss" of programming. It teaches you that:

* Everything is eventually **electricity** (High/Low voltage).
* **Pointers** are just memory addresses in a massive 4GB array of bytes.
* There is no `printf` until **you** write to the memory address `0xB8000`.

*Inspired by [OSDev Wiki](https://wiki.osdev.org/Bare_Bones).*
