#include <Pilot.h>

static struct LibRef *libref = 0;

extern void GLibClose(struct LibRef *);

void GLib_Libm(void)
{
    asm ("
.global acos
acos: move.l #1,%%d0; braw dispatch
.global acosh
acosh: move.l #2,%%d0; braw dispatch
.global airy
airy: move.l #3,%%d0; braw dispatch
.global asin
asin: move.l #4,%%d0; braw dispatch
.global asinh
asinh: move.l #5,%%d0; braw dispatch
.global atan
atan: move.l #6,%%d0; braw dispatch
.global atan2
atan2: move.l #7,%%d0; braw dispatch
.global atanh
atanh: move.l #8,%%d0; braw dispatch
.global bdtr
bdtr: move.l #9,%%d0; braw dispatch
.global bdtrc
bdtrc: move.l #10,%%d0; braw dispatch
.global bdtri
bdtri: move.l #11,%%d0; braw dispatch
.global beta
beta: move.l #12,%%d0; braw dispatch
.global cabs
cabs: move.l #13,%%d0; braw dispatch
.global cacos
cacos: move.l #14,%%d0; braw dispatch
.global cadd
cadd: move.l #15,%%d0; braw dispatch
.global casin
casin: move.l #16,%%d0; braw dispatch
.global catan
catan: move.l #17,%%d0; braw dispatch
.global cbrt
cbrt: move.l #18,%%d0; braw dispatch
.global cchsh
cchsh: move.l #19,%%d0; braw dispatch
.global ccos
ccos: move.l #20,%%d0; braw dispatch
.global ccot
ccot: move.l #21,%%d0; braw dispatch
.global cdiv
cdiv: move.l #22,%%d0; braw dispatch
.global ceil
ceil: move.l #23,%%d0; braw dispatch
.global cexp
cexp: move.l #24,%%d0; braw dispatch
.global chbevl
chbevl: move.l #25,%%d0; braw dispatch
.global chdtr
chdtr: move.l #26,%%d0; braw dispatch
.global chdtrc
chdtrc: move.l #27,%%d0; braw dispatch
.global chdtri
chdtri: move.l #28,%%d0; braw dispatch
.global clog
clog: move.l #29,%%d0; braw dispatch
.global cmov
cmov: move.l #30,%%d0; braw dispatch
.global cmul
cmul: move.l #31,%%d0; braw dispatch
.global cneg
cneg: move.l #32,%%d0; braw dispatch
.global cos
cos: move.l #33,%%d0; braw dispatch
.global cosdg
cosdg: move.l #34,%%d0; braw dispatch
.global cosh
cosh: move.l #35,%%d0; braw dispatch
.global cot
cot: move.l #36,%%d0; braw dispatch
.global cotdg
cotdg: move.l #37,%%d0; braw dispatch
.global csin
csin: move.l #38,%%d0; braw dispatch
.global csqrt
csqrt: move.l #39,%%d0; braw dispatch
.global csub
csub: move.l #40,%%d0; braw dispatch
.global ctan
ctan: move.l #41,%%d0; braw dispatch
.global ctans
ctans: move.l #42,%%d0; braw dispatch
.global dawsn
dawsn: move.l #43,%%d0; braw dispatch
.global dprec
dprec: move.l #44,%%d0; braw dispatch
.global ellie
ellie: move.l #45,%%d0; braw dispatch
.global ellik
ellik: move.l #46,%%d0; braw dispatch
.global ellpe
ellpe: move.l #47,%%d0; braw dispatch
.global ellpj
ellpj: move.l #48,%%d0; braw dispatch
.global ellpk
ellpk: move.l #49,%%d0; braw dispatch
.global erf
erf: move.l #50,%%d0; braw dispatch
.global erfc
erfc: move.l #51,%%d0; braw dispatch
.global exp
exp: move.l #52,%%d0; braw dispatch
.global exp10
exp10: move.l #53,%%d0; braw dispatch
.global exp2
exp2: move.l #54,%%d0; braw dispatch
.global expn
expn: move.l #55,%%d0; braw dispatch
.global fac
fac: move.l #56,%%d0; braw dispatch
.global fdtr
fdtr: move.l #57,%%d0; braw dispatch
.global fdtrc
fdtrc: move.l #58,%%d0; braw dispatch
.global fdtri
fdtri: move.l #59,%%d0; braw dispatch
.global floor
floor: move.l #60,%%d0; braw dispatch
.global fresnl
fresnl: move.l #61,%%d0; braw dispatch
.global frexp
frexp: move.l #62,%%d0; braw dispatch
.global gamma
gamma: move.l #63,%%d0; braw dispatch
.global gdtr
gdtr: move.l #64,%%d0; braw dispatch
.global gdtrc
gdtrc: move.l #65,%%d0; braw dispatch
.global hyp2f0
hyp2f0: move.l #66,%%d0; braw dispatch
.global hyp2f1
hyp2f1: move.l #67,%%d0; braw dispatch
.global hyperg
hyperg: move.l #68,%%d0; braw dispatch
.global i0
i0: move.l #69,%%d0; braw dispatch
.global i0e
i0e: move.l #70,%%d0; braw dispatch
.global i1
i1: move.l #71,%%d0; braw dispatch
.global i1e
i1e: move.l #72,%%d0; braw dispatch
.global igam
igam: move.l #73,%%d0; braw dispatch
.global igamc
igamc: move.l #74,%%d0; braw dispatch
.global igami
igami: move.l #75,%%d0; braw dispatch
.global incbet
incbet: move.l #76,%%d0; braw dispatch
.global incbi
incbi: move.l #77,%%d0; braw dispatch
.global incbps
incbps: move.l #78,%%d0; braw dispatch
.global iv
iv: move.l #79,%%d0; braw dispatch
.global j0
j0: move.l #80,%%d0; braw dispatch
.global j1
j1: move.l #81,%%d0; braw dispatch
.global jn
jn: move.l #82,%%d0; braw dispatch
.global jv
jv: move.l #83,%%d0; braw dispatch
.global k0
k0: move.l #84,%%d0; braw dispatch
.global k0e
k0e: move.l #85,%%d0; braw dispatch
.global k1
k1: move.l #86,%%d0; braw dispatch
.global k1e
k1e: move.l #87,%%d0; braw dispatch
.global kn
kn: move.l #88,%%d0; braw dispatch
.global ldexp
ldexp: move.l #89,%%d0; braw dispatch
.global ldprec
ldprec: move.l #90,%%d0; braw dispatch
.global lgam
lgam: move.l #91,%%d0; braw dispatch
.global log
log: move.l #92,%%d0; braw dispatch
.global log10
log10: move.l #93,%%d0; braw dispatch
.global log2
log2: move.l #94,%%d0; braw dispatch
.global nbdtr
nbdtr: move.l #95,%%d0; braw dispatch
.global nbdtrc
nbdtrc: move.l #96,%%d0; braw dispatch
.global ndtr
ndtr: move.l #97,%%d0; braw dispatch
.global ndtri
ndtri: move.l #98,%%d0; braw dispatch
.global onef2
onef2: move.l #99,%%d0; braw dispatch
.global p1evl
p1evl: move.l #100,%%d0; braw dispatch
.global pdtr
pdtr: move.l #101,%%d0; braw dispatch
.global pdtrc
pdtrc: move.l #102,%%d0; braw dispatch
.global pdtri
pdtri: move.l #103,%%d0; braw dispatch
.global poladd
poladd: move.l #104,%%d0; braw dispatch
.global polclr
polclr: move.l #105,%%d0; braw dispatch
.global poldiv
poldiv: move.l #106,%%d0; braw dispatch
.global poleva
poleva: move.l #107,%%d0; braw dispatch
.global polevl
polevl: move.l #108,%%d0; braw dispatch
.global polini
polini: move.l #109,%%d0; braw dispatch
.global polmov
polmov: move.l #110,%%d0; braw dispatch
.global polmul
polmul: move.l #111,%%d0; braw dispatch
.global polprt
polprt: move.l #112,%%d0; braw dispatch
.global polsbt
polsbt: move.l #113,%%d0; braw dispatch
.global polsub
polsub: move.l #114,%%d0; braw dispatch
.global pow
pow: move.l #115,%%d0; braw dispatch
.global powi
powi: move.l #116,%%d0; braw dispatch
.global psi
psi: move.l #117,%%d0; braw dispatch
.global redupi
redupi: move.l #118,%%d0; braw dispatch
.global rgamma
rgamma: move.l #119,%%d0; braw dispatch
.global shichi
shichi: move.l #120,%%d0; braw dispatch
.global sici
sici: move.l #121,%%d0; braw dispatch
.global sin
sin: move.l #122,%%d0; braw dispatch
.global sindg
sindg: move.l #123,%%d0; braw dispatch
.global sinh
sinh: move.l #124,%%d0; braw dispatch
.global spence
spence: move.l #125,%%d0; braw dispatch
.global sprec
sprec: move.l #126,%%d0; braw dispatch
.global sqrt
sqrt: move.l #127,%%d0; braw dispatch
.global stdtr
stdtr: move.l #128,%%d0; braw dispatch
.global struve
struve: move.l #129,%%d0; braw dispatch
.global tan
tan: move.l #130,%%d0; braw dispatch
.global tandg
tandg: move.l #131,%%d0; braw dispatch
.global tanh
tanh: move.l #132,%%d0; braw dispatch
.global threef0
threef0: move.l #133,%%d0; braw dispatch
.global y0
y0: move.l #134,%%d0; braw dispatch
.global y1
y1: move.l #135,%%d0; braw dispatch
.global yn
yn: move.l #136,%%d0; braw dispatch
.global yv
yv: move.l #137,%%d0; braw dispatch
.global zeta
zeta: move.l #138,%%d0; braw dispatch
.global zetac
zetac: move.l #139,%%d0; braw dispatch

libname:
    .asciz \"Math Library\"
    .even

dispatch:
    lea libname(%%pc),%%a1
    move.l %%a1,%%d2
    move.l %%a4,%%d1
    move.l %%d1,%%d1
    jbeq noglobals
    lea libref(%%a4),%%a1
noglobals:
    move.l %0,%%d1
    braw GLibDispatch
    " : : "i" ('Libm') );
}

register void *reg_a4 asm("%a4");

void GLib_Libm_clean(Word cmd, Ptr PBP, Word flags)
{
    if (reg_a4 && libref) {
	GLibClose(libref);
	libref = 0;
    }
}

asm("
.section ehook
.long GLib_Libm_clean
");
