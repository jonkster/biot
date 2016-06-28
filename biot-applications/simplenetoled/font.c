#include <inttypes.h>
#include "font.h"

/*============================================*/
/*========= Character pointers table =========*/
/*============================================*/

uint8_t * font_table[95] = {
    Font08px_32,
    Font08px_33,
    Font08px_34,
    Font08px_35,
    Font08px_36,
    Font08px_37,
    Font08px_38,
    Font08px_39,
    Font08px_40,
    Font08px_41,
    Font08px_42,
    Font08px_43,
    Font08px_44,
    Font08px_45,
    Font08px_46,
    Font08px_47,
    Font08px_N0,
    Font08px_N1,
    Font08px_N2,
    Font08px_N3,
    Font08px_N4,
    Font08px_N5,
    Font08px_N6,
    Font08px_N7,
    Font08px_N8,
    Font08px_N9,
    Font08px_58,
    Font08px_59,
    Font08px_60,
    Font08px_61,
    Font08px_62,
    Font08px_63,
    Font08px_64,
    Font08px_UA,
    Font08px_UB,
    Font08px_UC,
    Font08px_UD,
    Font08px_UE,
    Font08px_UF,
    Font08px_UG,
    Font08px_UH,
    Font08px_UI,
    Font08px_UJ,
    Font08px_UK,
    Font08px_UL,
    Font08px_UM,
    Font08px_UN,
    Font08px_UO,
    Font08px_UP,
    Font08px_UQ,
    Font08px_UR,
    Font08px_US,
    Font08px_UT,
    Font08px_UU,
    Font08px_UV,
    Font08px_UW,
    Font08px_UX,
    Font08px_UY,
    Font08px_UZ,
    Font08px_91,
    Font08px_92,
    Font08px_93,
    Font08px_94,
    Font08px_95,
    Font08px_96,
    Font08px_la,
    Font08px_lb,
    Font08px_lc,
    Font08px_ld,
    Font08px_le,
    Font08px_lf,
    Font08px_lg,
    Font08px_lh,
    Font08px_li,
    Font08px_lj,
    Font08px_lk,
    Font08px_ll,
    Font08px_lm,
    Font08px_ln,
    Font08px_lo,
    Font08px_lp,
    Font08px_lq,
    Font08px_lr,
    Font08px_ls,
    Font08px_lt,
    Font08px_lu,
    Font08px_lv,
    Font08px_lw,
    Font08px_lx,
    Font08px_ly,
    Font08px_lz,
    Font08px_123,
    Font08px_124,
    Font08px_125,
    Font08px_126
};

/*===================================*/
/*========= Characters data =========*/
/*===================================*/

uint8_t Font08px_32[3] = {2,
    bits2bytes(0,0,0,0,0,0,0,0),
    bits2bytes(0,0,0,0,0,0,0,0)};

uint8_t Font08px_33[2] = {1,
    bits2bytes(1,0,1,1,1,1,1,0)};

uint8_t Font08px_34[4] = {3,
    bits2bytes(0,0,0,0,0,1,1,0),
    bits2bytes(0,0,0,0,0,0,0,0),
    bits2bytes(0,0,0,0,0,1,1,0)};

uint8_t Font08px_35[6] = {5,
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,1,0,1,0,0,0)};

uint8_t Font08px_36[6] = {5,
    bits2bytes(0,1,0,0,1,0,0,0),
    bits2bytes(0,1,0,1,0,1,0,0),
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,1,0,1,0,1,0,0),
    bits2bytes(0,0,1,0,0,1,0,0)};

uint8_t Font08px_37[6] = {5,
    bits2bytes(0,1,0,0,0,1,1,0),
    bits2bytes(0,0,1,0,0,1,1,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(1,1,0,0,1,0,0,0),
    bits2bytes(1,1,0,0,0,1,0,0)};

uint8_t Font08px_38[6] = {5,
    bits2bytes(0,1,1,0,1,1,0,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,1,0,1,1,0,0),
    bits2bytes(0,1,0,0,0,0,0,0),
    bits2bytes(1,0,1,0,0,0,0,0)};

uint8_t Font08px_39[2] = {1,
    bits2bytes(0,0,0,0,0,1,1,0)};

uint8_t Font08px_40[4] = {3,
    bits2bytes(0,0,1,1,1,0,0,0),
    bits2bytes(0,1,0,0,0,1,0,0),
    bits2bytes(1,0,0,0,0,0,1,0)};

uint8_t Font08px_41[4] = {3,
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(0,1,0,0,0,1,0,0),
    bits2bytes(0,0,1,1,1,0,0,0)};

uint8_t Font08px_42[4] = {3,
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0)};

