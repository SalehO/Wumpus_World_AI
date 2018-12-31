
#include "MyAI.hpp"

MyAI::MyAI() : Agent()
{

}
	
Agent::Action MyAI::getAction
(
	bool stench,
	bool breeze,
	bool glitter,
	bool bump,
	bool scream
)
{

	world.updateWorld(stench, breeze, glitter, bump, scream);
	//world.print();
	if(world.movesLeft())
	{
		return world.getMove();
	}
	else
	{
		//cout << "going in" << endl;
		world.decideNextMove();
		//cout << "got out alive" << endl;
		return world.getMove();
	}
}


// ======================================================================
// YOUR CODE ENDS
// ======================================================================
