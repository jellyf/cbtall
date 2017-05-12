#include "GameLogger.h"

template<> GameLogger* SingLeton<GameLogger>::mSingleton = 0;
GameLogger* GameLogger::getSingletonPtr(void)
{
	return mSingleton;
}
GameLogger& GameLogger::getSingleton(void)
{
	assert(mSingleton);  return (*mSingleton);
}

GameLogger::GameLogger()
{
}

GameLogger::~GameLogger()
{
}
