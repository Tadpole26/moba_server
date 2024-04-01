#include "pch.h"
#include "en_de_code.h"
#include <string.h>

void CEnDeCode::EnCode(unsigned char* buf, uint32_t size)
{
	uint32_t tmpi = 0;
	int32_t conf = 0;
	int32_t tmpc = 0;
	conf = _key % 255;
	conf = conf >= 0 ? conf : -conf;

	for (tmpi = 0; tmpi < size; ++tmpi)
	{
		tmpc = buf[tmpi];
		tmpc += conf;
		if (tmpc > 255)
			tmpc -= 256;
		buf[tmpi] = ((unsigned char)tmpc) ^ 0xff;
	}
}

void CEnDeCode::DeCode(unsigned char* buf, uint32_t size)
{
	uint32_t tmpi = 0;
	int32_t conf = 0;
	int32_t tmpc = 0;
	conf = _key % 255;
	conf = conf >= 0 ? conf : -conf;
	for (tmpi = 0; tmpi < size; ++tmpi)
	{
		tmpc = buf[tmpi] ^ 0xff;
		tmpc = tmpc >= conf ? tmpc - conf : 256 + tmpc - conf;
		buf[tmpi] = (unsigned char)tmpc;
	}
}

uint32_t CEnDeCodeX::Rand()
{
	uint32_t r = _RandSeed = (_RandSeed * 1103515245 + 123456);
	return (uint32_t)((r << 16) | ((r >> 16) & 0xFFFF));
}

void CEnDeCodeX::SetEncryKey(const unsigned char* encryKey, size_t size)
{
	if (size <= 0) return;

	if (size < D_XORTABLE_LENGTH)
		memcpy(_EncryKey, encryKey, size);
	else
		memcpy(_EncryKey, encryKey, D_XORTABLE_LENGTH);

	for (int i = 0; i < 8; i++)
	{
		G_BYTE_XorTable[i] = (unsigned char)(_EncryKey[i] ^ G_BYTE_DOII_XorTable[i]);
	}
	_bInitKey = true;
}

unsigned char* CEnDeCodeX::Encrypt(unsigned char* data, size_t size)
{
	char l_BYTE_Xorl = (unsigned char)(size * D_WORD_XORLMUL);
	uint32_t l_WORD_Xor3 = D_WORD_XOR3BASE;
	size_t nCir = 0;
	for (nCir = 0; nCir < size; nCir++)
	{
		unsigned char l_BYTE_Xor3 = (unsigned char)(l_WORD_Xor3 >> 8);
		data[nCir] = (unsigned char)(data[nCir] ^ l_BYTE_Xorl ^ G_BYTE_XorTable[nCir % D_XORTABLE_LENGTH] ^ l_BYTE_Xor3);
		l_WORD_Xor3 *= D_WORD_XOR3MUL;
	}
	return data;
}
