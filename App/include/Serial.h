#pragma once
#include <string>
#include <Windows.h>

#define SERIAL_ACTION_BTN_ACT1 0x10
#define SERIAL_ACTION_COL_UPDATE 0x11
#define SERIAL_ACTION_CHECK 0x12
#define SERIAL_ACTION_BUZZER_ACTIVE 0x13

class Serial
{
private:
	HANDLE handle;
	std::string port;
public:
	Serial(const std::string& port);
	~Serial();

	bool Open();
	void Close();

	bool Write(uint8_t* buf, size_t size);
	int Read(uint8_t* buf, size_t size);
};