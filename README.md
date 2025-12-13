---

by Jerry McIntosh

---

# - WHY HIRE ME -
Systems programmer specializing in low-level optimization and x86-64 assembly.  
Expert at translating C algorithms into hand-tuned assembly for maximum CPU performance.  
Thrive on bare-metal coding, memory efficiency, and concurrent data structures.

Current project: Thread-safe B-Tree in pure assembly — 58k ops/sec under contention on laptop hardware.

---

## Real-World Performance (Dec 2025)

## Multithreaded Performance (December 13, 2025 — Dell XPS 15 9510, i7-11800H 8c/16t)

| Phase                          | Threads | Ops per Thread | Total Operations     | Workload Type                                   | Wall Time (avg) | Throughput (avg)    |
|--------------------------------|---------|----------------|----------------------|-------------------------------------------------|-----------------|---------------------|
| 1. Concurrent inserts          | 4       | 524,288        | 2,097,152 inserts    | Low contention                                  |                 |                     |
| 2. Concurrent insert + delete  | 8       | 524,288        | 4,194,304 inserts    |                                                 |                 |                     |
|                                |         |                | 4,194,304 deletes    | **High contention** (splits + merges + borrows) |                 |                     |
| **Total**                      | up to 8 |                | **8,388,608 mixed**  | Thread-safe, global rwlock                      | **~143 s**      | **~58,800 ops/sec** |

> Single global pthread_rwlock — conservative, deadlock-free design.  
> Survives extreme rebalancing contention on a laptop.  
> Run script: `./go_demo.sh` (multiple runs recommended for averaging)

---

<img width="639" height="409" alt="BTree_MT_Screenshot _20251213_153034" src="https://github.com/user-attachments/assets/0768dee1-6d28-4800-97ba-fe9b8bafd366" /><br>
<img width="638" height="489" alt="BTree_MT_Output_20251213_154337" src="https://github.com/user-attachments/assets/1fc483f0-d375-4c27-a3b3-e7ce2b411485" /><br>
<img width="635" height="47" alt="BTree_MT_LSCPU_20251213_153325" src="https://github.com/user-attachments/assets/5d82c12d-b763-4273-815b-dbb713ecda3c" /><br>

---

# INTRODUCTION
This is an Assembly Language implementation of a multi-threaded BTree (Multiway-Tree).  The BTree is implemented as a shared-library with a C interface.  There is also a C demo/benchmark program.

The BTree implementaton is based on a C++ implementation found at:

[GeeksforGeeks: Delete Operation in B-Tree](https://www.geeksforgeeks.org/delete-operation-in-b-tree/?ref=lbp)

which is close to the CLRS implementation. Translated to C, then fully reimplemented in x86-64 assembly for maximum performance and control.

## LIST OF REQUIREMENTS

+ Linux OS
+ Programming languages: C and Assembly
+ Netwide Assembler (NASM), the GCC compiler, and the make utility
+ your favorite text editor
+ and working at the command line

---

# CREATE THE DEMO
Run the following command in the `BTree_MT-main` folder:
```bash
sh ./btree_make.sh
```

---

# RUN THE DEMO
In folder `demo` enter the following command:
```bash
./go_demo.sh
```

---

# THINGS TO KNOW
You can modify a define in the C header file `main.h` in the `demo` folder:
```c
#define DATA_COUNT    (256 * 1024)
```
To modify this define I recommend changing the 256. Modifying this define will change the behavior of the demo program and it's output.

NOTE: The demo program will not check for a negative value for `DATA_COUNT`.

This demo uses multi-threading to demonstrate the integrity of the BTree(MT) library when in use by multiple users at the same time.  The program will launch four threads that each insert 524,288 objects into the tree, a total of 2,097,151 inserts.  Then a tree walk is preformed that will output the 2,097,151 objects.  Next eight threads are launched: four threads that each delete 524,288 objects from the tree, while four more threads insert another 524,288 objects each, a total of 2,097,151 objects will remain in the tree.  All this can be confirmed by the output file `out.txt` in the `demo` folder.

Best way to search the `out.txt` file:

+ Search for `tree walk` to find the beginning of a tree walk.
+ Search for `2097151:` to find the last object output from a tree walk.

Also, the BTree insert function will look for a duplicate object before performing an insertion.  If a duplicate exists in the BTree, the insertion function will return without performing an insertion.  In other words, duplicate keys are not aloud in this version of BTree.

There are calls to `printf` in the `btree.asm` file.  They are for demo purposes only and exist in conditional-blocks controled by the existence of macro BTREE_DEBUG. The format and text strings passed to `printf` are in the `.data` section of the `btree.asm` file within a conditional-block.

Have Fun!

---

