#define _WIN64

#define __int64 long long

typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef float FLOAT;
typedef FLOAT *PFLOAT;
typedef BOOL *PBOOL;
typedef BOOL *LPBOOL;
typedef BYTE *PBYTE;
typedef BYTE *LPBYTE;
typedef int *PINT;
typedef int *LPINT;
typedef WORD *PWORD;
typedef WORD *LPWORD;
typedef long *LPLONG;
typedef DWORD *PDWORD;
typedef DWORD *LPDWORD;
typedef void *LPVOID;
typedef const void *LPCVOID;

typedef int INT;
typedef unsigned int UINT;
typedef unsigned int *PUINT;
#include <Windows.h>
#include <winternl.h>

#define MAIN_CLASS_NAME "LogWin"

typedef FARPROC WINAPI (*_LxGetProcAddressT)(
    _In_ HMODULE hModule,
    _In_ LPCSTR lpProcName);
typedef int WINAPI (*_LxMessageBoxAT)(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType);
typedef HWND WINAPI (*_LxCreateWindowExAT)(
    _In_ DWORD dwExStyle,
    _In_opt_ LPCSTR lpClassName,
    _In_opt_ LPCSTR lpWindowName,
    _In_ DWORD dwStyle,
    _In_ int X,
    _In_ int Y,
    _In_ int nWidth,
    _In_ int nHeight,
    _In_opt_ HWND hWndParent,
    _In_opt_ HMENU hMenu,
    _In_opt_ HINSTANCE hInstance,
    _In_opt_ LPVOID lpParam);
typedef BOOL WINAPI (*_LxShowWindowT)(
    _In_ HWND hWnd,
    _In_ int nCmdShow);
typedef BOOL WINAPI (*_LxUpdateWindowT)(_In_ HWND hWnd);
typedef BOOL WINAPI (*_LxGetMessageAT)(
    _Out_ LPMSG lpMsg,
    _In_opt_ HWND hWnd,
    _In_ UINT wMsgFilterMin,
    _In_ UINT wMsgFilterMax);
typedef BOOL WINAPI (*_LxTranslateMessageT)(_In_ CONST MSG *lpMsg);
typedef LRESULT WINAPI (*_LxDispatchMessageAT)(_In_ CONST MSG *lpMsg);
typedef ATOM WINAPI (*_LxRegisterClassExAT)(_In_ CONST WNDCLASSEXA *);
typedef HMODULE WINAPI (*_LxGetModuleHandleAT)(_In_opt_ LPCSTR lpModuleName);
typedef LRESULT CALLBACK (*_LxDefWindowProcAT)(
    _In_ HWND hWnd,
    _In_ UINT Msg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam);
typedef HMODULE WINAPI (*_LxLoadLibraryAT)(_In_ LPCSTR lpLibFileName);
typedef VOID WINAPI (*_LxPostQuitMessageT)(_In_ int nExitCode);
typedef UINT WINAPI (*_LxGetDlgItemTextAT)(
    _In_ HWND hDlg,
    _In_ int nIDDlgItem,
    _Out_writes_(cchMax) LPSTR lpString,
    _In_ int cchMax);
typedef VOID WINAPI (*_LxExitProcessT)(_In_ UINT uExitCode);
typedef BOOL WINAPI (*_LxDestroyWindowT)(_In_ HWND hWnd);
typedef BOOL WINAPI (*_LxUnregisterClassAT)(
    _In_ LPCSTR lpClassName,
    _In_opt_ HINSTANCE hInstance);


typedef struct _ImageUnlocalLogWinHeader
{
    BYTE Reserved1[5];
    BYTE JmpBack[5];
    BYTE Resvered[2];
    CHAR SuccessTip[64];
    CHAR SuccessTitle[64];
    CHAR FalidTitle[64];
    CHAR FalidTip[64];
    CHAR MainTitle[16];
    CHAR LogBtn[16];
    CHAR CancelBtn[16];
    CHAR AccLab[16];
    CHAR PwdLab[16];
    CHAR AccAnswer[64];
    CHAR PwdAnswer[64];
    DWORD MainWinX;
    DWORD MainWinY;
}ImageUnlocalLogWinHeader, *PImageUnlocalLogWinHeader;

#define IDC_LOG_BTN 1
#define IDC_CANCEL_BTN 2
#define IDC_LABEL1 3
#define IDC_LABEL2 4
#define IDC_ACC_EDIT 5
#define IDC_PWD_EDIT 6

#define GLOBAL_VAR __attribute__((section(".rdata")))

GLOBAL_VAR ImageUnlocalLogWinHeader g_header;
GLOBAL_VAR HMODULE g_kernel32;
GLOBAL_VAR HMODULE g_user32;
GLOBAL_VAR HINSTANCE g_hInstance;
GLOBAL_VAR HWND g_hWnd;

GLOBAL_VAR _LxGetProcAddressT _GetProcAddress;
GLOBAL_VAR _LxMessageBoxAT _MessageBoxA;
GLOBAL_VAR _LxCreateWindowExAT _CreateWindowExA;
GLOBAL_VAR _LxShowWindowT _ShowWindow;
GLOBAL_VAR _LxUpdateWindowT _UpdateWindow;
GLOBAL_VAR _LxGetMessageAT _GetMessageA;
GLOBAL_VAR _LxTranslateMessageT _TranslateMessage;
GLOBAL_VAR _LxDispatchMessageAT _DispatchMessageA;
GLOBAL_VAR _LxRegisterClassExAT _RegisterClassExA;
GLOBAL_VAR _LxGetModuleHandleAT _GetModuleHandleA;
GLOBAL_VAR _LxDefWindowProcAT _DefWindowProcA;
GLOBAL_VAR _LxLoadLibraryAT _LoadLibraryA;
GLOBAL_VAR _LxPostQuitMessageT _PostQuitMessage;
GLOBAL_VAR _LxGetDlgItemTextAT _GetDlgItemTextA;
GLOBAL_VAR _LxExitProcessT _ExitProcess;
GLOBAL_VAR _LxDestroyWindowT _DestroyWindow;
GLOBAL_VAR _LxUnregisterClassAT _UnregisterClassA;


void GetKernelBase();
__int64 GetApi(PCSTR hModule, PCSTR lpszApi);
BOOL EqualW(PCWSTR a, PCWSTR b, __int64 size);
BOOL EqualA(PCSTR a, PCSTR b, __int64 size);
BOOL EqualWI(PCWSTR a, PCWSTR b, __int64 size);
BOOL EqualAI(PCSTR a, PCSTR b, __int64 size);
WCHAR ToUpChar(WCHAR c);
__int64 StrLenA(PCSTR a);
inline WCHAR ToUpChar(WCHAR c);
void GetFuncs();
BOOL _WinMain();
LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateLable(LPCSTR lpWindowName, int x, int y, int nWidth, int nHeight, int id);
HWND CreateButton(LPCSTR lpWindowName, int x, int y, int nWidth, int nHeight, int id);
HWND CreateEdit(int x, int y, int nWidth, int nHeight, int id);
HWND CreatePwdEdit(int x, int y, int nWidth, int nHeight, int id);