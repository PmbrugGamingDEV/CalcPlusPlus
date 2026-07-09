/*
* Public Domain
* Free of use for all works
*/

#include "pch.h"
#include "NMath.h"
#include <iostream>
#include <random>
#include <cstdint>
#include <cmath>

namespace NMath
{
	std::mt19937_64 CRandom::g_RandomEngine
	{ 
		std::random_device{}() 
	};

	static CRandom g_Random;
	CRandom* g_pRandom = &g_Random;

	static CMath g_Math;
	CMath* g_pMath = &g_Math;
}