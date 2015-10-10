#pragma once

#include "resource.h"

BOOL IsInstalled();
int  InstallSer();
int  UninstallSer();
void LogEvent(LPCTSTR pszFormat, ...);
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);