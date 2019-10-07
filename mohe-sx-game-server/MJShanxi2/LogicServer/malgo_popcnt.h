#pragma once




// https://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Other-Builtins.html#Other-Builtins  __builtin_popcountll
// https://msdn.microsoft.com/en-us/library/bb385231%28v=vs.100%29.aspx  __popcnt16, __popcnt, __popcnt64
// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
// T必须是无符号数
template <class T>
unsigned mjalgo_popcnt(T v) {
	v = v - ((v >> 1) & (T)~(T)0 / 3);
	v = (v & (T)~(T)0 / 15 * 3) + ((v >> 2) & (T)~(T)0 / 15 * 3);
	v = (v + (v >> 4)) & (T)~(T)0 / 255 * 15;
	T c = (T)(v * ((T)~(T)0 / 255)) >> (sizeof(T) - 1) * 8;
	return (unsigned)c;
}

