#include "bit2.h"
#include "bit.h"
#include "uarray.h"
#include "assert.h"
#include "mem.h"

struct Bit2_T {
        int width;
        int height;
        UArray_T outer_bits;
};

Bit2_T Bit2_new(int col, int row)
{
        Bit2_T bit2;
        NEW(bit2);

        assert(col >= 0 && row >= 0);
        bit2->width = col;
        bit2->height = row;
        bit2->outer_bits = UArray_new(col, sizeof(Bit2_T));

        for (int i = 0; i < col; i++)
        {
                Bit_T inner_bits = Bit_new(row);

                Bit_T *innerp = UArray_at(bit2->outer_bits, i);
                *innerp = inner_bits;
        }
        
        return bit2;
}

int Bit2_width(Bit2_T bit2)
{
        assert(bit2);
        return bit2->width;
}

int Bit2_height(Bit2_T bit2)
{
        assert(bit2);
        return bit2->height;
}

int Bit2_get(Bit2_T bit2, int col, int row)
{
        assert(bit2);
        assert(col >= 0 && col < bit2->width);
        assert(row >= 0 && row < bit2->height);

        Bit_T *innerp = UArray_at(bit2->outer_bits, col);
        Bit_T inner = *innerp;
        assert(sizeof(inner) == UArray_size(bit2->outer_bits));
        

        return Bit_get(inner, row);
}

int Bit2_put(Bit2_T bit2, int col, int row, int bit)
{
        assert(bit2);
        assert(col >= 0 && col < bit2->width);
        assert(row >= 0 && row < bit2->height);
        assert(bit == 0 || bit == 1);

        Bit_T *innerp = UArray_at(bit2->outer_bits, col);
        Bit_T inner = *innerp;
        assert(sizeof(inner) == UArray_size(bit2->outer_bits));

        int prev = Bit_get(inner, row);

        Bit_put(inner, row, bit);
        return prev;
}

void Bit2_map_col_major(Bit2_T bit2,
                                  void apply(int col, int row, Bit2_T bit2,
                                             int bit, void *cl),
                                  void *cl)
{
        assert(bit2);
        for (int col = 0; col < bit2->width; col++) {
                for (int row = 0; row < bit2->height; row++) {
                        Bit_T *innerp = UArray_at(bit2->outer_bits, col);
                        Bit_T inner = *innerp;
                        assert(sizeof(inner) == UArray_size(bit2->outer_bits));

                        apply(col, row, bit2, Bit_get(inner, row), cl);
                }
        }

}


void Bit2_map_row_major(Bit2_T bit2,
                                  void apply(int col, int row, Bit2_T bit2,
                                             int bit, void *cl),
                                  void *cl)
{
        assert(bit2);
        for (int row = 0; row < bit2->height; row++) {
                for (int col = 0; col < bit2->width; col++) {
                        Bit_T *innerp = UArray_at(bit2->outer_bits, col);
                        Bit_T inner = *innerp;
                        assert(sizeof(inner) == UArray_size(bit2->outer_bits));

                        apply(col, row, bit2, Bit_get(inner, row), cl);
                }
        }
}


void Bit2_free(Bit2_T *bit2) {
        assert(bit2 && *bit2);

        for (int col = 0; col < (*bit2)->width; col++) {
                Bit_T *innerp = UArray_at((*bit2)->outer_bits, col);
                Bit_free(innerp);
        }
        UArray_free(&(*bit2)->outer_bits);
        FREE(*bit2);
}