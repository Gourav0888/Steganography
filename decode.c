#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "decode.h" 
#include "types.h"
#include <unistd.h>

Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: Opening Required Files\n");
    usleep(500000);
    if(open_input_bmp(decInfo)==e_success)
    {
        printf("\033[0;32mINFO: Done\033[0m\n");   
    }
    else return e_failure;

    printf("\033[0;32mINFO: ## Decoding Procedure Started ##\033[0m\n");
    printf("INFO: Decoding Magic String Signature\n");
    usleep(500000);
    if(decode_and_check_magic_string(DECODE_STRING, decInfo) == e_success)
    {
        printf("\033[0;32mINFO: Done\033[0m\n");   
    }
    else return e_failure;
    
    // if(decInfo->output_file=='N')
    // {
    printf("INFO: Decoding Ouptut File Extention\n");
    // usleep(500000);
    if(decode_output_file_extn(decInfo) == e_success)
    {
        if(decInfo->output_file=='N')
        { 
            printf("INFO: Output File not mentioned. Creating decoded%s as default\n", decInfo->output_file_extn);
            // usleep(500000);
            // d_progress();
            decInfo->output_fname = (char*)malloc(15*sizeof(char));
            strcpy(decInfo->output_fname, "decoded");             
            strcat(decInfo->output_fname, decInfo->output_file_extn);
        }
        else 
        {
            // printf("OK\n");
            strcat(decInfo->output_fname, decInfo->output_file_extn);
        }
        d_progress();
        printf("\033[0;32mINFO: Done\033[0m\n");   
    }
    else return e_failure;
    // }
    // else
    // {
    //     fseek(decInfo->fptr_dec_bmp, (54 + 8*(sizeof(int) + strlen(DECODE_STRING) + sizeof(int) + strlen(decInfo->output_file_extn))), SEEK_SET);
    // }

    if(open_output_file(decInfo)==e_success)
    {
        // usleep(500000);
        printf("\033[0;32mINFO: Done. Opened all required files\033[0m\n");   
    }
    else return e_failure;

    printf("INFO: Decoding %s File Size\n", decInfo->output_fname);
    // usleep(500000);
    d_progress();
    if(decode_secret_data_size(decInfo) == e_success)
    {
        printf("\033[0;32mINFO: Done\033[0m\n");   
    }
    else return e_failure;

    printf("INFO: Decoding %s File Data\n", decInfo->output_fname);
    // usleep(500000);
    d_progress();
    if(decode_secret_data(decInfo) == e_success)
    {
        printf("\033[0;32mINFO: Done\033[0m\n");   
    }
    else return e_failure;

    return e_success;
}

Status open_input_bmp(DecodeInfo *decInfo)
{
    decInfo->fptr_dec_bmp = fopen(decInfo->dec_bmp_fname ,"r");
    if(decInfo->fptr_dec_bmp==NULL)
    {
        printf("ERROR: Unable to open %s\n", decInfo->dec_bmp_fname);
        return e_failure;
    }
    
    return e_success;
}


Status decode_and_check_magic_string(char *decode_string, DecodeInfo *decInfo)
{
    //skip bmp header
    fseek(decInfo->fptr_dec_bmp, 54, SEEK_SET); 
    int i, dec_string_size = strlen(decode_string);
    
    char alpha; 
    for(i = 0; i < dec_string_size; i++)
    {
        fread(decInfo->data_buffer, sizeof(char), 8, decInfo->fptr_dec_bmp);
        decode_lsb_to_byte(decInfo->data_buffer, &alpha);
        if(alpha != decode_string[i])
        {
            printf("ERROR: Incorrect magic string\n");
            return e_failure;
        }
    }

    return e_success;
}

Status decode_lsb_to_byte(char *data_buffer, char *data)
{
    int i;
    *data = 0;
    for(i =0; i < 8; i++)
    {//0000 0001 -> 1
        char bit = data_buffer[i] & 1;
        if(bit)
        {
            *data = *data | (bit<<i);
        }
    }

    return e_success;
}

Status decode_output_file_extn(DecodeInfo *decInfo)
{
    int ext_size = decode_and_get_output_extn_size(decInfo->fptr_dec_bmp);
    // printf("ext_size=%d\n", ext_size);
    int i;
    for(i=0; i<ext_size; i++)
    {
        fread(decInfo->data_buffer, sizeof(char), 8, decInfo->fptr_dec_bmp);
        decode_lsb_to_byte(decInfo->data_buffer, &(decInfo->output_file_extn[i]));
    }
    decInfo->output_file_extn[i] = '\0';

    // printf("extn = %s\n", decInfo->output_file_extn);
    return e_success;
}

uint decode_and_get_output_extn_size(FILE *fptr)
{
    int f_size, i;
    char buffer[8];
    char *dec_byte = (char*)&f_size;
 
    for(i = 0; i < sizeof(int); i++)
    {
        fread(buffer, sizeof(char), 8, fptr);
        decode_lsb_to_byte(buffer, &dec_byte[i]);
    }

    return f_size;
}

Status open_output_file(DecodeInfo *decInfo)
{
    decInfo->fptr_output_file = fopen(decInfo->output_fname, "w");
    if(decInfo->fptr_output_file==NULL)
    {
        printf("ERROR: Unable to create %s\n", decInfo->output_fname);
        return e_failure;
    }
    printf("INFO: Opened %s\n", decInfo->output_fname);
    return e_success;
}

Status decode_secret_data(DecodeInfo *decInfo)
{
    long data_size, i;
    data_size = decInfo->output_fsize;
    char msg;
    for(i=0; i<data_size; i++)
    {
        fread(decInfo->data_buffer, sizeof(char), 8, decInfo->fptr_dec_bmp);
        decode_lsb_to_byte(decInfo->data_buffer, &msg);
        fputc(msg, decInfo->fptr_output_file);
    }
    // printf("msg = %s\n", msg);
    return e_success;
}

Status decode_secret_data_size(DecodeInfo *decInfo)
{
    long data_size, i;
    char buffer[8];
    char *dec_byte = (char*)&data_size;
    // printf("fp = %lu\n", ftell(decInfo->fptr_dec_bmp));
    for(i=0; i<sizeof(long); i++)
    {
        fread(buffer, sizeof(char), 8, decInfo->fptr_dec_bmp);
        decode_lsb_to_byte(buffer, &dec_byte[i]);
    }

    decInfo->output_fsize = data_size;
    // printf("data size = %ld\n", data_size);
    return e_success;
}

void d_progress()
{
    for (int progress = 0; progress <= 100; progress += 10) {
        printf("\033[36m");  
        printf("\rPROGRESS: %3d%% [", progress); 
        
        for (int i = 0; i < progress / 10; i++) { 
            printf(".");  
        }
        for (int i = progress / 10; i < 10; i++) {
            printf(" ");  
        }
        printf("]");
        fflush(stdout);  
        usleep(100000);  
    }
      
    printf("\033[0m\n");
}