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
cmake . 
make
```

## Licence

[MIT](https://opensource.org/licenses/mit-license.php)

## Author

[Akihiro Sugeno](https://github.com/asugeno)


