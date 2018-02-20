/*#include <cassert>
#include "..\flightplan.hpp"
#include <iostream>

int main()
{
    {
        std::cerr << "Verify initalize state is invalid: ";
        flightplan fp;
        assert(!fp.validFlightPlan());

        std::cerr << "Done" << std::endl;
    }

    {
        std::cerr << "Parse # of flightCommands, malformed: ";
        char testPlan[] = "#1";
        flightplan fp;

        fp.parseFlightPlan(testPlan);

        assert(!fp.validFlightPlan());

        std::cerr << "Done" << std::endl;
    }

    {
        std::cerr << "Parse # of flightCommands: ";
        char testPlan[] = "#1;";
        flightplan fp;

        fp.parseFlightPlan(testPlan);

        assert(!fp.validFlightPlan());
        assert(fp.size() == 1);

        std::cerr << "Done" << std::endl;
    }

    {
        std::cerr << "Parse '+': ";
        char testPlan[] = "#1;+0901000;";
        flightplan fp;

        fp.parseFlightPlan(testPlan);

        assert(fp.validFlightPlan());
        assert(fp.size() == 1);
        assert(fp[0].rotationDirection == cw);
        assert(fp[0].heading == 90);
        assert(fp[0].rollTime == 1000);

        std::cerr << "Done" << std::endl;
    }

    {
        std::cerr << "Parse '-': ";
        char testPlan[] = "#1;-0901000;";
        flightplan fp;

        fp.parseFlightPlan(testPlan);

        assert(fp.validFlightPlan());
        assert(fp.size() == 1);
        assert(fp[0].rotationDirection == ccw);
        assert(fp[0].heading == 90);
        assert(fp[0].rollTime == 1000);

        std::cerr << "Done" << std::endl;
    }

    {
        std::cerr << "Parse '~': ";
        char testPlan[] = "#1;~0901000;";
        flightplan fp;

        fp.parseFlightPlan(testPlan);

        assert(fp.validFlightPlan());
        assert(fp.size() == 1);
        assert(fp[0].rotationDirection == shortest);
        assert(fp[0].heading == 90);
        assert(fp[0].rollTime == 1000);

        std::cerr << "Done" << std::endl;
    }

    {
        std::cerr << "Parse multipule, normalize heading: ";
        char testPlan[] = "#2;~0901000;+3601500;";
        flightplan fp;

        fp.parseFlightPlan(testPlan);

        assert(fp.validFlightPlan());
        assert(fp.size() == 2);

        // Test first command.
        assert(fp[0].rotationDirection == shortest);
        assert(fp[0].heading == 90);
        assert(fp[0].rollTime == 1000);

        // Test second command.
        assert(fp[1].rotationDirection == cw);
        assert(fp[1].heading == 0);
        assert(fp[1].rollTime == 1500);

        std::cerr << "Done" << std::endl;
    }

    return 0;
}
*/
