//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsInit.h - Header for gsInit.c
//
// Parts taken from emoon's BreakPoint Demo Library
//

#ifndef __GSINIT_H__
#define __GSINIT_H__

#include "gsKit.h"

#define GS_NONINTERLACED 0x00
#define GS_INTERLACED 0x01

#define GS_FIELD 0x00
#define GS_FRAME 0x01

#define GS_MODE_NTSC 0x02
#define GS_MODE_PAL  0x03

#define GS_MODE_VGA_640_60 0x1A
#define GS_MODE_VGA_640_72 0x1B
#define GS_MODE_VGA_640_75 0x1C
#define GS_MODE_VGA_640_85 0x1D

#define GS_MODE_VGA_800_56 0x2A
#define GS_MODE_VGA_800_60 0x2B
#define GS_MODE_VGA_800_72 0x2C
#define GS_MODE_VGA_800_75 0x2D
#define GS_MODE_VGA_800_85 0x2E

#define GS_MODE_VGA_1024_60 0x3B
#define GS_MODE_VGA_1024_70 0x3C
#define GS_MODE_VGA_1024_75 0x3D
#define GS_MODE_VGA_1024_85 0x3E

#define GS_MODE_DTV_480P  0x50
#define GS_MODE_DTV_1080I 0x51
#define GS_MODE_DTV_720P  0x52

#define GS_MODE_DVD_NTSC 0x72
#define GS_MODE_DVD_PAL  0x73
#define GS_MODE_DVD_480P 0x74

#define GS_PSM_CT32 0x00
#define GS_PSM_CT24 0x01
#define GS_PSM_CT16 0x02
#define GS_PSM_CT16S 0x0A
#define GS_PSM_T8 0x13
#define GS_PSM_T4 0x14

#define GS_PSMZ_32 0x00
#define GS_PSMZ_24 0x01
#define GS_PSMZ_16 0x02
#define GS_PSMZ_16S 0x0A

#define GS_BLEND_FRONT2BACK 0x12
#define GS_BLEND_BACK2FRONT 0x01

#define PMODE           ((volatile u64 *)(0x12000000))

#define GS_SET_PMODE(EN1,EN2,MMOD,AMOD,SLBG,ALP) \
        *PMODE = \
        ((u64)(EN1)     << 0)   | \
        ((u64)(EN2)     << 1)   | \
        ((u64)(001)     << 2)   | \
        ((u64)(MMOD)    << 5)   | \
        ((u64)(AMOD) << 6)      | \
        ((u64)(SLBG) << 7)      | \
        ((u64)(ALP)     << 8)

#define DISPFB2         ((volatile u64 *)(0x12000090))

#define GS_SET_DISPFB2(FBP,FBW,PSM,DBX,DBY) \
        *DISPFB2 = \
        ((u64)(FBP)     << 0)   | \
        ((u64)(FBW)     << 9)   | \
        ((u64)(PSM)     << 15)  | \
        ((u64)(DBX)     << 32)  | \
        ((u64)(DBY)     << 43)

#define DISPLAY2        ((volatile u64 *)(0x120000a0))
#define GS_SET_DISPLAY2(DX,DY,MAGH,MAGV,DW,DH) \
        *DISPLAY2 = \
        ((u64)(DX)      << 0)   | \
        ((u64)(DY)      << 12)  | \
        ((u64)(MAGH)    << 23)  | \
        ((u64)(MAGV)    << 27)  | \
        ((u64)(DW)      << 32)  | \
        ((u64)(DH)      << 44)

#define DISPFB1 ((volatile u64 *)(0x12000070))
#define GS_SET_DISPFB1(FBP,FBW,PSM,DBX,DBY) \
        *DISPFB1 = \
        ((u64)(FBP)     << 0)   | \
        ((u64)(FBW)     << 9)   | \
        ((u64)(PSM)     << 15)  | \
        ((u64)(DBX)     << 32)  | \
        ((u64)(DBY)     << 43)

#define DISPLAY1        ((volatile u64 *)(0x12000080))
#define GS_SET_DISPLAY1(DX,DY,MAGH,MAGV,DW,DH) \
        *DISPLAY1 = \
        ((u64)(DX)      << 0)   | \
        ((u64)(DY)      << 12)  | \
        ((u64)(MAGH)    << 23)  | \
        ((u64)(MAGV)    << 27)  | \
        ((u64)(DW)      << 32)  | \
        ((u64)(DH)      << 44)

#define BGCOLOR         ((volatile u64 *)(0x120000e0))
#define GS_SET_BGCOLOR(R,G,B) \
        *BGCOLOR = \
        ((u64)(R)       << 0)           | \
        ((u64)(G)       << 8)           | \
        ((u64)(B)       << 16)


#define GS_SETREG_XYOFFSET_1  GS_SETREG_XYOFFSET
#define GS_SETREG_XYOFFSET_2  GS_SETREG_XYOFFSET
#define GS_SETREG_XYOFFSET(ofx, ofy) ((u64)(ofx) | ((u64)(ofy) << 32))

#define GS_SETREG_ZBUF_1  GS_SETREG_ZBUF
#define GS_SETREG_ZBUF_2  GS_SETREG_ZBUF
#define GS_SETREG_ZBUF(zbp, psm, zmsk) \
  ((u64)(zbp) | ((u64)(psm) << 24) | \
  ((u64)(zmsk) << 32))

