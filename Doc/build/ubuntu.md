# safer 

## Overview

* Mac Build Instructions

## Compatible

* OS
  * Ubuntu (Tested by Ubuntu 16.04.2 LTS) 

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

Install Tools
```
sudo apt-get install make cmake clang
```

Switch the compiler to clang.
```
sudo update-alternatives --config c++  
sudo update-alternatives --config cc
```

Install for dependency
```
apt-get install libhunspell-1.3.0 libhunspell-dev
apt-get install libxerces-c3-dev
apt-get install libboost1.58-dev libboost-system1.58.0 libboost-program-options1.58-dev libboost-filesystem1.58-dev libboost-date-time1.58-dev libboost-regex1.58-dev
```

Build 
```
cmake . 
make
```

## Licence

[MIT](https://opensource.org/licenses/mit-license.php)

## Author

[Akihiro Sugeno](https://github.com/asugeno)


