// The functions contained in this file are pretty dummy
// and are included only as a placeholder. Nevertheless,
// they *will* get included in the shared library if you
// don't remove them :)
//
// Obviously, you 'll have to write yourself the super-duper
// functions to include in the resulting library...
// Also, it's not necessary to write every function in this file.
// Feel free to add more files in this project. They will be
// included in the resulting library.
#include <locale.h>
#include <Python.h>
#include "json/json.h"
#include "AdminRegionMgr.h"
#include <iostream>
using namespace std;

CAdminRegionMgr g_AdminRegionMgr;


int  UnicodeToUTF8( wchar_t *unicodeStr, char * utfStr, int maxUtfStrlen )
{
    //将unicode编码转换为utf8编码
    if(NULL==setlocale(LC_ALL,"zh_CN.utf8"))//设置unicode转换后的码,当前为utf8
    {
        printf("Bad Parameter\n");
        return -1;
    }
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
    char *file = NULL;
    std::string sts;
    double dx = 0.0;
    double dy = 0.0;
    string strRes = "-1";
    do
    {
        if (!PyArg_ParseTuple(args, "sdd", &file, &dx, &dy ) )
        {
            printf("PyArg_ParseTuple error\n");
            break;
        }
        //cout << file << endl;
        //cout << dx << "-" << dy << endl;
        if ( !g_AdminRegionMgr.Initialize( file ) )
        {
            printf("AdminRegionMgr Initialize Failed \n");
            break;
        }

        POINT point;
        ADMINREGIONINFO adminRegionInfo;
        //double dx = 116.12345;
        //double dy = 39.23456;
        point.x = (int)(dx * 86400);
        point.y = (int)(dy * 86400);

        if ( !g_AdminRegionMgr.GetAdminRegionInfo( &point, &adminRegionInfo ) )
        {
            printf("GetAdminRegionInfo error\n");
            break;
        }

        char szProvinceName[255] = {0};
        char szDJSName[255] = {0};
        char szCountyName[255] = {0};
        int nProvNameLen = 254;
        int nDJSNameLen = 254;
        int nCountyNameLen = 254;
        if ( UnicodeToUTF8( adminRegionInfo.pProvinceName, szProvinceName, nProvNameLen ) == -1 )
        {
            printf("trans ProvinceName error\n");
            break;
        }
        if ( UnicodeToUTF8( adminRegionInfo.pDJSName, szDJSName, nDJSNameLen ) == -1 )
        {
            printf("trans DJSName error\n");
            break;
        }
        if ( UnicodeToUTF8( adminRegionInfo.pCountyName, szCountyName, nCountyNameLen ) == -1 )
        {
            printf("trans CountyName error\n");
            break;
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
        strRes = jsonWriter.write( root );
        jsonRes.clear();
        root.clear();
    }
    while (0);

    return Py_BuildValue("s#", strRes.c_str(), strRes.size() );
}

static PyMethodDef AllMyMethods[] =
{
    // 名称，函数，参数类型和文档字符串
    // 暴露给Python的函数
    {"GetAdminInfo", GetAdminInfo, METH_VARARGS, "get admin info"},
    // 一个列表结束的标记
    {NULL,NULL,0,NULL}        /* Sentinel */
};

extern "C" void initAdminJudge()
{
    PyObject *m, *d;
    m = Py_InitModule("AdminJudge", AllMyMethods); //初始化本模块，并暴露函数
    d = PyModule_GetDict(m);
}

int main()
{
	return 0;
}



