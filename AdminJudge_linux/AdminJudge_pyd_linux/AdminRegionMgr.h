#ifndef _ADMINREGIONMGR_H_
#define _ADMINREGIONMGR_H_
//#include "gServiceDef.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>

using namespace std;

#define DEF_x32
//#define DEF_x64

typedef struct tagPOINT {
  int  x;
  int  y;
} POINT;

typedef unsigned short WORD;
typedef wchar_t TCHAR;
typedef unsigned char BYTE;
#ifdef DEF_x64
typedef unsigned int DWORD;
typedef long INT64;
#else
typedef unsigned long DWORD;
typedef long long INT64;
#endif


//////////////////////////////////////////////////////////////////////////
// This rectangle is customized for LBS applied data object.
typedef struct lbsRect {
    int minX;   // Minimum X of the rectangle.
    int minY;   // Minimum Y of the rectangle.
    int maxX;   // Maximum X of the rectangle.
    int maxY;   // Maximum Y of the rectangle.

    //lbsRect():minX(0),minY(0),maxX(0),maxY(0){}
    //~lbsRect(){}

    inline bool IsValidRect()
    {
        if(minX==0&&minY==0&&maxX==0&&maxY==0)
            return false;
        if(minX == maxX)
            return false;
        if(minY == maxY)
            return false;
        return true;
    }

    // Is the point in the rectangle ?
    inline bool PtInRect( /* [in] */ POINT* pPoint )
    {
        if ( pPoint == NULL )
            return false;
        if (   pPoint->x < minX
            || pPoint->x > maxX
            || pPoint->y < minY
            || pPoint->y > maxY )
            return false;
        return true;
    }

    inline bool IsIncluded( /* [in] */ lbsRect* pOtherRect )
    {
        if ( pOtherRect == NULL )
            return false;
        if (   pOtherRect->maxX <= maxX
            && pOtherRect->minX >= minX
            && pOtherRect->maxY <= maxY
            && pOtherRect->minY >= minY )
            return true;
        return false;
    }

    inline bool IsIntersected( /* [in] */ lbsRect* pOtherRect )
    {
        if ( pOtherRect == NULL )
            return false;
        if ( pOtherRect->maxX   < minX
            || pOtherRect->minX > maxX
            || pOtherRect->maxY < minY
            || pOtherRect->minY > maxY )
            return false;
        return true;
    }

    inline void UniteRect( lbsRect* pRect1)
    {
        minX = minX < pRect1->minX ? minX : pRect1->minX;
        maxX = maxX < pRect1->maxX ? pRect1->maxX : maxX;
        minY = minY < pRect1->minY ? minY : pRect1->minY;
        maxY = maxY < pRect1->maxY ? pRect1->maxY : maxY;
    }

    inline void InflateRect( long x, long y )
    {
        minX -= x;
        minY -= y;
        maxX += x;
        maxY += y;
    }

    inline void CutViewRect( lbsRect* prcViewport )
    {
        if (prcViewport->minX < minX)
        {
            prcViewport->minX = minX + 1;
        }
        else if (prcViewport->minX > maxX)
        {
            prcViewport->minX = maxX - 1;
        }
        if (prcViewport->maxX < minX)
        {
            prcViewport->maxX = minX + 1;
        }
        else if (prcViewport->maxX > maxX)
        {
            prcViewport->maxX = maxX - 1;
        }
        if (prcViewport->minY < minY)
        {
            prcViewport->minY = minY + 1;
        }
        else if (prcViewport->minY > maxY)
        {
            prcViewport->minY = maxY - 1;
        }
        if (prcViewport->maxY < minY)
        {
            prcViewport->maxY = minY + 1;
        }
        else if (prcViewport->maxY > maxY)
        {
            prcViewport->maxY = maxY - 1;
        }
    }

} LBSRECT;

