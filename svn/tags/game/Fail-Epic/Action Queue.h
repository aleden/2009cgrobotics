#pragma once
#include <list>

class Action
{
public:
	virtual bool action() = 0;
};


class Move : public Action
{
private:
	float t;
	float xSV;
	float ySV;
	
public:
	Move(float time, float xStickValue, float yStickValue);
	bool action();
};

class Load : public Action
{
public:
	Load();
	bool action();
};

class Pause : public Action
{
private:
	float t;
	
public:
	Pause(float time);
	bool action();
};

class ActionQueue
{
private:
	std::list<Action*> actions;
	
public:
	ActionQueue();
	void addAction(Action* action);
	Action* peek();
	bool isEmpty();
	bool doNext();
	void empty();
	void queueMove(float time, float xStickValue, float yStickValue);
	void queuePause(float time);
	~ActionQueue();
};
