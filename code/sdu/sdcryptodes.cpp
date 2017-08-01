#include "SDuPCH.h"
#include "sdcryptodes.h"
#include <cstdlib>

namespace Crown
{

	//
	// 32-bit integer manipulation macros (big endian)
	//
#ifndef GET_ULONG_BE

#define GET_ULONG_BE(n,b,i)                      \
	{                                                \
	(n) = ( (uint32) (b)[(i)    ] << 24 )        \
	| ( (uint32) (b)[(i) + 1] << 16 )			\
	| ( (uint32) (b)[(i) + 2] <<  8 )			\
	| ( (uint32) (b)[(i) + 3]       );			\
}

#endif

#ifndef PUT_ULONG_BE

#define PUT_ULONG_BE(n,b,i)                             \
	{                                                       \
	(b)[(i)    ] = (byte) ( (n) >> 24 );       \
	(b)[(i) + 1] = (byte) ( (n) >> 16 );       \
	(b)[(i) + 2] = (byte) ( (n) >>  8 );       \
	(b)[(i) + 3] = (byte) ( (n)       );       \
}

#endif

	//
	// DES S-boxes
	//
	static const uint32 SB1[64] =
	{
		0x01010400, 0x00000000, 0x00010000, 0x01010404,
		0x01010004, 0x00010404, 0x00000004, 0x00010000,
		0x00000400, 0x01010400, 0x01010404, 0x00000400,
		0x01000404, 0x01010004, 0x01000000, 0x00000004,
		0x00000404, 0x01000400, 0x01000400, 0x00010400,
		0x00010400, 0x01010000, 0x01010000, 0x01000404,
		0x00010004, 0x01000004, 0x01000004, 0x00010004,
		0x00000000, 0x00000404, 0x00010404, 0x01000000,
		0x00010000, 0x01010404, 0x00000004, 0x01010000,
		0x01010400, 0x01000000, 0x01000000, 0x00000400,
		0x01010004, 0x00010000, 0x00010400, 0x01000004,
		0x00000400, 0x00000004, 0x01000404, 0x00010404,
		0x01010404, 0x00010004, 0x01010000, 0x01000404,
		0x01000004, 0x00000404, 0x00010404, 0x01010400,
		0x00000404, 0x01000400, 0x01000400, 0x00000000,
		0x00010004, 0x00010400, 0x00000000, 0x01010004
	};

	static const uint32 SB2[64] =
	{
		0x80108020, 0x80008000, 0x00008000, 0x00108020,
		0x00100000, 0x00000020, 0x80100020, 0x80008020,
		0x80000020, 0x80108020, 0x80108000, 0x80000000,
		0x80008000, 0x00100000, 0x00000020, 0x80100020,
		0x00108000, 0x00100020, 0x80008020, 0x00000000,
		0x80000000, 0x00008000, 0x00108020, 0x80100000,
		0x00100020, 0x80000020, 0x00000000, 0x00108000,
		0x00008020, 0x80108000, 0x80100000, 0x00008020,
		0x00000000, 0x00108020, 0x80100020, 0x00100000,
		0x80008020, 0x80100000, 0x80108000, 0x00008000,
		0x80100000, 0x80008000, 0x00000020, 0x80108020,
		0x00108020, 0x00000020, 0x00008000, 0x80000000,
		0x00008020, 0x80108000, 0x00100000, 0x80000020,
		0x00100020, 0x80008020, 0x80000020, 0x00100020,
		0x00108000, 0x00000000, 0x80008000, 0x00008020,
		0x80000000, 0x80100020, 0x80108020, 0x00108000
	};

	static const uint32 SB3[64] =
	{
		0x00000208, 0x08020200, 0x00000000, 0x08020008,
		0x08000200, 0x00000000, 0x00020208, 0x08000200,
		0x00020008, 0x08000008, 0x08000008, 0x00020000,
		0x08020208, 0x00020008, 0x08020000, 0x00000208,
		0x08000000, 0x00000008, 0x08020200, 0x00000200,
		0x00020200, 0x08020000, 0x08020008, 0x00020208,
		0x08000208, 0x00020200, 0x00020000, 0x08000208,
		0x00000008, 0x08020208, 0x00000200, 0x08000000,
		0x08020200, 0x08000000, 0x00020008, 0x00000208,
		0x00020000, 0x08020200, 0x08000200, 0x00000000,
		0x00000200, 0x00020008, 0x08020208, 0x08000200,
		0x08000008, 0x00000200, 0x00000000, 0x08020008,
		0x08000208, 0x00020000, 0x08000000, 0x08020208,
		0x00000008, 0x00020208, 0x00020200, 0x08000008,
		0x08020000, 0x08000208, 0x00000208, 0x08020000,
		0x00020208, 0x00000008, 0x08020008, 0x00020200
	};