//! ����������Ϣ
#pragma pack(push,2)
typedef struct _tag_admin_region_info
{
    TCHAR* pProvinceName;	//! ʡ����
    WORD   wProvinceCode;	//! ʡ����
    TCHAR* pDJSName;		//! �ؼ�������
    DWORD  dwDJSCode;		//! �ؼ��б���
    TCHAR* pCountyName;		//! �ؼ�������
    DWORD  dwCountyCode;	//! �ؼ��б���
    _tag_admin_region_info()
    {
        memset(this, 0,sizeof(_tag_admin_region_info));
    }
    ~_tag_admin_region_info()
    {
        Release();
    }
    void Release(void)
    {
        if(pProvinceName)
        {
            delete[] pProvinceName;
            pProvinceName = NULL;
        }
        if(pDJSName)
        {
            delete[] pDJSName;
            pDJSName = NULL;
        }
        if(pCountyName)
        {
            delete[] pCountyName;
            pCountyName = NULL;
        }
        memset(this, 0,sizeof(_tag_admin_region_info));
    }
} ADMINREGIONINFO;
#pragma pack(pop)



#pragma pack(push,2)
//! �����������
class CAdminRegionMgr
{
public:
	CAdminRegionMgr();
	virtual ~CAdminRegionMgr();

	//! ����������Ϣ��ʼ��
	bool Initialize(const char* pFilename);

	//! ��ȡָ������������������Ϣ
	bool GetAdminRegionInfo( POINT* pPoint, ADMINREGIONINFO* pInfo );

	//! �ͷ�����
	void Release(void);

protected:

	//! �����������ͷ��Ϣ
	struct _tag_adminRegionHeader
	{
		char	szFileFlag[4];		//! �ļ�ͷ
		WORD    wProvinceNum;		//! ʡ����
		DWORD   dwProvinceOffset;	//! ʡ���ݵ�ַƫ��
		WORD    wDJSNum;			//! �ؼ��и���
		DWORD   dwDJSOffset;		//! �ؼ��е�ַƫ��
		WORD    wCountyNum;			//! �ؼ��и���
		DWORD   dwCountyOffset;		//! �ؼ��е�ַƫ��
		LBSRECT	lbsrcBound;			//! ����߽緶Χ
		WORD	wColumns;			//! ����
		WORD	wRows;				//! ����
		DWORD   dwRgnMeshOffset;	//! ���������ƫ��
	};

	//! ʡ���ݶ���
	struct _tag_ProvinceItem
	{
		WORD	wAdminCode;			//! ����
		WORD	wNameLength;		//! ���Ƴ���
		TCHAR*  pszNameText;		//! ����ָ��
		_tag_ProvinceItem()
		{
			memset(this, 0,sizeof(_tag_ProvinceItem));
		}
		~_tag_ProvinceItem()
		{
			if(pszNameText)
			{
				delete[] pszNameText;
				pszNameText = NULL;
			}
		}
	};

	//! �ؼ������ݶ���
	struct _tag_DJSItem
	{
		DWORD	dwAdminCode;		//! ����
		WORD	wProvinceIndex;		//! ʡ����
		WORD	wNameLength;		//! ���Ƴ���
		TCHAR*  pszNameText;		//! ����ָ��
		_tag_DJSItem()
		{
			memset(this, 0,sizeof(_tag_DJSItem));
		}
		~_tag_DJSItem()
		{
			if(pszNameText)
			{
				delete[] pszNameText;
				pszNameText = NULL;
			}
		}
	};

	//! �ؼ������ݶ���
	struct _tag_CountyItem
	{
		DWORD	dwAdminCode;		//! ����
		WORD	wProvinceIndex;		//! ʡ����
		WORD	wDJSIndex;			//! �ؼ�������
		WORD	wNameLength;		//! ���Ƴ���
		TCHAR*  pszNameText;		//! ����ָ��
		_tag_CountyItem()
		{
			memset(this, 0,sizeof(_tag_CountyItem));
		}
		~_tag_CountyItem()
		{
			if(pszNameText)
			{
				delete[] pszNameText;
				pszNameText = NULL;
			}
		}
	};

	//! �������ݶ���
	struct _tag_RgnItem
	{
		LBSRECT lbsBound;		//! ����߽緶Χ
		WORD	wCountyIndex;	//! �ؼ�������
		WORD	wPtCount;		//! �����
		POINT*	pPtArray;		//! ������
		_tag_RgnItem()
		{
			memset(this, 0,sizeof(_tag_RgnItem));
		}
		~_tag_RgnItem()
		{
			if(pPtArray)
			{
				delete[] pPtArray;
				pPtArray = NULL;
			}
		}
	};

