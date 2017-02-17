#pragma once
#include "SmartFox.h"
#include "Core/BaseEvent.h"
#include "SingLeton.h"
#include "cocos2d.h"

struct RoomData;
class SFSConnector : public SingLeton<SFSConnector>
{
public:
	SFSConnector();
	~SFSConnector();
	void InitializeSmartFox();

	static SFSConnector& getSingleton(void);
	static SFSConnector* getSingletonPtr(void);

	static void OnSmartFoxConnection(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnSmartFoxConnectionLost(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnSmartFoxDisconnection(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnSmartFoxLoginZone(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnSmartFoxLoginZoneError(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnSmartFoxLogoutZone(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnSmartFoxRoomJoin(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnSmartFoxRoomJoinError(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnUserExitRoom(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnUserEnterRoom(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnExtensionResponse(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnPublicMessage(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);
	static void OnPingPong(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent);

	void Connect(std::string host, int port);
	void Disconnect();
	void ProcessEvents();
	void LoginZone(std::string username, std::string password, std::string zone);
	void LogoutZone();
	void RequestLeaveRoom();
	void EnableLagMonitor();
    void ForceIPv6(bool value);

	void SendPublicMessage(std::string msg, boost::shared_ptr<ISFSObject> params, boost::shared_ptr<Room> room = NULL);
	void SendExtensionRequest(std::string cmd, boost::shared_ptr<ISFSObject> params, boost::shared_ptr<Room> room = NULL);
private:
	void SendRequest(boost::shared_ptr<IRequest> request);

	boost::shared_ptr<Sfs2X::SmartFox> mSmartFox;
    bool useIPv6 = false;
};

