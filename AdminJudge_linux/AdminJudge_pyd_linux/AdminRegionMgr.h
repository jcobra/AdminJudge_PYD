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

//! 行政区域信息
#pragma pack(push,2)
typedef struct _tag_admin_region_info
{
    TCHAR* pProvinceName;	//! 省名称
    WORD   wProvinceCode;	//! 省编码
    TCHAR* pDJSName;		//! 地级市名称
    DWORD  dwDJSCode;		//! 地级市编码
    TCHAR* pCountyName;		//! 县级市名称
    DWORD  dwCountyCode;	//! 县级市编码
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
//! 行政区域管理
class CAdminRegionMgr
{
public:
	CAdminRegionMgr();
	virtual ~CAdminRegionMgr();

	//! 行政区域信息初始化
	bool Initialize(const char* pFilename);

	//! 获取指定点所在行政区域信息
	bool GetAdminRegionInfo( POINT* pPoint, ADMINREGIONINFO* pInfo );

	//! 释放数据
	void Release(void);

protected:

	//! 行政区域管理头信息
	struct _tag_adminRegionHeader
	{
		char	szFileFlag[4];		//! 文件头
		WORD    wProvinceNum;		//! 省个数
		DWORD   dwProvinceOffset;	//! 省数据地址偏移
		WORD    wDJSNum;			//! 地级市个数
		DWORD   dwDJSOffset;		//! 地级市地址偏移
		WORD    wCountyNum;			//! 县级市个数
		DWORD   dwCountyOffset;		//! 县级市地址偏移
		LBSRECT	lbsrcBound;			//! 区域边界范围
		WORD	wColumns;			//! 列数
		WORD	wRows;				//! 行数
		DWORD   dwRgnMeshOffset;	//! 区域块数据偏移
	};

	//! 省数据对象
	struct _tag_ProvinceItem
	{
		WORD	wAdminCode;			//! 代码
		WORD	wNameLength;		//! 名称长度
		TCHAR*  pszNameText;		//! 名称指针
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

	//! 地级市数据对象
	struct _tag_DJSItem
	{
		DWORD	dwAdminCode;		//! 代码
		WORD	wProvinceIndex;		//! 省索引
		WORD	wNameLength;		//! 名称长度
		TCHAR*  pszNameText;		//! 名称指针
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

	//! 县级市数据对象
	struct _tag_CountyItem
	{
		DWORD	dwAdminCode;		//! 代码
		WORD	wProvinceIndex;		//! 省索引
		WORD	wDJSIndex;			//! 地级市索引
		WORD	wNameLength;		//! 名称长度
		TCHAR*  pszNameText;		//! 名称指针
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

	//! 区域数据对象
	struct _tag_RgnItem
	{
		LBSRECT lbsBound;		//! 区域边界范围
		WORD	wCountyIndex;	//! 县级市索引
		WORD	wPtCount;		//! 点个数
		POINT*	pPtArray;		//! 点数据
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

	//! 区域块数据对象
	struct _tag_RgnMeshItem
	{
		LBSRECT lbsBound;				//! 区域边界范围
		BYTE	row;					//! 行
		BYTE	col;					//! 列
		WORD	wRgnNum;				//! 区域个数
		DWORD*  pdwRgnOffset;			//! 区域偏移数组
		_tag_RgnItem* pRgnItemArray;	//! 区域数据数组

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

	//! 获取指定点所在县级市数据对象
	bool GetCountyItem(POINT* pPoint, _tag_CountyItem& countyItem);

	//! 获取指定点所在区域块对象索引
	bool GetRgnMeshItemIndex(POINT* pPoint, int& nIndex);

	//! 判断点是否在区域内
	bool PtInRegion(POINT* pPoint, _tag_RgnItem& rgnItem);

	//! 根据索引获取区域块对象
	bool GetRgnMeshItemById(int nIndex, _tag_RgnMeshItem& rgnMeshItem);

	//! 根据索引获取省对象
	bool GetProvinceItemById(int nIndex, _tag_ProvinceItem& provinceItem);

	//! 根据索引获取地级市对象
	bool GetDJSItemById(int nIndex, _tag_DJSItem& DJSItem);

	//! 根据索引获取县级市对象
	bool GetCountyItemById(int nIndex, _tag_CountyItem& countyItem);

private:
	//! 根据索引跳转文件到区域块数据
	bool SeekToRgnMeshItem(int nIndex);

	//! 根据索引跳转文件到省对象
	bool SeekToProvinceItem(int nIndex);

	//! 根据索引跳转文件到地级市对象
	bool SeekToDJSItem(int nIndex);

	//! 根据索引跳转文件到县级市对象
	bool SeekToCountyItem(int nIndex);

	//! 读区域块数据对象
	bool ReadRgnMeshItem(_tag_RgnMeshItem& rgnMeshItem);

	//! 读区域数据对象
	bool ReadRgnItem(_tag_RgnItem& rgnItem);

	//! 读省对象
	bool ReadProvinceItem(_tag_ProvinceItem& provinveItem);

	//! 读地级市数据对象
	bool ReadDJSItem(_tag_DJSItem& DJSItem);

	//! 读县级市对象
	bool ReadCountyItem(_tag_CountyItem& countyItem);

	//! 两点是否在线段同侧
	bool IsSameSide(POINT* l_start, POINT* l_end,
		POINT* p,POINT* q);

	//! 点是否在线上
	bool IsOnLine(POINT* l_start, POINT* l_end,POINT* p);

	//! 线段是否相交
	bool IsIntersect(POINT* s1_start, POINT* s1_end,
		POINT* s2_start, POINT* s2_end);

	//! 线段交点是否在线段节点上
	bool IsCrossNode(POINT* l_start, POINT* l_end,
		POINT* p,POINT* q);

	//! 是否初始化
	bool	m_bInit;

	//! 列间隔
	DWORD	m_dwColPer;

	//! 行间隔
	DWORD   m_dwRowPer;

	//! 左下角点
	POINT	m_LBPoint;

	//! 文件指针
	FILE*   m_pFile;

	//! 行政区域管理头信息对象
	_tag_adminRegionHeader m_adminRgnHeader;

//public:
	//BYTE->Any;
	bool ByteToWchar(void* Dst,BYTE* Src,int nSize,long &offs);

};
#pragma pack(pop)

#endif