	static const uint32 SB4[64] =
	{
		0x00802001, 0x00002081, 0x00002081, 0x00000080,
		0x00802080, 0x00800081, 0x00800001, 0x00002001,
		0x00000000, 0x00802000, 0x00802000, 0x00802081,
		0x00000081, 0x00000000, 0x00800080, 0x00800001,
		0x00000001, 0x00002000, 0x00800000, 0x00802001,
		0x00000080, 0x00800000, 0x00002001, 0x00002080,
		0x00800081, 0x00000001, 0x00002080, 0x00800080,
		0x00002000, 0x00802080, 0x00802081, 0x00000081,
		0x00800080, 0x00800001, 0x00802000, 0x00802081,
		0x00000081, 0x00000000, 0x00000000, 0x00802000,
		0x00002080, 0x00800080, 0x00800081, 0x00000001,
		0x00802001, 0x00002081, 0x00002081, 0x00000080,
		0x00802081, 0x00000081, 0x00000001, 0x00002000,
		0x00800001, 0x00002001, 0x00802080, 0x00800081,
		0x00002001, 0x00002080, 0x00800000, 0x00802001,
		0x00000080, 0x00800000, 0x00002000, 0x00802080
	};

	static const uint32 SB5[64] =
	{
		0x00000100, 0x02080100, 0x02080000, 0x42000100,
		0x00080000, 0x00000100, 0x40000000, 0x02080000,
		0x40080100, 0x00080000, 0x02000100, 0x40080100,
		0x42000100, 0x42080000, 0x00080100, 0x40000000,
		0x02000000, 0x40080000, 0x40080000, 0x00000000,
		0x40000100, 0x42080100, 0x42080100, 0x02000100,
		0x42080000, 0x40000100, 0x00000000, 0x42000000,
		0x02080100, 0x02000000, 0x42000000, 0x00080100,
		0x00080000, 0x42000100, 0x00000100, 0x02000000,
		0x40000000, 0x02080000, 0x42000100, 0x40080100,
		0x02000100, 0x40000000, 0x42080000, 0x02080100,
		0x40080100, 0x00000100, 0x02000000, 0x42080000,
		0x42080100, 0x00080100, 0x42000000, 0x42080100,
		0x02080000, 0x00000000, 0x40080000, 0x42000000,
		0x00080100, 0x02000100, 0x40000100, 0x00080000,
		0x00000000, 0x40080000, 0x02080100, 0x40000100
	};

	static const uint32 SB6[64] =
	{
		0x20000010, 0x20400000, 0x00004000, 0x20404010,
		0x20400000, 0x00000010, 0x20404010, 0x00400000,
		0x20004000, 0x00404010, 0x00400000, 0x20000010,
		0x00400010, 0x20004000, 0x20000000, 0x00004010,
		0x00000000, 0x00400010, 0x20004010, 0x00004000,
		0x00404000, 0x20004010, 0x00000010, 0x20400010,
		0x20400010, 0x00000000, 0x00404010, 0x20404000,
		0x00004010, 0x00404000, 0x20404000, 0x20000000,
		0x20004000, 0x00000010, 0x20400010, 0x00404000,
		0x20404010, 0x00400000, 0x00004010, 0x20000010,
		0x00400000, 0x20004000, 0x20000000, 0x00004010,
		0x20000010, 0x20404010, 0x00404000, 0x20400000,
		0x00404010, 0x20404000, 0x00000000, 0x20400010,
		0x00000010, 0x00004000, 0x20400000, 0x00404010,
		0x00004000, 0x00400010, 0x20004010, 0x00000000,
		0x20404000, 0x20000000, 0x00400010, 0x20004010
	};

