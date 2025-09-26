/**************************************************************
 *
 *                    unblackedges.c
 *
 *     Assignment: iii (CS 40 A2)
 *     Authors:    <tvales01,>
 *     Date:       <2025-09-25>
 *
 *     Transform PBM input by removing “black edge” pixels. A black
 *     edge pixel is any black pixel on the border, or 4-connected to
 *     another black edge pixel. Algorithm: read PBM with Pnmrdr into
 *     Bit2 img; BFS from all black border pixels over 4-neighbors,
 *     marking them in a same-size Bit2 edges; finally set those img
 *     pixels to white and emit plain PBM (P1).
 *
 *     Dependencies:
 *       pnmrdr.h, bit2.h, assert.h, mem.h, queue.h, stdlib/stdio.
 *
 *     Checked runtime errors (CREs):
 *       >1 argument; not PBM (md.type != Pnmrdr_bit); width<=0 or
 *       height<=0; file open failure; reader errors.
 *
 *     Output:
 *       Prints P1 header and pixels with spaces; newline at end of row.
 *
 **************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "assert.h"
#include "bit2.h"
#include "pnmrdr.h"
#include "queue.h"
#include "mem.h"

static void check_input(FILE *in);
static void store_in_bit2(Pnmrdr_T file);
static void print_pbm(Bit2_T img);
static void print_bit(int col, int row, Bit2_T bit2, int bit, void *cl);
static void check_black_edge(Bit2_T img);
static void enq_black(Bit2_T img, int col, int row, Queue_T bitQ, Bit2_T edges);
static void check_black_neighbors(Bit2_T img, Queue_T bitQ, Bit2_T edges);
static void black_to_white(int col, int row, Bit2_T bit2, int bit, void *cl);

/* Struct holds the index of a bit in bit2 */
typedef struct Index {
        int col;
        int row;
} *Index;

/********** main ********
 *
 * Parameters:
 *      int argc:     number of arguments given in the command-line
 *      char *argv[]: array that stores all the arguments
 *
 * Return:
 *      EXIT_SUCCESS (0) if program runs successfully, CRE otherwise
 *
 * Expects:
 *      a pbm file given in the command-line or stdin
 * 
 * Notes:
 *      CRE if more than one argument given, input file cannot be opened, error
 *      encountered reading file, memory allocation fails
 ************************/
};

typedef struct Index *Index;

/********** main ********
 * Transform PBM input by removing black edge pixels (predicate program).
 *
 * Parameters:
 *      int argc, char *argv[]:
 *          0 args  -> read PBM from stdin
 *          1 arg   -> read PBM from argv[1]
 *          >1 args -> CRE
 *
 * Returns:
 *      EXIT_SUCCESS on normal completion after writing a plain PBM (P1) to
 *      stdout.
 *
 * Effects:
 *      Opens input file when provided; builds a Pnmrdr; validates PBM type;
 *      constructs Bit2 image; performs edge-removal; prints P1 PBM to stdout.
 *
 * Checked run-time errors (CRE):
 *      - argc > 2
 *      - fopen failure when a filename is given
 *      - Pnmrdr rejects input or md.type != Pnmrdr_bit
 *      - width <= 0 or height <= 0
 ************************/
int main(int argc, char *argv[])
{
        assert(argc <= 2);

        FILE *in = NULL;

        if (argc == 2) {
                in = fopen(argv[1], "rb");
                assert(in != NULL);
        }
        else {
                in = stdin;
        }

        check_input(in);
        fclose(in);

        return EXIT_SUCCESS;
}

/********** check_input ********
 *
 * Checks if the given file is a valid PBM file. 
 *
 * Parameters:
 *      FILE *in: a pointer to a file object (PBM file)
 *
 * Return:
 *      none
 *
 * Notes:
 *      This function will free the memory it allocates.
 * 
 *      CRE if input is not a valid pbm
 * 
 ************************/
