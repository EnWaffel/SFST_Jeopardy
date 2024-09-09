#include "PortScanner.h"
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>

std::vector<std::string> PortScanner::ScanPortsFor(const std::string& name)
{
	std::vector<std::string> ports;

	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	char portName[256];

	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);

	if (hDevInfo == INVALID_HANDLE_VALUE) {
		return ports;
	}

	DWORD index = 0;
	while (SetupDiEnumDeviceInfo(hDevInfo, index, &DeviceInfoData)) {
		if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)portName, sizeof(portName), NULL)) {
			std::string comPortName;
			if (strstr(portName, name.c_str())) {
				std::string friendlyNameStr(portName);
				size_t startPos = friendlyNameStr.find("(COM");
				size_t endPos = friendlyNameStr.find(")");
				if (startPos != std::string::npos && endPos != std::string::npos && endPos > startPos) {
					comPortName = friendlyNameStr.substr(startPos + 1, endPos - startPos - 1);
					ports.push_back(comPortName);
				}
			}
		}
		index++;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return ports;
}

std::vector<std::string> PortScanner::GetAllPorts()
{
	std::vector<std::string> ports;

	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	char portName[256];

	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);

	if (hDevInfo == INVALID_HANDLE_VALUE) {
		return ports;
	}

	DWORD index = 0;
	while (SetupDiEnumDeviceInfo(hDevInfo, index, &DeviceInfoData)) {
		if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)portName, sizeof(portName), NULL)) {
			std::string comPortName;
			std::string friendlyNameStr(portName);
			size_t startPos = friendlyNameStr.find("(COM");
			size_t endPos = friendlyNameStr.find(")");
			if (startPos != std::string::npos && endPos != std::string::npos && endPos > startPos) {
				comPortName = friendlyNameStr.substr(startPos + 1, endPos - startPos - 1);
				ports.push_back(comPortName);
			}
		}
		index++;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return ports;
}
