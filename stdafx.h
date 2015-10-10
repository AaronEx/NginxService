#pragma once

#define _BIND_TO_CURRENT_VCLIBS_VERSION 1

#pragma warning(disable : 4996)
#pragma message("Disable Warning 4996, Security C")
#pragma warning(disable : 4267) //size_tµÄwarning
#pragma message("Disable Warning 4267, Something about the size_t cast")
#pragma warning( disable : 4018 )
#pragma message("Disable Warning 4018, signed/unsigned mismatch")
#pragma warning( disable : 4312 )
#pragma message("Disable Warning 4312, conversion from 'DWORD' to greater ")
#pragma warning( disable : 4311 )
#pragma message("Disable Warning 4311, 'type cast' : pointer truncation from 'LPVOID' to 'int' ")
#pragma warning( disable : 4800 )
#pragma message("Disable Warning 4800, 'LRESULT' : forcing value to bool 'true' or 'false' (performance warning)")

#define ASSERT(x) assert(x)

typedef unsigned char uint8;
typedef unsigned long uint32;
typedef unsigned short uint16;

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <assert.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <WTypes.h>
#include <WinCrypt.h>

#include <Winsock2.h>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")

#include <atlstr.h>

#include <map>
#include <list>
#include <string>
#include <vector>
#include <iosfwd>
#include <sstream>
#include <iostream>
using namespace std;