	//! ��������ݶ���
	struct _tag_RgnMeshItem
	{
		LBSRECT lbsBound;				//! ����߽緶Χ
		BYTE	row;					//! ��
		BYTE	col;					//! ��
		WORD	wRgnNum;				//! �������
		DWORD*  pdwRgnOffset;			//! ����ƫ������
		_tag_RgnItem* pRgnItemArray;	//! ������������

		_tag_RgnMeshItem()
		{
			memset(this, 0,sizeof(_tag_RgnMeshItem));
		}

		~_tag_RgnMeshItem()
		{
			if(pdwRgnOffset)
			{
				delete[] pdwRgnOffset;
				pdwRgnOffset = NULL;
			}
			if(pRgnItemArray)
			{
				delete[] pRgnItemArray;
				pRgnItemArray = NULL;
			}
		}
	};

	//! ��ȡָ���������ؼ������ݶ���
	bool GetCountyItem(POINT* pPoint, _tag_CountyItem& countyItem);

	//! ��ȡָ��������������������
	bool GetRgnMeshItemIndex(POINT* pPoint, int& nIndex);

	//! �жϵ��Ƿ���������
	bool PtInRegion(POINT* pPoint, _tag_RgnItem& rgnItem);

	//! ����������ȡ��������
	bool GetRgnMeshItemById(int nIndex, _tag_RgnMeshItem& rgnMeshItem);

	//! ����������ȡʡ����
	bool GetProvinceItemById(int nIndex, _tag_ProvinceItem& provinceItem);

	//! ����������ȡ�ؼ��ж���
	bool GetDJSItemById(int nIndex, _tag_DJSItem& DJSItem);

	//! ����������ȡ�ؼ��ж���
	bool GetCountyItemById(int nIndex, _tag_CountyItem& countyItem);

private:
	//! ����������ת�ļ������������
	bool SeekToRgnMeshItem(int nIndex);

	//! ����������ת�ļ���ʡ����
	bool SeekToProvinceItem(int nIndex);

	//! ����������ת�ļ����ؼ��ж���
	bool SeekToDJSItem(int nIndex);

	//! ����������ת�ļ����ؼ��ж���
	bool SeekToCountyItem(int nIndex);

	//! ����������ݶ���
	bool ReadRgnMeshItem(_tag_RgnMeshItem& rgnMeshItem);

	//! ���������ݶ���
	bool ReadRgnItem(_tag_RgnItem& rgnItem);

	//! ��ʡ����
	bool ReadProvinceItem(_tag_ProvinceItem& provinveItem);

	//! ���ؼ������ݶ���
	bool ReadDJSItem(_tag_DJSItem& DJSItem);

	//! ���ؼ��ж���
	bool ReadCountyItem(_tag_CountyItem& countyItem);

	//! �����Ƿ����߶�ͬ��
	bool IsSameSide(POINT* l_start, POINT* l_end,
		POINT* p,POINT* q);

	//! ���Ƿ�������
	bool IsOnLine(POINT* l_start, POINT* l_end,POINT* p);

	//! �߶��Ƿ��ཻ
	bool IsIntersect(POINT* s1_start, POINT* s1_end,
		POINT* s2_start, POINT* s2_end);

	//! �߶ν����Ƿ����߶νڵ���
	bool IsCrossNode(POINT* l_start, POINT* l_end,
		POINT* p,POINT* q);

	//! �Ƿ��ʼ��
	bool	m_bInit;

	//! �м��
	DWORD	m_dwColPer;

	//! �м��
	DWORD   m_dwRowPer;

	//! ���½ǵ�
	POINT	m_LBPoint;

	//! �ļ�ָ��
	FILE*   m_pFile;

	//! �����������ͷ��Ϣ����
	_tag_adminRegionHeader m_adminRgnHeader;

//public:
	//BYTE->Any;
	bool ByteToWchar(void* Dst,BYTE* Src,int nSize,long &offs);

};
#pragma pack(pop)

#endif
