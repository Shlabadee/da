#ifndef __psqACrFHaY
#define __psqACrFHaY

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

uint64_t da_rand64();
uint32_t da_rand32();
uint16_t da_rand16();
float da_randf(int use_negative);
float da_randf_range(float min, float max);
void da_initrstate(uint64_t rseed);

#endif
