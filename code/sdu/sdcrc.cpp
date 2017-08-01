#include "SDuPCH.h"
#include "sdcrc.h"
#include "sdfile.h"
#include "Assertion.h"
#include "sdfilemapping.h"


#define CRC_FLAG			"CRC"
#define CRC_FLAG_LENGTH		sizeof("CRC")
#define CRC_LENGTH			8

static unsigned int crcTable[256];
static bool crcTableValid = false;
const static uint32 ChunkSize = 4096;

static void SDCalculateCRCTable()
{
	unsigned int val;

	for(int i = 0; i < 256; i++)
	{
		val = i;
		for(int j = 0; j < 8; j++)
		{
			if(val & 0x01)
			{
				val = 0xedb88320 ^ (val >> 1);
			}
			else
			{
				val = val >> 1;
			}
		}
		crcTable[i] = val;
	}

	crcTableValid = true;
}

namespace Crown
{



	uint32 SDAPI SDCRC(const void * buffer, uint32 len, uint32 crcVal)
	{
		// check if need to generate the crc table
		if(!crcTableValid)
		{
			SDCalculateCRCTable();
		}

		// now calculate the crc
		char * buf = (char*)buffer;
		for(uint32 i = 0; i < len; i++)
		{
			crcVal = crcTable[(crcVal ^ buf[i]) & 0xff] ^ (crcVal >> 8);
		}
		return(crcVal);
	}

	uint32 SDAPI SDFileCRC(const char* strFileName, bool bFileMapping )
	{
		uint32 crcValue = 0;
		if (bFileMapping)
		{
			CnVerify(false);
			//SFileMapping fileMap;
			//SDFileMapping(fileMap, strFileName);

			//ULONG lastPos = 0;
			//while(lastPos < fileMap.size)
			//{
			//	if(fileMap.size - lastPos < ChunkSize)
			//	{
			//		crcValue  = SDCRC(((byte*)fileMap.mem) + lastPos, fileMap.size - lastPos, crcValue);
			//		lastPos = fileMap.size;
			//	}
			//	else
			//	{
			//		crcValue  = SDCRC(((byte*)fileMap.mem) + lastPos, ChunkSize, crcValue);
			//		lastPos += ChunkSize;
			//	}
			//}
			//SDFileUnMapping(fileMap);
			return crcValue;
		}
		else
		{
			if (!SDFileExists(strFileName))
			{
				return 0;
			}

			CSDFile file;
			file.Open(strFileName, "rb");

			char buf[ChunkSize];
			while(!file.Eof())
			{
				uint32 size = file.Read(buf, sizeof(buf));
				crcValue = SDCRC(buf, size, crcValue);
			}
			file.Close();
			return crcValue;
		}
	}

}

