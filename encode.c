#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
/* Function Definitions */
  
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */ 
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("INFO: Image width: %u\n", width);
    usleep(500000);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("INFO: Image height: %u\n", height);
    usleep(500000);
    
    // fseek(fptr_image, 0, SEEK_SET);
    rewind(fptr_image);
    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    usleep(500000);
    printf("INFO: Opened %s\n", encInfo->src_image_fname);

    // Secret file 
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    usleep(500000);
    printf("INFO: Opened %s\n", encInfo->secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    usleep(500000);
    printf("INFO: Opened %s\n", encInfo->stego_image_fname);

    // No failure return e_success

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    //open_files();
    printf("INFO: Opening required files\n");
    if(open_files(encInfo)==e_success)
    {
        usleep(500000);
        printf("\033[0;32mINFO: Done\033[0m\n");
    }
    else
    {
        printf("INFO: Error in encoding\n");
        return e_failure;
    }
     
    printf("\033[0;32mINFO: ## Encoding Procedure Started ##\033[0m\n");
    if(check_capacity(encInfo)==e_success)
    {    
        // e_progress();
        printf("\033[0;32mINFO: Done\033[0m\n");     
    }
    else
    {
        printf("INFO: Error in check capacity\n");
        return e_failure;
    }
    
    // usleep(500000);
    printf("INFO: Copying Image Header\n");
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
        e_progress();
        printf("\033[0;32mINFO: Done\033[0m\n");
    }
    else
    {
        printf("INFO: Error in copying  image header\n");
        return e_failure;
    }

    // usleep(500000);
    printf("INFO: Encoding Magic String Signature\n");
    if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
    {
        e_progress();
        printf("\033[0;32mINFO: Done\033[0m\n");
    }  
    else
    {
        printf("INFO: Error in encoding magic string\n");
        return e_failure;
    }

    // usleep(500000);
    printf("INFO: Encoding %s File Extention\n", encInfo->secret_fname);
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo)==e_success)
    {
        e_progress();
        printf("\033[0;32mINFO: Done\033[0m\n");
    }
    else
    {
        printf("INFO: Error in encoding secret file extn\n");
        return e_failure;
    }

    // usleep(500000);
    printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo)==e_success)
    {
        e_progress();
        printf("\033[0;32mINFO: Done\033[0m\n");
    }
    else
    {
        printf("INFO: Error in encoding secret file size\n");
        return e_failure;
    }
    
    // usleep(500000);
    // printf("fptr_bmp = %lu\nfptr_output = %lu\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));
    printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
    if(encode_secret_file_data(encInfo)==e_success)
    {
        e_progress();
        printf("\033[0;32mINFO: Done\033[0m\n");
    }
    else
    {
        printf("INFO: Error in encoding secret file data\n");
        return e_failure;
    }

    
    // printf("fptr_bmp = %lu\nfptr_output = %lu\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));
    
    usleep(500000);
    printf("INFO: Copying Left Over Data\n");
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
        e_progress();
        printf("\033[0;32mINFO: Done\033[0m\n");
    }
    else
    {
        printf("INFO: Error in copying left over data\n");
        return e_failure;
    }
    // printf("fptr_bmp = %lu\nfptr_output = %lu\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    //check secret.txt is not empty
    printf("INFO: Checking for %s size\n", encInfo->secret_fname);
    
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encInfo->size_secret_file==0) 
    {
        printf("ERROR: %s is empty\n", encInfo->secret_fname);
        return e_failure;
    }
    printf("\033[0;32mINFO: Done. Not Empty\033[0m\n");
    // rewind(encInfo->fptr_secret);
    //check capacity (.bpm size > (8*(ms(2) + file ext size(4(int)) + ext storing + data size (4(int)) + data size storing)))
    //size of image or RGB data
    printf("INFO: Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    // uint output_fsize = 8*(sizeof(int) + strlen(MAGIC_STRING) + sizeof(int) + strlen(encInfo->extn_secret_file) + sizeof(long int) + encInfo->size_secret_file);
    uint output_fsize = 8*(strlen(MAGIC_STRING) + sizeof(int) + strlen(encInfo->extn_secret_file) + sizeof(long int) + encInfo->size_secret_file);
    // printf("%lu %lu %lu %lu %u\n",strlen(MAGIC_STRING), sizeof(int), strlen(encInfo->extn_secret_file), sizeof(int), encInfo->size_secret_file);
    // printf("img size = %u\noutput file size = %u\nsecret data size = %u\n", encInfo->image_capacity, output_fsize, encInfo->size_secret_file);
    
    if(output_fsize > encInfo->image_capacity)
    {
        printf("ERROR: %s doesn't have enough space to store %s\n", encInfo->src_image_fname, encInfo->secret_fname);
        return e_failure;
    }
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    uint f_size = 0;
    char ch;
    while((fgetc(fptr))!=EOF) f_size++;
    return f_size;
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char ch;
    while(1)
    { 
        if(ftell(fptr_src_image)==54) break; 

        fread(&ch, sizeof(char), 1, fptr_src_image);
        fwrite(&ch, sizeof(char), 1, fptr_dest_image);
    }
    return  e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int idx = 0;
    fseek(encInfo->fptr_src_image, 54, SEEK_SET);
    fseek(encInfo->fptr_stego_image, 54, SEEK_SET);
    // char buffer[8];
   
    idx=0;
    while(magic_string[idx] != '\0')
    {
        fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);

        encode_byte_to_lsb(magic_string[idx], encInfo->image_data);
        
        fwrite(encInfo->image_data, sizeof(char), 8, encInfo->fptr_stego_image);

        idx++;
    }

    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0; i<= 7; i++)
    {
        image_buffer[i] = image_buffer[i] & (~1);
        unsigned char mask =  (data & (1<<i)) >> i;
        image_buffer[i] = image_buffer[i] | mask;
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int idx = 0;
    int extn_size = strlen(encInfo->extn_secret_file); //(".txt" or ".c" or  ".sh")
    char *byte = (char*)&extn_size;
    //encoding file exten size
    for(idx = 0;  idx < 4; idx++)
    {
        fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);
        // printf("byte = %hhd\n", *(byte+idx));
        encode_byte_to_lsb(*(byte+idx), encInfo->image_data);

        fwrite(encInfo->image_data, sizeof(char), 8, encInfo->fptr_stego_image);
    }

    idx = 0;
    while(file_extn[idx] != '\0')  //".txt"
    {
        fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);
        
        encode_byte_to_lsb(file_extn[idx], encInfo->image_data);

        fwrite(encInfo->image_data, sizeof(char), 8, encInfo->fptr_stego_image);

        idx++;
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char *size_byte = (char*)&file_size;
    for(int i=0; i<8; i++)
    {
        fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(*(size_byte+i),encInfo->image_data);
        fwrite(encInfo->image_data, sizeof(char), 8, encInfo->fptr_stego_image);
    }

    return e_success;
} 

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    fread(encInfo->secret_data, sizeof(char), encInfo->size_secret_file, encInfo->fptr_secret);
    encInfo->secret_data[encInfo->size_secret_file] = '\0';
    // printf("string = %s\n", encInfo->secret_data);
    encode_data_to_image(encInfo->secret_data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    // printf("string = %s\n", encInfo->secret_data);
    return e_success;
}


Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for(int i=0; i<size; i++)
    {
        fread(image_buffer, sizeof(char), 8, fptr_src_image);
        encode_byte_to_lsb(data[i], image_buffer);
        fwrite(image_buffer, sizeof(char), 8, fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{   
    // printf("fptr_bmp = %lu\nfptr_output = %lu\n", ftell(fptr_src), ftell(fptr_dest));

    char ch;
    while(fread(&ch, sizeof(char), 1, fptr_src)>0)
    {
        // printf("fptr_bmp = %lu\nfptr_output = %lu\n", ftell(fptr_src), ftell(fptr_dest));
        // if(feof(fptr_src)) break;
        fwrite(&ch, sizeof(char), 1, fptr_dest);
    }
    
    // printf("fptr_bmp = %lu\nfptr_output = %lu\n", ftell(fptr_src), ftell(fptr_dest));
    return  e_success;
}


void e_progress()
{
    for (int progress = 0; progress <= 100; progress += 10) {
        printf("\033[35m");  
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