	static const uint32 SB7[64] =
	{
		0x00200000, 0x04200002, 0x04000802, 0x00000000,
		0x00000800, 0x04000802, 0x00200802, 0x04200800,
		0x04200802, 0x00200000, 0x00000000, 0x04000002,
		0x00000002, 0x04000000, 0x04200002, 0x00000802,
		0x04000800, 0x00200802, 0x00200002, 0x04000800,
		0x04000002, 0x04200000, 0x04200800, 0x00200002,
		0x04200000, 0x00000800, 0x00000802, 0x04200802,
		0x00200800, 0x00000002, 0x04000000, 0x00200800,
		0x04000000, 0x00200800, 0x00200000, 0x04000802,
		0x04000802, 0x04200002, 0x04200002, 0x00000002,
		0x00200002, 0x04000000, 0x04000800, 0x00200000,
		0x04200800, 0x00000802, 0x00200802, 0x04200800,
		0x00000802, 0x04000002, 0x04200802, 0x04200000,
		0x00200800, 0x00000000, 0x00000002, 0x04200802,
		0x00000000, 0x00200802, 0x04200000, 0x00000800,
		0x04000002, 0x04000800, 0x00000800, 0x00200002
	};

	static const uint32 SB8[64] =
	{
		0x10001040, 0x00001000, 0x00040000, 0x10041040,
		0x10000000, 0x10001040, 0x00000040, 0x10000000,
		0x00040040, 0x10040000, 0x10041040, 0x00041000,
		0x10041000, 0x00041040, 0x00001000, 0x00000040,
		0x10040000, 0x10000040, 0x10001000, 0x00001040,
		0x00041000, 0x00040040, 0x10040040, 0x10041000,
		0x00001040, 0x00000000, 0x00000000, 0x10040040,
		0x10000040, 0x10001000, 0x00041040, 0x00040000,
		0x00041040, 0x00040000, 0x10041000, 0x00001000,
		0x00000040, 0x10040040, 0x00001000, 0x00041040,
		0x10001000, 0x00000040, 0x10000040, 0x10040000,
		0x10040040, 0x10000000, 0x00040000, 0x10001040,
		0x00000000, 0x10041040, 0x00040040, 0x10000040,
		0x10040000, 0x10001000, 0x10001040, 0x00000000,
		0x10041040, 0x00041000, 0x00041000, 0x00001040,
		0x00001040, 0x00040040, 0x10000000, 0x10041000
	};

	//
	// PC1: left and right halves bit-swap
	//
	static const uint32 LHs[16] =
	{
		0x00000000, 0x00000001, 0x00000100, 0x00000101,
		0x00010000, 0x00010001, 0x00010100, 0x00010101,
		0x01000000, 0x01000001, 0x01000100, 0x01000101,
		0x01010000, 0x01010001, 0x01010100, 0x01010101
	};

	static const uint32 RHs[16] =
	{
		0x00000000, 0x01000000, 0x00010000, 0x01010000,
		0x00000100, 0x01000100, 0x00010100, 0x01010100,
		0x00000001, 0x01000001, 0x00010001, 0x01010001,
		0x00000101, 0x01000101, 0x00010101, 0x01010101,
	};

	//
	// Initial Permutation macro
	//
#define DES_IP(X,Y)                                             \
	{                                                               \
	T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
	T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
	T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
	T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
	Y = ((Y << 1) | (Y >> 31)) & 0xFFFFFFFF;                    \
	T = (X ^ Y) & 0xAAAAAAAA; Y ^= T; X ^= T;                   \
	X = ((X << 1) | (X >> 31)) & 0xFFFFFFFF;                    \
		}

	//
	// Final Permutation macro
	//
#define DES_FP(X,Y)                                             \
	{                                                               \
	X = ((X << 31) | (X >> 1)) & 0xFFFFFFFF;                    \
	T = (X ^ Y) & 0xAAAAAAAA; X ^= T; Y ^= T;                   \
	Y = ((Y << 31) | (Y >> 1)) & 0xFFFFFFFF;                    \
	T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
	T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
	T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
	T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
		}

	//
	// DES round macro
	//
#define DES_ROUND(X,Y)                          \
	{                                               \
	T = *SK++ ^ X;                              \
	Y ^= SB8[ (T      ) & 0x3F ] ^              \
	SB6[ (T >>  8) & 0x3F ] ^              \
	SB4[ (T >> 16) & 0x3F ] ^              \
	SB2[ (T >> 24) & 0x3F ];               \
	\
	T = *SK++ ^ ((X << 28) | (X >> 4));         \
	Y ^= SB7[ (T      ) & 0x3F ] ^              \
	SB5[ (T >>  8) & 0x3F ] ^              \
	SB3[ (T >> 16) & 0x3F ] ^              \
	SB1[ (T >> 24) & 0x3F ];               \
		}

#define SWAP(a,b) { uint32 t = a; a = b; b = t; t = 0; }

