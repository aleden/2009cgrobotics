#pragma once

class Plugin
{
private:
	bool ownThread; // does this plugin's update method get called in its own thread
public:
	virtual void update() = 0;
	virtual char * getName() = 0;
	bool isOwnThread();
};

void loadPlugin(Plugin &plugin);
void updatePlugins();
