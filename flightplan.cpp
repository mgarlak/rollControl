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
    int pos = 0;

    // Parse '#number;'
    if (plan[pos++] != '#')
    {
        // missing #, invalid plan
        return;
    }

    numberOfFlightCommands = 0;
    // While the char is a number;
    while ('0' <= plan[pos] && plan[pos] <='9')
    {
        // If this is the end of the string.
        if (plan[pos] == 0)
        {
            // short circuit, invalid plan
            return;
        }
        numberOfFlightCommands *= 10;
        numberOfFlightCommands += plan[pos] - '0';
        ++pos;
    }
}