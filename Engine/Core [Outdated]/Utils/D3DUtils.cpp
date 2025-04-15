#include "D3DUtils.h"

std::string FeatureLevelToString(D3D_FEATURE_LEVEL level)
{
	switch (level)
	{
	case D3D_FEATURE_LEVEL_12_0: return "12_0";
	case D3D_FEATURE_LEVEL_12_1: return "12_1";
	case D3D_FEATURE_LEVEL_11_1: return "11_1";
	case D3D_FEATURE_LEVEL_11_0: return "11_0";
	default: return "Unknown";
	}
}