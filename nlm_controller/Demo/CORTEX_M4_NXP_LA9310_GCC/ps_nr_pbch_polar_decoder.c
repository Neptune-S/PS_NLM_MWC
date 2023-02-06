#include "FreeRTOS.h"
#include "task.h"
#include "debug_console.h"
#include "pbch.h"
#include <string.h>

void ps_nr_pbch_polar_decoder(float *llr, uint32_t *infoBits, uint32_t *crc, uint8_t* scratchBuf)
{
    // Decoding Parameterr
    
    uint16_t N = 512; // Total Input and Output Bits
    uint16_t node = 0;
    uint16_t npos = 0;
    uint8_t K = 56; // Information + CRC bits. Total Output Bits
    uint8_t A = 32; // Information Bits K-P
    uint8_t n = 9;  // log2(N);
    uint8_t done = 0;
    uint8_t depth = 0;
    uint16_t index = 0;
    uint8_t idx;
    uint8_t bdx;
    uint8_t flag;
    uint8_t ns[2*N - 1];
    uint8_t *bits[n + 1];
    uint8_t info_bits[K];
    float *L[n+1]; 
    
    uint32_t infoBitPattern[16] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xE0800000, 0x0, 0x0, 0x0, 0xE8808000, 0x80000000, 0xFA808000, 0xFEE08000, 0xFFFEFEE8};
    uint16_t info_bit_loc[56] = {247, 253, 254, 255, 367, 375, 379, 381, 382, 383, 415, 431, 439, 441, 443, 444, 445, 446, 447, 463, 469, 470, 471, 473, 474, 475, 476, 477, 478, 479, 483, 485, 486, 487, 489, 490, 491, 492, 493, 494, 495, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511};
    uint8_t crc_interleaver_pattern[56] = {0, 2, 3, 5, 7, 10, 11, 12, 14, 15, 18, 19, 21, 24, 26, 30, 31, 32, 1, 4, 6, 8, 13, 16, 20, 22, 25, 27, 33, 9, 17, 23, 28, 34, 29, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55};

    /* Initializing the bit memory */
    bits[0] = (uint8_t *)&scratchBuf[0];
    L[0]    = (float *)&scratchBuf[640];
    
    
    for (uint8_t k = 1; k < (n + 1); k++)
    {
        bits[k] = (uint8_t*)&scratchBuf[k*64];
        L[k]    = (float *)&scratchBuf[2688 + (k*1024)];
        log_dbg("Address of L is 0x%x, bit is 0x%x\n\r",L[k], bits[k]);
    }

    *infoBits = 0;
    *crc = 0;

    memset(ns, 0, ((2 * N - 1) * sizeof(uint8_t)));
    memcpy(L[0],llr, (512*sizeof(float)));
    memset(bits[0], 0, (640* sizeof(uint8_t)));

    while (done == 0)
    {
        if (depth == n)
        {

            idx = (node >> 5);   //get the info bit pattern index
            bdx = (node & 0x1F); //bit idx
            flag = (infoBitPattern[idx] >> bdx) & 0x1;

            idx = (node >> 3);
            bdx = (node & 0x7);

            if (flag == 0) // Frozen Node
                bits[n][idx] |= 0;
            else
                bits[n][idx] |= (((L[n][node & 0xff] >= 0) ? 0 : 1) << bdx);

            if (node == (N - 1))
                done++;
            else
            {
                node = (node >> 1);
                depth--;
            }
        }
        else
        {
            npos = ((1 << depth) - 1) + node;
            uint16_t temp = 1 << (n - depth);

            if (ns[npos] == 0) // Left Child
            {
                f_min_sum(L[depth], L[depth + 1], node, temp);
                node = (node << 1);
                depth++;
                ns[npos] = 1;
            }
            else
            {
                if (ns[npos] == 1) // Right Child
                {
                    g_repetition(L[depth], L[depth + 1], bits[depth + 1], node, temp);
                    node = (node << 1) + 1;
                    depth++;
                    ns[npos] = 2;
                }
                else
                {
                    bitSum(bits[depth], bits[depth + 1], node, temp);
                    node = (node >> 1);
                    depth--;
                }
            }
        }
    }


    for (uint16_t k = 0; k < K; k++)
    {

        index = info_bit_loc[k];
        idx = (index >> 3);
        bdx = (index & 0x7);
        info_bits[crc_interleaver_pattern[k]] = (bits[n][idx] >> bdx) & 0x1;
    }

    for (uint8_t k = 0; k < A; k++)
    {
        *infoBits += (1 << k) * info_bits[k];
    }

    for (uint8_t k = A; k < K; k++)
    {
        *crc += (1 << (k - A)) * info_bits[k];
    }

}

