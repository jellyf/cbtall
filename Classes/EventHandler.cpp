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

void RuntimeHandler::setActive(bool active)
{
	mIsActive = active;
}

void RuntimeHandler::finish()
{
	mIsActive = false;
}

bool RuntimeHandler::IsAvtive()
{
	return mIsActive;
}
