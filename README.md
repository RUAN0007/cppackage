# cppackage includes a handful tools collected during my project and reading. 

It is forked and built from an existing cpp11 metaprogramming library. 
Welcome others to complement, share and use!
***

## Introduction
cppackage consist of the following three components, each occupying a directory under include/.

* tlp/ is a C\++ template programming package. I add some TypeTrait and meta functions. 
* loki/ implements some techniques such as Multimethod, Small Object, Thread Model and so on, introduced in Book *Modern C++ Design: Generic Programming and Design Patterns Applied* by **Andrei Alexandrescu**.
* utils/ include some handy tools commonly used for project, such as Timer, Log and so on. 

NOTE: Modules in utils/ depend on those in tlp/ and modules in loki/ depend on those in utils/ 
## Install and Test
### Get 

~~~ bash
git clone git@github.com:RUAN0007/cppackage.git
~~~

### Use
All modules are header only. Just include them in your project. 
All modules are easy to use. Refer to test cases for exact usage. 

### Build

~~~bash
cd cppackage
mkdir build
cd build
cmake ..
make -j4
~~~

### Test TLP

Tlp use its own static test framework for testing.
* Just compile the testcases for testing. If all tests are compiled ok, it means that all tests pass.

Other modules uses Gtest for runtime testing. 

~~~ bash
# First build the projct
./test/static-test/static_test # Static Test for TLP
./test/dynamic-test/dynamic_test # Dynamic Test for Utils and Loki
~~~

### Supported Platform:

- [MAC OS X] supported
- [Linux] supported
- [Windows] not supported

### Supported Compilers:

- [CLANG] 3.4 or later.
- [GCC] 4.8 or later.
- [MSVC] not supported.
