Hierarchical-Cache-Simulator
=========================
This repository contains a modular C++ implementation of three level cache hierarchy. It provides a wide range of functionalities such as different choice of replacement policy and different architectures. For eg. One can keep separate L1/L2 cache for data and instruction or unified cache.

How to use
-------------
+ First generate address trace of a program. To understand how to do this, visit [this](https://github.com/mkbera/multilevel-cache-sim) repository. We will assume that trace is stored in a file called `addrtrace.out`
+ Specify your architecture by appropriately modifying the code `simulate.cpp`
+ Compile using `g++ -std=c++11 simulate.cpp L1Cache.cpp L2Cache.cpp -o simulate`
+ Run `./simulate {}addrtrace.out > LLCtrace.out'
Now you have trace for LLC in the file `LLCtrace.out`.
+ Change specifications of LLC according to your need
+ Compile using `g++ -std=c++11 llc_simulation.cpp LLC.cpp CE_Belady.cpp -o llc`
+ Run `./llc LLCtrace.out LLCtrace_hitfile.out LLCtrace_sharefile.out LLCtrace_reusefile.out'
Note that, I am collecting hit, reuse and sharing statistics of cache blocks that's why these files are needed as argument. One can very easily modify/add code in `L1Cache.cpp, L2Cache.cpp, LLC.cpp` to collect different statistics. Moreover, one can add new replacement policies.

This code has been tested on Ubuntu machine and works without any issue. If you have any question, comment, doubt, feel free to get in touch with Abhishek Kumar[theabhishek763@gmail.com]
