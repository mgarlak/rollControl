#include "flightplan.hpp"

const flightCommand flightplan::operator[](int i) const
{
    return flightCommand();
}

int flightplan::size()
{
    return numberOfFlightCommands;
}

bool flightplan::validFlightPlan()
{
    return valid;
}

void flightplan::parseFlightPlan(char* plan)
{

}