static void check_input(FILE *in)
/********** check_input ********
 * Validate that input is a PBM (bitmap) and dispatch reading/processing.
 *
 * Parameters:
 *      FILE *in: open stream (stdin or file)
 *
 * Returns:
 *      None
 *
 * Effects:
 *      Constructs a Pnmrdr, checks md.type == Pnmrdr_bit, calls store_in_bit2.
 *
 * CRE
 *      CRE if Pnmrdr rejects input or type is not PBM
 ************************/
void check_input(FILE *in)
{
        Pnmrdr_T file = Pnmrdr_new(in);
        Pnmrdr_mapdata data = Pnmrdr_data(file);
        assert(data.type == Pnmrdr_bit);

        store_in_bit2(file);

        Pnmrdr_free(&file);
}

/********** store_in_bit2 ********
 *
 * Stores all the pixels from the given file in Bit2_T
 *
 * Parameters:
 *      Pnmrdr_T file: the opened file that's ready to be read
 *
 * Return:
 *      none
 *
 * Expects:
 *      a valid pbm file that has been opened
 *
 * Notes:
 *      this function will free the memory it allocates. the 2d array from this
 *      function is passed and used in other functions.
 * 
 *      this function will also call the print_pbm function to print the result
 * 
 *      CRE if width or height <= 0     
 * 
 ************************/
static void store_in_bit2(Pnmrdr_T file)

/********** store_in_bit2 ********
 * Read PBM pixels into a Bit2 grid and run the unblackedges transform.
 *
 * Parameters:
 *      Pnmrdr_T file: bitmap reader (width, height > 0)
 *
 * Returns:
 *      None
 *
 * Effects:
 *      Allocates Bit2 img; fills from Pnmrdr_get; calls check_black_edge();
 *      prints P1 PBM via print_pbm(); frees img.
 *
 * CRE
 *      CRE if width/height <= 0 or reader errors
 ************************/
void store_in_bit2(Pnmrdr_T file)
{
        Pnmrdr_mapdata data = Pnmrdr_data(file);
        assert(data.width > 0 && data.height > 0);
        int width = data.width;
        int height = data.height;

        /* 2D bit array that will store the original image*/
        Bit2_T img = Bit2_new(width, height);

        /* Walk through each bit and store in Bit2 */
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        int bit = Pnmrdr_get(file);
                        Bit2_put(img, col, row, bit);
                }
        }

        check_black_edge(img);
        print_pbm(img);

        Bit2_free(&img);
}

/********** check_black_edge ********
 *
 * Check and store each black edge pixel of the image and in a parallel bit2
 * array and queue which will be used to track neighboring black pixels.
 *
 * Parameters:
 *      Bit2_T img: the bit2 array that stores each pixel of the image
 *
 * Return:
 *      none
 *
 * Expects:
 *      a valid Bit2_T that has been initialized
 *
 * Notes:
 *      this function dynamically allocates memory for Queue_T which will be
 *      used for checking black neighbors. the memory is freed in this function
 *      HOWEVER, the memory of the pointers in the queue will NOT be freed in 
 *      this function
 * 
 *      this function also creates another Bit2_T which is a parallel to the 
 *      given array and marks traversed back pixels. its memory is also freed 
 *      in this function.
 * 
 ************************/
static void check_black_edge(Bit2_T img)

/********** check_black_edge ********
 * Mark all border-connected black pixels using BFS and remove them.
 *
 * Parameters:
 *      Bit2_T img: input/output bitmap (1=black, 0=white)
 *
 * Returns:
 *      None
 *
 * Effects:
 *      Allocates a same-size Bit2 edges marking edge-connected black pixels.
 *      Enqueues black border pixels; runs BFS over 4-neighbors; then
 *      calls remove_black() to write 0s at marked locations; frees edges.
 ************************/
