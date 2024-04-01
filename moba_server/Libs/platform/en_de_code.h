#pragma once

#include "dtype.h"
#include <stdint.h>

class CEnDeCode
{
public:
	//º”√‹
	void EnCode(unsigned char* buf, uint32_t size);
	//Ω‚√‹
	void DeCode(unsigned char* buf, uint32_t size);

private:
	CEnDeCode() {}
	~CEnDeCode() {}

	int32_t _key = 0;		//√‹‘ø
};

class CEnDeCodeX
{
public:
	uint32_t Rand();

	bool IsInitRand() { return _bInitRand; }
	bool IsInitKey() { return _bInitKey; }

	int GetRandSeed() const { return _RandSeed; }
	void SetRandSeed(int val) { _RandSeed = val; _bInitRand = true; }

	void SetEncryKey(const unsigned char* encryKey, size_t size);
	unsigned char* Encrypt(unsigned char* data, size_t size);

	void Clear() { _bInitRand = false; _bInitKey = false; }

private:
	int _RandSeed = 0;
	bool _bInitRand = false;
	bool _bInitKey = false;

	static const uint32_t D_WORD_XORLMUL = 0x009d;
	static const uint32_t D_WORD_XOR3BASE = 0x086d;
	static const uint32_t D_WORD_XOR3MUL = 0x087b;
	static const size_t D_XORTABLE_LENGTH = 8;

	unsigned char _EncryKey[D_XORTABLE_LENGTH];
	unsigned char G_BYTE_DOII_XorTable[D_XORTABLE_LENGTH] = { 0x15, 0xcd, 0x5b, 0x07, 0xb1, 0x68, 0xde, 0x3e };
	unsigned char G_BYTE_XorTable[D_XORTABLE_LENGTH];

};
