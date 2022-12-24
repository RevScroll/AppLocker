#include "_defines.h"

void _start()
{
    GetKernelBase();
    _GetProcAddress = (_LxGetProcAddressT)GetApi((LPCSTR)g_kernel32, "GetProcAddress");
    GetFuncs();
    if (!_WinMain())
        return;
    auto a = &g_header.JmpBack[0];
    __asm__ volatile(
        "mov %0, %%rax;"
        "add $0x30, %%rsp;"
        "pop %%rbp;"
        "jmp %%rax;"
        : "=m"(a)
    );
    return;
}

void GetKernelBase()
{
    PPEB pPEB;
    __asm__ volatile(
        "mov %%gs:(0x60), %0;"
        : "=r"(pPEB));
    auto pLdr = &pPEB->Ldr->InMemoryOrderModuleList;
    auto pLdrTemp = pLdr;
    do
    {
        auto pLdrData = (PLDR_DATA_TABLE_ENTRY)pLdr;
        if (EqualWI(pLdrData->FullDllName.Buffer, L"KERNEL32.DLL", pLdrData->FullDllName.Length / sizeof(wchar_t)))
        {
            g_kernel32 = (HMODULE)pLdrData->Reserved2[0];
            return;
        }
        pLdr = pLdr->Blink;
    } while (pLdr != pLdrTemp);
}