uint8_t Font08px_43[4] = {3,
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,1,1,1,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0)};

uint8_t Font08px_44[2] = {1,
    bits2bytes(1,1,0,0,0,0,0,0)};

uint8_t Font08px_45[4] = {3,
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0)};

uint8_t Font08px_46[2] = {1,
    bits2bytes(1,0,0,0,0,0,0,0)};

uint8_t Font08px_47[4] = {3,
    bits2bytes(1,1,0,0,0,0,0,0),
    bits2bytes(0,0,1,1,1,0,0,0),
    bits2bytes(0,0,0,0,0,1,1,0)};

uint8_t Font08px_N0[6] = {5,
    bits2bytes(0,1,1,1,1,1,0,0),
    bits2bytes(1,0,1,0,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,0,1,0,1,0),
    bits2bytes(0,1,1,1,1,1,0,0)};

uint8_t Font08px_N1[6] = {5,
    bits2bytes(0,0,0,0,0,0,0,0),
    bits2bytes(1,0,0,0,0,1,0,0),
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(0,0,0,0,0,0,0,0)};

uint8_t Font08px_N2[6] = {5,
    bits2bytes(1,0,0,0,0,1,0,0),
    bits2bytes(1,1,0,0,0,0,1,0),
    bits2bytes(1,0,1,0,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,0,1,1,0,0)};

uint8_t Font08px_N3[6] = {5,
    bits2bytes(0,1,0,0,0,1,0,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(0,1,1,0,1,1,0,0)};

uint8_t Font08px_N4[6] = {5,
    bits2bytes(0,0,1,1,0,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,0,1,0,0,1,0,0),
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,1,0,0,0,0,0)};

uint8_t Font08px_N5[6] = {5,
    bits2bytes(0,1,0,1,1,1,1,0),
    bits2bytes(1,0,0,0,1,0,1,0),
    bits2bytes(1,0,0,0,1,0,1,0),
    bits2bytes(1,0,0,0,1,0,1,0),
    bits2bytes(0,1,1,1,0,0,1,0)};

uint8_t Font08px_N6[6] = {5,
    bits2bytes(0,1,1,1,1,1,0,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(0,1,1,0,0,1,0,0)};

uint8_t Font08px_N7[6] = {5,
    bits2bytes(0,0,0,0,0,0,1,0),
    bits2bytes(0,0,0,0,0,0,1,0),
    bits2bytes(1,1,1,1,0,0,1,0),
    bits2bytes(0,0,0,0,1,0,1,0),
    bits2bytes(0,0,0,0,0,1,1,0)};

uint8_t Font08px_N8[6] = {5,
    bits2bytes(0,1,1,0,1,1,0,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(0,1,1,0,1,1,0,0)};

uint8_t Font08px_N9[6] = {5,
    bits2bytes(0,1,0,0,1,1,0,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(0,1,1,1,1,1,0,0)};

uint8_t Font08px_58[2] = {1,
    bits2bytes(0,1,0,0,0,1,0,0)};

uint8_t Font08px_59[2] = {1,
    bits2bytes(1,1,0,0,0,1,0,0)};

uint8_t Font08px_60[5] = {4,
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,1,0,0,0,1,0,0),
    bits2bytes(1,0,0,0,0,0,1,0)};

uint8_t Font08px_61[5] = {4,
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0)};

uint8_t Font08px_62[5] = {4,
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(0,1,0,0,0,1,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0)};

uint8_t Font08px_63[6] = {5,
    bits2bytes(0,0,0,0,0,1,0,0),
    bits2bytes(0,0,0,0,0,0,1,0),
    bits2bytes(1,0,1,0,0,0,1,0),
    bits2bytes(0,0,0,1,0,0,1,0),
    bits2bytes(0,0,0,0,1,1,0,0)};

uint8_t Font08px_64[9] = {8,
    bits2bytes(0,0,1,1,1,0,0,0),
    bits2bytes(0,1,0,0,0,1,0,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,1,0,1,0,1,0),
    bits2bytes(1,0,1,0,1,0,1,0),
    bits2bytes(1,0,1,1,1,0,1,0),
    bits2bytes(0,0,1,0,0,1,0,0),
    bits2bytes(0,0,0,1,1,0,0,0)};

uint8_t Font08px_UA[6] = {5,
    bits2bytes(1,1,1,1,1,0,0,0),
    bits2bytes(0,0,0,1,0,1,0,0),
    bits2bytes(0,0,0,1,0,0,1,0),
    bits2bytes(0,0,0,1,0,1,0,0),
    bits2bytes(1,1,1,1,1,0,0,0)};

uint8_t Font08px_UB[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(0,1,1,0,1,1,0,0)};

uint8_t Font08px_UC[5] = {4,
    bits2bytes(0,1,1,1,1,1,0,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(0,1,0,0,0,1,0,0)};

uint8_t Font08px_UD[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(0,1,1,1,1,1,0,0)};

uint8_t Font08px_UE[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,0,0,0,1,0)};

uint8_t Font08px_UF[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,0,1,0,0,1,0),
    bits2bytes(0,0,0,1,0,0,1,0),
    bits2bytes(0,0,0,0,0,0,1,0)};

