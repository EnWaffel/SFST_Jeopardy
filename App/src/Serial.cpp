#include "Serial.h"
#include "Logger.h"

Serial::Serial(const std::string& port) : port(port), handle(INVALID_HANDLE_VALUE)
{
}

Serial::~Serial()
{
}

bool Serial::Open()
{
    std::wstring dev(port.begin(), port.end());
    handle = CreateFile((L"\\\\.\\" + dev).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (handle == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    BOOL success = FlushFileBuffers(handle);
    if (!success)
    {
        Close();
        return false;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    success = SetCommTimeouts(handle, &timeouts);
    if (!success)
    {
        Close();
        return false;
    }
    DCB state = { 0 };
    state.DCBlength = sizeof(DCB);
    state.BaudRate = 115200;
    state.ByteSize = 8;
    state.Parity = NOPARITY;
    state.StopBits = ONESTOPBIT;
    success = SetCommState(handle, &state);
    if (!success)
    {
        Close();
        return false;
    }

    SetLogColor(COLOR_FOREGROUND_INTENSITY);
    LogInfo("Serial::Open(): " + port);

    return true;
}

void Serial::Close()
{
    if (handle == nullptr) return;
    CloseHandle(handle);
    SetLogColor(COLOR_FOREGROUND_INTENSITY);
    LogInfo("Serial::Close()");
}

bool Serial::Write(uint8_t* buf, size_t size)
{
    std::string b;
    for (int i = 0; i < size; i++)
    {
        b.append(std::to_string(buf[i]));
        b.push_back(' ');
    }
    SetLogColor(COLOR_FOREGROUND_INTENSITY);
    LogInfo("Serial::Write(): " + b);
    
    DWORD written;
    BOOL success = WriteFile(handle, buf, size, &written, NULL);
    if (!success)
    {
        return false;
    }
    return true;
}

int Serial::Read(uint8_t* buf, size_t size)
{
    DWORD received;
    BOOL success = ReadFile(handle, buf, size, &received, NULL);
    if (!success)
    {
        return 0;
    }
    return received;
}