#define GS_SETREG_TEST_1 GS_SETREG_TEST
#define GS_SETREG_TEST_2 GS_SETREG_TEST
#define GS_SETREG_TEST(ate, atst, aref, afail, date, datm, zte, ztst) \
  ((u64)(ate)         | ((u64)(atst) << 1) | \
  ((u64)(aref) << 4)  | ((u64)(afail) << 12) | \
  ((u64)(date) << 14) | ((u64)(datm) << 15) | \
  ((u64)(zte) << 16)  | ((u64)(ztst) << 17))

#define GS_SETREG_COLCLAMP(clamp) ((u64)(clamp))

#define GS_SETREG_FRAME_1 GS_SETREG_FRAME
#define GS_SETREG_FRAME_2 GS_SETREG_FRAME
#define GS_SETREG_FRAME(fbp, fbw, psm, fbmask) \
  ((u64)(fbp)        | ((u64)(fbw) << 16) | \
  ((u64)(psm) << 24) | ((u64)(fbmask) << 32))  

#define GS_SETREG_SCISSOR_1 GS_SETREG_SCISSOR
#define GS_SETREG_SCISSOR_2 GS_SETREG_SCISSOR
#define GS_SETREG_SCISSOR(scax0, scax1, scay0, scay1) \
  ((u64)(scax0)        | ((u64)(scax1) << 16) | \
  ((u64)(scay0) << 32) | ((u64)(scay1) << 48))

#define GS_SETREG_CLAMP_1 GS_SET_CLAMP
#define GS_SETREG_CLAMP_2 GS_SET_CLAMP
#define GS_SETREG_CLAMP(wms, wmt, minu, maxu, minv, maxv) \
  ((u64)(wms)         | ((u64)(wmt) << 2) | \
  ((u64)(minu) << 4)  | ((u64)(maxu) << 14) | \
  ((u64)(minv) << 24) | ((u64)(maxv) << 34))

struct gsBGColor
{
	u8 Red;
	u8 Green;
	u8 Blue;
};
typedef struct gsBGColor GSBGCOLOR;

struct gsTest
{
	u8 ATE;
	u8 ATST;
	u8 AREF;
	u8 AFAIL;
	u8 DATE;
	u8 DATM;
	u8 ZTE;
	u8 ZTST;
};
typedef struct gsTest GSTEST;

struct gsClamp
{
	u8 WMS;
	u8 WMT;
	int MINU;
	int MAXU;
	int MINV;
	int MAXV;
};
typedef struct gsClamp GSCLAMP;

struct gsGlobals
{
	s16 Mode;
	s16 Interlace;
	s16 Field;
	u8 Setup;
	u32 CurrentPointer;
	u32 ScreenBuffer[2];
	u32 ZBuffer;
	u8 EvenOrOdd;
	int ActiveBuffer;
	int Width;
	int Height;
	int Aspect;
	int OffsetX;
	int OffsetY;
	int StartX;
	int StartY;
	int MagX;
	int MagY;
	GSBGCOLOR *BGColor;
	GSTEST *Test;
	GSCLAMP *Clamp;
	int PSM;
	int PSMZ;
	int PrimContext;
	int PrimFogEnable;
	int PrimAAEnable;
	int PrimAlphaEnable;
	u8 PrimAlpha;
};
typedef struct gsGlobals GSGLOBAL;

struct gsTexture
{
        u32     Width;
        u32     Height;
        u32     PSM;
        void    *Mem;
        void    *Clut;
        u32     Vram;
        u32     VramClut;
};
typedef struct gsTexture GSTEXTURE;

struct gsFont
{
        char *Path;
        char *Path_BMP;
        char *Path_DAT;
        u8 Type;
	u8 *RawData;
	int RawSize;
        GSTEXTURE *Texture;
        u32 CharWidth;
        u32 CharHeight;
        u32 HChars;
        u32 VChars;
        u8 *Additional;
};
typedef struct gsFont GSFONT;

struct gsRegisters {
 u64 SIGNAL:      1 __attribute__((packed)); /* ro */
 u64 FINISH:      1 __attribute__((packed)); /* ro */
 u64 HSINT:       1 __attribute__((packed)); /* ro */
 u64 VSINT:       1 __attribute__((packed)); /* ro */
 u64 reserved04:  3 __attribute__((packed)); /* ro */
 u64 pad07:       1 __attribute__((packed));
 u64 FLUSH:       1 __attribute__((packed)); /* rw */
 u64 RESET:       1 __attribute__((packed)); /* N/A */
 u64 pad10:       2 __attribute__((packed));
 u64 NFIELD:      1 __attribute__((packed)); /* ro */
 u64 FIELD:       1 __attribute__((packed)); /* ro */
 u64 FIFO:        2 __attribute__((packed)); /* ro */
 u64 REV:         8 __attribute__((packed)); /* ro */
 u64 ID:          8 __attribute__((packed)); /* ro */
 u64 pad32:      32 __attribute__((packed));
};
typedef struct gsRegisters GSREG __attribute__((packed));

#ifdef __cplusplus
extern "C" {
#endif

void gsKit_init_screen(GSGLOBAL *gsGlobal);
GSGLOBAL *gsKit_init_global(u8 mode);
GSFONT *gsKit_init_font(u8 type, char *path);
GSFONT *gsKit_init_font_raw(u8 type, u8 *data, int size);

#ifdef __cplusplus
}
#endif

#endif /* __GSINIT_H__ */