uint8_t Font08px_UG[6] = {5,
    bits2bytes(0,1,1,1,1,1,0,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,1,1,1,0,1,0,0)};

uint8_t Font08px_UH[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(1,1,1,1,1,1,1,0)};

uint8_t Font08px_UI[4] = {3,
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(1,0,0,0,0,0,1,0)};

uint8_t Font08px_UJ[5] = {4,
    bits2bytes(0,1,0,0,0,0,0,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(0,1,1,1,1,1,1,0)};

uint8_t Font08px_UK[6] = {5,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,1,0,0,0,1,0,0),
    bits2bytes(1,0,0,0,0,0,1,0)};

uint8_t Font08px_UL[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(1,0,0,0,0,0,0,0)};

uint8_t Font08px_UM[6] = {5,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,0,0,0,1,0,0),
    bits2bytes(0,0,0,0,1,0,0,0),
    bits2bytes(0,0,0,0,0,1,0,0),
    bits2bytes(1,1,1,1,1,1,1,0)};

uint8_t Font08px_UN[6] = {5,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,0,0,0,1,0,0),
    bits2bytes(0,0,0,1,1,0,0,0),
    bits2bytes(0,1,1,0,0,0,0,0),
    bits2bytes(1,1,1,1,1,1,1,0)};

uint8_t Font08px_UO[5] = {4,
    bits2bytes(0,1,1,1,1,1,0,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(0,1,1,1,1,1,0,0)};

uint8_t Font08px_UP[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,0,1,0,0,1,0),
    bits2bytes(0,0,0,1,0,0,1,0),
    bits2bytes(0,0,0,0,1,1,0,0)};

uint8_t Font08px_UQ[6] = {5,
    bits2bytes(0,1,1,1,1,1,0,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(1,0,1,0,0,0,1,0),
    bits2bytes(0,1,0,0,0,0,1,0),
    bits2bytes(1,0,1,1,1,1,0,0)};

uint8_t Font08px_UR[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,0,1,0,0,1,0),
    bits2bytes(0,0,1,1,0,0,1,0),
    bits2bytes(1,1,0,0,1,1,0,0)};

uint8_t Font08px_US[5] = {4,
    bits2bytes(0,1,0,0,1,1,0,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(0,1,1,0,0,1,0,0)};

uint8_t Font08px_UT[6] = {5,
    bits2bytes(0,0,0,0,0,0,1,0),
    bits2bytes(0,0,0,0,0,0,1,0),
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,0,0,0,0,1,0),
    bits2bytes(0,0,0,0,0,0,1,0)};

