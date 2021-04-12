# HiveMindBridge

## Requirements
* [CMake](https://cmake.org/) 3.13
* Any C++ 17 compiler
* [Clang tools](https://clang.llvm.org/docs/ClangTools.html) are used to match the style and warnings used in the project
    * [clang-format](https://clang.llvm.org/docs/ClangFormat.html) to match the coding style
    * [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) for additional compiler warnings
* [Doxygen](https://github.com/doxygen/doxygen) and [graphviz](https://gitlab.com/graphviz/graphviz/) to generate the documentation
* [Protoc](https://developers.google.com/protocol-buffers) and some python deps to build [Propolis](https://github.com/SwarmUS/Propolis). Check Propolis repo for more info

## Using HiveMindBridge

Clone the repo, build and install HiveMindBridge on your system.

```
git clone https://github.com/SwarmUS/HiveMindBridge.git
cd HiveMindBridge
mkdir build
cd build
cmake ..
make -j
sudo make install
```

Once installed, HiveMindBridge can be included in a CMake project using `find_package(swarmus-hivemind-bridge)`. Here is a basic `CmakeLists.txt` example: 

```cmake
cmake_minimum_required(VERSION 3.13)
project(myTestProject)
set (CMAKE_CXX_STANDARD 17)

find_package(swarmus-hivemind-bridge CONFIG REQUIRED)

add_executable(myTestExecutable main.cpp)

target_link_libraries(myTestExecutable swarmus-hivemind-bridge)
```

## Uninstalling HiveMindBridge
Assuming you installed HiveMindBridge with the aforementioned instructions, you can simply uninstall everything by using:

```shell
# Navigate to to where you cloned the repository
cd HiveMindBridge/build

# Delete all the files that were installed.
sudo make uninstall
```
