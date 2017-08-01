#include "SDuPCH.h"
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include "sdserverid.h"
#include "sdstring.h"
namespace Crown
{

    bool _IsNum(const char* str)
    {
        int32 nLen = (int32)strlen(str);
        if (0 == nLen)
        {
            return false;
        }

        for (int32 n = 0; n < nLen; n++)
        {
            if (str[n] > '9' || str[n] < '0')
            {
                return false;
            }
        }
        return true;
    }

    static void SDCalcMasks( uint32 maskLen[] , uint32 submask[], uint32 dwMask)
    {
        for (int32 i = 0; i < 4; i ++)
        {
            maskLen[i] = (dwMask >> i*8) &0xFF;
            submask[i] = 1 ;
            submask[i] = (submask[i] << (maskLen[i])) -1;
        }
    }

//////////////////////////////////////////////////////////////////////////
// exported functions

    std::string SDAPI SDServerIDUtoa(uint32 dwServerID, uint32 dwMask /*=0x08080808*/)
    {
        char pszServerID[128];
        int32 dwBuffLen = sizeof(pszServerID);

        uint32 maskLen[4] ;
        uint32 submask[4] ;

        SDCalcMasks(maskLen,submask,dwMask);

        uint32 szBytes[4] = {0};

        szBytes[0] = (dwServerID >> (maskLen[0]+ maskLen[1] + maskLen[2])) & submask[3];
        szBytes[1] = (dwServerID >> (maskLen[0]+ maskLen[1])) & submask[2];
        szBytes[2] = (dwServerID >> (maskLen[0] )) & submask[1];
        szBytes[3] = (dwServerID ) & submask[0];


        SDSnprintf(pszServerID, dwBuffLen-1, "%d-%d-%d-%d",
                   szBytes[3],
                   szBytes[2],
                   szBytes[1],
                   szBytes[0]);

        pszServerID[dwBuffLen-1] = '\0';

        return pszServerID;
    }

    uint32 SDAPI SDServerIDAtou(const char* pszServerID,uint32 dwMask )
    {
        char szBuf[32]  = {0};
        char* pBeg      = szBuf;
        char* pPos      = nullptr;
        int32 nByteVal  = 0;
        uint32 dwVal    = 0;

        char ch         = '-';

        int32 values[4];
        uint32 maskLen[4] ;
        uint32 submask[4] ;
        SDCalcMasks(maskLen,submask,dwMask);

        strncpy(szBuf, pszServerID, sizeof(szBuf)-1);
        int32 i =3;
        pPos = strchr(szBuf,ch);
        while (pPos != nullptr)
        {
            *pPos = '\0';
            if (!_IsNum(pBeg))
            {
                return 0;
            }
            nByteVal = atoi(pBeg);
            nByteVal = submask[i]  & nByteVal;
            values[i] = nByteVal;
            i--;

            pBeg = pPos + 1;
            pPos = strchr(pBeg,ch);
        }
        values[0] = atoi(pBeg);

        dwVal = (values[0] << (maskLen[2] + maskLen[1] + maskLen[0]))
                | (values[1] << (maskLen[1] + maskLen[0]))
                | (values[2] << maskLen[0])
                | (values[3] );


        return dwVal;
    }

    CSDServerId::CSDServerId(uint32 dwId , uint32 dwMask )
    {
        m_mask = dwMask;
        this->SetID(dwId, m_mask);
    }


    void SDAPI CSDServerId::SetID(uint32 dwID, uint32 dwMask )
    {
        m_id = dwID;
        m_mask = dwMask ;
		SDCalcMasks(m_maskLen,m_subMask,m_mask);
        m_strId = SDServerIDUtoa(dwID,m_mask);
    }

    void SDAPI CSDServerId::SetID(const char* pszID, uint32 dwMask )
    {
        m_strId = pszID;
        m_mask = dwMask ;
		SDCalcMasks(m_maskLen,m_subMask,m_mask);
        m_id  = SDServerIDAtou(pszID,m_mask);
    }

    void SDAPI CSDServerId::SetMask(uint32 dwMask)
    {
        m_mask = dwMask;
    }

    uint32 SDAPI CSDServerId::GetMask()
    {
        return m_mask;
    }

    uint32 SDAPI  CSDServerId::GetAreaId()
    {
		
		return (m_id) & m_subMask[0];
    }

    uint32 SDAPI  CSDServerId::GetGroupId()
    {
		return (m_id >> m_maskLen[0] ) & m_subMask[1];
    }

    uint32 SDAPI  CSDServerId::GetServerType()
    {
		return (m_id >> (m_maskLen[0] +m_maskLen[1])) & m_subMask[2];
    }

    uint32 SDAPI  CSDServerId::GetServerIndex()
    {
		return (m_id >> (m_maskLen[0] +m_maskLen[1]+m_maskLen[2])) & m_subMask[3];
    }

    std::string SDAPI  CSDServerId::AsString()
    {
        return m_strId;
    }

