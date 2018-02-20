#include "flightplan.hpp"


///flightplan Implementation

const

flightCommand flightplan::operator[](int i) const
{
    if (0 <= i && i < size())
    {
        return commands[i];
    }
    return flightCommand();
}

void flightplan::parseFlightPlan(char* plan)
{
    /// Returning before the last line of the method will
    /// leave the validFlightPlan() == false.

    int pos = 0;

    // Parse '#number;'
    if (plan[pos++] != '#')
    {
        // missing #, invalid plan
        return;
    }

    numberOfFlightCommands = 0;
    // While the char is a number;
    while (isDigit(plan[pos]))
    {
        // If this is the end of the string.
        if (plan[pos] == 0)
        {
            // short circuit, invalid plan
            return;
        }
        numberOfFlightCommands = appendCharDigit(numberOfFlightCommands, plan[pos]);
        ++pos;
    }

    // Move off of ';'
    pos++;

    // Get rid of the old flight data.
    delete commands;

    // Allocate space for new flight commands.
    commands = new flightCommand[numberOfFlightCommands];

    // Read each flight command.
    for (int i = 0; i < numberOfFlightCommands; ++i)
    {
        // Parse rotation direction.
        switch (plan[pos])
        {
            case '+':
                commands[i].rotationDirection = cw;
                break;
            case '-':
                commands[i].rotationDirection = ccw;
                break;
            case '~':
                commands[i].rotationDirection = shortest;
                break;
            default:
                // Invalid rotation direction.
                return;
        }
        ++pos;

        // Parse end orientation.
        // Verify next 3 chars are digits.
        if (areDigits(&plan[pos], 3))
        {
            commands[i].heading = getNumberFromChars(&plan[pos], 3) % 360;
            pos += 3;
        }
        else
        {
            // Invalid degree angle.
            return;
        }

        // Parse time.
        if (areDigits(&plan[pos],4))
        {
            commands[i].rollTime = getNumberFromChars(&plan[pos], 4);
            pos += 4;
        }
        else
        {
            // Invalid milli second
            return;
        }


        // Check semi-colon.
        if (plan[pos++] != ';')
        {
            return;
        }
    }

    // If it gets here, all formatting is correct.
    valid = true;
}
