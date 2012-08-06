
#include <math.h>
#include "./mathExtensions.h"
#include <stdio.h>

uint32_t logBinario(uint32_t value)
{
    uint32_t l = 0;
    while( (value >> l) > 1 ) ++l;
    return l;
}

int32_t potencia(int32_t base, uint16_t exp)
{
	if(exp == 0)
		return 1;

	int32_t result = base;

	for(int i = 1; i < exp; i++)
		result *=base;

	return result;
}

uint32_t sumMitades(uint32_t number)
{
	if((number % 2) && number != 1)
		return 0;

	int32_t dev = number;
	int32_t half = number/2;

	while(half != 0)
	{
		dev += half;
		half /= 2;
	}

	return dev;
}