    uint32  CSDServerId::AsNumber()
    {
        return m_id;
    }

//////////////////////////////////////////////////////////////////////////
// CSDServerIdEx



    std::string SDAPI SDServerIDExUtoa(uint64 dwServerID, uint32 dwMask /*=0x08080808*/)
    {
        char szServerId[128];

        uint32 maskLen[4] ;
        uint32 submask[4] ;
        SDCalcMasks(maskLen,submask,dwMask);
        uint32 serverId = (uint32)(dwServerID >> 32);

        uint32 gameId = (uint32)(dwServerID & 0xFFFFFFFF);

        uint32 szBytes[4] = {0};

        szBytes[0] = (serverId >> (maskLen[0]+ maskLen[1] + maskLen[2])) & submask[3];
        szBytes[1] = (serverId >> (maskLen[0]+ maskLen[1])) & submask[2];
        szBytes[2] = (serverId >> (maskLen[0] )) & submask[1];
        szBytes[3] = (serverId ) & submask[0];

        SDSnprintf(szServerId,sizeof(szServerId) -1,"%d-%d-%d-%d-%d",
                   gameId,
                   szBytes[3],
                   szBytes[2],
                   szBytes[1],
                   szBytes[0]);


        szServerId[sizeof(szServerId)-1] = '\0';

        return szServerId;
    }

    uint64 SDAPI SDServerIDExAtou(const char* pszServerID,uint32 dwMask )
    {
        char szBuf[32]  = {0};
        char* pBeg      = szBuf;
        char* pPos      = nullptr;
        int32 nByteVal  = 0;
        uint32 dwVal    = 0;

        char ch         = '-';

        int32 values[4];

        uint32 maskLen[4] ;
        uint32 submask[4] ;

        SDCalcMasks(maskLen,submask,dwMask);

        const char * pServerId = strchr(pszServerID,ch);
        int32 gameId = atoi(pszServerID);

        strncpy(szBuf, pServerId +1, sizeof(szBuf)-1);
        int32 i = 3;
        pPos = strchr(szBuf,ch);
        while (pPos != nullptr)
        {
            *pPos = '\0';
            if (!_IsNum(pBeg))
            {
                return 0;
            }
            nByteVal = atoi(pBeg);
            nByteVal = submask[i]  & nByteVal;
            values[i] = nByteVal;
            i--;

            pBeg = pPos + 1;
            pPos = strchr(pBeg,ch);
        }
        values[0] = atoi(pBeg);

        dwVal = (values[0] << (maskLen[2] + maskLen[1] + maskLen[0]))
                | (values[1] << (maskLen[1] + maskLen[0]))
                | (values[2] << maskLen[0])
                | (values[3] );

        uint64 result = dwVal;
        return (result << 32) | gameId;
    }

    CSDServerIdEx::CSDServerIdEx(uint64 dwId , uint32 dwMask )
    {
        m_mask = dwMask;
        this->SetID(dwId, m_mask);
    }


    void SDAPI CSDServerIdEx::SetID(uint64 dwID, uint32 dwMask )
    {
        m_id = dwID;
        m_mask = dwMask ;
		SDCalcMasks(m_maskLen,m_subMask,m_mask);
        m_strId = SDServerIDExUtoa(dwID,m_mask);
    }

    void SDAPI CSDServerIdEx::SetID(const char* pszID, uint32 dwMask )
    {
        m_strId = pszID;
        m_mask = dwMask ;
        SDCalcMasks(m_maskLen,m_subMask,m_mask);
        m_id = SDServerIDAtou(pszID,m_mask);
    }

    void SDAPI CSDServerIdEx::SetMask(uint32 dwMask)
    {
        m_mask = dwMask;
        SDCalcMasks(m_maskLen,m_subMask,dwMask);
    }

    uint32 SDAPI CSDServerIdEx::GetMask()
    {
        return m_mask;
    }
    uint32 SDAPI CSDServerIdEx::GetGameId()
    {
        return (uint32)m_id & 0xFFFFFFFF;
    }

    uint32 SDAPI  CSDServerIdEx::GetAreaId()
    {
		return (uint32)(m_id >> 32 ) & m_subMask[0];
	}

	uint32 SDAPI  CSDServerIdEx::GetGroupId()
	{
		return ((uint32)(m_id >> 32 ) >> m_maskLen[0] ) & m_subMask[1];
	}

	uint32 SDAPI  CSDServerIdEx::GetServerType()
	{
		return ((uint32)(m_id >> 32 ) >> (m_maskLen[0] +m_maskLen[1])) & m_subMask[2];
	}

	uint32 SDAPI CSDServerIdEx::GetServerIndex()
	{
		return ((uint32)(m_id >> 32 ) >> (m_maskLen[0] +m_maskLen[1]+m_maskLen[2])) & m_subMask[3];
	}

	std::string SDAPI  CSDServerIdEx::AsString()
	{
        return m_strId;
    }

    uint64 SDAPI  CSDServerIdEx::AsNumber()
    {
        return m_id;
    }
} // namespace Crown