uint8_t Font08px_UU[5] = {4,
    bits2bytes(0,1,1,1,1,1,1,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(0,1,1,1,1,1,1,0)};

uint8_t Font08px_UV[6] = {5,
    bits2bytes(0,0,0,0,1,1,1,0),
    bits2bytes(0,0,1,1,0,0,0,0),
    bits2bytes(1,1,0,0,0,0,0,0),
    bits2bytes(0,0,1,1,0,0,0,0),
    bits2bytes(0,0,0,0,1,1,1,0)};

uint8_t Font08px_UW[6] = {5,
    bits2bytes(0,0,1,1,1,1,1,0),
    bits2bytes(1,1,0,0,0,0,0,0),
    bits2bytes(0,0,1,1,1,0,0,0),
    bits2bytes(1,1,0,0,0,0,0,0),
    bits2bytes(0,0,1,1,1,1,1,0)};

uint8_t Font08px_UX[6] = {5,
    bits2bytes(1,1,0,0,0,1,1,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(1,1,0,0,0,1,1,0)};

uint8_t Font08px_UY[6] = {5,
    bits2bytes(0,0,0,0,0,1,1,0),
    bits2bytes(0,0,0,0,1,0,0,0),
    bits2bytes(1,1,1,1,0,0,0,0),
    bits2bytes(0,0,0,0,1,0,0,0),
    bits2bytes(0,0,0,0,0,1,1,0)};

uint8_t Font08px_UZ[6] = {5,
    bits2bytes(1,1,0,0,0,0,1,0),
    bits2bytes(1,0,1,0,0,0,1,0),
    bits2bytes(1,0,0,1,0,0,1,0),
    bits2bytes(1,0,0,0,1,0,1,0),
    bits2bytes(1,0,0,0,0,1,1,0)};

uint8_t Font08px_91[4] = {3,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(1,0,0,0,0,0,1,0)};

uint8_t Font08px_92[4] = {3,
    bits2bytes(0,0,0,0,0,1,1,0),
    bits2bytes(0,0,1,1,1,0,0,0),
    bits2bytes(1,1,0,0,0,0,0,0)};

uint8_t Font08px_93[4] = {3,
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(1,1,1,1,1,1,1,0)};

uint8_t Font08px_94[4] = {3,
    bits2bytes(0,0,0,0,0,1,0,0),
    bits2bytes(0,0,0,0,0,0,1,0),
    bits2bytes(0,0,0,0,0,1,0,0)};

uint8_t Font08px_95[4] = {3,
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(1,0,0,0,0,0,0,0)};

uint8_t Font08px_96[3] = {2,
    bits2bytes(0,0,0,0,0,0,1,0),
    bits2bytes(0,0,0,0,0,1,0,0)};

uint8_t Font08px_la[5] = {4,
    bits2bytes(0,1,1,1,0,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(1,1,1,1,1,0,0,0)};

uint8_t Font08px_lb[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(0,1,1,1,0,0,0,0)};

uint8_t Font08px_lc[5] = {4,
    bits2bytes(0,1,1,1,0,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0)};

uint8_t Font08px_ld[5] = {4,
    bits2bytes(0,1,1,1,0,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(1,1,1,1,1,1,1,0)};

uint8_t Font08px_le[5] = {4,
    bits2bytes(0,1,1,1,0,0,0,0),
    bits2bytes(1,0,1,0,1,0,0,0),
    bits2bytes(1,0,1,0,1,0,0,0),
    bits2bytes(0,0,1,1,0,0,0,0)};

uint8_t Font08px_lf[4] = {3,
    bits2bytes(1,1,1,1,1,1,0,0),
    bits2bytes(0,0,0,1,0,0,1,0),
    bits2bytes(0,0,0,0,0,0,1,0)};

uint8_t Font08px_lg[5] = {4,
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(1,0,1,0,1,0,0,0),
    bits2bytes(1,0,1,0,1,0,0,0),
    bits2bytes(0,1,1,1,1,0,0,0)};

uint8_t Font08px_lh[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,0,0,1,0,0,0),
    bits2bytes(1,1,1,1,0,0,0,0)};

uint8_t Font08px_li[2] = {1,
    bits2bytes(1,1,1,1,1,0,1,0)};

uint8_t Font08px_lj[3] = {2,
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(0,1,1,1,1,0,1,0)};

uint8_t Font08px_lk[5] = {4,
    bits2bytes(1,1,1,1,1,1,1,0),
    bits2bytes(0,0,1,0,0,0,0,0),
    bits2bytes(0,1,0,1,0,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0)};

uint8_t Font08px_ll[2] = {1,
    bits2bytes(1,1,1,1,1,1,1,0)};

uint8_t Font08px_lm[6] = {5,
    bits2bytes(1,1,1,1,1,0,0,0),
    bits2bytes(0,0,0,0,1,0,0,0),
    bits2bytes(1,1,1,1,0,0,0,0),
    bits2bytes(0,0,0,0,1,0,0,0),
    bits2bytes(1,1,1,1,0,0,0,0)};

uint8_t Font08px_ln[5] = {4,
    bits2bytes(1,1,1,1,1,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,0,0,1,0,0,0),
    bits2bytes(1,1,1,1,0,0,0,0)};

uint8_t Font08px_lo[5] = {4,
    bits2bytes(0,1,1,1,0,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(0,1,1,1,0,0,0,0)};

uint8_t Font08px_lp[5] = {4,
    bits2bytes(1,1,1,1,1,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0)};

uint8_t Font08px_lq[5] = {4,
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(0,0,1,0,1,0,0,0),
    bits2bytes(1,1,1,1,1,0,0,0)};

uint8_t Font08px_lr[4] = {3,
    bits2bytes(1,1,1,1,1,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,0,0,1,0,0,0)};

uint8_t Font08px_ls[5] = {4,
    bits2bytes(1,0,0,1,0,0,0,0),
    bits2bytes(1,0,1,0,1,0,0,0),
    bits2bytes(1,0,1,0,1,0,0,0),
    bits2bytes(0,1,0,0,1,0,0,0)};

uint8_t Font08px_lt[4] = {3,
    bits2bytes(0,0,0,0,1,0,0,0),
    bits2bytes(0,1,1,1,1,1,0,0),
    bits2bytes(1,0,0,0,1,0,0,0)};

uint8_t Font08px_lu[5] = {4,
    bits2bytes(0,1,1,1,1,0,0,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(1,1,1,1,1,0,0,0)};

uint8_t Font08px_lv[6] = {5,
    bits2bytes(0,0,0,1,1,0,0,0),
    bits2bytes(0,1,1,0,0,0,0,0),
    bits2bytes(1,0,0,0,0,0,0,0),
    bits2bytes(0,1,1,0,0,0,0,0),
    bits2bytes(0,0,0,1,1,0,0,0)};

uint8_t Font08px_lw[6] = {5,
    bits2bytes(0,0,1,1,1,0,0,0),
    bits2bytes(1,1,0,0,0,0,0,0),
    bits2bytes(0,0,1,1,0,0,0,0),
    bits2bytes(1,1,0,0,0,0,0,0),
    bits2bytes(0,0,1,1,1,0,0,0)};

uint8_t Font08px_lx[6] = {5,
    bits2bytes(1,0,0,0,1,0,0,0),
    bits2bytes(0,1,0,1,0,0,0,0),
    bits2bytes(0,0,1,0,0,0,0,0),
    bits2bytes(0,1,0,1,0,0,0,0),
    bits2bytes(1,0,0,0,1,0,0,0)};

uint8_t Font08px_ly[5] = {4,
    bits2bytes(0,0,0,1,1,0,0,0),
    bits2bytes(1,0,1,0,0,0,0,0),
    bits2bytes(1,0,1,0,0,0,0,0),
    bits2bytes(0,1,1,1,1,0,0,0)};

uint8_t Font08px_lz[4] = {3,
    bits2bytes(1,1,0,0,1,0,0,0),
    bits2bytes(1,0,1,0,1,0,0,0),
    bits2bytes(1,0,0,1,1,0,0,0)};

uint8_t Font08px_123[4] = {3,
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,1,1,0,1,1,0,0),
    bits2bytes(1,0,0,0,0,0,1,0)};

uint8_t Font08px_124[2] = {1,
    bits2bytes(1,1,1,1,1,1,1,0)};

uint8_t Font08px_125[4] = {3,
    bits2bytes(1,0,0,0,0,0,1,0),
    bits2bytes(0,1,1,0,1,1,0,0),
    bits2bytes(0,0,0,1,0,0,0,0)};

uint8_t Font08px_126[6] = {5,
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,0,0,1,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0),
    bits2bytes(0,0,1,0,0,0,0,0),
    bits2bytes(0,0,0,1,0,0,0,0)};
