# Notation
Purpose: generate string asm from raw byte then using __asm__ inside c for running

Evaluate: __asm__ require string literal

Next purpose: Generate .s or .asm file for extern uising from c

Approach:
+ choosing .s for taking advantage of as in gcc instead of install nasm for .asm file
+ need a python to handle gen and testing

More detail in f_project
