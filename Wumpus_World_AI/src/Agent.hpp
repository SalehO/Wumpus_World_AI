

#ifndef AGENT_LOCK
#define AGENT_LOCK

class Agent
{
public:

	// Actuators
	enum Action
	{
		TURN_LEFT,
		TURN_RIGHT,
		FORWARD,
		SHOOT,
		GRAB,
		CLIMB
	};
	
	virtual Action getAction
	(
		// Sensors
		bool stench,
		bool breeze,
		bool glitter,
		bool bump,
		bool scream
		
	) = 0;
};

#endif