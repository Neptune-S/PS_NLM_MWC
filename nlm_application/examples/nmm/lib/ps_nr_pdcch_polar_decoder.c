#include <sys/types.h>
#include "armral.h"
#include <string.h>
#include "ps_nr_pdcch.h"
#include <math.h>
#include <stdio.h>
#include <rte_memcpy.h>

void ps_nr_pdcch_polar_decoder(float *llr, uint32_t *infoBits, uint32_t *info_bit_pattern, uint8_t *interleaver_pattern, void *scratchBuf, uint16_t N, uint8_t K)
{
    // Decoding Parameterr
    uint16_t node = 0;
    uint16_t npos = 0;
    uint8_t  n;  // log2(N);
    uint8_t  done  = 0;
    uint8_t  depth = 0;
    uint16_t index = 0;
    uint8_t idx;
    uint8_t bdx;
    uint8_t flag;
    uint8_t *scratch_buf = (uint8_t *)scratchBuf;
    uint8_t *ns; //[2*N - 1];
    uint8_t *bits[11]; //[n + 1];
    uint8_t *info_bits; //[K];
    float *L[11]; //[n+1]; 
    
    n  = log2(N);
    ns = &scratch_buf[0];
   
    memset(ns, 0, (N<<1)); //scratch_buf size (2*N-1) =  1024 byte;
     
    info_bits = &scratch_buf[(N<<1)];
 
    memset(info_bits,0,64);
    
    for (uint8_t k = 0; k <(n+1); k++)
    {
        bits[k] = &scratch_buf[(N<<1)+64*(k+1)];
        memset(bits[k], 0, (N>>3)*sizeof(uint8_t));
    }

    for (uint8_t k = 0; k < (n + 1); k++)
    {
        L[k]    = (float *)&scratch_buf[(N<<1)+64*(n+2)+k*(N<<2)];
        memset(L[k],0,N*sizeof(float));
    }

    rte_memcpy(L[0],llr, (size_t)(N*sizeof(float)));
        
    while (done == 0)
    {   
      
        if (depth == n)
        {
            idx  = (node >> 5);   //get the info bit pattern index
            bdx  = (node & 0x1F); //bit idx
            flag = (info_bit_pattern[idx] >> bdx) & 0x1;

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
    
    uint8_t n_four_bytes_n = N>>5;
    uint16_t *info_bit_loc = (uint16_t*)&scratch_buf[(N<<1)+64*(n+2)+(n+1)*(N<<2)];
    uint8_t p = 0;
    
    for(uint16_t k = 0; k<n_four_bytes_n;k++)
    {
       for(uint8_t t = 0; t<32; t++)
       {   
            if((info_bit_pattern[k]>>t)&0x1)
            {
               uint16_t loc = (k<<5)+t;
               info_bit_loc[p] = loc;
               p++;
            }
       }     
    } 
    
   
    for (uint16_t k = 0; k < K; k++)
    {
        index = info_bit_loc[k];
        idx = (index >> 3);
        bdx = (index & 0x7);
        info_bits[interleaver_pattern[k]] = (bits[n][idx] >> bdx) & 0x1;
    }
    
    uint8_t infoBits_size_32bit = ((K+16)>>5);
    memset(infoBits,0,infoBits_size_32bit*sizeof(uint32_t));

    for (uint8_t k = 0; k < K; k++)
    {  
        uint8_t offset = k>>5;
        infoBits[offset] += info_bits[k]<<(k%32);
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