	static void DESSetKeySub( uint32 SK[32], byte key[8] )
	{
		int32  i;
		uint32 X, Y, T;

		GET_ULONG_BE( X, key, 0 );
		GET_ULONG_BE( Y, key, 4 );

		//
		// Permuted Choice 1
		//
		T =  ((Y >>  4) ^ X) & 0x0F0F0F0F;
		X ^= T;
		Y ^= (T <<  4);
		T =  ((Y      ) ^ X) & 0x10101010;
		X ^= T;
		Y ^= (T      );

		X =   (LHs[ (X      ) & 0xF] << 3) | (LHs[ (X >>  8) & 0xF ] << 2)
			| (LHs[ (X >> 16) & 0xF] << 1) | (LHs[ (X >> 24) & 0xF ]     )
			| (LHs[ (X >>  5) & 0xF] << 7) | (LHs[ (X >> 13) & 0xF ] << 6)
			| (LHs[ (X >> 21) & 0xF] << 5) | (LHs[ (X >> 29) & 0xF ] << 4);

		Y =   (RHs[ (Y >>  1) & 0xF] << 3) | (RHs[ (Y >>  9) & 0xF ] << 2)
			| (RHs[ (Y >> 17) & 0xF] << 1) | (RHs[ (Y >> 25) & 0xF ]     )
			| (RHs[ (Y >>  4) & 0xF] << 7) | (RHs[ (Y >> 12) & 0xF ] << 6)
			| (RHs[ (Y >> 20) & 0xF] << 5) | (RHs[ (Y >> 28) & 0xF ] << 4);

		X &= 0x0FFFFFFF;
		Y &= 0x0FFFFFFF;

		//
		// calculate subkeys
		//
		for( i = 0; i < 16; i++ )
		{
			if( i < 2 || i == 8 || i == 15 )
			{
				X = ((X <<  1) | (X >> 27)) & 0x0FFFFFFF;
				Y = ((Y <<  1) | (Y >> 27)) & 0x0FFFFFFF;
			}
			else
			{
				X = ((X <<  2) | (X >> 26)) & 0x0FFFFFFF;
				Y = ((Y <<  2) | (Y >> 26)) & 0x0FFFFFFF;
			}

			*SK++ =   ((X <<  4) & 0x24000000) | ((X << 28) & 0x10000000)
				| ((X << 14) & 0x08000000) | ((X << 18) & 0x02080000)
				| ((X <<  6) & 0x01000000) | ((X <<  9) & 0x00200000)
				| ((X >>  1) & 0x00100000) | ((X << 10) & 0x00040000)
				| ((X <<  2) & 0x00020000) | ((X >> 10) & 0x00010000)
				| ((Y >> 13) & 0x00002000) | ((Y >>  4) & 0x00001000)
				| ((Y <<  6) & 0x00000800) | ((Y >>  1) & 0x00000400)
				| ((Y >> 14) & 0x00000200) | ((Y      ) & 0x00000100)
				| ((Y >>  5) & 0x00000020) | ((Y >> 10) & 0x00000010)
				| ((Y >>  3) & 0x00000008) | ((Y >> 18) & 0x00000004)
				| ((Y >> 26) & 0x00000002) | ((Y >> 24) & 0x00000001);

			*SK++ =   ((X << 15) & 0x20000000) | ((X << 17) & 0x10000000)
				| ((X << 10) & 0x08000000) | ((X << 22) & 0x04000000)
				| ((X >>  2) & 0x02000000) | ((X <<  1) & 0x01000000)
				| ((X << 16) & 0x00200000) | ((X << 11) & 0x00100000)
				| ((X <<  3) & 0x00080000) | ((X >>  6) & 0x00040000)
				| ((X << 15) & 0x00020000) | ((X >>  4) & 0x00010000)
				| ((Y >>  2) & 0x00002000) | ((Y <<  8) & 0x00001000)
				| ((Y >> 14) & 0x00000808) | ((Y >>  9) & 0x00000400)
				| ((Y      ) & 0x00000200) | ((Y <<  7) & 0x00000100)
				| ((Y >>  7) & 0x00000020) | ((Y >>  3) & 0x00000011)
				| ((Y <<  2) & 0x00000004) | ((Y >> 21) & 0x00000002);
		}
	}





