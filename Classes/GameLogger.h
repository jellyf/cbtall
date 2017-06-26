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

	bool isEnable();
	void setEnabled(bool enabled);
	void setHost(std::string host);
	void setUser(UserData user);
	void setRoom(bool isQuan, long money, std::string tableId);
	void logUserBash(BashData data);
	void logUserBashBack(BashBackData data);
	void logUserForward(ForwardData data);
	void logUserHold(HoldData data);
	void logUserPenet(PenetData data);
	void logUserPick(PickData data);
	void logEndMatch(EndMatchData data);
	void logEndMatchTie();
private:
	std::string cardHandToString(CardHandData data);
	void log(std::string slog);

	bool mIsEnabled;
	std::string mHost;
	std::string mRoom;
	std::string mUser;
};

