/*
    Copyright 2020 everholis

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http ://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissionsand
    limitations under the License.
*/

#ifndef __LETSLOGGER_H__
#define __LETSLOGGER_H__

#include <iostream>
#include <fstream>
#include <vector>

#include <time.h>
#include <string>
#include <sstream>
#include <iomanip>


#if defined(_WIN32) 
#include <Windows.h>
#include <atlstr.h>
#else
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> 
#include <time.h>

#endif


#define MAX_BUFFER_SIZE     1024


enum class LogType
{
    Error   = 0x1,
	Info    = 0x2,
	Warn    = 0x4,
    Debug   = 0x8,
    Bubble  = 0x10,
    Trap    = 0x40,

};

enum class LogOption
{
    DateTime = 0x1,
    ProcessId = 0x2,
    ThreadId = 0x4,
    LogLevel = 0x8,
    Filename = 0x10,
    LineNumber = 0x40,
    Function = 0x80
};


class CLetsLogger
{

/// 
///
///
public:

    ~CLetsLogger()
    {
        if (true == m_objOutFileStream.is_open()) {
            m_objOutFileStream.close();
        }
        
    }

    static CLetsLogger& GetInstance()
    {
        static CLetsLogger s;
        return s;
    }

    void InitW(std::wstring strLogFileNameW)
    {
        
#if defined(_WIN32) 
        USES_CONVERSION;
        m_strLogFileName = CW2A(strLogFileNameW.c_str(), CP_UTF8);
#else
        m_strLogFileName.assign(strLogFileNameW.begin(), strLogFileNameW.end());
#endif
        
    }

    void InitA(std::string strLogFileNameA)
    {

#if defined(_WIN32) 
        USES_CONVERSION;
        m_strLogFileName = CW2A(CA2W(strLogFileNameA.c_str()), CP_UTF8);
#else
        m_strLogFileName = strLogFileNameA;
#endif
        
    }

    void LogA(LogType eLevel, const char* szPrefix, const char* szFile, int nLine, const char* szFunc, const char* fmt, ...)
    {
        if (NULL == szFile || NULL == szFunc)
        {
            return;
        }

        char szBuffer[MAX_BUFFER_SIZE + 2] = { '\0', };

        va_list args;
        va_start(args, fmt);
        vsnprintf(szBuffer, MAX_BUFFER_SIZE, fmt, args);
        va_end(args);

        std::string strPrefixA = "";
        std::string strFileA = "";
        std::string strLineA = "";
        std::string strFuncA = "";
        std::string strLogA = "";

#if defined(_WIN32) 
        USES_CONVERSION;
        strPrefixA = CW2A(CA2W(szPrefix), CP_UTF8);
        strFileA = CW2A(CA2W(szFile), CP_UTF8);
        strLineA = std::to_string(nLine);
        strFuncA = CW2A(CA2W(szFunc), CP_UTF8);
        strLogA = CW2A(CA2W(szBuffer), CP_UTF8);
#else
        strPrefixA = szPrefix;
        strFileA = szFile;
        strLineA = std::to_string(nLine);
        strFuncA = szFunc;
        strLogA = szBuffer;
#endif

        LogWriter(eLevel, strPrefixA, strFileA, strLineA, strFuncA, strLogA);
        
    }

