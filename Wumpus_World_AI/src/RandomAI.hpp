

#ifndef RANDOMAI_LOCK
#define RANDOMAI_LOCK

#include <cstdlib>
#include "Agent.hpp"

class RandomAI : public Agent
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
		if ( glitter )
			return GRAB;
		
		return actions [ rand() % 6 ];
	}
	
private:

	const Action actions[6] =
	{
		TURN_LEFT,
		TURN_RIGHT,
		FORWARD,
		SHOOT,
		GRAB,
		CLIMB
	};
};

#endif