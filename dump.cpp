#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <exception>

#include <Windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "Dbghelp.lib")

namespace
{
    static FILE *g_crashLogFile = nullptr;

    static void BuildCrashBasePath(char *out, size_t size)
    {
        DWORD dirLen = GetModuleFileNameA(nullptr, out, static_cast<DWORD>(size));

        if (dirLen == 0 || dirLen >= size)
        {
            out[0] = '\0';
            dirLen = 0;
        }
        else
        {
            while (dirLen > 0 && out[dirLen - 1] != '\\' && out[dirLen - 1] != '/')
                --dirLen;
        }

        SYSTEMTIME time;
        GetLocalTime(&time);

        snprintf(
            out + dirLen,
            size - dirLen,
            "Crash_%04u%02u%02u_%02u%02u%02u",
            time.wYear, time.wMonth, time.wDay,
            time.wHour, time.wMinute, time.wSecond);
    }

    static void OpenCrashLog(const char *basePath)
    {
        char logPath[MAX_PATH + 4];
        snprintf(logPath, sizeof(logPath), "%s.log", basePath);

        g_crashLogFile = fopen(logPath, "w");
    }

    static void CloseCrashLog()
    {
        if (g_crashLogFile)
        {
            fclose(g_crashLogFile);
            g_crashLogFile = nullptr;
        }
    }

    static void CrashLog(const char *fmt, ...)
    {
        va_list args;

        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);

        if (g_crashLogFile)
        {
            va_start(args, fmt);
            vfprintf(g_crashLogFile, fmt, args);
            va_end(args);

            fflush(g_crashLogFile);
        }
    }

    class SymbolHandler
    {
    public:
        SymbolHandler()
        {
            m_process = GetCurrentProcess();

            SymSetOptions(
                SYMOPT_LOAD_LINES |
                SYMOPT_UNDNAME |
                SYMOPT_DEFERRED_LOADS);

            SymInitialize(m_process, nullptr, TRUE);
        }

        ~SymbolHandler()
        {
            SymCleanup(m_process);
        }

        HANDLE Process() const
        {
            return m_process;
        }

    private:
        HANDLE m_process;
    };

    static bool GetModuleInfo(
        HANDLE process,
        DWORD64 address,
        char *moduleName,
        size_t moduleNameSize,
        DWORD64 &rva)
    {
        IMAGEHLP_MODULE64 moduleInfo{};
        moduleInfo.SizeOfStruct = sizeof(moduleInfo);

        if (!SymGetModuleInfo64(process, address, &moduleInfo))
            return false;

        snprintf(
            moduleName,
            moduleNameSize,
            "%s",
            moduleInfo.ModuleName[0]
                ? moduleInfo.ModuleName
                : "(unknown)");

        /*
         * RVA = Runtime - LoadedBase
         */
        rva = address - moduleInfo.BaseOfImage;

        return true;
    }

    static void PrintStackFrame(
        HANDLE process,
        DWORD64 address)
    {
        char moduleName[MAX_PATH] = "(unknown)";
        DWORD64 rva = address;

        GetModuleInfo(
            process,
            address,
            moduleName,
            sizeof(moduleName),
            rva);

        /*
         * Format:
         *
         * [CurrentAddr] [RVA] [Module]
         */
        CrashLog(
            "[0x%llX] [0x%llX] [%s]\n",
            static_cast<unsigned long long>(address),
            static_cast<unsigned long long>(rva),
            moduleName);
    }

    static void PrintStackTrace(CONTEXT *context)
    {
        if (!context)
        {
            CrashLog("No exception context available.\n");
            return;
        }

        SymbolHandler symbols;

        HANDLE process = symbols.Process();
        HANDLE thread = GetCurrentThread();

        STACKFRAME64 frame{};

#ifdef _M_IX86

        constexpr DWORD machineType = IMAGE_FILE_MACHINE_I386;

        frame.AddrPC.Offset = context->Eip;
        frame.AddrPC.Mode = AddrModeFlat;

        frame.AddrFrame.Offset = context->Ebp;
        frame.AddrFrame.Mode = AddrModeFlat;

        frame.AddrStack.Offset = context->Esp;
        frame.AddrStack.Mode = AddrModeFlat;

#else

#error Only x86 Windows builds are supported.

#endif

        CrashLog("Stack trace:\n");

        int frameCount = 0;

        while (StackWalk64(
            machineType,
            process,
            thread,
            &frame,
            context,
            nullptr,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            nullptr))
        {
            const DWORD64 address = frame.AddrPC.Offset;

            if (address == 0)
                break;

            PrintStackFrame(process, address);
            frameCount++;
        }

        // Fallback if DbgHelp's StackWalk64 fails to trace any symbols
        if (frameCount == 0)
        {
            CrashLog("Symbol traceback failed. Falling back to raw stack capture:\n");

            void *frames[128]{};

            const USHORT count =
                CaptureStackBackTrace(
                    0, // Start capturing from the current frame
                    static_cast<DWORD>(_countof(frames)),
                    frames,
                    nullptr);

            for (USHORT i = 0; i < count; ++i)
            {
                PrintStackFrame(
                    process,
                    reinterpret_cast<DWORD64>(frames[i]));
            }
        }
    }

    static void PrintCurrentStackTrace()
    {
        void *frames[128]{};

        const USHORT count =
            CaptureStackBackTrace(
                1,
                static_cast<DWORD>(_countof(frames)),
                frames,
                nullptr);

        SymbolHandler symbols;

        CrashLog("Stack trace:\n");

        for (USHORT i = 0; i < count; ++i)
        {
            PrintStackFrame(
                symbols.Process(),
                reinterpret_cast<DWORD64>(frames[i]));
        }
    }

    static void WriteMiniDump(const char *basePath, EXCEPTION_POINTERS *exceptionPointers)
    {
        char path[MAX_PATH + 4];
        snprintf(path, sizeof(path), "%s.dmp", basePath);

        HANDLE file = CreateFileA(
            path,
            GENERIC_WRITE,
            0,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

        if (file == INVALID_HANDLE_VALUE)
        {
            CrashLog("Failed to create dump file '%s'.\n", path);
            return;
        }

        MINIDUMP_EXCEPTION_INFORMATION dumpInfo{};
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ExceptionPointers = exceptionPointers;
        dumpInfo.ClientPointers = FALSE;

        const BOOL written = MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            file,
            MiniDumpNormal,
            exceptionPointers ? &dumpInfo : nullptr,
            nullptr,
            nullptr);

        CloseHandle(file);

        if (written)
        {
            CrashLog("Wrote crash dump to '%s'.\n", path);
        }
        else
        {
            CrashLog("Failed to write crash dump to '%s' (0x%08lX).\n", path, GetLastError());
        }
    }

} // namespace

LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS *exceptionPointers)
{
    char basePath[MAX_PATH];
    BuildCrashBasePath(basePath, sizeof(basePath));
    OpenCrashLog(basePath);

    if (!exceptionPointers ||
        !exceptionPointers->ExceptionRecord)
    {
        CrashLog("Unhandled Windows exception.\n");
        CloseCrashLog();
        return EXCEPTION_EXECUTE_HANDLER;
    }

    const EXCEPTION_RECORD *record =
        exceptionPointers->ExceptionRecord;

    CrashLog(
        "Unhandled exception.\n"
        "Code: 0x%08lX\n"
        "Address: %p\n",
        record->ExceptionCode,
        record->ExceptionAddress);

    PrintStackTrace(exceptionPointers->ContextRecord);

    WriteMiniDump(basePath, exceptionPointers);

    CloseCrashLog();

    return EXCEPTION_EXECUTE_HANDLER;
}

void __cdecl HandlePureCall()
{
    char basePath[MAX_PATH];
    BuildCrashBasePath(basePath, sizeof(basePath));
    OpenCrashLog(basePath);

    CrashLog("Pure virtual function call detected.\n");

    PrintCurrentStackTrace();

    CloseCrashLog();

    std::abort();
}

void __cdecl HandleInvalidParameter(
    wchar_t const *expression,
    wchar_t const *function,
    wchar_t const *file,
    unsigned int line,
    uintptr_t /*pReserved*/)
{
    char basePath[MAX_PATH];
    BuildCrashBasePath(basePath, sizeof(basePath));
    OpenCrashLog(basePath);

    CrashLog(
        "Invalid parameter detected.\n"
        "Function: %ls\n"
        "File: %ls\n"
        "Line: %u\n",
        function ? function : L"(unknown)",
        file ? file : L"(unknown)",
        line);

    if (expression)
    {
        CrashLog("Expression: %ls\n", expression);
    }

    PrintCurrentStackTrace();

    CloseCrashLog();

    std::abort();
}

void HandleStdException()
{
    char basePath[MAX_PATH];
    BuildCrashBasePath(basePath, sizeof(basePath));
    OpenCrashLog(basePath);

    if (auto exception = std::current_exception())
    {
        try
        {
            std::rethrow_exception(exception);
        }
        catch (const std::exception &e)
        {
            CrashLog("Unhandled C++ exception: %s\n", e.what());
        }
        catch (...)
        {
            CrashLog("Unhandled unknown C++ exception.\n");
        }
    }

    PrintCurrentStackTrace();

    CloseCrashLog();

    std::abort();
}

void InitCrashDump()
{
    std::set_terminate(HandleStdException);

    _set_purecall_handler(HandlePureCall);

    _set_invalid_parameter_handler(HandleInvalidParameter);

    SetUnhandledExceptionFilter(ExceptionFilter);
}