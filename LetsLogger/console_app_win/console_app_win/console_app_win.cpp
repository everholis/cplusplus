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

#include <iostream>
#include "LetsLogger.h"

#if defined(_WIN32) 
#pragma warning( disable : 4996) // for _sleep C4996 warning
#endif



int main()
{
    printf("Hello World!\n");


#if defined(_WIN32) 
    LETSLOG_INIT(L".\\test.log");
    LETSLOG_ERRO(L"Im Error Log, n:(%d) s:(%s), b:(%d)", 123, L"logdemo", true);
    LETSLOG_WARN(L"Im Warning Log, n:(%d) s:(%s), b:(%d)", 123, L"logdemo", true);
    LETSLOG_INFO(L"Im Info Log int:(%d), str:(%s)", 123, L"logdemo");
    LETSLOG_TRAP(1 == 0);

    {
        LETSLOG_BBL(L"Im Bubble Log");
    }

#else

    LETSLOG_INIT("./test.log");
    LETSLOG_ERRO("Im Error Log, n:(%d) s:(%s), b:(%d)", 123, "logdemo", true);
    LETSLOG_WARN("Im Warning Log, n:(%d) s:(%s), b:(%d)", 123, "logdemo", true);
    LETSLOG_INFO("Im Info Log int:(%d), str:(%s)", 123, "logdemo");
    LETSLOG_TRAP(1 == 0);
    {
        LETSLOG_BBL("Im Bubble Log");
    }
#endif

    return 0;
}