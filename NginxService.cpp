#include "stdafx.h"
#include "NginxService.h"

#pragma comment (lib, "Version.lib")

#include "Psapi.h"
#include <shellapi.h>
#define MAX_LOADSTRING 100

#pragma comment(lib,"Psapi.lib")

TCHAR g_szServiceName[] = _T("Nginx Service");
SERVICE_STATUS_HANDLE g_hServiceStatus;
SERVICE_STATUS g_Status;
HANDLE g_hCtlEventHandle = NULL;

void KillAllNginx();
void CreateNginxProcess();

void StartForTest()
{
	KillAllNginx();
	CreateNginxProcess();
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
	//TRY
	//{ 
	g_hCtlEventHandle = NULL;
	g_hServiceStatus = NULL;
	g_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_Status.dwCurrentState = SERVICE_STOPPED;
	g_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_Status.dwWin32ExitCode = 0;
	g_Status.dwServiceSpecificExitCode = 0;
	g_Status.dwCheckPoint = 0;
	g_Status.dwWaitHint = 0;


		//要注册的服务的信息
		SERVICE_TABLE_ENTRY st[] =
		{
			{ g_szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
			{ NULL, NULL }
		};

		//安装或反安装服务
		if (_tcscmp(lpCmdLine, _T("/install")) == 0)
		{
			int nResult = InstallSer();
			if (nResult != 0)
			{
				TCHAR pErrorInfo[1024] = { 0 };
				_stprintf(pErrorInfo,_T("Install Error, the Error Code:%d"),nResult);
				MessageBox(NULL,pErrorInfo,_T("Error When Install Server!"),MB_OK);
			}
		}
		else if (_tcscmp(lpCmdLine, _T("/uninstall")) == 0)
		{
			int nResult = UninstallSer();
			if (nResult != 0)
			{
				TCHAR pErrorInfo[1024] = { 0 };
				_stprintf(pErrorInfo,_T("Install Error, the Error Code:%d"),nResult);
				MessageBox(NULL,pErrorInfo,_T("Error When Install Server!"),MB_OK);
			}
		}
		else if (_tcscmp(lpCmdLine, _T("/test")) == 0)
		{
		/*	MessageBox(NULL,_T("Test"),_T("Error"),MB_OK);
			LOG("TEST");*/
			StartForTest();
		}
		else
		{
			if (!::StartServiceCtrlDispatcher(st))
			{
				LogEvent(_T("Register Service Main Function Error!"));
			}
		}
	//}
	//CATCH_ALL(CException e)
	//{

	//}
	//END_CATCH_ALL

	return 0;
	
}

DWORD FindProcess(TCHAR *strProcessName)
{
	DWORD aProcesses[1024], cbNeeded, cbMNeeded;
	HMODULE hMods[1024];
	HANDLE hProcess;
	TCHAR szProcessName[MAX_PATH] = { 0 };

	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )  return 0;
	for(int i=0; i< (int) (cbNeeded / sizeof(DWORD)); i++)
	{
		hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
		EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbMNeeded);
		GetModuleFileNameEx( hProcess, hMods[0], szProcessName,sizeof(szProcessName));

		if (aProcesses[i] == 8020)
		{
			int i = 0;
			i++;
		}

		CString strProcess = PathFindFileName(szProcessName);
		//CString strProcess(szProcessName);

		if(strProcess.CompareNoCase(strProcessName) == 0)
		{
			return(aProcesses[i]);
		}
	}


	return 0;
}

//
// GetDebugPriv
// 在 Windows NT/2000/XP 中可能因权限不够导致以上函数失败
// 如以　System 权限运行的系统进程，服务进程
// 用本函数取得　debug 权限即可,Winlogon.exe 都可以终止哦 :)
//
BOOL GetDebugPriv()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if ( ! OpenProcessToken( GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
	{
		return FALSE;
	}


	if ( ! LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ) )
	{
		CloseHandle( hToken );
		return FALSE;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) )
	{
		CloseHandle( hToken );
		return FALSE;
	}

	return TRUE;
}

//杀死所有的nginx进程
void KillAllNginx()
{
	//给自己提权
	//GetDebugPriv();

	int nGuard = 0;
	do
	{
		nGuard++;
		DWORD dwProcessId = FindProcess(L"nginx.exe");
		if(dwProcessId > 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,
						FALSE, dwProcessId);
			if (hProcess)
			{
				if(TerminateProcess(hProcess, 0) == 0)
				{
				}
			}
		}
		else
		{
			break;
		}

		if(nGuard >= 100)
		{
			break;
		}
	}while(true);
}

