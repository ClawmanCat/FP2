# A\* in subsets of C++
This project contains three different implementations of the A\* algorithm:
- `./imperative` contains a "standard C++" implementation
- `./semifunctional` contains a constexpr-evaluated implementation using some functional features, like monads.
- `./metaprogram` contains a type-trait implementation. 

### Building
Since not all compilers support the required C++20 features, Clang (12 or newer) is recommended to build this project.  
If you use a different compiler - assuming it supports the required features - you may have to copy `profile_clang.cmake` and replace
the evaluation depth settings with the equivalent settings for your compiler.
Conan is required to install the required packages. You can simply use `pip install conan` if you already have Python installed.

To build the project (with Ninja):
```bash
mkdir out
cd out
cmake -DCMAKE_BUILD_TYPE=[DEBUG|RELEASE] -G Ninja -DCMAKE_C_COMPILER=[clang++|clang-cl] -DCMAKE_CXX_COMPILER=[clang++|clang-cl] ../
cmake --build ./[debug|release] --target all
```

### Output
The file `./define_graph.hpp` contains the definition of the graph that will be searched.  
With the default graph, all versions of the algorithm should produce the following path:  
`N1 --> N4 --> N8 --> N11 --> N12 --> N15 --> N16`