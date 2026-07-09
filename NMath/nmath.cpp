//========= Copyright PmbrugGaming. All rights reserved. ============//
//
// Purpose: NMath.cpp: the entry point 
//
// $NoKeywords: $
//
//=============================================================================//

#include "pch.h"
#include "NMath.h"
#include "imath.h"
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
}

static NMath::CRandom s_Random;
static NMath::CMath s_Math;

IRandom* g_pRandom = &s_Random;
IMath* g_pMath = &s_Math;