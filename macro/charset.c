#include <iconv.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <err.h>

#include "charset.h"

// These tables are from libwpd:
//
// https://sourceforge.net/p/libwpd/code/ci/master/tree/src/lib/libwpd_internal.cpp

/* WP6 multinational characters (charset 1) */
static const unsigned multinationalWP6[] =
{
	0x0300, 0x00b7, 0x0303, 0x0302, 0x0335, 0x0338, 0x0301, 0x0308, // 0 - 7
	0x0304, 0x0313, 0x0315, 0x02bc, 0x0326, 0x0315, 0x00b0, 0x0307, // 8 - 15
	0x030b, 0x0327, 0x0328, 0x030c, 0x0337, 0x0305, 0x0306, 0x00df, // 16
	0x0138, 0x006a, 0x00c1, 0x00e1, 0x00c2, 0x00e2, 0x00c4, 0x00e4, // 24
	0x00c0, 0x00e0, 0x00c5, 0x00e5, 0x00c6, 0x00e6, 0x00c7, 0x00e7, // 32
	0x00c9, 0x00e9, 0x00ca, 0x00ea, 0x00cb, 0x00eb, 0x00c8, 0x00e8, // 40
	0x00cd, 0x00ed, 0x00ce, 0x00ee, 0x00cf, 0x00ef, 0x00cc, 0x00ec, // 48
	0x00d1, 0x00f1, 0x00d3, 0x00f3, 0x00d4, 0x00f4, 0x00d6, 0x00f6, // 56
	0x00d2, 0x00f2, 0x00da, 0x00fa, 0x00db, 0x00fb, 0x00dc, 0x00fc, // 64
	0x00d9, 0x00f9, 0x0178, 0x00ff, 0x00c3, 0x00e3, 0x0110, 0x0111, // 72
	0x00d8, 0x00f8, 0x00d5, 0x00f5, 0x00dd, 0x00fd, 0x00d0, 0x00f0, // 80
	0x00de, 0x00fe, 0x0102, 0x0103, 0x0100, 0x0101, 0x0104, 0x0105, // 88
	0x0106, 0x0107, 0x010c, 0x010d, 0x0108, 0x0109, 0x010a, 0x010b, // 96
	0x010e, 0x010f, 0x011a, 0x011b, 0x0116, 0x0117, 0x0112, 0x0113, // 104
	0x0118, 0x0119, 0x01f4, 0x01f5, 0x011e, 0x011f, 0x01e6, 0x01e7, // 112
	0x0122, 0x0123, 0x011c, 0x011d, 0x0120, 0x0121, 0x0124, 0x0125, // 120
	0x0126, 0x0127, 0x0130, 0x0069, 0x012a, 0x012b, 0x012e, 0x012f, // 128
	0x0128, 0x0129, 0x0132, 0x0133, 0x0134, 0x0135, 0x0136, 0x0137, // 136
	0x0139, 0x013a, 0x013d, 0x013e, 0x013b, 0x013c, 0x013f, 0x0140, // 144
	0x0141, 0x0142, 0x0143, 0x0144, 0x0000, 0x0149, 0x0147, 0x0148, // 152
	0x0145, 0x0146, 0x0150, 0x0151, 0x014c, 0x014d, 0x0152, 0x0153, // 160
	0x0154, 0x0155, 0x0158, 0x0159, 0x0156, 0x0157, 0x015a, 0x015b, // 168
	0x0160, 0x0161, 0x015e, 0x015f, 0x015c, 0x015d, 0x0164, 0x0165, // 176
	0x0162, 0x0163, 0x0166, 0x0167, 0x016c, 0x016d, 0x0170, 0x0171, // 184
	0x016a, 0x016b, 0x0172, 0x0173, 0x016e, 0x016f, 0x0168, 0x0169, // 192
	0x0174, 0x0175, 0x0176, 0x0177, 0x0179, 0x017a, 0x017d, 0x017e, // 200
	0x017b, 0x017c, 0x014a, 0x014b, 0x0000, 0x0000, 0x0000, 0x0000, // 208
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 216
	0x0000, 0x0000, 0x1ef2, 0x1ef3, 0x010e, 0x010f, 0x01a0, 0x01a1, // 224
	0x01af, 0x01b0, 0x0114, 0x0115, 0x012c, 0x012d, 0x0049, 0x0131, // 232
	0x014e, 0x014f                                                  // 240 - 241
};

