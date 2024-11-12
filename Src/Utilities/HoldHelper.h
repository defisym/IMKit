#pragma once

template<typename T>
struct HoldHelper {
	T* pDest = nullptr;
	T old = {};

	HoldHelper(T* p) :pDest(p), old(*p) {}
	~HoldHelper() {
		*pDest = old;
	}
};