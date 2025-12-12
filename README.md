
---

Multi-Threaded BTree Implementation in x86_64 Assembly Language as a Shared Library with a C interface

by Jerry McIntosh

---

## Real-World Performance (Dec 2025)

| Machine                  | CPU                     | RAM       | Operations          | Wall Time   | Sustained Throughput     |
|--------------------------|-------------------------|-----------|---------------------|-------------|--------------------------|
| Dell XPS 15 9510         | i7-11800H (8c/16t)      | 32 GB DDR4| 4,194,304 (ins+del) | 135.6 s     | **31,000 ops/sec**       |
| (single-threaded test)   | Tiger Lake, 45 W TDP    |           |                     |             | **~1.95 M ops/sec/core** |

> Fully thread-safe (pthread_rwlock), generic object size, zero external dependencies.

---

# INTRODUCTION
This is an Assembly Language implementation of a multi-threaded BTree (Multiway-Tree).  The BTree is implemented as a shared-library with a C interface.  There is also a C demo program.

The BTree implementaton is based on a C++ implementation found at:

[GeeksforGeeks: Delete Operation in B-Tree](https://www.geeksforgeeks.org/delete-operation-in-b-tree/?ref=lbp)

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

This demo uses multi-threading to demonstrate the integrity of the BTree(MT) library when in use by multiple users at the same time.  The program will launch two threads that each insert 65,536 objects into the tree, resulting in a total of 131,072 objects being inserted into the tree.  Then a tree walk is preformed that will output the 131,072 objects.  Next four threads are launched: two threads will delete the 131,072 objects in the tree, while two more threads insert another 65,536 objects each, resulting in a total of 131,072 objects in the tree.  All this can be confirmed by the output file `out.txt` in the `demo` folder.

Best was to search the `out.txt` file:

+ Search for `tree walk` to find the beginning of a tree walk.
+ Search for `131072:` to find the last object output from a tree walk.

Also, the BTree insert function will look for a duplicate object before performing an insertion.  If a duplicate exists in the BTree, the insertion function will return without performing an insertion.  In other words, duplicate keys are not aloud in this version of BTree.

There are calls to `printf` in the `btree.asm` file.  They are for demo purposes only and can be removed or commented out.  The `printf` code sections are marked with comment lines: `BEGIN PRINTF`; and `END PRINTF`.  The format and text strings passed to `printf` are in the `.data` section of the `btree.asm` file.

Have Fun!

---