/* WP6 box drawing symbol (charset 3) */
static const unsigned boxdrawingWP6[] =
{
	0x2591, 0x2592, 0x2593, 0x2588, 0x258c, 0x2580, 0x2590, 0x2584,
	0x2500, 0x2502, 0x250c, 0x2510, 0x2518, 0x2514, 0x251c, 0x252c,
	0x2524, 0x2534, 0x253c, 0x2550, 0x2551, 0x2554, 0x2557, 0x255d,
	0x255a, 0x2560, 0x2566, 0x2563, 0x2569, 0x256c, 0x2552, 0x2555,
	0x255b, 0x2558, 0x2553, 0x2556, 0x255c, 0x2559, 0x255e, 0x2565,
	0x2561, 0x2568, 0x255f, 0x2564, 0x2562, 0x2567, 0x256b, 0x256a,
	0x2574, 0x2575, 0x2576, 0x2577, 0x2578, 0x2579, 0x257a, 0x257b,
	0x257c, 0x257e, 0x257d, 0x257f, 0x251f, 0x2522, 0x251e, 0x2521,
	0x252e, 0x2532, 0x252d, 0x2531, 0x2527, 0x2526, 0x252a, 0x2529,
	0x2536, 0x253a, 0x2535, 0x2539, 0x2541, 0x2546, 0x253e, 0x2540,
	0x2544, 0x254a, 0x253d, 0x2545, 0x2548, 0x2543, 0x2549, 0x2547
};

/* WP6 typographic symbol (charset 4) */
static const unsigned typographicWP6[] =
{
	0x25cf, 0x25cb, 0x25a0, 0x2022, 0x002a, 0x00b6, 0x00a7, 0x00a1,
	0x00bf, 0x00ab, 0x00bb, 0x00a3, 0x00a5, 0x20a7, 0x0192, 0x00aa,
	0x00ba, 0x00bd, 0x00bc, 0x00a2, 0x00b2, 0x207f, 0x00ae, 0x00a9,
	0x00a4, 0x00be, 0x00b3, 0x201b, 0x2019, 0x2018, 0x201f, 0x201d,
	0x201c, 0x2013, 0x2014, 0x2039, 0x203a, 0x25cb, 0x25a1, 0x2020,
	0x2021, 0x2122, 0x2120, 0x211e, 0x25cf, 0x25e6, 0x25a0, 0x25aa,
	0x25a1, 0x25ab, 0x2012, 0xfb00, 0xfb03, 0xfb04, 0xfb01, 0xfb02,
	0x2026, 0x0024, 0x20a3, 0x20a2, 0x20a0, 0x20a4, 0x201a, 0x201e,
	0x2153, 0x2154, 0x215b, 0x215c, 0x215d, 0x215e, 0x24c2, 0x24c5,
	0x20ac, 0x2105, 0x2106, 0x2030, 0x2116, 0x2014, 0x00b9, 0x2409,
	0x240c, 0x240d, 0x240a, 0x2424, 0x240b, 0x267c, 0x20a9, 0x20a6,
	0x20a8, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x1d11e, 0x1d122
};

