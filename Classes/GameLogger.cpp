#include "GameLogger.h"
#include "SFSRequest.h"
#include "Constant.h"
#include "Utils.h"

using namespace std;

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

bool GameLogger::isEnable()
{
	return mIsEnabled;
}

void GameLogger::setEnabled(bool enabled)
{
	mIsEnabled = enabled;
}

void GameLogger::setHost(std::string host)
{
	mHost = host;
}

void GameLogger::setUser(UserData user)
{
	mUser = "%20Player: " + user.Name + " - " + to_string(user.UserID);
}

void GameLogger::setRoom(bool isQuan, long money, std::string tableId)
{
	mRoom = "%20Phong: " + to_string(money) + (isQuan ? " Quan" : " Xu") + " - Ban " + tableId;
}

void GameLogger::logUserBash(BashData data)
{
	std::string str = "%20DanhBai: " + to_string(data.UId) + " danh " + to_string((int)data.CardId);
	str += "%20Chuyen luot:" + to_string(data.TurnId);
	if (data.CanPenet) str += "%20Co the Chiu";
	if (data.CanPenetWin) str += "%20Co the Chiu U";
	str += cardHandToString(data.CardHand);
	log(str);
}

void GameLogger::logUserBashBack(BashBackData data)
{
	std::string str = "%20TraCua: " + to_string(data.UId) + " danh " + to_string((int)data.CardId) + " cho " + to_string(data.BackId);
	str += "%20Chuyen luot:" + to_string(data.TurnId);
	if (data.CanPenet) str += "%20Co the Chiu";
	if (data.CanPenetWin) str += "%20Co the Chiu U";
	str += cardHandToString(data.CardHand);
	log(str);
}

void GameLogger::logUserForward(ForwardData data)
{
	std::string str = "%20DuoiBai: " + to_string(data.UId) + " duoi " + to_string((int)data.CardId);
	str += "%20Chuyen luot:" + to_string(data.TurnId);
	log(str);
}

void GameLogger::logUserHold(HoldData data)
{
	std::string str = "%20AnBai: " + to_string(data.UId) + " lay " + to_string((int)data.CardId) + " an " + to_string(data.CardIdHold);
	str += "%20Chuyen luot:" + to_string(data.TurnId);
	str += cardHandToString(data.CardHand);
	log(str);
}

void GameLogger::logUserPenet(PenetData data)
{
	std::string str = "%20Chiu: " + to_string(data.UId) + " chiu " + to_string((int)data.CardId);
	str += cardHandToString(data.CardHand);
	log(str);
}

void GameLogger::logUserPick(PickData data)
{
	std::string str = "%20Boc: " + to_string(data.UId) + " boc " + to_string((int)data.CardId);
	str += "%20Chuyen luot:" + to_string(data.TurnId);
	if (data.CanPenet) str += "%20Co the Chiu";
	if (data.CanWin) str += "%20Co the U";
	log(str);
}

void GameLogger::logEndMatch(EndMatchData data)
{
	std::string str = "%20HetVan: " + to_string(data.WinId) + " thang";
	log(str);
}

void GameLogger::logEndMatchTie()
{
	std::string str = "%20HetVan: Hoa`";
	log(str);
}

std::string GameLogger::cardHandToString(CardHandData data)
{
	std::string str = "%20BaiTrenTay:";
	str += "%20ThienKhai:";
	for (int i = 0; i < data.ThienKhai.size(); i++) {
		str += to_string((int)data.ThienKhai[i]) + ",";
	}
	str += "%20Chan:";
	for (int i = 0; i < data.Chan.size(); i++) {
		str += to_string((int)data.Chan[i]) + ",";
	}
	str += "%20BaDau:";
	for (int i = 0; i < data.BaDau.size(); i++) {
		str += to_string((int)data.BaDau[i]) + ",";
	}
	str += "%20Ca:";
	for (int i = 0; i < data.Ca.size(); i++) {
		str += to_string((int)data.Ca[i]) + ",";
	}
	str += "%20Que:";
	for (int i = 0; i < data.Que.size(); i++) {
		str += to_string((int)data.Que[i]) + ",";
	}
	return str;
}

void GameLogger::log(std::string slog)
{
	if (mIsEnabled) {
		std::string data = mUser + mRoom + slog;
		//CCLOG("%s", (mHost + data).c_str());
		SFSRequest::getSingleton().RequestHttpGet(mHost + data, constant::TAG_HTTP_LOG);
	}
}
