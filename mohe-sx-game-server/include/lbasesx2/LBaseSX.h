#pragma once
#ifdef LBASESX_EXPORTS
#define API_LBASESX _declspec(dllexport)
#else
#define API_LBASESX _declspec(dllimport)
#endif
