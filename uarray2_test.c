#include <stdbool.h>
#include <stdio.h>
#include "uarray2.h"

const int DIM1 = 5;
const int DIM2 = 7;
const int ELEMENT_SIZE = sizeof(int);
const int MARKER = 99;

int main(int argc, char *argv[])
{
        (void)argc;
        (void)argv;

        UArray2_T test_array;
        bool OK = true;

        test_array = UArray2_new(DIM1, DIM2, ELEMENT_SIZE);
        (void)test_array;
        /*
        OK = (UArray2_width(test_array) == DIM1) &&
             (UArray2_height(test_array) == DIM2) &&
             (UArray2_size(test_array) == ELEMENT_SIZE);*/


        
        /* Note: we are only setting a value on the corner of the array */
        /*
        *((number *)UArray2_at(test_array, DIM1 - 1, DIM2 - 1)) = MARKER;

        printf("Trying column major\n");
        UArray2_map_col_major(test_array, check_and_print, &OK);

        printf("Trying row major\n");
        UArray2_map_row_major(test_array, check_and_print, &OK);
        */
        UArray2_free(&test_array);
        

        printf("The array is %sOK!\n", (OK ? "" : "NOT "));
        

}
