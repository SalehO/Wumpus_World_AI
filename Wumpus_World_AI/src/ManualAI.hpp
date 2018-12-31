
#ifndef MANUALAI_LOCK
#define MANUALAI_LOCK

#include<iostream>
#include "Agent.hpp"

class ManualAI : public Agent
{
public:

	Action getAction
	(
		bool stench,
		bool breeze,
		bool glitter,
		bool bump,
		bool scream
	)
	{
		// Print Command Menu
		std::cout << "Press 'w' to Move Forward  'a' to 'Turn Left' 'd' to 'Turn Right'" << std::endl;
		std::cout << "Press 's' to Shoot         'g' to 'Grab'      'c' to 'Climb'" << std::endl;
		
		// Get Input
		std::cout << "Please input: ";
		char userInput;
		std::cin >> userInput;
		std::cin.ignore(9999, '\n');
		
		// Return Action Associated with Input
		if ( userInput == 'w' )
			return FORWARD;
		
		if ( userInput == 'a' )
			return TURN_LEFT;
		
		if ( userInput == 'd' )
			return TURN_RIGHT;
		
		if ( userInput == 's' )
			return SHOOT;
		
		if ( userInput == 'g' )
			return GRAB;

		return CLIMB;
	}
};

#endif