void check_black_edge(Bit2_T img)
{
        /* Queue to check each black edge pixel */
        Queue_T bitQ = Queue_new();
        /*
         * Bit2 is a parallel array to original image that will mark the bits 
         * that need to be unblacked
         */
        Bit2_T edges = Bit2_new(Bit2_width(img), Bit2_height(img));

        /* The two for loops check for black pixels at the very edge */
        for (int col = 0; col < Bit2_width(img); col++)  {
                enq_black(img, col, 0, bitQ, edges);
                enq_black(img, col, Bit2_height(img) - 1, bitQ, edges);
        }

        for (int row = 0; row < Bit2_height(img); row++) {
                enq_black(img, 0, row, bitQ, edges);
                enq_black(img, Bit2_width(img) - 1, row, bitQ, edges);   
        }

        check_black_neighbors(img, bitQ, edges);
        Bit2_map_row_major(edges, black_to_white, img);

        Bit2_free(&edges);
        Queue_free(&bitQ);
}

/********** black_to_white ********
 *
 * Callback function for the Bit2 map function that which convert all black edge
 * pixels to white.
 *
 * Parameters:
 *      int col:     the col index of the current pixel
 *      int row:     the row index of the current pixel
 *      Bit2_T bit2: the Bit2 array that marks the black pixels to be unblacked
 *      int bit:     the bit value of the current pixel
 *      void *cl:    this void * is the Bit2 array of the original image
 *
 * Return:
 *      none
 *
 * Notes:
 *      this function will update the content of the Bit2 array given as the 
 *      closure variable. It converts all the black edge pixels to white
 * 
 ************************/
static void black_to_white(int col, int row, Bit2_T bit2, int bit, void *cl)
/********** remove_black ********
 * For every marked edge pixel, set corresponding img pixel to white (0).
 *
 * Parameters:
 *      Bit2_T img, Bit2_T edges
 *
 * Returns:
 *      None
 ************************/
void remove_black(Bit2_T img, Bit2_T edges)
{
        Bit2_map_row_major(edges, black_to_white, img);
}


/********** black_to_white (map callback) ********
 * If the edges bit at (col,row) is 1, set img(col,row) to 0.
 *
 * Parameters:
 *      int col, int row, Bit2_T edges, int bit, void *cl (img as closure)
 ************************/
void black_to_white(int col, int row, Bit2_T bit2, int bit, void *cl)
{
        (void)bit2;
        /*
         * If the current bit in the parallel array is 1, it means that pixel  
         * should be unblacked in the original img
         */
        if (bit == 1) {
                Bit2_put((Bit2_T)cl, col, row, 0);
        }
}

/********** check_black_neighbors ********
 *
 * Checks all the neighbors of black edge pixels and mark them in the parallel
 * array
 *
 * Parameters:
 *      Bit2_T img:   the bit2 array of the original image
 *      Queue_T bitQ: the queue used to track black neighbors
 *      Bit2_T edges: the bit2 array parallel to img to mark traversed black 
 *                    edges
 *
 * Return:
 *      none
 *
 * Notes:
 *      this function calls another function that enqueues the queue and modify
 *      the parallel array
 * 
 *      all memory stored in the queue will be freed after this function.
 * 
 ************************/
static void check_black_neighbors(Bit2_T img, Queue_T bitQ, Bit2_T edges)
/********** check_black_neighbors ********
 * BFS pop from queue and examine 4-neighbors, enqueueing newly discovered
 * edge-connected black pixels.
 *
 * Parameters:
 *      Bit2_T img, Queue_T bitQ, Bit2_T edges
 *
 * Returns:
 *      None
 ************************/
