# Steganography in C

This project implements Steganography, a technique for hiding files within an image, using C programming. It supports hiding various types of files, such as `.txt`, `.sh`, and `.c` files, within BMP images.

## Features:
- Hide any text-based file (`.txt`, `.sh`, `.c`, etc.) inside a BMP image.
- Extract hidden files from the BMP image.
- The image remains visually unchanged, but the file's data is embedded within its pixels.
  
## How It Works:
1. **File to Binary Conversion**: 
   - The content of the input file (e.g., `.txt`, `.sh`, `.c`) is converted into binary form.
   
2. **Embedding in the Image**: 
   - The binary data of the file is embedded into the least significant bits (LSBs) of the image’s pixel data.
   
3. **Extracting the Hidden File**: 
   - The hidden file can be extracted by reading the LSBs of the image pixels and converting the binary data back into the original file format.

## Usage:
1. **Hiding a File**:
   - Provide an input file (e.g., `secret.txt`, `script.sh`, or `code.c`) and a BMP image.
   - The program will embed the file's contents into the image.

2. **Extracting a Hidden File**:
   - Provide the steganographic image, and the program will extract the hidden file from it.

## Example:
- **Hiding a `.txt` file**: You can hide a text file, such as a secret message, by embedding it into a BMP image.
- **Hiding a `.sh` script**: The project can also hide executable scripts (`.sh`) or source code files (`.c`) inside an image.

## Requirements:
- A BMP image file as the carrier for embedding the data.
- The file to be hidden can be in `.txt`, `.sh`, `.c`, or any other text-based format.

## Future Enhancements:
- Support for more file formats (non-text binary files).
- Enhanced error checking and compression for more efficient hiding.
