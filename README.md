# HiveMindBridge

## Requirements
### Nanopb

HiveMindBridge relies on nanopb underneath for the serialization and deserialization of Protobuf messages. For a better integration with the build system, SwarmUS has forked the nanopb repository. You must install nanopb to use HiveMindBridge :

```shell
git clone https://github.com/SwarmUS/nanopb.git
cd nanopb
mkdir build
cmake ..
make -j
sudo make install
```

## Using HiveMindBridge

Clone the repo, build and install HiveMindBridge on your system.

```
git clone https://github.com/SwarmUS/HiveMindBridge.git
cd HiveMindBridge
mkdir build
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
Assuming you installed HiveMindBridge with the aforementioned instructions, you can simply uninstall everything by using the CMake install manifest:

```shell
# Navigate to to where you cloned the repository
cd HiveMindBridge/build

# Delete all the files and folders that were installed.
(cat install_manifest.txt; echo) | sudo sh -c 'while read i ; do rm "$i" ; rmdir --ignore-fail-on-non-empty -p "${i%/*}" ; done'
```

This will also delete the nanopb installation.