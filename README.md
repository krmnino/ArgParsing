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

- **Input arguments**: none.
- **Output**: a pointer to the allocated singleton ArgParsing object.
- This method will allocate a single instance of the `ArgParsing` class. Only one instance of the `ArgParsing` class is allowed to exist throughout the execution of the program.

### `void set_input_args(int input_argc, char** input_argv)`

- **Input arguments**:
  - `input_argc`: the number of C strings in program's argv to be passed to the `ArgParsing` object.
  - `input_argv`: an array of pointers to C strings to be passed to the `ArgParsing` object.
- **Output**: none.
- This method will assign the pointer to the array of strings containing the program's argument list (argv) and how many of them are in the list (argc) to the `ArgParsing` instance.

### `int set_arg_table(APTableEntry* arg_table_ptr, size_t n_entries)`

- **Input arguments**: Pointer to an array of `APTableEntry` objects that will be used to define `ArgParsing`'s argument table.
- **Output**: return 0 if no errors occurred. Return 1 if an error was encountred plus an informational message.
- This method will initialize an argument table in the `ArgParsing` object. This table of arguments will be used to validate the program's command line arguments and hold their values once parsed and validated.
- This function performs validation to prevent duplicate argument identifiers, use of reserved keywords, etc.

### `int set_arg_table(std::vector<APTableEntry>& arg_table)`

- **Input arguments**:
- **Output**:

### `int parse()`

- **Input arguments**:
- **Output**:

### `std::string get_arg_value(std::string arg_key, bool is_abbr_input)`

- **Input arguments**:
- **Output**:

## Changelog

### v1.0

- Initial release.
