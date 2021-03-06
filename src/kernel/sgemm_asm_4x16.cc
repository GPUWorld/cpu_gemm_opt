
#define _KERNEL_SELECT
#include "sgemm_micro_kernel.h"
#include <stdio.h>
//#include <x86intrin.h>

#include <immintrin.h> // AVX2
#include <assert.h>

void sgemm_asm_4x16(int m, int n, int k,
    float alpha,
    const float * A, const float * B,
    float beta,
    float * C, int ldc)
{
    //assert(m==4 && n==16 && "4x16 kernel");
    unsigned long long k_itr = k/4;
    unsigned long long k_rem = k%4;
    unsigned long long ldc_  = ldc;

    asm volatile(
        "movq           %2,         %%rax                   \n" // A
        "movq           %3,         %%rbx                   \n" // B

        "vxorps         %%ymm8,     %%ymm8,     %%ymm8      \n"
        "vxorps         %%ymm9,     %%ymm9,     %%ymm9      \n"
        "vxorps         %%ymm10,    %%ymm10,    %%ymm10     \n"
        "vxorps         %%ymm11,    %%ymm11,    %%ymm11     \n"
        "vxorps         %%ymm12,    %%ymm12,    %%ymm12     \n"
        "vxorps         %%ymm13,    %%ymm13,    %%ymm13     \n"
        "vxorps         %%ymm14,    %%ymm14,    %%ymm14     \n"
        "vxorps         %%ymm15,    %%ymm15,    %%ymm15     \n"
                                                                // y8,  y9
                                                                // y10, y11
                                                                // y12, y13
                                                                // y14, y15
        "movq           %0,         %%rsi                   \n" // k_itr
        "testq          %%rsi,      %%rsi                   \n"
        "je             .LOOP_ITER_END                      \n"

        //"prefetcht0     0*64(%%rbx)                         \n" // prefetch B
        //"prefetcht0     1*64(%%rbx)                         \n" // prefetch B
        //"prefetcht0     (%%rax)                             \n" // prefetch next A

        ".LOOP_ITER:                                        \n"
                                                                // iter 0
        //"prefetcht0     2*64(%%rbx)                         \n" // prefetch B
        //"prefetcht0     64(%%rax)                           \n" // prefetch A for next loop
        "vmovaps        0*32(%%rbx),  %%ymm0                \n" // B panel 0
        "vmovaps        1*32(%%rbx),  %%ymm1                \n" // B panel 1

        "vbroadcastss   0*4(%%rax), %%ymm2                  \n" // A broadcast 0
        "vbroadcastss   1*4(%%rax), %%ymm3                  \n" // A broadcast 1
        "vfmadd231ps    %%ymm0,     %%ymm2,    %%ymm8       \n"
        "vfmadd231ps    %%ymm1,     %%ymm2,    %%ymm9       \n"
        "vfmadd231ps    %%ymm0,     %%ymm3,    %%ymm10      \n"
        "vfmadd231ps    %%ymm1,     %%ymm3,    %%ymm11      \n"

        "vbroadcastss   2*4(%%rax), %%ymm2                  \n" // A broadcast 0
        "vbroadcastss   3*4(%%rax), %%ymm3                  \n" // A broadcast 1
        "vfmadd231ps    %%ymm0,     %%ymm2,    %%ymm12      \n"
        "vfmadd231ps    %%ymm1,     %%ymm2,    %%ymm13      \n"
        "vfmadd231ps    %%ymm0,     %%ymm3,    %%ymm14      \n"
        "vfmadd231ps    %%ymm1,     %%ymm3,    %%ymm15      \n"

                                                                // iter 1
        //"prefetcht0     3*64(%%rbx)                         \n" // prefetch B
        "vmovaps        2*32(%%rbx),  %%ymm0                \n" // B panel 0
        "vmovaps        3*32(%%rbx),  %%ymm1                \n" // B panel 1

        "vbroadcastss   4*4(%%rax), %%ymm2                  \n" // A broadcast 0
        "vbroadcastss   5*4(%%rax), %%ymm3                  \n" // A broadcast 1
        "vfmadd231ps    %%ymm0,     %%ymm2,    %%ymm8       \n"
        "vfmadd231ps    %%ymm1,     %%ymm2,    %%ymm9       \n"
        "vfmadd231ps    %%ymm0,     %%ymm3,    %%ymm10      \n"
        "vfmadd231ps    %%ymm1,     %%ymm3,    %%ymm11      \n"

        "vbroadcastss   6*4(%%rax), %%ymm2                  \n" // A broadcast 0
        "vbroadcastss   7*4(%%rax), %%ymm3                  \n" // A broadcast 1
        "vfmadd231ps    %%ymm0,     %%ymm2,    %%ymm12      \n"
        "vfmadd231ps    %%ymm1,     %%ymm2,    %%ymm13      \n"
        "vfmadd231ps    %%ymm0,     %%ymm3,    %%ymm14      \n"
        "vfmadd231ps    %%ymm1,     %%ymm3,    %%ymm15      \n"
                                                                // iter 2
        //"prefetcht0     4*64(%%rbx)                         \n" // prefetch B
        "vmovaps        4*32(%%rbx),  %%ymm0                \n" // B panel 0
        "vmovaps        5*32(%%rbx),    %%ymm1              \n" // B panel 1

        "vbroadcastss   8*4(%%rax),  %%ymm2                 \n" // A broadcast 0
        "vbroadcastss   9*4(%%rax),  %%ymm3                 \n" // A broadcast 1
        "vfmadd231ps    %%ymm0,     %%ymm2,    %%ymm8       \n"
        "vfmadd231ps    %%ymm1,     %%ymm2,    %%ymm9       \n"
        "vfmadd231ps    %%ymm0,     %%ymm3,    %%ymm10      \n"
        "vfmadd231ps    %%ymm1,     %%ymm3,    %%ymm11      \n"

        "vbroadcastss   10*4(%%rax),  %%ymm2                \n" // A broadcast 0
        "vbroadcastss   11*4(%%rax),  %%ymm3                \n" // A broadcast 1
        "vfmadd231ps    %%ymm0,     %%ymm2,    %%ymm12      \n"
        "vfmadd231ps    %%ymm1,     %%ymm2,    %%ymm13      \n"
        "vfmadd231ps    %%ymm0,     %%ymm3,    %%ymm14      \n"
        "vfmadd231ps    %%ymm1,     %%ymm3,    %%ymm15      \n"

                                                                // iter 3
        //"prefetcht0     5*64(%%rbx)                         \n" // prefetch B
        "vmovaps        6*32(%%rbx),  %%ymm0                \n" // B panel 0
        "vmovaps        7*32(%%rbx),  %%ymm1                \n" // B panel 1

        "vbroadcastss   12*4(%%rax),  %%ymm2                \n" // A broadcast 0
        "vbroadcastss   13*4(%%rax),  %%ymm3                \n" // A broadcast 1
        "vfmadd231ps    %%ymm0,     %%ymm2,    %%ymm8       \n"
        "vfmadd231ps    %%ymm1,     %%ymm2,    %%ymm9       \n"
        "vfmadd231ps    %%ymm0,     %%ymm3,    %%ymm10      \n"
        "vfmadd231ps    %%ymm1,     %%ymm3,    %%ymm11      \n"

        "vbroadcastss   14*4(%%rax),  %%ymm2                \n" // A broadcast 0
        "vbroadcastss   15*4(%%rax),  %%ymm3                \n" // A broadcast 1
        "vfmadd231ps    %%ymm0,     %%ymm2,    %%ymm12      \n"
        "vfmadd231ps    %%ymm1,     %%ymm2,    %%ymm13      \n"
        "vfmadd231ps    %%ymm0,     %%ymm3,    %%ymm14      \n"
        "vfmadd231ps    %%ymm1,     %%ymm3,    %%ymm15      \n"

                                                                // iter end
        "addq           $64,        %%rax                   \n"
        "addq           $256,       %%rbx                   \n"
        "subq           $1,         %%rsi                   \n"
        "jne            .LOOP_ITER                          \n"
        ".LOOP_ITER_END:                                    \n"

        "movq           %1,         %%rsi                   \n"
        "testq          %%rsi,      %%rsi                   \n"
        "je             .POST                               \n"

        ".LOOP_REM:                                         \n"
        "vmovaps        (%%rbx),    %%ymm0                  \n" // B panel 0
        "vmovaps        32(%%rbx),  %%ymm1                  \n" // B panel 1

        "vbroadcastss   (%%rax),    %%ymm2                  \n" // A broadcast 0
        "vbroadcastss   4(%%rax),   %%ymm3                  \n" // A broadcast 1
        "vfmadd231ps    %%ymm0,     %%ymm2,    %%ymm4       \n"
        "vfmadd231ps    %%ymm1,     %%ymm2,    %%ymm5       \n"
        "vfmadd231ps    %%ymm0,     %%ymm3,    %%ymm6       \n"
        "vfmadd231ps    %%ymm1,     %%ymm3,    %%ymm7       \n"

        "vbroadcastss   8(%%rax),   %%ymm2                  \n" // A broadcast 0
        "vbroadcastss   12(%%rax),  %%ymm3                  \n" // A broadcast 1
        "vfmadd231ps    %%ymm0,     %%ymm2,    %%ymm8       \n"
        "vfmadd231ps    %%ymm1,     %%ymm2,    %%ymm9       \n"
        "vfmadd231ps    %%ymm0,     %%ymm3,    %%ymm10      \n"
        "vfmadd231ps    %%ymm1,     %%ymm3,    %%ymm11      \n"

        "addq           $16,        %%rax                   \n"
        "addq           $64,        %%rbx                   \n"
        "subq           $1,         %%rsi                   \n"
        "jne            .LOOP_REM                           \n"

        ".POST:                                             \n"
        "movq           %4,     %%rax                       \n" // C
        "movq           %5,     %%rdi                       \n"
        "leaq           (%%rax, %%rdi, 4), %%rbx            \n"
        "leaq           (%%rbx, %%rdi, 4), %%rcx            \n"
        "leaq           (%%rcx, %%rdi, 4), %%rdx            \n"

        "vaddps         (%%rax),    %%ymm8,  %%ymm8         \n" // AT&T syntax: vaddps ymm3, ymm2, ymm1, ymm3+ymm2 -> ymm1
        "vaddps         32(%%rax),  %%ymm9,  %%ymm9         \n"
        "vaddps         (%%rbx),    %%ymm10, %%ymm10        \n"
        "vaddps         32(%%rbx),  %%ymm11, %%ymm11        \n"
        "vaddps         (%%rcx),    %%ymm12, %%ymm12        \n"
        "vaddps         32(%%rcx),  %%ymm13, %%ymm13        \n"
        "vaddps         (%%rdx),    %%ymm14, %%ymm14        \n"
        "vaddps         32(%%rdx),  %%ymm15, %%ymm15        \n"

        "vmovaps        %%ymm8,     (%%rax)                 \n"
        "vmovaps        %%ymm9,     32(%%rax)               \n"
        "vmovaps        %%ymm10,    (%%rbx)                 \n"
        "vmovaps        %%ymm11,    32(%%rbx)               \n"
        "vmovaps        %%ymm12,    (%%rcx)                 \n"
        "vmovaps        %%ymm13,    32(%%rcx)               \n"
        "vmovaps        %%ymm14,    (%%rdx)                 \n"
        "vmovaps        %%ymm15,    32(%%rdx)               \n"

    : // output
    : // input
        "r"(k_itr),     // 0
        "r"(k_rem),     // 1
        "m"(A),         // 2
        "m"(B),         // 3
        "m"(C),         // 4
        "r"(ldc_)       // 5
    : // clobber list
        "rax","rbx","rcx","rdx","rsi","rdi",
        "r8","r9","r10","r11",
        "ymm0","ymm1","ymm2","ymm3","ymm4","ymm5","ymm6",
        "ymm7","ymm8","ymm9","ymm10","ymm11","ymm12","ymm13",
        "ymm14","ymm15"
    );
}