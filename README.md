# DASM

Dragon Assembler, a simple assembler for a made-up microprocessor architecture

<p align="center">
  <img src="https://raw.githubusercontent.com/mertyildiran/DASM/master/docs/img/dasm.gif" alt="Dragon Assembler"/>
</p>

## Basic Architecture

- 16-bit data bus.
- 12-bit address bus. Processor can address 4K of memory.
- 4 general purpose registers (GPR's). Will be expanded to 8 later.
- 4 special purpose registers: Program Counter (PC), Address Register (AR), Instruction Register (IR), FLAGS register. Stack pointer (SP) and stack-related operations (PUSH, POP, CALL, RET) will be added later.

Two addressing modes will be used:

- Register indirect: Uses the contents of a general purpose register as a pointer to a memory location(used by load/store instructions)
- PC relative: Adds the offset contained in the instruction to the contents of PC to obtain a pointer to a memory location (used by jump instructions)

Please scroll down for diagrams of the architecture.

## Instruction set

Opcodes are 4 bits, and there are 16 different instruction types:

| Opcode (in hexadecimal) |               Opcode Mnemonic               | Instruction        |
|:-----------------------:|:-------------------------------------------:|--------------------|
| 1                       | LDI                                         | Load Immediate     |
| 2                       | LD                                          | Load               |
| 3                       | ST                                          | Store              |
| 4                       | JZ                                          | Jump if zero       |
| 5                       | JMP                                         | Unconditional jump |
| 6                       | Unused                                      | Unused             |
| 7                       | ADD, SUB, AND, OR, NOT,  XOR, MOV, INC, DEC | ALU operations     |
| 8                       | PUSH                                        | Push to stack      |
| 9                       | POP                                         | Pop from stack     |
| A                       | CALL                                        | Call a procedure   |
| B                       | RET                                         | Return from a proc.|

Later on, we will add the instructions PUSH, POP, CALL and RET to this list. Note that all these instructions require a stack, and therefore we have to implement a stack and a SP register before implementing them.


### Version

0.2.9

### Installation

On the maintainer’s system(*Make sure you have automake installed*):

```Shell
aclocal # Set up an m4 environment
autoconf # Generate configure from configure.ac
automake --add-missing # Generate Makefile.in from Makefile.am
./configure # Generate Makefile from Makefile.in
make distcheck # Use Makefile to build and test a tarball to distribute
```

On the end-user’s system:

```Shell
./configure # Generate Makefile from Makefile.in
make # Use Makefile to build the program
make install # Use Makefile to install the program
dasm example.asm
```

### Special Thanks to

Asst. Prof. Mehmet Kadir Baran at Marmara University

for [this](http://marmara-cse-lectures.com/comparch/) magnificent educational material about Microprocessors and for giving Dragon Assembler a head start with his example source code for a simple assembler.

## Diagrams

![Main](https://raw.githubusercontent.com/mertyildiran/DASM/master/docs/img/logisim/main.png)

![ALU](https://raw.githubusercontent.com/mertyildiran/DASM/master/docs/img/logisim/ALU.png)

![GPRs](https://raw.githubusercontent.com/mertyildiran/DASM/master/docs/img/logisim/GPRs.png)

![IR](https://raw.githubusercontent.com/mertyildiran/DASM/master/docs/img/logisim/IR.png)

![PC](https://raw.githubusercontent.com/mertyildiran/DASM/master/docs/img/logisim/PC.png)

![CU](https://raw.githubusercontent.com/mertyildiran/DASM/master/docs/img/logisim/CU.png)

### License

The MIT License (MIT)

Copyright (c) 2016 Mehmet Mert Yıldıran mert.yildiran@bil.omu.edu.tr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
