#include "stdafx.h"
#include "AdminRegionMgr.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAdminRegionMgr::CAdminRegionMgr():
	m_bInit(false),
	m_dwColPer(0),
	m_dwRowPer(0),
	m_pFile(0)
{
}

CAdminRegionMgr::~CAdminRegionMgr()
{
	Release();
}

bool CAdminRegionMgr::Initialize(const char* pFilename)
{
	bool T = false;
	Release();
	if(pFilename)
	{
		FILE *fp = fopen(pFilename,("rb"));
		if(fp)
		{
			DWORD dwRead =	fread(&m_adminRgnHeader, sizeof(m_adminRgnHeader), 1, fp);

			if( (dwRead == 1)
				&& ( strncmp(m_adminRgnHeader.szFileFlag, "REGI",sizeof(m_adminRgnHeader.szFileFlag))==0))
			{
				LBSRECT lbsBound = m_adminRgnHeader.lbsrcBound;
				m_LBPoint.x = lbsBound.minX;
				m_LBPoint.y = lbsBound.minY;
				m_dwColPer  = DWORD(lbsBound.maxX - lbsBound.minX) / DWORD(m_adminRgnHeader.wColumns);
				m_dwRowPer  = DWORD(lbsBound.maxY - lbsBound.minY) / DWORD(m_adminRgnHeader.wRows);
				m_pFile     = fp;
				m_bInit = true;
				T = true;
			}
			else
			{
				fclose(fp);
			}
		}
	}

	return T;
}

