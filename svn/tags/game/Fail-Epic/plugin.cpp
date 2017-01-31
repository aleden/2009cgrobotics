#include "plugin.h"
#include <list>
#include "WPILib.h"
/*
static std::list<Plugin *> plugins;
static int runThreadedPlugin(UINT32 arg0, UINT32 arg1, UINT32 arg2, UINT32 arg3, UINT32 arg4, 
		UINT32 arg5, UINT32 arg6, UINT32 arg7, UINT32 arg8, UINT32 arg9);

void loadPlugin(Plugin &plugin)
{
	// check if it is its own thread
	if (plugin.isOwnThread()) {
		Task *t = new Task(plugin.getName(), runThreadedPlugin);
		t->Start( (UINT32) &plugin);
		// t goes off into neverland
	} else { // t belongs in the list of plugins to be updated
		plugins.push_back(&plugin);
	}
}

int runThreadedPlugin(UINT32 arg0, UINT32 arg1, UINT32 arg2, UINT32 arg3, UINT32 arg4, 
		UINT32 arg5, UINT32 arg6, UINT32 arg7, UINT32 arg8, UINT32 arg9)
{
	Plugin *plugin = (Plugin *) arg0;
	
	for (;;)
		plugin->update();
}

void updatePlugins()
{
	for (std::list<Plugin *>::iterator i = plugins.begin(); i != plugins.end(); i++)
		(*i)->update();
}
*/