/* WP6 math/scientific (charset 6) */
static const unsigned mathWP6[] =
{
	0x2212, 0x00b1, 0x2264, 0x2265, 0x221d, 0x002f, 0x2215, 0x2216, // 0 - 7
	0x00f7, 0x2223, 0x27e8, 0x27e9, 0x223c, 0x2248, 0x2261, 0x2208, // 8 - 15
	0x2229, 0x2225, 0x2211, 0x221e, 0x00ac, 0x2192, 0x2190, 0x2191, // 16 -23
	0x2193, 0x2194, 0x2195, 0x25b8, 0x25c2, 0x25b4, 0x25be, 0x22c5, // 24 - 31
	0x00b7, 0x2218, 0x2219, 0x212b, 0x00b0, 0x00b5, 0x203e, 0x00d7, // 32 - 39
	0x222b, 0x220f, 0x2213, 0x2207, 0x2202, 0x2032, 0x2033, 0x2192, // 40 - 47
	0x212f, 0x2113, 0x210f, 0x2111, 0x211c, 0x2118, 0x21c4, 0x21c6, // 48 - 55
	0x21d2, 0x21d0, 0x21d1, 0x21d3, 0x21d4, 0x21d5, 0x2197, 0x2198, // 56 - 63
	0x2196, 0x2199, 0x222a, 0x2282, 0x2283, 0x2286, 0x2287, 0x220d, // 64 - 71
	0x2205, 0x2308, 0x2309, 0x230a, 0x230b, 0x226a, 0x226b, 0x2220, // 72 - 79
	0x2297, 0x2295, 0x2296, 0x2a38, 0x2299, 0x2227, 0x2228, 0x22bb, // 80 - 87
	0x22a4, 0x22a5, 0x2312, 0x22a2, 0x22a3, 0x25a1, 0x25a0, 0x25ca, // 88 - 95
	0x25c6, 0x27e6, 0x27e7, 0x2260, 0x2262, 0x2235, 0x2234, 0x2237, // 96 - 103
	0x222e, 0x2112, 0x212d, 0x2128, 0x2118, 0x20dd, 0x29cb, 0x25c7, // 104 - 111
	0x22c6, 0x2034, 0x2210, 0x2243, 0x2245, 0x227a, 0x227c, 0x227b, // 112 - 119
	0x227d, 0x2203, 0x2200, 0x22d8, 0x22d9, 0x228e, 0x228a, 0x228b, // 120 - 127
	0x2293, 0x2294, 0x228f, 0x2291, 0x22e4, 0x2290, 0x2292, 0x22e5, // 128 - 135
	0x25b3, 0x25bd, 0x25c3, 0x25b9, 0x22c8, 0x2323, 0x2322, 0x25ef, // 136 - 143
	0x219d, 0x21a9, 0x21aa, 0x21a3, 0x21bc, 0x21bd, 0x21c0, 0x21c1, // 144 - 151
	0x21cc, 0x21cb, 0x21bf, 0x21be, 0x21c3, 0x21c2, 0x21c9, 0x21c7, // 152 - 159
	0x22d3, 0x22d2, 0x22d0, 0x22d1, 0x229a, 0x229b, 0x229d, 0x2127, // 160 - 167
	0x2221, 0x2222, 0x25c3, 0x25b9, 0x25b5, 0x25bf, 0x2214, 0x2250, // 168 - 175
	0x2252, 0x2253, 0x224e, 0x224d, 0x22a8, 0x2258, 0x226c, 0x0285, // 176 - 183
	0x2605, 0x226e, 0x2270, 0x226f, 0x2271, 0x2241, 0x2244, 0x2247, // 184 - 191
	0x2249, 0x2280, 0x22e0, 0x2281, 0x22e1, 0x2284, 0x2285, 0x2288, // 192 - 199
	0x2289, 0x0020, 0x0020, 0x22e2, 0x22e3, 0x2226, 0x2224, 0x226d, // 200 - 207
	0x2204, 0x2209, 0x2247, 0x2130, 0x2131, 0x2102, 0x0020, 0x2115, // 208 - 215
	0x211d, 0x225f, 0x22be, 0x220b, 0x22ef, 0x2026, 0x22ee, 0x22f1, // 216 - 223
	0x0020, 0x20e1, 0x002b, 0x002d, 0x003d, 0x002a, 0x2032, 0x2033, // 224 - 231
	0x2034, 0x210b, 0x2118, 0x2272, 0x2273, 0x0020                  // 232 - 237
};

