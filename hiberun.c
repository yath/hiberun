#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <powrprof.h>

BOOL run(LPSTR cmd, BOOL wait) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
//    si.lpDesktop = "winsta0\\default";

    if(!CreateProcess(NULL,
        cmd,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi)) {
            fprintf(stderr, "CreateProcess failed (%d).\n", GetLastError());
            return FALSE;
    }

    if (wait)
        WaitForSingleObject(pi.hProcess, INFINITE);

    return TRUE;
}

DWORD getppid(void) {
    PROCESSENTRY32 ent;
    HANDLE snapshot;

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    ent.dwSize = sizeof(ent);
    BOOL ret = Process32First(snapshot, &ent);
    DWORD mypid = GetCurrentProcessId();
    while (ret) {
        if (ent.th32ProcessID == mypid)
            return ent.th32ParentProcessID;

        ent.dwSize = sizeof(ent);
        ret = Process32Next(snapshot, &ent);
    }
    return -1;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!lpCmdLine)
        return 1;

    if (!*lpCmdLine) {
        DWORD ppid = getppid();
        HANDLE pproc = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE, ppid);
        if (pproc) {
            WaitForSingleObject(pproc, INFINITE);
            SetSuspendState(TRUE, FALSE, FALSE);
        }
    } else {
        run(__argv[0], FALSE);
        run(lpCmdLine, TRUE);
    }

    return 0;
}