	//
	// DES key schedule (56-bit, encryption)
	//
	void DESSetEncKey( SDESContext *pCtx, byte acKey[8] )
	{
		DESSetKeySub( pCtx->aSubKey, acKey );
	}

	//
	// DES key schedule (56-bit, decryption)
	//
	void DESSetDecKey( SDESContext *pCtx, byte acKey[8] )
	{
		int32 i;

		DESSetKeySub( pCtx->aSubKey, acKey );

		for ( i = 0; i < 16; i += 2 )
		{
			SWAP( pCtx->aSubKey[i    ], pCtx->aSubKey[30 - i] );
			SWAP( pCtx->aSubKey[i + 1], pCtx->aSubKey[31 - i] );
		}
	}

	void DESCryptECB( SDESContext *pCtx,
		byte acInput[8],
		byte acOutput[8] )
	{
		int32  i;
		uint32 X, Y, T, *SK;

		SK = pCtx->aSubKey;

		GET_ULONG_BE( X, acInput, 0 );
		GET_ULONG_BE( Y, acInput, 4 );

		DES_IP( X, Y );

		for ( i = 0; i < 8; i++ )
		{
			DES_ROUND( Y, X );
			DES_ROUND( X, Y );
		}

		DES_FP( Y, X );

		PUT_ULONG_BE( Y, acOutput, 0 );
		PUT_ULONG_BE( X, acOutput, 4 );

	}

	void DESCryptCBC( SDESContext *pCtx,
		int32 nMode,
		int32 nLength,
		//byte iv[8],
		SDESBlock		*pIV,
		byte *pacInput,
		byte *pacOutput )
	{
		int32 i;
		byte temp[8];

		if( nMode == DES_ENCRYPT )
		{
			while ( nLength > 0 )
			{
				for ( i = 0; i < 8; i++ )
				{
					pacOutput[i] = (byte)( pacInput[i] ^ pIV->acBlock[i] );
				}

				DESCryptECB( pCtx, pacOutput, pacOutput );
				memcpy( pIV->acBlock, pacOutput, 8 );

				pacInput  += 8;
				pacOutput += 8;
				nLength   -= 8;
			}
		}
		else /* DES_DECRYPT */
		{
			while ( nLength > 0 )
			{
				memcpy( temp, pacInput, 8 );
				DESCryptECB( pCtx, pacInput, pacOutput );

				for ( i = 0; i < 8; i++ )
				{
					pacOutput[i] = (byte)( pacOutput[i] ^ pIV->acBlock[i] );
				}

				memcpy( pIV->acBlock, temp, 8 );

				pacInput  += 8;
				pacOutput += 8;
				nLength -= 8;
			}
		}
	}

	// Generate 8 bytes random number
	void DESRandomKey( SDESBlock *pBlock )
	{
		srand((unsigned)time(0)+rand());

		byte nRandNum[8] = {0};

		for (int32 i = 0; i < 8; i++ )
		{
			nRandNum[i] = rand() & 0xFF;
		}

		memcpy( pBlock->acBlock, nRandNum, 8);
	}

	void GenDES3Key( SDES3Block  *pKey )
	{
		srand((unsigned)time(0)+rand());

		byte nRandNum[24] = {0};

		for (int32 i = 0; i < 24; i++ )
		{
			nRandNum[i] = rand() & 0xFF;
		}

		memcpy( pKey->acBlock, nRandNum, 24);
	}


	static void DES3Setkey( uint32 esk[96], uint32 dsk[96], byte  key[24] )
	{
		int i;

		DESSetKeySub( esk, key );
		DESSetKeySub( dsk + 32, key +  8 );
		DESSetKeySub( esk + 64, key + 16 );

		for( i = 0; i < 32; i += 2 )
		{
			dsk[i     ] = esk[94 - i];
			dsk[i +  1] = esk[95 - i];

			esk[i + 32] = dsk[62 - i];
			esk[i + 33] = dsk[63 - i];

			dsk[i + 64] = esk[30 - i];
			dsk[i + 65] = esk[31 - i];
		}
	}

