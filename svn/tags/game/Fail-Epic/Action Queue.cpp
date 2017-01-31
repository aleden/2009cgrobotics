#include "WPILib.h"
#include "Action Queue.h"
#include "wrappers.h"
#include "drive.h"

Move::Move(float time, float xStickValue, float yStickValue)
{
	t = time;
	xSV = xStickValue;
	ySV = yStickValue;
}

bool Move::action()
{
	driver->drive(xSV, ySV);
	Wait(t);
	return true;
}

#ifdef WOBBLE
bool Load::action()
{
	hal->elevatorUp();
	while (hal->isNewWobbleTrip())
	{ 	Wait(1);	}
	hal->elevatorStop();
	return true;
}
#endif

Pause::Pause(float time)
{	t = time;	}

bool Pause::action()
{
	Wait(t);
	return true;
}

void ActionQueue::addAction(Action* action)
{	actions.push_back(action);	}

void ActionQueue::queueMove(float time, float xStickValue, float yStickValue)
{	addAction(new Move(time, xStickValue, yStickValue));	}

void ActionQueue::queuePause(float time)
{	addAction(new Pause(time));	}

Action* ActionQueue::peek()
{	return actions.front();	}

bool ActionQueue::isEmpty()
{	return actions.empty();	}

bool ActionQueue::doNext()
{
	Action* action = actions.front();
	bool returnValue = action->action();
	delete action;
	actions.pop_front();
	return returnValue;
}

void ActionQueue::empty()
{
	for (std::list<Action*>::iterator iterator = actions.begin(); iterator != actions.end(); iterator++){
		delete *iterator;
	}
	actions.clear();
}

ActionQueue::~ActionQueue()
{
	empty();
}
