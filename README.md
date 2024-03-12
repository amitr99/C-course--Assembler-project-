# C-course--Assembler-project-
# Assembler
A C program that imitates an assembler. 
The final assignment (#14) of the C course (20465) at The Open University.  
It received a grade of 90.

This project is based on the **_two-pass assembler_** model.  
**Note:** the computer model for this project and the given assembly language are **imaginary**.

## Getting Started

The project was coded and compiled using Ubuntu, but it may run on all Linux versions.

### Usage

Use makefile to compile the project like this:
```
>   make
```

After preparing assembly files **with an `.as` extension**, open *terminal* and pass file names as arguments (without the file extensions) as following:

As for the files x.as, y.as, hello.as we will run:
```
>   assembler x y hello
```
The assembler will generate output files with the same filenames and the following extensions:  
- `.ob` - Object file
- `.ent` - Entries file
- `.ext` - Externals file

## Hardware
- CPU
- RAM with the size of 1024 *words*.
- A *word*'s size in memory is **12 bits**.
- Uses signed *2's complement* arithmetic for integers (with no support for real numbers).

-  ### Registers
   The CPU has 8 registers: 8 general registers (named r0,r1,...,r7).  
   The size of each register is a *word* (12 bits).  
  
  ## Instruction Structure
  Every software *instruction* is encoded into a few *words* in memory (max is 3 *words*).

  Encoding of each instruction word is done using an **imaginary** base64 as defined here:
  ```
0: A   
1: B
2: C
3: D
4: E
5: F
6: G
7: H
8: I
9: J
10: K
11: L
12: M
13: N
14: O
15: P
16: Q
17: R
18: S
19: T
20: U
21: V
22: W
23: X
24: Y
25: Z
26: a
27: b
28: c
29: d
30: e
31: f
32: g
33: h 
34: i
35: j
36: k
37: l
38: m
39: n
40: o
41: p
42: q
43: r 
44: s
45: t
46: u
47: v
48: w
49: x
50: y
51: z
52: 0 
53: 1
54: 2
55: 3
56: 4
57: 5
58: 6
59: 7
60: 8
61: 9
62: +
63: /
```
so that each *word* can be encoded as 2 digits in this base.

## Commands
The commands allowed are:

| Opcode (decimal) | Command Name |
| ---------------- | ------------ |
|	0	|	mov	|
|	1	|	cmp	|
|	2	|	add	|
|	3	|	sub	|
|	4	|	not	|
|	5	|	clr	|
|	6	|	lea	|
|	7	|	inc	|
|	8	|	dec	|
|	9	|	jmp	|
|	10	|	bne	|
|	11	|	red	|
|	12	|	prn	|
|	13	|	jsr	|
|	14	|	rts	|
|	15	|	stop	|


## Directives
A **_directive_** line of the following structure:

1. An **optional** preceding *label*. e.g. `PLACE1: `.
2. A _directive_: `.data`, `.string`, `.entry` or `.extern`.
3. Operands according to the type of the *directive*.

    ### `.data`
   This directive allocates memory in the data image for storing received integers later in memory.
   The parameters of `.data` are valid integers (separated by a comma).  
   e.g. `LABEL1: .data      +7, -56, 4, 9`.
   
   ### `.string`
   This direcive receives a string as an operand and stores it in the data image. It stores all characters by their order in the string, encoded ny their *ASCII* values.
   e.g. `STRING1: .string "abcdef"` is a valid directive.

   ### `.entry`
   This directive outputs a received name of a *label* to the *symbol table*, so that later it will be recognized by other assembly files (and they would be able to use it).
   e.g. 
   ```
   ; file1.as
   .entry HELLO
   HELLO: add #1, r1 
   ```
   ### `.extern`
   This directive receives a name of a *label* as a parameter and declares the *label* as being external (defined in another file) and that the current file shall use it.  
   This way, the directive `.extern HELLO` in `file2.as` will match the `.entry` directive in the previous example.
