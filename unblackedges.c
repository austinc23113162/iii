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

void check_input(FILE *in);
void store_in_bit2(Pnmrdr_T file);
void print_pbm(Bit2_T img);
void print_bit(int col, int row, Bit2_T bit2, int bit, void *cl);
void check_black_edge(Bit2_T img);
void enq_if_black(Bit2_T img, int col, int row, Queue_T bitQ, Bit2_T edges);
void check_black_neighbors(Bit2_T img, Queue_T bitQ, Bit2_T edges);
void remove_black(Bit2_T img, Bit2_T edges);
void black_to_white(int col, int row, Bit2_T bit2, int bit, void *cl);

struct Index {
        int col;
        int row;
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
        (void)argv;

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

        Bit2_T img = Bit2_new(width, height);

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
        Queue_T bitQ = Queue_new();

        Bit2_T edges = Bit2_new(Bit2_width(img), Bit2_height(img));

        for (int col = 0; col < Bit2_width(img); col++)  {
                enq_if_black(img, col, 0, bitQ, edges);
                enq_if_black(img, col, Bit2_height(img) - 1, bitQ, edges);
        }

        for (int row = 0; row < Bit2_height(img); row++) {
                enq_if_black(img, 0, row, bitQ, edges);
                enq_if_black(img, Bit2_width(img) - 1, row, bitQ, edges);   
        }

        check_black_neighbors(img, bitQ, edges);
        remove_black(img, edges);

        Bit2_free(&edges);
        Queue_free(&bitQ);
}

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
        if (bit == 1) {
                Bit2_put((Bit2_T)cl, col, row, 0);
        }
}

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
        while (!Queue_empty(bitQ)) {
                Index i = Queue_deq(bitQ);
                int col = i->col;
                int row = i->row;

                if (col - 1 >= 0) {
                        enq_if_black(img, col - 1, row, bitQ, edges);
                }
                if (col + 1 < Bit2_width(img)) {
                        enq_if_black(img, col + 1, row, bitQ, edges);
                }
                if (row - 1 >= 0) {
                        enq_if_black(img, col, row - 1, bitQ, edges);
                }
                if (row + 1 < Bit2_height(img)) {
                        enq_if_black(img, col, row + 1, bitQ, edges);
                }
                FREE(i);
        }
}

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
        if (Bit2_get(img, col, row) == 1 && Bit2_get(edges, col, row) == 0) {
                Index i;
                NEW(i);
                i->col = col;
                i->row = row;

                Queue_enq(bitQ, i);
                Bit2_put(edges, col, row, 1);
        }
}

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

void print_bit(int col, int row, Bit2_T bit2, int bit, void *cl)
{
        (void)row;
        (void)cl;
        printf("%d", bit);
        if (col == Bit2_width(bit2) - 1) {
                printf("\n");
        }
}