BOOL EqualW(PCWSTR a, PCWSTR b, __int64 size)
{
    if (!size)
        return FALSE;
    for (__int64 i = 0; i < size; i++)
    {
        if (*(a + i) != *(b + i))
        {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL EqualA(PCSTR a, PCSTR b, __int64 size)
{
    if (!size)
        return FALSE;
    for (__int64 i = 0; i < size; i++)
    {
        if (*(a + i) != *(b + i))
        {
            return FALSE;
        }
    }
    return TRUE;
}

__int64 StrLenA(PCSTR a)
{
    __int64 total = 0;
    while (*a != 0)
    {
        ++a;
        ++total;
    }
    return total;
}

__int64 GetApi(PCSTR hModule, PCSTR lpszApi)
{
    auto pDos = (PIMAGE_DOS_HEADER)hModule;
    auto pNt = (PIMAGE_NT_HEADERS64)(hModule + pDos->e_lfanew);
    auto pExport = (PIMAGE_EXPORT_DIRECTORY)(pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + hModule);
    auto nameAddrs = (LPDWORD)(hModule + pExport->AddressOfNames);
    for (__int64 i = 0; i < pExport->AddressOfNames; i++)
    {
        PCSTR name = hModule + *(nameAddrs + i);
        if (EqualA(lpszApi, name, StrLenA(lpszApi)))
        {
            auto nameOrdinals = (LPWORD)(pExport->AddressOfNameOrdinals + hModule);
            nameOrdinals += i;
            auto funcAddrs = (LPDWORD)(pExport->AddressOfFunctions + hModule);
            funcAddrs += *nameOrdinals;
            return *funcAddrs + (__int64)hModule;
        }
    }
    return 0;
}

inline CHAR ToUpChar(CHAR c)
{
    return c > 0x61 && c < 0x7A ? c - 0x20 : c;
}

inline WCHAR ToUpChar(WCHAR c)
{
    return c > 0x61 && c < 0x7A ? c - 0x20 : c;
}

BOOL EqualWI(PCWSTR a, PCWSTR b, __int64 size)
{
    if (!size)
        return FALSE;
    for (__int64 i = 0; i < size; i++)
    {
        if (ToUpChar(*(a + i)) != ToUpChar(*(b + i)))
        {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL EqualAI(PCSTR a, PCSTR b, __int64 size)
{
    if (!size)
        return FALSE;
    for (__int64 i = 0; i < size; i++)
    {
        if (ToUpChar(*(a + i)) != ToUpChar(*(b + i)))
        {
            return FALSE;
        }
    }
    return TRUE;
}

void GetFuncs()
{
    _GetModuleHandleA = (_LxGetModuleHandleAT)_GetProcAddress(g_kernel32, "GetModuleHandleW");
    _LoadLibraryA = (_LxLoadLibraryAT)_GetProcAddress(g_kernel32, "LoadLibraryA");
    _ExitProcess = (_LxExitProcessT)_GetProcAddress(g_kernel32, "ExitProcess");

    g_user32 = _LoadLibraryA("USER32.DLL");
    _GetMessageA = (_LxGetMessageAT)_GetProcAddress(g_user32, "GetMessageA");
    _CreateWindowExA = (_LxCreateWindowExAT)_GetProcAddress(g_user32, "CreateWindowExA");
    _ShowWindow = (_LxShowWindowT)_GetProcAddress(g_user32, "ShowWindow");
    _UpdateWindow = (_LxUpdateWindowT)_GetProcAddress(g_user32, "UpdateWindow");
    _TranslateMessage = (_LxTranslateMessageT)_GetProcAddress(g_user32, "TranslateMessage");
    _DispatchMessageA = (_LxDispatchMessageAT)_GetProcAddress(g_user32, "DispatchMessageA");
    _RegisterClassExA = (_LxRegisterClassExAT)_GetProcAddress(g_user32, "RegisterClassExA");
    _PostQuitMessage = (_LxPostQuitMessageT)_GetProcAddress(g_user32, "PostQuitMessage");
    _GetDlgItemTextA = (_LxGetDlgItemTextAT)_GetProcAddress(g_user32, "GetDlgItemTextA");
    _MessageBoxA = (_LxMessageBoxAT)_GetProcAddress(g_user32, "MessageBoxA");
    _DestroyWindow = (_LxDestroyWindowT)_GetProcAddress(g_user32, "DestroyWindow");
    _DefWindowProcA = (_LxDefWindowProcAT)_GetProcAddress(g_user32, "DefWindowProcA");
    _UnregisterClassA = (_LxUnregisterClassAT)_GetProcAddress(g_user32, "UnregisterClassA");
}

BOOL _WinMain()
{
    g_hInstance = _GetModuleHandleA(NULL);
    WNDCLASSEXA wndClass{0};
    wndClass.cbSize = sizeof(WNDCLASSEXW);
    wndClass.lpfnWndProc = MainWinProc;
    wndClass.hInstance = g_hInstance;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wndClass.lpszClassName = MAIN_CLASS_NAME;
    if (!_RegisterClassExA(&wndClass))
    {
        _MessageBoxA(NULL, "µÇÂ¼¿ò´´½¨Ê§°Ü!", 0, MB_OK | MB_ICONERROR);
        return FALSE;
    }
    g_hWnd = _CreateWindowExA(
        WS_EX_CLIENTEDGE,
        MAIN_CLASS_NAME,
        g_header.MainTitle,
        DS_MODALFRAME | DS_FIXEDSYS | WS_MINIMIZEBOX | WS_POPUP | WS_CAPTION | WS_SYSMENU,
        g_header.MainWinX,
        g_header.MainWinY,
        300,
        140,
        NULL,
        NULL,
        g_hInstance,
        NULL);
    _ShowWindow(g_hWnd, SW_SHOWNORMAL);
    _UpdateWindow(g_hWnd);
    MSG msg{};
    while (_GetMessageA(&msg, NULL, NULL, NULL))
    {
        _TranslateMessage(&msg);
        _DispatchMessageA(&msg);
    }
    return TRUE;
}

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        g_hWnd = hWnd;
        CreateLable(g_header.AccLab, 10, 10, 90, 22, IDC_LABEL1);
        CreateLable(g_header.PwdLab, 10, 40, 90, 22, IDC_LABEL2);
        CreateEdit(105, 10, 175, 22, IDC_ACC_EDIT);
        CreatePwdEdit(105, 40, 175, 22, IDC_PWD_EDIT);
        CreateButton(g_header.LogBtn, 60, 70, 60, 25, IDC_LOG_BTN);
        CreateButton(g_header.CancelBtn, 170, 70, 60, 25, IDC_CANCEL_BTN);
        return 0;
    }
    case WM_CLOSE:
    {
        _ExitProcess(0);
        return 0;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_LOG_BTN:
        {
            constexpr int cchMax = sizeof(ImageUnlocalLogWinHeader::AccAnswer);
            char buffer1[cchMax]{0};
            char buffer2[cchMax]{0};
            _GetDlgItemTextA(hWnd, IDC_ACC_EDIT, buffer1, cchMax);
            _GetDlgItemTextA(hWnd, IDC_PWD_EDIT, buffer2, cchMax);

            if (
                EqualA(buffer1, g_header.AccAnswer, cchMax) &&
                EqualA(buffer2, g_header.PwdAnswer, cchMax))
            {
                _MessageBoxA(hWnd, g_header.SuccessTip, g_header.SuccessTitle, MB_OK | MB_ICONINFORMATION);
                _DestroyWindow(hWnd);
                _UnregisterClassA(MAIN_CLASS_NAME, g_hInstance);
                _PostQuitMessage(0);
            }
            else
            {
                _MessageBoxA(hWnd, g_header.FalidTip, g_header.FalidTitle, MB_OK | MB_ICONERROR);
            }
            return 0;
        }
        case IDC_CANCEL_BTN:
        {
            _ExitProcess(0);
            return 0;
        }
        default:
            break;
        }
    }
    }
    return _DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

inline HWND CreateLable(LPCSTR lpWindowName, int x, int y, int nWidth, int nHeight, int id)
{
    return _CreateWindowExA(
        NULL,
        "static",
        lpWindowName,
        WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
        x, y, nWidth, nHeight,
        g_hWnd,
        (HMENU)id,
        g_hInstance,
        NULL);
}

inline HWND CreateButton(LPCSTR lpWindowName, int x, int y, int nWidth, int nHeight, int id)
{
    return _CreateWindowExA(
        NULL,
        "button",
        lpWindowName,
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_PUSHBUTTON,
        x, y, nWidth, nHeight,
        g_hWnd,
        (HMENU)id,
        g_hInstance,
        NULL);
}

inline HWND CreateEdit(int x, int y, int nWidth, int nHeight, int id)
{
    return _CreateWindowExA(
        WS_EX_TOPMOST,
        "edit",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        x, y, nWidth, nHeight,
        g_hWnd,
        (HMENU)id,
        g_hInstance,
        NULL);
}

inline HWND CreatePwdEdit(int x, int y, int nWidth, int nHeight, int id)
{
    return _CreateWindowExA(
        WS_EX_TOPMOST,
        "edit",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD,
        x, y, nWidth, nHeight,
        g_hWnd,
        (HMENU)id,
        g_hInstance,
        NULL);
}