    void LogW(LogType eLevel, const wchar_t* szPrefix, const wchar_t* szFile, int nLine, const wchar_t* szFunc, const wchar_t* fmt, ...)
    {
        if (NULL == szFile || NULL == szFunc)
        {
            return;
        }

        wchar_t szBuffer[MAX_BUFFER_SIZE + 2] = { L'\0', };

        va_list args;
        va_start(args, fmt);
        vswprintf(szBuffer, MAX_BUFFER_SIZE, fmt, args);
        va_end(args);

        std::wstring    strPrefixW = L"";
        std::string     strPrefixA = "";
        std::wstring    strFileW = L"";
        std::string     strFileA = "";
        std::string     strLineA = "";
        std::wstring    strFuncW = L"";
        std::string     strFuncA = "";
        std::wstring    strLogW = L"";
        std::string     strLogA = "";


#if defined(_WIN32)
        USES_CONVERSION;

        strPrefixA = CW2A(szPrefix, CP_UTF8);
        strFileA = CW2A(szFile, CP_UTF8);
        strLineA = std::to_string(nLine);
        strFuncA = CW2A(szFunc, CP_UTF8);
        strLogA = CW2A(szBuffer, CP_UTF8);

#else
        strPrefixW = szPrefix;
        strFileW = szFile;
        strFuncW = L"";
        strLogW = szBuffer;

        strPrefixA.assign(strPrefixW.begin(), strPrefixW.end());
        strFileA.assign(strFileW.begin(), strFileW.end());
        strLineA = std::to_string(nLine);
        strFuncA.assign(strFuncW.begin(), strFuncW.end());
        strLogA.assign(strLogW.begin(), strLogW.end());

#endif

        LogWriter(eLevel, strPrefixA, strFileA, strLineA, strFuncA, strLogA);
    }

    ///
    ///
    ///
private:

    void LogWriter(LogType level, std::string strPrefix, std::string strFile, std::string strLine, std::string strFunc, std::string strLogInput)
    {
 
        std::string LogInput = "";

        std::string strLogOutput = AddLogHeader(level, strPrefix, strFile, strLine, strFunc, strLogInput);
        WriteFile(strLogOutput);
    }

    std::string AddLogHeader(LogType level, std::string strPrefix, std::string strFile, std::string strLine, std::string strFunc, std::string strLogInput)
    {

        std::string strLog = "";

        //////////////////////////////////////////////////
        if (m_loggableItem & static_cast<int>(LogOption::DateTime))
        {
            strLog += GetCurrentDateTime();
            strLog += " ";
        }

        //////////////////////////////////////////////////
        if (m_loggableItem & static_cast<int>(LogOption::ProcessId))
        {
            char  szPid[10] = "";
#if defined(_WIN32) 
            //strLog += std::to_string(GetCurrentProcessId());
            sprintf_s(szPid, sizeof(szPid), "%5d", GetCurrentProcessId());
            strLog += szPid;
            strLog += " ";
#else
            //strLog += std::to_string(getpid());
            sprintf(szPid, "%5d", getpid());
            strLog += szPid;
            strLog += " ";
#endif
        }

        //////////////////////////////////////////////////
        if (m_loggableItem & static_cast<int>(LogOption::ThreadId))
        {

            char  szTid[10] = "";
#if defined(_WIN32) 

            //strLog += std::to_string(GetCurrentThreadId());
            sprintf_s(szTid, sizeof(szTid), "%5d", GetCurrentThreadId());
            strLog += szTid;
            strLog += " ";
#else
            //strLog += std::to_string(pthread_self());
            sprintf(szTid, "%5d", pthread_self());
            strLog += szTid;
            strLog += " ";
#endif

        }

        //////////////////////////////////////////////////
        if (m_loggableItem & static_cast<int>(LogOption::LogLevel))
        {
            std::string strLevel = "";

            switch (level)
            {
            case LogType::Error:
                strLevel = "[E]"; //error
                break;

            case LogType::Warn:
                strLevel = "[W]"; //warning
                break;

            case LogType::Info:
                strLevel = "[I]"; //info
                break;

            case LogType::Debug:
                strLevel = "[D]"; //debug
                break;

            case LogType::Bubble:
                strLevel = "[B]"; //bubble
                break;

            case LogType::Trap:
                strLevel = "[T]"; //trap
                break;
            }

            strLog += strLevel;
            strLog += " ";

        }

        //////////////////////////////////////////////////
        strLog += strPrefix;
        strLog += " ";

        strLog += strLogInput;
        strLog += "   __________";


        //////////////////////////////////////////////////
        if (m_loggableItem & static_cast<int>(LogOption::Filename))
        {
            strLog += strFile;
            strLog += " ";
        }

        //////////////////////////////////////////////////
        if (m_loggableItem & static_cast<int>(LogOption::LineNumber))
        {
            strLog += strLine;
            strLog += " ";
        }

        //////////////////////////////////////////////////
        if (m_loggableItem & static_cast<int>(LogOption::Function))
        {
            strLog += strFunc;
            strLog += " ";
        }

        return strLog;
    }

