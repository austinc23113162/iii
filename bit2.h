#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED

typedef struct Bit2_T *Bit2_T;

extern Bit2_T Bit2_new(int col, int row);
extern void Bit2_free(Bit2_T *bit2);

extern int Bit2_width (Bit2_T bit2);
extern int Bit2_height(Bit2_T bit2);

extern int Bit2_get(Bit2_T bit2, int col, int row);
extern int Bit2_put(Bit2_T bit2, int col, int row, int bit);

extern void Bit2_map_row_major(
        Bit2_T bit2,
        void apply(int col, int row, Bit2_T bit2, int bit, void *cl),
        void *cl);

extern void Bit2_map_col_major(
        Bit2_T bit2,
        void apply(int col, int row, Bit2_T bit2, int bit, void *cl),
        void *cl);

#undef T
#endif 