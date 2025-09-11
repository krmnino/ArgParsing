# ArgParsing

A simple command-line argument parser library for C++ programs.

## Repository Structure

- `src`: contains all the source files, Makefile, and bash script required to build the `libArgParsing.so` file.
- `testing`: test driver program to validate operation of the ArgParsing library.

## Requirements and Dependencies

- C++17
- Make

## Build

1. Navigate into `src`.
2. Issue the command `sh Initialize.sh` to prepare the environment before creating the library file.
3. Issue the command `make`. This will generate and place the library file in `../res/libArgParsing.so`.
4. You may now copy the header file `src/ArgParsing.hpp` and `res/libArgParsing.so` to a desired location for your project.

## Usage

Below you will find the publicly exposed methods belonging to the `ArgParsing` class.

### `ArgParsing* ArgParsing_get_instance()`

- Input arguments:
- Output:

### `void set_input_args(int input_argc, char** input_argv)`

- Input arguments:
- Output:

### `int set_arg_table(APTableEntry* arg_table_ptr, size_t n_entries)`

- Input arguments:
- Output:

### `int set_arg_table(std::vector<APTableEntry>& arg_table)`

- Input arguments:
- Output:

### `int parse()`

- Input arguments:
- Output:

### `std::string get_arg_value(std::string arg_key, bool is_abbr_input)`

- Input arguments:
- Output:

## Changelog

### v1.0

- Initial release.