    void WriteFile(std::string strText)
    {

        if (true == m_strLogFileName.empty()) {
            return;
        }

        if (false == m_objOutFileStream.is_open()) {
            m_objOutFileStream.open(m_strLogFileName, std::ios::app);
        }

        m_objOutFileStream << strText << std::endl;

    }

    std::string GetCurrentDateTime()
    {
        time_t     now = time(NULL);
        char       buf[80] = { 0 };
        

#if defined(_WIN32) 
        struct tm  stTm = {};
        localtime_s(&stTm, &now);
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &stTm);
#else
        struct tm* pTm = localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", pTm);
#endif
        

        return buf;
    }
   




///
///
///
private:
    //
    CLetsLogger() 
    {
        m_loggableItem = static_cast<int>(LogOption::DateTime) | static_cast<int>(LogOption::ProcessId) |
            static_cast<int>(LogOption::ThreadId) | static_cast<int>(LogOption::LogLevel) |
            static_cast<int>(LogOption::Filename) | static_cast<int>(LogOption::LineNumber) |
            static_cast<int>(LogOption::Function);
    };
    CLetsLogger(const CLetsLogger& other);

    
    //
    int             m_loggableItem;
    std::ofstream   m_objOutFileStream;
    std::string		m_strLogFileName;

};


#define LETSLOG                      CLetsLogger::GetInstance()

#define LETSLOG_W_INIT(x)            LETSLOG.InitW(x);
#define LETSLOG_W_ERRO(fmt, ...)     LETSLOG.LogW(LogType::Error, L" ", __FILEW__, __LINE__, __FUNCTIONW__, fmt, ##__VA_ARGS__);
#define LETSLOG_W_WARN(fmt, ...)     LETSLOG.LogW(LogType::Warn, L" ", __FILEW__, __LINE__, __FUNCTIONW__, fmt, ##__VA_ARGS__);
#define LETSLOG_W_INFO(fmt, ...)     LETSLOG.LogW(LogType::Info, L" ", __FILEW__, __LINE__, __FUNCTIONW__, fmt, ##__VA_ARGS__);
#define LETSLOG_W_TRAP(x)            if(!x) LETSLOG.LogW(LogType::Trap, L" ", __FILEW__, __LINE__, __FUNCTIONW__, L"%s", L#x);
#define LETSLOG_W_BBL(x)             CHBubbleLoggerW __FILEW__##__LINE__##__FUNCTIONW__##__LINE__(__FILEW__, __LINE__, __FUNCTIONW__, L#x)
//#define LETSLOG_W_BBL_IN(u,v,w,x)    LETSLOG.LogW(LogType::Bubble, L">", u, v, w, L"%s", L#x);
#define LETSLOG_W_BBL_IN(u,v,w,x)    LETSLOG.LogW(LogType::Bubble, L">", u, v, w, L"%s", x);
//#define LETSLOG_W_BBL_OUT(u,v,w,x)   LETSLOG.LogW(LogType::Bubble, L"<", u, v, w, L"%s", L#x);
#define LETSLOG_W_BBL_OUT(u,v,w,x)   LETSLOG.LogW(LogType::Bubble, L"<", u, v, w, L"%s", x);

