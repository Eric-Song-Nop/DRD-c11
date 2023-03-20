#!/bin/bash
cd ../
make
cd test
/home/vagrant/c11tester-benchmarks/clang++ -g -Xclang -load -Xclang /home/vagrant/llvm/build/lib/libCDSPass.so -L/home/vagrant/c11tester -lmodel -Wno-unused-command-line-argument userprog.cc -o userprog.o
LD_LIBRARY_PATH=/home/vagrant/c11tester C11TESTER='--verbose=3' valgrind --leak-check=full --show-leak-kinds=all ./userprog.o