/**
    \file
    File with functions for reading from file

    Thank you for using this program!
    \authors Anna Savchuk
    \note    You need to delete all arrays that were taken \n
    \date    Last update was 10.31.20 at 19:43
*/
#ifndef READING_FILE_H
#define READING_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

size_t get_size_of_file (FILE *file);

char  *get_the_text     (FILE *file, size_t *length, int mode_read);

size_t get_num_lines    (char *buf, size_t length);

char  *reading_file     (const char *file_name, size_t *length, size_t *num_lines, int mode_read);

void   put_line         (FILE* out, unsigned char* put_ptr, size_t ptr_length);

#endif
