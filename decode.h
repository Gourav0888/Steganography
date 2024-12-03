#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAX_DATA_BUF_SIZE 8
#define MAX_FILE_EXTN_SIZE 5
 
typedef struct _DecodeInfo
{
    //input .bmp file info
    char *dec_bmp_fname;  
    FILE *fptr_dec_bmp;
    char data_buffer[MAX_DATA_BUF_SIZE];

    //output file info
    char *output_fname;
    FILE *fptr_output_file;
    char output_file_extn[MAX_FILE_EXTN_SIZE];
    char output_file;     // Y if ouput file is present N if not
    long output_fsize;
}DecodeInfo;
 
/*Read and validate decode args*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
 
/*Perform the  decoding*/
Status do_decoding(DecodeInfo *decInfo);

/*Open files and get file pointers*/
Status open_input_bmp(DecodeInfo *decInfo);

/*Decode magic string from bmp file*/
Status decode_and_check_magic_string(char *decode_string, DecodeInfo *decInfo);

/*Decode output file extn*/
Status decode_output_file_extn(DecodeInfo *decInfo);

/*Get output extn size*/
uint decode_and_get_output_extn_size(FILE *fptr);
 
/*Open output file*/
Status open_output_file(DecodeInfo *decInfo);

/*Decode secret data*/
Status decode_secret_data(DecodeInfo *decInfo);

/*Get secret file size*/
Status decode_secret_data_size(DecodeInfo *decInfo);

/*Decode from lsb*/
Status decode_lsb_to_byte(char *data_buffer, char *data);

void d_progress();

#endif
