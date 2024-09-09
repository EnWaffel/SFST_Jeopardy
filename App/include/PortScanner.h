#pragma once
#include <vector>
#include <string>

class PortScanner
{
public:
	static std::vector<std::string> ScanPortsFor(const std::string& name);
	static std::vector<std::string> GetAllPorts();
};