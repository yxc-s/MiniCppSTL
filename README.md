[中文版 (Chinese Version)](./README_zh.md)  
# MiniCppSTL

## Repository Introduction
#### This repository implements a lightweight C++ Standard Template Library. It extensively uses template metaprogramming, static and dynamic polymorphism, conditional compilation, and other techniques to achieve an efficient lightweight STL.

## Implemented Features:
#### Allocator: Simple logic, uses operator new() and operator delete() for memory allocation and deallocation.
#### Containers: Vector, Array, Deque, List, and series of Red-Black Tree containers.
#### Iterators: Iterators are implemented as nested classes within containers, and all iterators inherit from an abstract class `mstl_iterator_base` that mandates the functionalities an iterator must implement.
#### Adapters: Queue, Stack, Priority Queue (Heap).
#### Algorithms: Sorting, Binary Search, Min/Max Finding, etc.
#### Utilities: Perfect Forwarding, Move Semantics, Pair, Pointer Management objects, etc.

## Usage
#### Requires compiler support for C++17.
#### Include the files and you can use them directly.
#### All basic functionalities have been tested in `test.cpp`, but no guarantee is provided that there will be no bugs in certain use cases.
#### You are free to use the contents of this repository as you wish.
#### If this repository helps you in your learning or work, please give it a Star to support!

## TODOs:
#### More efficient memory management strategies.
#### Fill out the algorithm library.
#### Higher-level data structures?


