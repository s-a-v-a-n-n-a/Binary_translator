# Binary_translator
***
**DESCRIPTION**
---------------
This project translates my assembler (AASM) used in [My_CPU](https://github.com/s-a-v-a-n-n-a/My_CPU) into assembly x86-64 binary code. Final file is executive. To help myself write big code on AASM I used my language from [My_compiler](https://github.com/s-a-v-a-n-n-a/My_compiler) and shellcode.

**FUNCTIONAL**
--------------
Every command from [Command.h](Consts_and_structures/Commands.h) is processing and generating a machine code in a special buffer. 
Than some headers that are used in elf-files generating in a new buffer. Special buffer is being copied after it. To get some memory for program stack and helping some earlier written functions like PRINT and SCAN more bytes are being added to the new buffer. This array is being written to the file.

**LITERATURE AND LINKS**
-----------------------
To learn how simple elf format works instruments *readelf* and *xxd* were used.

Materials to translate AASM into x86-64 were used from [this web-site](http://www.cs.loyola.edu/~binkley/371/Encoding_Real_x86_Instructions.html) and [this one](http://ref.x86asm.net/coder.html#x4A).