void CAdminRegionMgr::Release(void)
{
	if(m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

bool CAdminRegionMgr::GetAdminRegionInfo(POINT* pPoint, ADMINREGIONINFO* pInfo )
{
    if ( !m_bInit || pPoint == NULL || pInfo == NULL )
    {
        return false;
    }

	_tag_CountyItem countyItem;
    if ( !GetCountyItem(pPoint, countyItem) )
    {
        return false;
    }
	_tag_ProvinceItem provinceItem;
	_tag_DJSItem DJSItem;
	if ( !GetDJSItemById(countyItem.wDJSIndex, DJSItem) || !GetProvinceItemById(countyItem.wProvinceIndex, provinceItem) )
	{
        return false;
    }
	pInfo->Release();
	pInfo->wProvinceCode = provinceItem.wAdminCode;
	pInfo->dwDJSCode	 = DJSItem.dwAdminCode;
	pInfo->dwCountyCode  = countyItem.dwAdminCode;
	pInfo->pProvinceName = new TCHAR[provinceItem.wNameLength + 1];
	if(pInfo->pProvinceName)
	{
		wcscpy(pInfo->pProvinceName, provinceItem.pszNameText);
	}
	pInfo->pDJSName = new TCHAR[DJSItem.wNameLength + 1];
	if(pInfo->pDJSName)
	{
		wcscpy(pInfo->pDJSName, DJSItem.pszNameText);
	}
	pInfo->pCountyName = new TCHAR[countyItem.wNameLength + 1];
	if(pInfo->pCountyName)
	{
		wcscpy(pInfo->pCountyName, countyItem.pszNameText);
	}
	return true;
}

bool CAdminRegionMgr::SeekToRgnMeshItem(int nIndex)
{
	bool T = false;
	int iRet = fseek(m_pFile,
		m_adminRgnHeader.dwRgnMeshOffset + nIndex*sizeof(DWORD), SEEK_SET);
	if(iRet == 0)
	{
		DWORD dwOffset;
		DWORD dwRead = fread(&dwOffset, sizeof(DWORD),1,m_pFile);
		if(dwRead == 1)
		{
			iRet = fseek(m_pFile, dwOffset, SEEK_SET);
			if(iRet == 0)
			{
				T = true;
			}
		}
	}
	return T;
}

bool CAdminRegionMgr::SeekToProvinceItem(int nIndex)
{
	bool T = false;
	int iRet = fseek(m_pFile,
		m_adminRgnHeader.dwProvinceOffset + nIndex*sizeof(DWORD), SEEK_SET);
	if(iRet == 0)
	{
		DWORD dwOffset;
		DWORD dwRead = fread(&dwOffset, sizeof(DWORD),1,m_pFile);
		if(dwRead == 1)
		{
			iRet = fseek(m_pFile, dwOffset, SEEK_SET);
			if(iRet == 0)
			{
				T = true;
			}
		}
	}
	return T;
}

bool CAdminRegionMgr::SeekToDJSItem(int nIndex)
{
	bool T = false;
	int iRet = fseek(m_pFile,
		m_adminRgnHeader.dwDJSOffset + nIndex*sizeof(DWORD), SEEK_SET);
	if(iRet == 0)
	{
		DWORD dwOffset;
		DWORD dwRead = fread(&dwOffset, sizeof(DWORD),1,m_pFile);
		if(dwRead == 1)
		{
			iRet = fseek(m_pFile, dwOffset, SEEK_SET);
			if(iRet == 0)
			{
				T = true;
			}
		}
	}
	return T;
}

bool CAdminRegionMgr::SeekToCountyItem(int nIndex)
{
	bool T = false;
	int iRet = fseek(m_pFile,
		m_adminRgnHeader.dwCountyOffset + nIndex*sizeof(DWORD), SEEK_SET);
	if(iRet == 0)
	{
		DWORD dwOffset;
		DWORD dwRead = fread(&dwOffset, sizeof(DWORD),1,m_pFile);
		if(dwRead == 1)
		{
			iRet = fseek(m_pFile, dwOffset, SEEK_SET);
			if(iRet == 0)
			{
				T = true;
			}
		}
	}
	return T;
}

bool CAdminRegionMgr::GetCountyItem(POINT* pPoint, _tag_CountyItem& countyItem)
{
	bool T = false;
	int nRgnMeshItemIndex;

	if(GetRgnMeshItemIndex(pPoint, nRgnMeshItemIndex))
	{
		_tag_RgnMeshItem rgnMeshItem;
		if(GetRgnMeshItemById(nRgnMeshItemIndex, rgnMeshItem))
		{
			for (int i=0; i<rgnMeshItem.wRgnNum; i++)
			{
				if(PtInRegion(pPoint, rgnMeshItem.pRgnItemArray[i]))
				{
					int nIndex = rgnMeshItem.pRgnItemArray[i].wCountyIndex;
					if(GetCountyItemById(nIndex,countyItem))
					{
						T = true;
					}
					break;
				}
			}
		}
	}
	return T;
}

bool CAdminRegionMgr::GetRgnMeshItemById(int nIndex, _tag_RgnMeshItem& rgnMeshItem)
{
	return (SeekToRgnMeshItem(nIndex) && ReadRgnMeshItem(rgnMeshItem));
}

bool CAdminRegionMgr::GetProvinceItemById(int nIndex, _tag_ProvinceItem& provinceItem)
{
	return (SeekToProvinceItem(nIndex) && ReadProvinceItem(provinceItem));
}

bool CAdminRegionMgr::GetDJSItemById(int nIndex, _tag_DJSItem& DJSItem)
{
	return (SeekToDJSItem(nIndex) && ReadDJSItem(DJSItem));
}

bool CAdminRegionMgr::GetCountyItemById(int nIndex, _tag_CountyItem& countyItem)
{
	return (SeekToCountyItem(nIndex) && ReadCountyItem(countyItem));
}

bool CAdminRegionMgr::GetRgnMeshItemIndex(POINT* pPoint,  int& nIndex)
{
	bool T = false;
	LBSRECT lbsBound = m_adminRgnHeader.lbsrcBound;
	if(lbsBound.PtInRect(pPoint))
	{
		int nRow = (pPoint->y - m_LBPoint.y)/m_dwRowPer;
		int nCol = (pPoint->x - m_LBPoint.x)/m_dwColPer;
		if ( (nRow <= m_adminRgnHeader.wRows)
			&& (nCol<=m_adminRgnHeader.wColumns) )
		{
			nIndex = nRow*m_adminRgnHeader.wColumns + nCol;
			T = true;
		}
	}
	return T;
}

bool CAdminRegionMgr::ReadRgnMeshItem(_tag_RgnMeshItem& rgnMeshItem)
{
	bool T = false;
	DWORD dwRead = fread(&rgnMeshItem, 20, 1, m_pFile);
	if(dwRead == 1)
	{
		rgnMeshItem.pdwRgnOffset = new DWORD[rgnMeshItem.wRgnNum];
		if(rgnMeshItem.pdwRgnOffset)
		{
			dwRead = fread(rgnMeshItem.pdwRgnOffset,
				rgnMeshItem.wRgnNum*sizeof(DWORD), 1, m_pFile);
			if(dwRead == 1)
			{
				rgnMeshItem.pRgnItemArray =
					new _tag_RgnItem[rgnMeshItem.wRgnNum];
				if(rgnMeshItem.pRgnItemArray)
				{
					for (int i=0; i<rgnMeshItem.wRgnNum; i++)
					{
						int iRet = fseek(m_pFile,
							rgnMeshItem.pdwRgnOffset[i], SEEK_SET );
						if(iRet == 0)
						{
							T = ReadRgnItem(rgnMeshItem.pRgnItemArray[i]);
							if(!T)
							{
								break;
							}
						}
					}
				}
			}
		}
	}
	return T;
}

bool CAdminRegionMgr::ReadRgnItem(_tag_RgnItem& rgnItem)
{
	bool T = false;
	DWORD dwRead = fread(&rgnItem, 20, 1, m_pFile);
	if(dwRead == 1)
	{
		rgnItem.pPtArray = new POINT[rgnItem.wPtCount];
		if(rgnItem.pPtArray)
		{
			dwRead = fread(rgnItem.pPtArray,
				sizeof(POINT)*rgnItem.wPtCount, 1, m_pFile);
			if(dwRead == 1)
			{
				T = true;
			}
		}
	}
	return T;
}

bool CAdminRegionMgr::ReadProvinceItem(_tag_ProvinceItem& provinveItem)
{
	bool T = false;
	DWORD dwRead = fread(&provinveItem, 4, 1, m_pFile);
	if(dwRead == 1)
	{
		provinveItem.pszNameText = new TCHAR[provinveItem.wNameLength + 1];
		if(provinveItem.pszNameText)
		{
            BYTE *bTmp;
            long nSize = 2*provinveItem.wNameLength;

            bTmp = (BYTE*)malloc(nSize);
            memset(bTmp,0,sizeof(BYTE));
            dwRead = fread( bTmp,nSize,1, m_pFile);

            nSize = 2;
            long nOffSize = 0;

			if(dwRead == 1)
			{
			    for(int i = 0; i < provinveItem.wNameLength; i++)
                {
                    wchar_t wcTmp;

                    ByteToWchar(&wcTmp,bTmp,nSize,nOffSize);

                    provinveItem.pszNameText[i] = wcTmp;
                }
				provinveItem.pszNameText[provinveItem.wNameLength] = L'\0';
				T = true;
			}
		}
	}
	return T;
}

bool CAdminRegionMgr::ReadDJSItem(_tag_DJSItem& DJSItem)
{
	bool T = false;
	DWORD dwRead = fread(&DJSItem, 8, 1, m_pFile);
	if(dwRead == 1)
	{
		DJSItem.pszNameText = new TCHAR[DJSItem.wNameLength + 1];
		if(DJSItem.pszNameText)
		{
            BYTE *bTmp;
            long nSize = 2*DJSItem.wNameLength;

            bTmp = (BYTE*)malloc(nSize);
            memset(bTmp,0,sizeof(BYTE));
            dwRead = fread( bTmp,nSize,1, m_pFile);

            nSize = 2;
            long nOffSize = 0;

			if(dwRead == 1)
			{
                for(int i = 0; i < DJSItem.wNameLength; i++)
                {
                    wchar_t wcTmp;

                    ByteToWchar(&wcTmp,bTmp,nSize,nOffSize);

                    DJSItem.pszNameText[i] = wcTmp;
                }

				DJSItem.pszNameText[DJSItem.wNameLength] = L'\0';
				T = true;
			}
		}
	}
	return T;
}

bool CAdminRegionMgr::ReadCountyItem(_tag_CountyItem& countyItem)
{
	bool T = false;
	DWORD dwRead = fread(&countyItem, 10, 1, m_pFile);
	if(dwRead == 1)
	{
		countyItem.pszNameText = new TCHAR[countyItem.wNameLength + 1];
		if(countyItem.pszNameText)
		{
            BYTE *bTmp;
            long nSize = 2*countyItem.wNameLength;

            bTmp = (BYTE*)malloc(nSize);
            memset(bTmp,0,sizeof(BYTE));
            dwRead = fread( bTmp,nSize,1, m_pFile);

            nSize = 2;
            long nOffSize = 0;
			if(dwRead == 1)
			{
                for(int i = 0; i < countyItem.wNameLength; i++)
                {
                    wchar_t wcTmp;

                    ByteToWchar(&wcTmp,bTmp,nSize,nOffSize);

                    countyItem.pszNameText[i] = wcTmp;
                }

				countyItem.pszNameText[countyItem.wNameLength] = L'\0';
				T = true;
			}
		}
	}
	return T;
}

bool CAdminRegionMgr::PtInRegion(POINT* pPoint, _tag_RgnItem& rgnItem)
{
	bool T = false;
	if(rgnItem.wPtCount>=3 && rgnItem.lbsBound.PtInRect(pPoint))
	{
		POINT w;
		int i, j, c, d;
		POINT* vl = rgnItem.pPtArray;

		w.x = rgnItem.lbsBound.maxX+1;
		w.y = pPoint->y;
		c = 0;
		d = 0;
		int nPtCount = rgnItem.wPtCount - 1;
		for(i=0; i<nPtCount; i++)
		{
			j = (i+1) % nPtCount;

			while ((vl[i].x == vl[j].x) && (vl[i].y == vl[j].y))
			{
				i++;
				j = (i+1) % nPtCount;
			}

			if(IsOnLine(vl+i, vl+j, pPoint))
			{
				T = true;
				break;
			}
			else if(IsIntersect(vl+i, vl+j, pPoint, &w))
			{
				c++;
				if(IsCrossNode(pPoint, &w, vl+i, vl+j))
				{
					d++;
				}
			}
		}
		if(!T)
		{
			T =  ((c - d/2)%2) ? true : false;
		}
	}
	return T;
}

bool CAdminRegionMgr::IsOnLine(POINT* l_start, POINT* l_end,POINT* p)
{
	INT64 dx = l_end->x - l_start->x;
	INT64 dy = l_end->y - l_start->y;
	INT64 dx1= p->x - l_start->x;
	INT64 dy1= p->y - l_start->y;

	return ((dx*dy1-dy*dx1) == 0 ? true : false);
}

bool CAdminRegionMgr::IsSameSide(POINT* l_start, POINT* l_end,
				POINT* p,POINT* q)
{
	INT64 dx = l_end->x - l_start->x;
	INT64 dy = l_end->y - l_start->y;
	INT64 dx1= p->x - l_start->x;
	INT64 dy1= p->y - l_start->y;
	INT64 dx2= q->x - l_end->x;
	INT64 dy2= q->y - l_end->y;

	return ((dx*dy1-dy*dx1)*(dx*dy2-dy*dx2) > 0 ? true : false);
}

bool CAdminRegionMgr::IsCrossNode(POINT* l_start, POINT* l_end, POINT* p,POINT* q)
{
	INT64 dx = l_end->x - l_start->x;
	INT64 dy = l_end->y - l_start->y;
	INT64 dx1= p->x - l_start->x;
	INT64 dy1= p->y - l_start->y;
	INT64 dx2= q->x - l_end->x;
	INT64 dy2= q->y - l_end->y;

	return ((dx*dy1-dy*dx1)*(dx*dy2-dy*dx2) == 0 ? true : false);
}

bool CAdminRegionMgr::IsIntersect(POINT* s1_start, POINT* s1_end,
		POINT* s2_start, POINT* s2_end)
{
	return ((IsSameSide(s1_start, s1_end, s2_start, s2_end)==0 &&
		IsSameSide(s2_start, s2_end, s1_start, s1_end)==0) ? true : false);
}

bool CAdminRegionMgr::ByteToWchar(void* Dst,BYTE* Src,int nSize,long &offs)
{

    Src += offs;
    memcpy(Dst,Src,nSize);

    offs += nSize;
    return true;
}