void check_black_neighbors(Bit2_T img, Queue_T bitQ, Bit2_T edges)
{
        /* Breadth-first traversal to check all neighbors*/
        while (!Queue_empty(bitQ)) {
                Index i = Queue_deq(bitQ);
                int col = i->col;
                int row = i->row;

                /* Check if the 4 neighbors are black */
                if (col - 1 > 0) {
                        enq_black(img, col - 1, row, bitQ, edges);
                if (col - 1 >= 0) {
                        enq_if_black(img, col - 1, row, bitQ, edges);
                }
                if (col + 1 < Bit2_width(img) - 1) {
                        enq_black(img, col + 1, row, bitQ, edges);
                if (col + 1 < Bit2_width(img)) {
                        enq_if_black(img, col + 1, row, bitQ, edges);
                }
                if (row - 1 > 0) {
                        enq_black(img, col, row - 1, bitQ, edges);
                if (row - 1 >= 0) {
                        enq_if_black(img, col, row - 1, bitQ, edges);
                }
                if (row + 1 < Bit2_height(img) - 1) {
                        enq_black(img, col, row + 1, bitQ, edges);
                if (row + 1 < Bit2_height(img)) {
                        enq_if_black(img, col, row + 1, bitQ, edges);
                }
                FREE(i);
        }
}

/********** enq_black ********
 *
 * Enqueues the pixel at the given index if it's black and has not been 
 * traversed yet
 *
 * Parameters:
 *      Bit2_T img:   the bit2 array of the original image
 *      int col:      the column index of the pixel to be checked
 *      int row:      the row index of the pixel to be checked
 *      Queue_T bitQ: the queue used to track black neighbors
 *      Bit2_T edges: the parallel bit2 array of img to mark traversed black 
 *                    edge pixels
 *
 * Return:
 *      none
 *
 * Expects:
 *      a valid index of img
 *
 * Notes:
 *      this function might enqueue to the given queue and update edges.
 * 
 ************************/
static void enq_black(Bit2_T img, int col, int row, Queue_T bitQ, Bit2_T edges)
/********** enq_if_black ********
 * If (col,row) is black in img and unmarked in edges, mark and enqueue.
 *
 * Parameters:
 *      Bit2_T img, int col, int row, Queue_T bitQ, Bit2_T edges
 *
 * Returns:
 *      None
 *
 * CRE
 *      CRE if indices are out of bounds (via Bit2_get/put)
 ************************/
void enq_if_black(Bit2_T img, int col, int row, Queue_T bitQ, Bit2_T edges)
{
        /* If the pixel at index is black and has not been traversed yet */
        if (Bit2_get(img, col, row) == 1 && Bit2_get(edges, col, row) == 0) {
                Index i;
                NEW(i);
                i->col = col;
                i->row = row;

                /* Enqueue the pixel to the queue and mark it in the bit array*/
                Queue_enq(bitQ, i);
                Bit2_put(edges, col, row, 1);
        }
}

/********** print_pbm ********
 *
 * Prints the pbm with the information in the given Bit2 array
 *
 * Parameters:
 *      Bit2_T img:   the bit2 array of the image to be printed
 *
 * Return:
 *      none
 *
 * Expects:
 *      a Bit2 array that stores PBM data
 *
 ************************/
static void print_pbm(Bit2_T img)
/********** print_pbm / print_bit ********
 * Emit plain PBM (P1) for the current image.
 *
 * Parameters:
 *      Bit2_T img
 *
 * Returns:
 *      None
 *
 * Output:
 *      Prints "P1\nW H\n" followed by W*H 0/1 values, newline at row end.
 ************************/
void print_pbm(Bit2_T img)
{
        printf("P1\n%d %d\n", Bit2_width(img), Bit2_height(img));
        Bit2_map_row_major(img, print_bit, NULL);
}

/********** print_bit ********
 *
 * the callback function to the map function in print_pbm. Prints the current 
 * bit
 *
 * Parameters:
 *      int col:      the column index of the pixel to be checked
 *      int row:      the row index of the pixel to be checked
 *      Bit2_T edges: the parallel bit2 array of img to mark traversed black 
 *                    edge pixels
 *      int bit:      the bit value of the current pixel
 *      void *cl:     a closure variable that is not used
 *
 * Notes:
 *      this function prints to standard output
 * 
 ************************/
static void print_bit(int col, int row, Bit2_T bit2, int bit, void *cl)
{
        (void)row;
        (void)cl;
        printf("%d", bit);
        if (col == Bit2_width(bit2) - 1) {
                printf("\n");
        }
}