/* WP6 math/scientific extended (charset 7) */
static const unsigned mathextWP6[] =
{
	0x2320, 0x2321, 0x23a5, 0x23bd, 0x221a, 0x0020, 0x2211, 0x220f,
	0x2210, 0x222b, 0x222e, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x23a7, 0x23a8, 0x23a9, 0x23aa, 0x0020, 0x0020, 0x0020,
	0x0020, 0x23ab, 0x23ac, 0x23ad, 0x23aa, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x222a, 0x222b, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x239b, 0x239d, 0x239c, 0x0020, 0x0020, 0x0020, 0x0020, 0x239e,
	0x23a8, 0x239f, 0x0020, 0x0020, 0x0020, 0x0020, 0x23a1, 0x23a3,
	0x23a2, 0x0020, 0x20aa, 0x0020, 0x0020, 0x23a4, 0x23a6, 0x23a5,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x22c3, 0x22c2, 0x228e, 0x2a04, 0x2294, 0x2a06, 0x2227, 0x22c0,
	0x2228, 0x22c1, 0x2297, 0x2a02, 0x2295, 0x2a01, 0x2299, 0x2a00,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x0020, 0x0020, 0x0020, 0x0020, 0x229d, 0x0020, 0x2238, 0x0020,
	0x27e6, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x27e7,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x21bc, 0x21bd, 0x0020, 0x296c, 0x296d, 0x296a, 0x296b, 0x0020,
	0x21c9, 0x21c7, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
	0x21be, 0x21bf, 0x21c3, 0x21c2, 0x0020, 0x2293, 0x2a05, 0x23a1,
	0x0020, 0x0020, 0x0020, 0x0020, 0x0020
};

/* WP5 International 2 (charset 2) */
static const unsigned international2WP5[] =
{
	0x0323, 0x0324, 0x02da, 0x0325, 0x02bc, 0x032d, 0x2017, 0x005f,
	0x0138, 0x032e, 0x033e, 0x2018, 0x0020, 0x02bd, 0x02db, 0x0327,
	0x0321, 0x0322, 0x030d, 0x2019, 0x0329, 0x0020, 0x0621, 0x02be,
	0x0306, 0x0310, 0x2032, 0x2034
};

/* WP5 Box drawing (charset 3) */
/* identical to WP6 Box drawing symbol charset */


/* WP5 Typographic symbols (charset 4) */
/* identical to WP6 Typographic symbols charset */

/* WP5 Iconic symbols (charset 5) */
static const unsigned iconicWP5[] =
{
	0x2665, 0x2666, 0x2663, 0x2660, 0x2642, 0x2640, 0x263c, 0x263a,
	0x263b, 0x266a, 0x266c, 0x25ac, 0x2302, 0x203c, 0x221a, 0x21a8,
	0x2310, 0x2319, 0x25d8, 0x25d9, 0x21b5, 0x261e, 0x261c, 0x2713,
	0x2610, 0x2612, 0x2639, 0x266f, 0x266d, 0x266e, 0x260e, 0x231a,
	0x231b, 0x2104, 0x23b5
};

/* WP5 math/scientific (charset 6) */
/* is identical to the WP6 math/scientific charset */

/* WP5 math/scientific extended (charset 7) */
/* is identical to the WP6 math/scientific extended charset */

