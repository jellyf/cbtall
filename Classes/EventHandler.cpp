#include "EventHandler.h"

template<> EventHandler* SingLeton<EventHandler>::mSingleton = 0;
EventHandler* EventHandler::getSingletonPtr(void)
{
	return mSingleton;
}
EventHandler& EventHandler::getSingleton(void)
{
	assert(mSingleton);  return (*mSingleton);
}

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}
