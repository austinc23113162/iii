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

void check_input(FILE *in)
{
        Pnmrdr_T file = Pnmrdr_new(in);
        Pnmrdr_mapdata data = Pnmrdr_data(file);
        assert(data.type == Pnmrdr_bit);

        store_in_bit2(file);

        Pnmrdr_free(&file);
}

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

void remove_black(Bit2_T img, Bit2_T edges)
{
        Bit2_map_row_major(edges, black_to_white, img);
}

//Callback function for the map in remove_black
//This is for every pixel in the edge bit array that marked all the pixels to 
//be removed from the original
void black_to_white(int col, int row, Bit2_T bit2, int bit, void *cl)
{
        (void)bit2;
        if (bit == 1) {
                Bit2_put((Bit2_T)cl, col, row, 0);
        }
}

void check_black_neighbors(Bit2_T img, Queue_T bitQ, Bit2_T edges)
{
        while (!Queue_empty(bitQ)) {
                Index i = Queue_deq(bitQ);
                int col = i->col;
                int row = i->row;

                if (col - 1 > 0) {
                        enq_if_black(img, col - 1, row, bitQ, edges);
                }
                if (col + 1 < Bit2_width(img) - 1) {
                        enq_if_black(img, col + 1, row, bitQ, edges);
                }
                if (row - 1 > 0) {
                        enq_if_black(img, col, row - 1, bitQ, edges);
                }
                if (row + 1 < Bit2_height(img) - 1) {
                        enq_if_black(img, col, row + 1, bitQ, edges);
                }
                FREE(i);
        }
}

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