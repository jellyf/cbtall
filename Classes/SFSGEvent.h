#pragma once
#include "SmartFox.h"
#include "Core/BaseEvent.h"
#include "SingLeton.h"
#include "cocos2d.h"

class SFSGEvent : public SingLeton<SFSGEvent>
{
public:
	SFSGEvent();
	~SFSGEvent();
	static SFSGEvent& getSingleton(void);
	static SFSGEvent* getSingletonPtr(void);

	void Reset();
    void ProcessEvents();
    void DoWork(bool isWork);
	void OnSFSEvent(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	void OnConnection(map<string, boost::shared_ptr<void>> params);
	void OnConnectionLost(map<string, boost::shared_ptr<void>> params);
	void OnLoginZone(map<string, boost::shared_ptr<void>> params);
	void OnLoginZoneError(map<string, boost::shared_ptr<void>> params);
	void OnLogoutZone(map<string, boost::shared_ptr<void>> params);
	void OnJoinRoom(map<string, boost::shared_ptr<void>> params);
	void OnJoinRoomError(map<string, boost::shared_ptr<void>> params);
	void OnUserEnterRoom(map<string, boost::shared_ptr<void>> params);
	void OnUserExitRoom(map<string, boost::shared_ptr<void>> params);
	void OnExtensionResponse(map<string, boost::shared_ptr<void>> params);
	void OnPublicMessage(map<string, boost::shared_ptr<void>> params);
	void OnPingPong(map<string, boost::shared_ptr<void>> params);
    void OnConnectionException(std::exception e);
private:
	map<string, function<void(map<string, boost::shared_ptr<void>>)>> mapFunctions;
	vector<pair<string, map<string, boost::shared_ptr<void>>>> eventQueue;
	int ptrQueue;
	int sizeQueue;
    bool isWork = true;
};

