# safer 

## Overview

* Mac Build Instructions

## Compatible

* OS
  * macOS Sierra or later (Tested by macOS Sierra) 

## Requirement

* Xerces-C++
* libhunspell
* boost Version 1.58.0.0
* boost_program_options
* boost_regex
* boost_filesystem
* boost_system
* boost_date_time
* C++11 compiler

## Tutorial for build (for Mac)

Get source code
```
git clone --recursive https://github.com/asugeno/safer.git . 
```

Install for dependency
```
brew install xerces-c 
brew install hunspell
brew install boost
```

Build 
```
cmake -Dwithout-test=on . 
make
```

## Tutorial for Test (for Mac)

Install Google test
```
mkdir work_dir
cd work_dir
wget https://github.com/google/googletest/archive/release-1.8.0.zip
unzip gtest-1.8.0.zip
cd gtest-1.8.0
./configure
make
make install
```
*** Install from source code.

Test
```
cd <checkout_dir>
cmake .
make
make test
```

## Licence

[MIT](https://opensource.org/licenses/mit-license.php)

## Author

[Akihiro Sugeno](https://github.com/asugeno)