void f_min_sum(float *L_depth, float *L_depth_1, uint16_t node, uint16_t temp)
{
    float a;
    float b;
    float c;
    float d;
    float out;
    uint16_t tNode = node << 1;
    uint16_t offset = temp >> 1;
    uint16_t offs = 0;
    uint16_t offt = temp * node;
    uint16_t offu = offset * tNode;
    uint16_t mod_flag = (temp == 512) ? 0xffff : 0xff;

    for (uint16_t k = 0; k < offset; k++)
    {
        offs = (offt + k) & mod_flag;

        a = L_depth[offs];
        b = L_depth[offs + offset];

        c = a;
        d = b;

        a = (a >= 0) ? a : -a;
        b = (b >= 0) ? b : -b;

        if (a >= b)
            out = (c >= 0) ? d : -d;
        else
            out = (d >= 0) ? c : -c;

        offs = (offu + k) & 0xff;

        L_depth_1[offs] = out;
    }
}

void g_repetition(float *L_depth, float *L_depth_1, uint8_t *bits, uint16_t node, uint16_t temp)
{
    float a;
    float b;
    float out;
    uint16_t index;
    uint8_t idx;
    uint8_t bdx;
    uint8_t tempBit;
    uint16_t lNode = node << 1;
    uint16_t tNod = lNode + 1;
    uint16_t offset = temp >> 1;
    uint16_t offs = 0;
    uint16_t mod_flag = (temp == 512) ? 0xffff : 0xff;
    uint16_t offt = temp * node;
    uint16_t offu = offset * tNod;
    uint16_t offv = offset * lNode;

    for (uint16_t k = 0; k < offset; k++)
    {
        offs = (offt + k) & mod_flag;
        a = L_depth[offs];
        b = L_depth[offs + offset];

        index = (offv + k);
        idx = (index >> 3);
        bdx = (index & 0x7);
        tempBit = (bits[idx] >> bdx) & 0x1;
        out = (tempBit == 0) ? (b + a) : (b - a);

        offs = (offu + k) & 0xff;
        L_depth_1[offs] = out;
    }
}

void bitSum(uint8_t *bits, uint8_t *bits_1, uint16_t node, uint16_t temp)
{
    uint16_t index;
    uint16_t lnode = node << 1;
    uint16_t rnode = lnode + 1;
    uint16_t offset = temp >> 1;
    uint16_t offs = offset * lnode;
    uint16_t offt = offset * rnode;
    uint16_t offu = temp * node;
    uint8_t bitL;
    uint8_t bitR;
    uint8_t idx;
    uint8_t bdx;
    uint8_t p = 0;

    for (uint16_t k = 0; k < offset; k++)
    {

        index = offs + k;
        idx = index >> 3;
        bdx = index & 0x7;
        bitL = (bits_1[idx] >> bdx) & 0x1;
        index = offt + k;
        idx = index >> 3;
        bdx = (index & 0x7);
        bitR = (bits_1[idx] >> bdx) & 0x1;
        index = offu + p;
        idx = index >> 3;
        bdx = (index & 0x7);
        bits[idx] |= ((bitL + bitR) & 0x1) << bdx;
        p++;
    }
    for (uint16_t k = 0; k < offset; k++)
    {
        index = offset * rnode + k;
        idx = index >> 3;
        bdx = index & 0x7;
        bitR = (bits_1[idx] >> bdx) & 0x1;
        index = temp * node + p;
        idx = index >> 3;
        bdx = index & 0x7;
        bits[idx] |= (bitR << bdx);
        p++;
    }
}
