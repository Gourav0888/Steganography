#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1000    
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 5
 
typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname; //file name.bmp
    FILE *fptr_src_image; //file pointer address
    uint image_capacity; //file size
    uint bits_per_pixel; 
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname; //secret file name
    FILE *fptr_secret; //secret file pointer
    char extn_secret_file[MAX_FILE_SUFFIX]; //.txt or .c or .sh
    char secret_data[MAX_SECRET_BUF_SIZE]; //store the secret data
    long size_secret_file; //size of secret data

    /* Stego Image Info */
    char *stego_image_fname; //output file name
    FILE *fptr_stego_image; //file pointer
 
} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);                //1st op

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);             //2nd  op

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);      //2nd op p1

/* Get file size */
uint get_file_size(FILE *fptr);      //2nd op p2 (secret.txt file size)

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);   //3rd op

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);  //4th op

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);  //5th  op

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);    //6th op

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);   //7th op  

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);   //inside 7th op

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);  //8th  op
 
void e_progress();

#endif
