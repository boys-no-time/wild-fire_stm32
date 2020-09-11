/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved.
You are not allowed to copy or distribute the code without permission.
*********************************************************************/

#include "inodes_include.h"

const char who[] = COMPILE_BY ;
const char where[] = COMPILE_AT ;
const char compiler[] = COMPILER ;
const char build_date[] = __DATE__;  // e.g. 'Dec 15 2009'
const char build_time[] = __TIME__;  // e.g. '15:25:56'
const char git_ver[] = GIT_VER ;
const char git_info[] = GIT_INFO ;
const char version_chk[] = VERSION_CHECK ;

void inodes_print_ver(PRINTFUNC pf)
{
    pf("Build version: %s %s \r\n", git_ver, git_info);
    pf("Build by %s@%s, %s %s \r\n", who, where, build_date, build_time);
    pf("Compiler: %s\r\n", compiler);
}

