# Overview
I often develop various C++ gadgets under Windows and often write some repetitive code, so I organized some of the codes I wrote or found to form such a small C++ code warehouse. Its main function is first to allow me to no longer have to worry about not having some basic components when writing gadgets again, and it can also make it easier for others to view how some codes are written.  

# Components
## Algorithm
String Algorithm in C++

## Common
Basic definitions or base components

## Encryption
Calculate MD5, Crc32

## FileSystem
base file system interfaces without C++ 17 <filesystem>

## Platform
Platform-related basic code base

## Text
Text encoding conversion, UTF16 to UTF8, etc.

# How to tests it
Clone the current project, please pay attention to the sub-module googleTests, and then use Cmake to generate the project.

# How to use it
This is a pure header file C++ project, you only need to add CppMiniToolkit to the include directory of your C++ project.

`Most components of this project use the C++ 11 standard, and a few components use the C++ 14 standard. Components rarely depend on each other.`