/* WP5 greek (charset 8) */
static const unsigned greekWP5[] =
{
	0x0391, 0x03b1, 0x0392, 0x03b2, 0x0392, 0x03d0, 0x0393, 0x03b3,
	0x0394, 0x03b4, 0x0395, 0x03b5, 0x0396, 0x03b6, 0x0397, 0x03b7,
	0x0398, 0x03b8, 0x0399, 0x03b9, 0x039a, 0x03ba, 0x039b, 0x03bb,
	0x039c, 0x03bc, 0x039d, 0x03bd, 0x039e, 0x03be, 0x039f, 0x03bf,
	0x03a0, 0x03c0, 0x03a1, 0x03c1, 0x03a3, 0x03c3, 0x03f9, 0x03db,
	0x03a4, 0x03c4, 0x03a5, 0x03c5, 0x03a6, 0x03d5, 0x03a7, 0x03c7,
	0x03a8, 0x03c8, 0x03a9, 0x03c9, 0x03ac, 0x03ad, 0x03ae, 0x03af,
	0x03ca, 0x03cc, 0x03cd, 0x03cb, 0x03ce, 0x03b5, 0x03d1, 0x03f0,
	0x03d6, 0x1fe5, 0x03d2, 0x03c6, 0x03c9, 0x037e, 0x0387, 0x0384,
	0x00a8, 0x0385, 0x1fed, 0x1fef, 0x1fc0, 0x1fbd, 0x1fbf, 0x1fbe,
	0x1fce, 0x1fde, 0x1fcd, 0x1fdd, 0x1fcf, 0x1fdf, 0x0384, 0x1fef,
	0x1fc0, 0x1fbd, 0x1fbf, 0x1fce, 0x1fde, 0x1fcd, 0x1fdd, 0x1fcf,
	0x1fdf, 0x1f70, 0x1fb6, 0x1fb3, 0x1fb4, 0x1fb7, 0x1f00, 0x1f04,
	0x1f02, 0x1f06, 0x1f80, 0x1f84, 0x1f86, 0x1f01, 0x1f05, 0x1f03,
	0x1f07, 0x1f81, 0x1f85, 0x1f87, 0x1f72, 0x1f10, 0x1f14, 0x1f13,
	0x1f11, 0x1f15, 0x1f13, 0x1f74, 0x1fc6, 0x1fc3, 0x1fc4, 0x1fc2,
	0x1fc7, 0x1f20, 0x1f24, 0x1f22, 0x1f26, 0x1f90, 0x1f94, 0x1f96,
	0x1f21, 0x1f25, 0x1f23, 0x1f27, 0x1f91, 0x1f95, 0x1f97, 0x1f76,
	0x1fd6, 0x0390, 0x1fd2, 0x1f30, 0x1f34, 0x1f32, 0x1f36, 0x1f31,
	0x1f35, 0x1f33, 0x1f37, 0x1f78, 0x1f40, 0x1f44, 0x1f42, 0x1f41,
	0x1f45, 0x1f43, 0x1f7a, 0x1fe6, 0x03b0, 0x1fe3, 0x1f50, 0x1f54,
	0x1f52, 0x1f56, 0x1f51, 0x1f55, 0x1f53, 0x1f57, 0x1f7c, 0x1ff6,
	0x1ff3, 0x1ff4, 0x1ff2, 0x1ff7, 0x1f60, 0x1f64, 0x1f62, 0x1f66,
	0x1fa0, 0x1fa4, 0x1fa6, 0x1f61, 0x1f65, 0x1f63, 0x1f67, 0x1fa1,
	0x1fa5, 0x1fa7, 0x0374, 0x0375, 0x03db, 0x03dd, 0x03d9, 0x03e1,
	0x0386, 0x0388, 0x0389, 0x038a, 0x038c, 0x038e, 0x038f, 0x03aa,
	0x03ab, 0x1fe5
};

/* WP5 Hebrew (charset 9) */
static const unsigned hebrewWP5[] =
{
	0x05d0, 0x05d1, 0x05d2, 0x05d3, 0x05d4, 0x05d5, 0x05d6, 0x05d7,
	0x05d8, 0x05d9, 0x05da, 0x05db, 0x05dc, 0x05dd, 0x05de, 0x05df,
	0x05e0, 0x05e1, 0x05e2, 0x05e3, 0x05e4, 0x05e5, 0x05e6, 0x05e7,
	0x05e8, 0x05e9, 0x05ea, 0x05be, 0x05c0, 0x05c3, 0x05f3, 0x05f4,
	0x05b0, 0x05b1, 0x05b2, 0x05b3, 0x05b4, 0x05b5, 0x05b6, 0x05b7,
	0x05b8, 0x05b9, 0x05ba, 0x05bb, 0x05bc, 0x05bd, 0x05bf, 0x05b7,
	0xfbe1, 0x05f0, 0x05f1, 0x05f2, 0x0591, 0x0596, 0x05ad, 0x05a4,
	0x059a, 0x059b, 0x05a3, 0x05a5, 0x05a6, 0x05a7, 0x09aa, 0x0592,
	0x0593, 0x0594, 0x0595, 0x0597, 0x0598, 0x0599, 0x05a8, 0x059c,
	0x059d, 0x059e, 0x05a1, 0x05a9, 0x05a0, 0x059f, 0x05ab, 0x05ac,
	0x05af, 0x05c4, 0x0544, 0x05d0, 0xfb31, 0xfb32, 0xfb33, 0xfb34,
	0xfb35, 0xfb4b, 0xfb36, 0x05d7, 0xfb38, 0xfb39, 0xfb3b, 0xfb3a,
	0x05da, 0x05da, 0x05da, 0x05da, 0x05da, 0x05da, 0xfb3c, 0xfb3e,
	0xfb40, 0x05df, 0xfb41, 0xfb44, 0xfb46, 0xfb47, 0xfb2b, 0xfb2d,
	0xfb2a, 0xfb2c, 0xfb4a, 0xfb4c, 0xfb4e, 0xfb1f, 0xfb1d
};

