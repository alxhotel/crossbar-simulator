# Crossbar Simulator

## Requirements

- C++ 11
- CMake
- Qt5

## Library depedencies

- libQasm
- naxos

## Installation

```sh
sudo apt-get install build-essential libfontconfig1 qt5-default
```

## Build

```sh
mkdir cbuild
cd cbuild
cmake ..
make
```

## Structure

```
- gui			-- GUI elements
- crossbar		-- Crossbar model
- parser		-- cQASM parser
- libs			-- Library dependencies
```
