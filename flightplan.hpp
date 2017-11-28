

enum direction
{
    cw,         // clockwise
    ccw,        // counter clockwise
    shortest    // take the shortest route
};


struct flightCommand
{
    direction rotationDirection;    // Direction to rotate
    short heading;                  // Ending heading, in degrees
    short rollTime;                 // Time to get to new heading, in milliseconds
};

/// if (validFlightPlan) Ensures:
///     - size() > 0
///     - 0 <= op[] < size() returns valid flightCommand
class flightplan
{
public:
    flightplan()
    {
        valid = false;
        commands = new flightCommand[0];
    }

    ~flightplan()
    { delete commands; }

    const flightCommand operator[](int i) const;

    int size();

    bool validFlightPlan();

    /// Parse a flight plan string format:
    /// '#3;+0901000;-0001000;~1802000;\0'
    void parseFlightPlan(char*);

private:
    bool valid;
    int numberOfFlightCommands;
    flightCommand* commands;
};