/* WP5 cyrillic (charset 10) */
static const unsigned cyrillicWP5[] =
{
	0x0410, 0x0430, 0x0411, 0x0431, 0x0412, 0x0432, 0x0413, 0x0433, // 0 - 7
	0x0414, 0x0434, 0x0415, 0x0435, 0x0401, 0x0451, 0x0416, 0x0436, // 8 - 15
	0x0417, 0x0437, 0x0418, 0x0438, 0x0419, 0x0439, 0x041a, 0x043a, // 16 - 23
	0x041b, 0x043b, 0x041c, 0x043c, 0x041d, 0x043d, 0x041e, 0x043e, // 24 - 31
	0x041f, 0x043f, 0x0420, 0x0440, 0x0421, 0x0441, 0x0422, 0x0442, // 32 - 39
	0x0423, 0x0443, 0x0424, 0x0444, 0x0425, 0x0445, 0x0426, 0x0446, // 40 - 47
	0x0427, 0x0447, 0x0428, 0x0448, 0x0429, 0x0449, 0x042a, 0x044a, // 48 - 55
	0x042b, 0x044b, 0x042c, 0x044c, 0x042d, 0x044d, 0x042e, 0x044e, // 56 - 63
	0x042f, 0x044f, 0x0490, 0x0491, 0x0402, 0x0452, 0x0403, 0x0453, // 64 - 71
	0x0404, 0x0454, 0x0405, 0x0455, 0x0406, 0x0456, 0x0407, 0x0457, // 72 - 79
	0x0408, 0x0458, 0x0409, 0x0459, 0x040a, 0x045a, 0x040b, 0x045b, // 80 - 87
	0x040c, 0x045c, 0x040e, 0x045e, 0x040f, 0x045f, 0x0462, 0x0463, // 88 - 95
	0x0472, 0x0473, 0x0474, 0x0475, 0x046a, 0x046b, 0xa640, 0xa641, // 96 - 103
	0x0429, 0x0449, 0x04c0, 0x04cf, 0x0466, 0x0467, 0x0000, 0x0000, // 104 - 111
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 112 - 119
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 120 - 127
	0x0000, 0x0000, 0x0400, 0x0450, 0x0000, 0x0000, 0x040d, 0x045d, // 128 - 135
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 136 - 143
	0x0000, 0x0000, 0x0000, 0x0000, 0x0301, 0x0300                  // 144 - 149
};

