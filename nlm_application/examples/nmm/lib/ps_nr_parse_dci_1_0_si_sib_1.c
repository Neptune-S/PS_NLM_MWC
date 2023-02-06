#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ps_nr_pdcch.h"

void ps_nr_parse_dci_1_0_si_sib_1( uint32_t* payload, uint32_t payloadlen, dci_1_0_si_sib_1_t* dci_1_0_si_sib_1)
{
	
	uint8_t i = 0;
	uint8_t fdralen = payloadlen -28; //13 other info bits+15 reserved bits needs to be removed for fdra based on BWP

	for( i = 0; i < fdralen; i++)
	{
		dci_1_0_si_sib_1->fdra |= ((((payload[0]>>(32 - fdralen))>>i) & 0x1))<<(fdralen-i-1);
	}
	
	dci_1_0_si_sib_1->tdra = (((((payload[0])>>(31-fdralen))&0x1)) | (((((payload[0])>>(30-fdralen))&0x1))<<1) | 
							  (((((payload[0])>>(29-fdralen))&0x1))<<2) | (((((payload[0])>>(28-fdralen))&0x1))<<3));

	if((((payload[0])>>(27-fdralen))&0x1) == 0)
	  dci_1_0_si_sib_1->mapping = 0;//non-interleaved
	else
	  dci_1_0_si_sib_1->mapping =1;//Interleaved

    dci_1_0_si_sib_1->mcs = ((((((payload[0])>>(26-fdralen))&0x1)))  | (((((payload[0])>>(25-fdralen))&0x1))<<1) | (((((payload[0])>>(24-fdralen))&0x1))<<2) |
							 (((((payload[0])>>(23-fdralen))&0x1))<<3) | (((((payload[0])>>(22-fdralen))&0x1))<<4));
							 
	dci_1_0_si_sib_1->rv  = ((((((payload[0])>>(21-fdralen))&0x1))) | (((((payload[0])>>(20-fdralen))&0x1))<<1));

	if( ((((payload[0])>>(19-fdralen))&0x1)) == 0)
	  dci_1_0_si_sib_1->si_ind = 1;//sib-1
	else
	  dci_1_0_si_sib_1->si_ind = 0;//others
}

