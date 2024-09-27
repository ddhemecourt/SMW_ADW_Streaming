#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "adw_constructor.h"
#include <math.h>



void* adw_constructor(char* adw_word,struct adw_s adw){


	uint16_t freq_offset = (adw.FREQ_OFFSET)/2.4e9*pow(2,32);
	uint16_t level_offset = pow(10,(-adw.LEVEL_OFFSET/20))*(pow(2,15)-1);
	uint16_t phase_offset = (adw.PHASE_OFFSET/360*pow(2,16));
	int burst_sri =(int)(adw.BURST_SRI*1e-6*2.4e9);
		
	printf("ADW burst sri: %f BURST SRI: %d  MATH: %d\n",adw.BURST_SRI,burst_sri,(int)(adw.BURST_SRI*2.4e9));
	printf("SEG IDX: %d\n", adw.SEGMENT_IDX);

	adw_word[0] = 0x00;
	adw_word[1] = 0x00;
	adw_word[2] = 0x00;
	adw_word[3] = 0x00;
	adw_word[4] = 0x00;
	adw_word[5] = 0x00;
	adw_word[6] = (0x00)|(adw.SEG << 3)|(adw.USE_EXTENSION << 2);
	adw_word[7] = (0x00)|(adw.SEG_INTERRUPT << 6)|(false << 5)|(adw.IGNORE_ADW << 4)|(false << 3)|(adw.M3 << 2)|(adw.M2 << 1)|(adw.M1);
	adw_word[8] =  (freq_offset >> 24) & (0xFF);
	adw_word[9] =  (freq_offset >> 16) & (0xFF);
	adw_word[10] = (freq_offset >> 8) & (0xFF);
	adw_word[11] = (freq_offset) & (0xFF);
	adw_word[12] = (level_offset >> 8) & (0xFF);
	adw_word[13] = (level_offset) & (0xFF);
	adw_word[14] = (phase_offset >> 8) & (0xFF);
	adw_word[15] = (phase_offset) & (0xFF);
	adw_word[16] = (adw.SEGMENT_IDX >> 16) & (0xFF);
	adw_word[17] = (adw.SEGMENT_IDX >> 8) & (0xFF);
	adw_word[18] = (adw.SEGMENT_IDX) & (0xFF);
	adw_word[19] = 0x00;
	adw_word[20] = 0x00;
	adw_word[21] = 0x00;
	adw_word[22] = 0x00;
	adw_word[23] = 0x00;
	adw_word[24] = 0x00;
	adw_word[25] = 0x00;
	adw_word[26] = (burst_sri >> 24) & (0xFF);
	adw_word[27] = (burst_sri >> 16) & (0xFF);
	adw_word[28] = (burst_sri >> 8) & (0xFF);
	adw_word[29] = (burst_sri) & (0xFF);
	adw_word[30] = (adw.BURST_ADD_SEGMENTS >> 8) & (0xFF);
	adw_word[31] = (adw.BURST_ADD_SEGMENTS) & (0xFF);

	for(int i = 0; i<32; i++){
		printf("%02x ", adw_word[i]);
	}
	printf("\n");
}
/*
int main(){

struct adw_s my_adw = adw_default;

char *adw_word = malloc(sizeof(char *)*33);
adw_constructor(adw_word,my_adw);



printf("TEST\n");
for(int i = 0; i<32; i++){
	printf("%x",adw_word[i]);
}

}
*/