#define LETSLOG_A_INIT(x)            LETSLOG.InitA(x);
#define LETSLOG_A_ERRO(fmt, ...)     LETSLOG.LogA(LogType::Error, " ", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LETSLOG_A_WARN(fmt, ...)     LETSLOG.LogA(LogType::Warn, " ", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LETSLOG_A_INFO(fmt, ...)     LETSLOG.LogA(LogType::Info, " ", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define LETSLOG_A_TRAP(x)            if(!x) LETSLOG.LogA(LogType::Trap, " ", __FILE__, __LINE__, __FUNCTION__, "%s", x);
#define LETSLOG_A_BBL(x)             CHBubbleLoggerA __FILE__##__LINE__##__FUNCTION__(__FILE__, __LINE__, __FUNCTION__,#x)
#define LETSLOG_A_BBL_IN(u,v,w,x)    LETSLOG.LogA(LogType::Bubble, ">", u, v, w, "%s", x);
#define LETSLOG_A_BBL_OUT(u,v,w,x)   LETSLOG.LogA(LogType::Bubble, "<", u, v, w, "%s", x);


#if defined(_WIN32) && (defined(_UNICODE) || defined(UNICODE))
#define LETSLOG_INIT		LETSLOG_W_INIT
#define LETSLOG_ERRO     	LETSLOG_W_ERRO
#define LETSLOG_WARN     	LETSLOG_W_WARN
#define LETSLOG_INFO     	LETSLOG_W_INFO
#define LETSLOG_TRAP     	LETSLOG_W_TRAP
#define LETSLOG_BBL      	LETSLOG_W_BBL
#define LETSLOG_BBL_IN   	LETSLOG_W_BBL_IN
#define LETSLOG_BBL_OUT  	LETSLOG_W_BBL_OUT
#else
#define LETSLOG_INIT		LETSLOG_A_INIT
#define LETSLOG_ERRO     	LETSLOG_A_ERRO
#define LETSLOG_WARN     	LETSLOG_A_WARN
#define LETSLOG_INFO     	LETSLOG_A_INFO
#define LETSLOG_TRAP     	LETSLOG_A_TRAP
#define LETSLOG_BBL      	LETSLOG_A_BBL
#define LETSLOG_BBL_IN   	LETSLOG_A_BBL_IN
#define LETSLOG_BBL_OUT  	LETSLOG_A_BBL_OUT
#endif




class CHBubbleLoggerW
{
public:
    CHBubbleLoggerW(std::wstring strFile, int nLine, std::wstring strFunc, std::wstring strLog) {
        m_strFile = strFile;
        m_nLine = nLine;
        m_strFunc = strFunc;
        m_strLog = strLog;
        LETSLOG_W_BBL_IN(m_strFile.c_str(), m_nLine, m_strFunc.c_str(), m_strLog.c_str());
    }

    ~CHBubbleLoggerW() {
        LETSLOG_W_BBL_OUT(m_strFile.c_str(), m_nLine, m_strFunc.c_str(), m_strLog.c_str());
    }

private:
    std::wstring m_strFile;
    int m_nLine;
    std::wstring m_strFunc;
    std::wstring m_strLog;
};

class CHBubbleLoggerA
{
public:
    CHBubbleLoggerA(std::string strFile, int nLine, std::string strFunc, std::string strLog) {
        m_strFile = strFile;
        m_nLine = nLine;
        m_strFunc = strFunc;
        m_strLog = strLog;
        LETSLOG_A_BBL_IN(m_strFile.c_str(), m_nLine, m_strFunc.c_str(), m_strLog.c_str());
    }

    ~CHBubbleLoggerA() {
        LETSLOG_A_BBL_OUT(m_strFile.c_str(), m_nLine, m_strFunc.c_str(), m_strLog.c_str());
    }

private:
    std::string m_strFile;
    int m_nLine;
    std::string m_strFunc;
    std::string m_strLog;
};


#endif //__HLOGGER_H__