	void DES3SetEncKey( SDES3Context *pCtx, SDES3Block *pKey )
	{
		uint32 sk[96] = {0};

		DES3Setkey( pCtx->aSubKey, sk, pKey->acBlock);
		memset( sk, 0, sizeof( sk ) );
	}

	void DES3SetDecKey( SDES3Context *pCtx, SDES3Block *pKey )
	{
		uint32 sk[96] = {0};

		DES3Setkey( sk, pCtx->aSubKey, pKey->acBlock);
		memset( sk, 0, sizeof( sk ) );
	}

	void DES3CryptECB( SDES3Context *pCtx,
		byte acInput[8],
		byte acOutput[8] )
	{
		int32 i;
		uint32 X, Y, T, *SK;

		SK = pCtx->aSubKey;

		GET_ULONG_BE( X, acInput, 0 );
		GET_ULONG_BE( Y, acInput, 4 );

		DES_IP( X, Y );

		for( i = 0; i < 8; i++ )
		{
			DES_ROUND( Y, X );
			DES_ROUND( X, Y );
		}

		for( i = 0; i < 8; i++ )
		{
			DES_ROUND( X, Y );
			DES_ROUND( Y, X );
		}

		for( i = 0; i < 8; i++ )
		{
			DES_ROUND( Y, X );
			DES_ROUND( X, Y );
		}

		DES_FP( Y, X );

		PUT_ULONG_BE( Y, acOutput, 0 );
		PUT_ULONG_BE( X, acOutput, 4 );
	}

	void DES3CryptCBC ( SDES3Context  *pCtx,
		int32		 nMode,
		int32		 nLength,
		SDES3Block	 *pIV,
		byte *pacInput,
		byte *pacOutput )
	{
		int i;
		byte acTemp[8];

		if( nMode == DES_ENCRYPT )
		{
			while( nLength > 0 )
			{
				for( i = 0; i < 8; i++ )
					pacOutput[i] = (byte)( pacInput[i] ^ pIV->acBlock[i] );

				DES3CryptECB( pCtx, pacOutput, pacOutput );
				memcpy( pIV, pacOutput, 8 );

				pacInput  += 8;
				pacOutput += 8;
				nLength -= 8;
			}
		}
		else /* DES_DECRYPT */
		{
			while( nLength > 0 )
			{
				memcpy( acTemp, pacInput, 8 );
				DES3CryptECB( pCtx, pacInput, pacOutput );

				for( i = 0; i < 8; i++ )
					pacOutput[i] = (byte)( pacOutput[i] ^ pIV->acBlock[i] );

				memcpy( pIV, acTemp, 8 );

				pacInput  += 8;
				pacOutput += 8;
				nLength -= 8;
			}
		}
	}


	//
	// The following arrays were used to test encryption/decryption
	//
	static const byte des_test_keys[24] =
	{
		0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
		0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
		0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
	};

	static const byte des_test_iv[8] =
	{
		0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
	};

	static const byte des_test_buf[8] =
	{
		0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74
	};

	static const byte des_test_ecb_dec[3][8] =
	{
		{ 0xCD, 0xD6, 0x4F, 0x2F, 0x94, 0x27, 0xC1, 0x5D },
		{ 0x69, 0x96, 0xC8, 0xFA, 0x47, 0xA2, 0xAB, 0xEB },
		{ 0x83, 0x25, 0x39, 0x76, 0x44, 0x09, 0x1A, 0x0A }
	};

	static const byte des_test_ecb_enc[3][8] =
	{
		{ 0x6A, 0x2A, 0x19, 0xF4, 0x1E, 0xCA, 0x85, 0x4B },
		{ 0x03, 0xE6, 0x9F, 0x5B, 0xFA, 0x58, 0xEB, 0x42 },
		{ 0xDD, 0x17, 0xE8, 0xB8, 0xB4, 0x37, 0xD2, 0x32 }
	};

	static const byte des_test_cbc_dec[3][8] =
	{
		{ 0x12, 0x9F, 0x40, 0xB9, 0xD2, 0x00, 0x56, 0xB3 },
		{ 0x47, 0x0E, 0xFC, 0x9A, 0x6B, 0x8E, 0xE3, 0x93 },
		{ 0xC5, 0xCE, 0xCF, 0x63, 0xEC, 0xEC, 0x51, 0x4C }
	};

