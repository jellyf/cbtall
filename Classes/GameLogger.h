#pragma once
#include "cocos2d.h"
#include "SingLeton.h"
#include "Data.h"

class GameLogger : public SingLeton <GameLogger>
{
public:
	GameLogger();
	~GameLogger();

	static GameLogger& getSingleton(void);
	static GameLogger* getSingletonPtr(void);
};