//创建nginx进程
void CreateNginxProcess()
{
	//取得当前Exe的路径地址
	TCHAR szFilePath[512];
	memset(szFilePath, 0, 512 * sizeof(TCHAR));
	GetModuleFileName(NULL, szFilePath, sizeof(szFilePath)/sizeof(TCHAR));
	PathRemoveFileSpec(szFilePath);
	CString strCurrentDir = szFilePath;
	PathAppend(szFilePath,L"nginx.exe");

	

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};

	si.cb= sizeof(si);
	GetStartupInfo( &si );
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;

	if(CreateProcess( szFilePath, L"", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW, NULL, (LPCTSTR)strCurrentDir, &si, &pi )){
		LogEvent(L"Nginx 启动成功");
	} else {
		LogEvent(L"Nginx 启动失败");
	}




	//SHELLEXECUTEINFO si = { 0 };
	//si.cbSize = sizeof(si);
	//si.lpFile = szFilePath;
	//si.lpParameters = L"";
	//si.nShow = SW_SHOW;
	//si.fMask = SEE_MASK_FLAG_NO_UI;
	//ShellExecuteEx(&si);
}

void WINAPI ServiceMain()
{
	g_Status.dwCurrentState = SERVICE_START_PENDING;
	g_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;//表明接受停止命令,否则将不能控制其停止

	//注册服务控制
	g_hServiceStatus = RegisterServiceCtrlHandler(g_szServiceName, ServiceStrl);
	if (g_hServiceStatus == NULL)
	{
		LogEvent(_T("Handler not installed"));
		return;
	}

	SetServiceStatus(g_hServiceStatus, &g_Status);

	g_hCtlEventHandle = CreateEvent(
		NULL,    // default security attributes
		TRUE,    // manual reset event
		FALSE,   // not signaled
		NULL);   // no name

	if ( g_hCtlEventHandle == NULL)
	{
		g_Status.dwWin32ExitCode = E_FAIL;
		g_Status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(g_hServiceStatus, &g_Status);

		return;
	}

	//启动Nginx进程	
	KillAllNginx();
	CreateNginxProcess();

	g_Status.dwWin32ExitCode = S_OK;
	g_Status.dwCheckPoint = 0;
	g_Status.dwWaitHint = 0;
	g_Status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(g_hServiceStatus, &g_Status);

	//等待用户停止
	while (1)
	{
		WaitForSingleObject(g_hCtlEventHandle,INFINITE);

		//不管是PAUSE还是什么状态，直接退出,不用再做判断
		break;
	}

	//杀死所有的nigix进程
	KillAllNginx();

	SetServiceStatus(g_hServiceStatus, &g_Status); 

	g_Status.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(g_hServiceStatus, &g_Status);
}

//安装
int InstallSer()
{
	int nError = 0;

	//取得当前Exe的路径地址
	TCHAR szFilePath[512];
	memset(szFilePath, 0, 512 * sizeof(TCHAR));
	GetModuleFileName(NULL, szFilePath, sizeof(szFilePath)/sizeof(TCHAR));

	//打开服务控制管理器
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if ( hSCM )
	{
		//创建服务
		SC_HANDLE hService = ::CreateService(
			hSCM, g_szServiceName, g_szServiceName,
			SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
			szFilePath, NULL, NULL, _T(""), NULL, NULL);

		if (hService)
		{
			::CloseServiceHandle(hService);
		}
		else
		{
			nError = GetLastError();
		}
		
		::CloseServiceHandle(hSCM);
	}
	else
	{
		nError = GetLastError();
	}
	return nError;
}

int UninstallSer()
{
	int nErrorNo = 0;
	//打开服务控制管理器
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM)
	{
		//打开服务
		SC_HANDLE hService = OpenService(hSCM, g_szServiceName, SERVICE_STOP | DELETE);
		if (hService)
		{
			//停止服务
			if (ControlService(hService, SERVICE_CONTROL_STOP, &g_Status))
			{
				if ( DeleteService(hService) )
				{
					nErrorNo = 0;
				}
				else
				{
					nErrorNo = GetLastError();
				}
			}
			else
			{
				nErrorNo = GetLastError();
			}
		}
	}
	else
	{
		nErrorNo = GetLastError();
	}
	return nErrorNo;
}



//*********************************************************
void LogEvent(LPCTSTR pFormat, ...)
{
	TCHAR    chMsg[256];
	HANDLE  hEventSource;
	LPTSTR  lpszStrings[1];
	va_list pArg;

	va_start(pArg, pFormat);
	_vstprintf(chMsg, pFormat, pArg);
	va_end(pArg);

	lpszStrings[0] = chMsg;

	hEventSource = RegisterEventSource(NULL, g_szServiceName);
	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
		DeregisterEventSource(hEventSource);
	}
}


//服务控制主函数，这里实现对服务的控制，
//当在服务管理器上停止或其它操作时，将会运行此处代码
//*********************************************************
void WINAPI ServiceStrl(DWORD dwOpcode)
{
	switch (dwOpcode)
	{
	case SERVICE_CONTROL_STOP:
		{
			KillAllNginx();
			g_Status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(g_hServiceStatus, &g_Status);
			if (g_hCtlEventHandle)
			{
				::SetEvent(g_hCtlEventHandle);
			}
		}
		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	default:
		LogEvent(_T("Bad service request"));
	}
}