#pragma once

#define RELEASED_TRIGGER(IS_IDENTIFIER, TRIGGER_EXPR)\
	static bool last##IS_IDENTIFIER = false;\
	bool this##IS_IDENTIFIER##Trigger = (TRIGGER_EXPR);\
	bool IS_IDENTIFIER = (!(this##IS_IDENTIFIER##Trigger) && (last##IS_IDENTIFIER));\
	last##IS_IDENTIFIER = this##IS_IDENTIFIER##Trigger;

#define PRESSED_TRIGGER(IS_IDENTIFIER, TRIGGER_EXPR)\
	static bool last##IS_IDENTIFIER = false;\
	bool this##IS_IDENTIFIER##Trigger = (TRIGGER_EXPR);\
	bool IS_IDENTIFIER = ((this##IS_IDENTIFIER##Trigger) && !(last##IS_IDENTIFIER));\
	last##IS_IDENTIFIER = this##IS_IDENTIFIER##Trigger;