/* WP5 Japanese (charset 11) */
static const unsigned japaneseWP5[] =
{
	0x3041, 0x3043, 0x3045, 0x3047, 0x3049, 0x3053, 0x3083, 0x3085,
	0x3087, 0x3094, 0x3095, 0x3096, 0x3042, 0x3044, 0x3046, 0x3048,
	0x304a, 0x304b, 0x304d, 0x3047, 0x3051, 0x3053, 0x304c, 0x304e,
	0x3050, 0x3052, 0x3054, 0x3055, 0x3057, 0x3059, 0x305b, 0x305d,
	0x3056, 0x3058, 0x305a, 0x305c, 0x305e, 0x305f, 0x3051, 0x3064,
	0x3066, 0x3068, 0x3060, 0x3062, 0x3065, 0x3067, 0x3069, 0x306a,
	0x306b, 0x306c, 0x306d, 0x306e, 0x306f, 0x3072, 0x3075, 0x3078,
	0x307b, 0x3070, 0x3073, 0x3076, 0x3079, 0x307c, 0x3071, 0x3074,
	0x3077, 0x307a, 0x307d, 0x307e, 0x307f, 0x3080, 0x3081, 0x3082,
	0x3084, 0x3086, 0x3088, 0x3089, 0x308a, 0x308b, 0x308c, 0x308d,
	0x308e, 0x3092, 0x3093, 0x3014, 0x3015, 0xff3b, 0xff3d, 0x300c,
	0x300d, 0x300c, 0x300d, 0x302a, 0x3002, 0x3001, 0x309d, 0x309e,
	0x3003, 0x30fc, 0x309b, 0x309c, 0x30a1, 0x30a3, 0x30a5, 0x30a7,
	0x30a9, 0x30c3, 0x30e3, 0x30e5, 0x3057, 0x30f4, 0x30f5, 0x30f6,
	0x30a2, 0x30a4, 0x30a6, 0x30a8, 0x30aa, 0x30ab, 0x30ad, 0x30af,
	0x30b1, 0x30b3, 0x30ac, 0x30ae, 0x30b0, 0x30b2, 0x30b4, 0x30b5,
	0x30c4, 0x30b9, 0x30bb, 0x30bd, 0x30b6, 0x30b8, 0x30ba, 0x30bc,
	0x30be, 0x30bf, 0x30c1, 0x30c4, 0x30c6, 0x30c8, 0x30c0, 0x30c2,
	0x30c5, 0x30c7, 0x30c9, 0x30ca, 0x30cb, 0x30cc, 0x30cd, 0x30ce,
	0x30cf, 0x30d2, 0x30d5, 0x30d8, 0x03d0, 0x30db, 0x30d3, 0x30d6,
	0x30d9, 0x30dc, 0x30d1, 0x30d4, 0x30d7, 0x30da, 0x30dd, 0x30de,
	0x30df, 0x30e0, 0x30e1, 0x30e2, 0x30e4, 0x30e6, 0x30e8, 0x30e9,
	0x30ea, 0x30ab, 0x30ec, 0x30ed, 0x30ef, 0x30f2, 0x30f3, 0x30fd,
	0x30fe
};

struct {
    size_t size;
    const unsigned *data;
} charsets[] = {
   { 0, NULL },
   { sizeof multinationalWP6, multinationalWP6 },
   { sizeof international2WP5, international2WP5 },
   { sizeof boxdrawingWP6, boxdrawingWP6 },
   { sizeof typographicWP6, typographicWP6 },
   { sizeof iconicWP5, iconicWP5 },
   { sizeof mathWP6, mathWP6 },
   { sizeof mathextWP6, mathextWP6 },
   { sizeof greekWP5, greekWP5 },
   { sizeof hebrewWP5, hebrewWP5 },
   { sizeof cyrillicWP5, cyrillicWP5 },
   { sizeof japaneseWP5, japaneseWP5 },
};

int fputwpc(wpc_t wpc, FILE *stream)
{
    iconv_t cd;
    char outbuf[MB_LEN_MAX];
    uint32_t inchar = 0;
    size_t insize   = sizeof(uint32_t);
    size_t outsize  = sizeof(outbuf);
    char *outptr    = (char *) outbuf;
    char *inptr     = (char *) &inchar;

    memset(outbuf, 0, sizeof outbuf);

    // No translation necessary.
    if (wpc.set == WP_ASCII) {
        return fputc(wpc.c, stream);
    }

    cd = iconv_open("UTF-8", "UCS-4LE");

    if (cd == (iconv_t) -1) {
        err(EXIT_FAILURE, "Could not initialize iconv");
    }

    // Translation necessary, convert to UCS-4
    inchar = charsets[wpc.set].data[wpc.c];

    if (inchar == 0) {
        errx(EXIT_FAILURE, "character %03d is not defined", inchar);
    }

    if (iconv(cd, &inptr,
                  &insize,
                  &outptr,
                  &outsize) == -1) {
        err(EXIT_FAILURE, "cannot convert character %04hx", wpc.word);
    }

    for (char *p = outbuf; p != outptr; p++)
        fputc(*p, stream);

    iconv_close(cd);

    return 0;
}

bool finducs(uint32_t ucs, wpc_t *result)
{
    if (isascii(ucs)) {
        fprintf(stderr, "debug: ucs character %02x is WP_ASCII?\n", ucs);
        result->set = WP_ASCII;
        result->c   = ucs;
        return true;
    }

    for (result->set = WP_ASCII + 1;
         result->set < WP_NUMSETS;
         result->set++) {
        for (result->c = 0;
             result->c < charsets[result->set].size / sizeof(uint32_t);
             result->c++) {
            if (charsets[result->set].data[result->c] == ucs)
                return true;
        }
    }

    return false;
}