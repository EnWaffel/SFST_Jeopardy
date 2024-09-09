#include "Telemetry.h"
#include <fstream>

Telemetry::Telemetry()
{
}

Telemetry::~Telemetry()
{
}

void Telemetry::Save()
{
	std::ofstream s("telemetry.txt");
	
	s << "----- Daten vom letzten Spiel: -----\n" << std::endl;
	s << "- Kandidaten: " << contestants << std::endl;
	s << "- richtig beantwortete Fragen: " << correctGuesses << std::endl;
	s << "- falsch beantwortete Fragen: " << wrongGuesses << std::endl;

	s.close();
}