	static const byte des_test_cbc_enc[3][8] =
	{
		{ 0x54, 0xF1, 0x5A, 0xF6, 0xEB, 0xE3, 0xA4, 0xB4 },
		{ 0x35, 0x76, 0x11, 0x56, 0x5F, 0xA1, 0x8E, 0x4D },
		{ 0xCB, 0x19, 0x1F, 0x85, 0xD1, 0xED, 0x84, 0x39 }
	};

	static const byte des3_test_keys[24] =
	{
		0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
		0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
		0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
	};

	static const byte des3_test_iv[8] =
	{
		0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
	};

	static const byte des3_test_buf[8] =
	{
		0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74
	};


	int32 DESSelfTest( int32 nVerbose )
	{
		int32 i, j, u, v;
		SDESContext ctx;
		byte key[24];
		byte buf[8];
		byte prv[8];
		SDESBlock		iv;

		memset( key, 0, 24 );

		//
		// ECB mode
		//
		for ( i = 0; i < 2; i++ )
		{
			u = i >> 1;
			v = i  & 1;

			if ( nVerbose != 0 )
				printf( "  DES%c-ECB-%3d (%s): ",
				( u == 0 ) ? ' ' : '3', 56 + u * 56,
				( v == DES_DECRYPT ) ? "dec" : "enc" );

			memcpy( buf, des_test_buf, 8 );

			switch ( i )
			{
			case 0:
				DESSetDecKey( &ctx, (byte *) des_test_keys );
				break;

			case 1:
				DESSetEncKey( &ctx, (byte *) des_test_keys );
				break;


			default:
				return( 1 );
			}

			for ( j = 0; j < 10000; j++ )
			{
				if ( u == 0 )
				{
					DESCryptECB( &ctx, buf, buf );
				}
			}

			if ( ( v == DES_DECRYPT &&
				memcmp( buf, des_test_ecb_dec[u], 8 ) != 0 ) ||
				( v != DES_DECRYPT &&
				memcmp( buf, des_test_ecb_enc[u], 8 ) != 0 ) )
			{
				if ( nVerbose != 0 )
					printf( "failed\n" );

				return( 1 );
			}

			if ( nVerbose != 0 )
				printf( "passed\n" );
		}

		if ( nVerbose != 0 )
			printf( "\n" );

		//
		// CBC mode
		//
		for ( i = 0; i < 2; i++ )
		{
			u = i >> 1;
			v = i  & 1;

			if ( nVerbose != 0 )
				printf( "  DES%c-CBC-%3d (%s): ",
				( u == 0 ) ? ' ' : '3', 56 + u * 56,
				( v == DES_DECRYPT ) ? "dec" : "enc" );

			memcpy( iv.acBlock, des_test_iv,  8 );
			memcpy( prv, des_test_iv,  8 );
			memcpy( buf, des_test_buf, 8 );

			switch ( i )
			{
			case 0:
				DESSetDecKey( &ctx, (byte *) des_test_keys );
				break;

			case 1:
				DESSetEncKey( &ctx, (byte *) des_test_keys );
				break;

			default:
				return 1;
			}

			if ( v == DES_DECRYPT )
			{
				for ( j = 0; j < 10000; j++ )
				{
					if ( u == 0 )
					{
						DESCryptCBC( &ctx, v, 8, &iv, buf, buf );
					}
				}
			}
			else
			{
				for ( j = 0; j < 10000; j++ )
				{
					byte tmp[8];

					if ( u == 0 )
					{
						DESCryptCBC( &ctx, v, 8, &iv, buf, buf );
					}

					memcpy( tmp, prv, 8 );
					memcpy( prv, buf, 8 );
					memcpy( buf, tmp, 8 );
				}

				memcpy( buf, prv, 8 );
			}

			if( ( v == DES_DECRYPT &&
				memcmp( buf, des_test_cbc_dec[u], 8 ) != 0 ) ||
				( v != DES_DECRYPT &&
				memcmp( buf, des_test_cbc_enc[u], 8 ) != 0 ) )
			{
				if ( nVerbose != 0 )
				{
					printf( "failed\n" );
				}

				return 1;
			}

			if ( nVerbose != 0 )
			{
				printf( "passed\n" );
			}
		}

		if ( nVerbose != 0 )
		{
			printf( "\n" );
		}

		return( 0 );
	}

}


