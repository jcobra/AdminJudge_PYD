// AdminJudge.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
#include <locale.h>
#include <tchar.h>
#include "AdminJudge.h"
#include <Python.h>
#include "globalfun.h"
#include "json.h"
#include "AdminRegionMgr.h"


CAdminRegionMgr g_AdminRegionMgr;



int UnicodeToUTF8( wchar_t *unicodeStr, char * utfStr, int maxUtfStrlen )
{
    //char* locname = setlocale(LC_ALL,".65001");
    //printf("Current locale2 is: %s\n", locname);


    //将unicode编码转换为utf8编码
    /*if(NULL==setlocale(LC_ALL,"Chinese (Simplified)_People's Republic of China.65001"))//设置unicode转换后的码,当前为utf8
    {
        printf("Bad Parameter\n");
        return -1;
    }*/
    int utfLen=wcstombs(NULL,unicodeStr,0);//计算转换后的长度
    if(utfLen<=0)
    {
        printf("Can not Transfer!!!\n");
        return -1;
    }
    else if(utfLen>=maxUtfStrlen)//判断空间是否足够
    {
        printf("Dst Str memory not enough\n");
        return -1;
    }
    wcstombs(utfStr,unicodeStr,utfLen);
    utfStr[utfLen]=0;//添加结束符
    return utfLen;
}


static PyObject* GetAdminInfo( PyObject *self, PyObject *args )
{
    char* locname = setlocale(LC_ALL, NULL);
    printf("Current locale is: %s\n", locname);

    locname = setlocale(LC_ALL,"CHS");
    printf("Current locale is: %s\n", locname);

    char *file = NULL;
    char url2[100] = {0};
    int len;
    std::string sts;
    double dx = 0.0;
    double dy = 0.0;
    if (!PyArg_ParseTuple(args, "sdd", &file, &dx, &dy ) )
    {
        printf("error\n");
        return NULL;
    }
    cout << file << endl;
    cout << dx << "-" << dy << endl;

    /*memcpy(url2,url,len);
    url2[len] = 0;
    //sts = Recognise_Img(url);
    cout << sts << endl;
    memcpy( url2, sts.c_str(), sts.size() );
    //return Py_BuildValue("s#", url2, sts.size() );
    */

    if ( !g_AdminRegionMgr.Initialize( file ) )
    {
        printf("error1\n");
        return NULL;
    }

    POINT point;
    ADMINREGIONINFO adminRegionInfo;

    //double dx = 116.12345;
    //double dy = 39.23456;

    point.x = (long)(dx * 86400);
    point.y = (long)(dy * 86400);

    if ( !g_AdminRegionMgr.GetAdminRegionInfo( &point, &adminRegionInfo ) )
    {
        printf("error2\n");
        return NULL;
    }

    TCHAR szInfo[255] = {0};
    _stprintf( szInfo, _T("%s-%s-%s\n"), adminRegionInfo.pProvinceName, adminRegionInfo.pDJSName, 
        adminRegionInfo.pCountyName );
    //_tprintf( szInfo );
    //_tprintf( _T("------") );

    char szProvinceName[255] = {0}; //{"北京市"};
    char szDJSName[255] = {0}; //{"海淀区"};
    char szCountyName[255] = {0}; //{"海淀区"};
    int nProvNameLen = 254;
    int nDJSNameLen = 254;
    int nCountyNameLen = 254;
    if ( UnicodeToUTF8( adminRegionInfo.pProvinceName, szProvinceName, 254 ) == -1 )
    {
        printf("error3\n");
        return NULL;
    }
    if ( UnicodeToUTF8( adminRegionInfo.pDJSName, szDJSName, 254 ) == -1 )
    {
        printf("error4\n");
        return NULL;
    }
    if ( UnicodeToUTF8( adminRegionInfo.pCountyName, szCountyName, 254 ) == -1 )
    {
        printf("error5\n");
        return NULL;
    }

    Json::Value root;
    Json::Value jsonRes;
    jsonRes["prov_code"] = (int)adminRegionInfo.wProvinceCode;
    jsonRes["prov_desc"] = Json::Value( szProvinceName );
    jsonRes["djs_code"] = (int)adminRegionInfo.dwDJSCode;
    jsonRes["djs_desc"] = Json::Value( szDJSName );
    jsonRes["county_code"] = (int)adminRegionInfo.dwCountyCode;
    jsonRes["county_desc"] = Json::Value( szCountyName );

    // 按字符串处理的，应该处理为对象
    root["status"] = 0;
    root["res"] = jsonRes;

    Json::FastWriter jsonWriter;
    string strRes = jsonWriter.write( root );

    /*
    char* lpszRetBuf = NULL;
    int nlen = strRes.length();
    lpszRetBuf = new char[nlen+1];
    memset( lpszRetBuf, 0, (nlen+1) * sizeof(char) );
    strcpy( lpszRetBuf, (char*)strRes.c_str() );
    */

    jsonRes.clear();
    root.clear();

    return Py_BuildValue("s#", strRes.c_str(), strRes.size() );
}

/*
std::string Recognise_Img(const std::string url)
{
    //返回结果
    string str = "从dll中返回的数据... : " +url;
    return str;
}

// 返回字符串
static PyObject* Recognise(PyObject *self, PyObject *args)
{
    const char *url;
    char url2[100] = {0};
    int len;
    std::string sts;
    if (!PyArg_ParseTuple(args, "s#", &url,&len))
    {
        printf("error\n");
        return NULL;
    }
    cout << url << endl;
    memcpy(url2,url,len);
    url2[len] = 0;
    sts = Recognise_Img(url);
    cout << sts << endl;
    memcpy( url2, sts.c_str(), sts.size() );
    return Py_BuildValue("s#", url2, sts.size() );
}

// 返回整型
static PyObject* RecogniseInt(PyObject *self, PyObject *args)
{
    int len;
    if (!PyArg_ParseTuple(args, "i", &len))
    {
        printf("error\n");
        return NULL;
    }
    int sum = len + 100;
    return Py_BuildValue("i", sum );
}
*/

static PyMethodDef AllMyMethods[] = 
{
    // 名称，函数，参数类型和文档字符串
    // 暴露给Python的函数
    {"GetAdminInfo", GetAdminInfo, METH_VARARGS, "get admin info"},
    // 一个列表结束的标记
    {NULL,NULL,0,NULL}        /* Sentinel */
};

extern "C" ADMINJUDGE_API void initAdminJudge()
{
    PyObject *m, *d;
    m = Py_InitModule("AdminJudge", AllMyMethods); //初始化本模块，并暴露函数
    d = PyModule_GetDict(m);
}

