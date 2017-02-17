#pragma once
#include "SFSConnector.h"
#include "SFSResponse.h"
#include "Constant.h"
#include "Data.h"
#include "Command.h"
#include "Utils.h"
#include "EventHandler.h"

#include "Bitswarm/BitSwarmEvent.h"
#include "Util/EventDispatcher.h"
#include "Requests/LoginRequest.h"
#include "Requests/LogoutRequest.h"
#include "Requests/ExtensionRequest.h"
#include "Requests/PublicMessageRequest.h"
#include "Requests/JoinRoomRequest.h"
#include "Requests/LeaveRoomRequest.h"

template<> SFSConnector* SingLeton<SFSConnector>::mSingleton = 0;
SFSConnector* SFSConnector::getSingletonPtr(void)
{
	return mSingleton;
}
SFSConnector& SFSConnector::getSingleton(void)
{
	assert(mSingleton);  return (*mSingleton);
}

SFSConnector::SFSConnector()
{
	mSmartFox = boost::shared_ptr<Sfs2X::SmartFox>();
}


SFSConnector::~SFSConnector()
{
}

void SFSConnector::InitializeSmartFox()
{
	// Initialize Smart Fox
	mSmartFox = boost::shared_ptr<Sfs2X::SmartFox>(new Sfs2X::SmartFox(true));
	mSmartFox->ThreadSafeMode(true);
    mSmartFox->ForceIPv6(useIPv6);

	// Add event listeners
	mSmartFox->AddEventListener(SFSEvent::CONNECTION, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnSmartFoxConnection, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::CONNECTION_LOST, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnSmartFoxConnectionLost, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::LOGIN, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnSmartFoxLoginZone, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::LOGIN_ERROR, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnSmartFoxLoginZoneError, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::LOGOUT, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnSmartFoxLogoutZone, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::ROOM_JOIN, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnSmartFoxRoomJoin, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::ROOM_JOIN_ERROR, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnSmartFoxRoomJoinError, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::USER_EXIT_ROOM, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnUserExitRoom, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::USER_ENTER_ROOM, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnUserEnterRoom, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::EXTENSION_RESPONSE, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnExtensionResponse, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::PUBLIC_MESSAGE, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnPublicMessage, (unsigned long long)this)));
	mSmartFox->AddEventListener(SFSEvent::PING_PONG, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnPingPong, (unsigned long long)this)));
	//mSmartFox->AddEventListener(BitSwarmEvent::DISCONNECT, boost::shared_ptr<EventListenerDelegate>(new EventListenerDelegate(SFSConnector::OnSmartFoxDisconnection, (unsigned long long)this)));
}

void SFSConnector::OnSmartFoxConnection(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	boost::shared_ptr<map<string, boost::shared_ptr<void>>> ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueSuccess = (*ptrEventParams)["success"];
	boost::shared_ptr<bool> ptrErrorMessage = ((boost::static_pointer_cast<bool>))(ptrEventParamValueSuccess);

	if (*ptrErrorMessage == true)
	{
		CCLOG("OnSmartFoxConnection: successful");
		// Connection with SmartFox Server has been established
		// Set internal event to notify the connection establishment
		//SFSConnector::getSingleton().LoginZone("", "g", constant::ZONE_AUTHEN);
		if (EventHandler::getSingleton().onConnected != NULL) {
			EventHandler::getSingleton().onConnected();
		}
	} else {
		CCLOG("OnSmartFoxConnection: failed");
		if (EventHandler::getSingleton().onConnectionFailed != NULL) {
			EventHandler::getSingleton().onConnectionFailed();
		}
	}
}

// -------------------------------------------------------------------------
// OnSmartFoxConnectionLost
// -------------------------------------------------------------------------
void SFSConnector::OnSmartFoxConnectionLost(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamReason = (*ptrEventParams)["reason"];
	boost::shared_ptr<string> ptrReason = ((boost::static_pointer_cast<string>))(ptrEventParamReason);

	CCLOG("OnSmartFoxConnectionLost: %s", (*ptrReason).c_str());
	if (EventHandler::getSingleton().onConnectionLost != NULL) {
		EventHandler::getSingleton().onConnectionLost(*ptrReason);
	}
}

// -------------------------------------------------------------------------
// OnSmartFoxDisconnection
// -------------------------------------------------------------------------
void SFSConnector::OnSmartFoxDisconnection(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamReason = (*ptrEventParams)["reason"];
	boost::shared_ptr<string> ptrReason = ((boost::static_pointer_cast<string>))(ptrEventParamReason);

	CCLOG("OnSmartFoxDisconnection: %s", (*ptrReason).c_str());
	if (EventHandler::getSingleton().onDisconnected != NULL) {
		EventHandler::getSingleton().onDisconnected(*ptrReason);
	}
}

// -------------------------------------------------------------------------
// OnSmartFoxLogin
// -------------------------------------------------------------------------
void SFSConnector::OnSmartFoxLoginZone(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	CCLOG("OnSmartFoxLoginZone");
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueZone = (*ptrEventParams)["zone"];
	boost::shared_ptr<string> ptrNotifiedZone = ((boost::static_pointer_cast<string>))(ptrEventParamValueZone);

	SFSConnector::getSingleton().EnableLagMonitor();
	Utils::getSingleton().currentZoneName = *ptrNotifiedZone;
	CCLOG("%s", ptrNotifiedZone->c_str());
	/*for (auto iter = ptrEventParams->begin(); iter != ptrEventParams->end(); iter++) {
		CCLOG("%s", iter->first.c_str());
	}*/

	if (EventHandler::getSingleton().onLoginZone != NULL) {
		EventHandler::getSingleton().onLoginZone();
	}
}

// -------------------------------------------------------------------------
// OnSmartFoxLoginError
// -------------------------------------------------------------------------
void SFSConnector::OnSmartFoxLoginZoneError(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	// Get the error message parameter of the event
	// Get the error code parameter of the event
	boost::shared_ptr<map<string, boost::shared_ptr<void>>> ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueErrorMessage = (*ptrEventParams)["errorMessage"];
	boost::shared_ptr<string> ptrErrorMessage = ((boost::static_pointer_cast<string>))(ptrEventParamValueErrorMessage);

	boost::shared_ptr<void> ptrEventParamValueErrorCode = (*ptrEventParams)["errorCode"];
	boost::shared_ptr<short int> ptrErrorCode = ((boost::static_pointer_cast<short int>))(ptrEventParamValueErrorCode);

	if (EventHandler::getSingleton().onLoginZoneError != NULL) {
		EventHandler::getSingleton().onLoginZoneError(*ptrErrorCode, *ptrErrorMessage);
	}
	string message = "OnSmartFoxLoginZoneError: " + *ptrErrorMessage;
	CCLOG("%d %s", *ptrErrorCode, message.c_str());
}

void SFSConnector::OnSmartFoxLogoutZone(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	CCLOG("OnSmartFoxLogoutZone");
	Utils::getSingleton().logoutZone();
	if (EventHandler::getSingleton().onLogoutZone != NULL) {
		EventHandler::getSingleton().onLogoutZone();
	}
}

void SFSConnector::OnSmartFoxRoomJoin(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	//room joined
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueRoom = (*ptrEventParams)["room"];
	boost::shared_ptr<Room> ptrNotifiedRoom = ((boost::static_pointer_cast<Room>))(ptrEventParamValueRoom);
	
	Utils::getSingleton().currentRoomId = ptrNotifiedRoom->Id();
	Utils::getSingleton().currentRoomName = *(ptrNotifiedRoom->Name());
	CCLOG("RoomJoin: %d %s", ptrNotifiedRoom->Id(), ptrNotifiedRoom->Name()->c_str());
	if (EventHandler::getSingleton().onJoinRoom != NULL) {
		EventHandler::getSingleton().onJoinRoom(ptrNotifiedRoom->Id(), *(ptrNotifiedRoom->Name()));
	}
}

void SFSConnector::OnSmartFoxRoomJoinError(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueErrorMessage = (*ptrEventParams)["errorMessage"];
	boost::shared_ptr<string> ptrErrorMessage = ((boost::static_pointer_cast<string>))(ptrEventParamValueErrorMessage);

	CCLOG("RoomJoinError: %s", (*ptrErrorMessage).c_str());
	if (EventHandler::getSingleton().onJoinRoomError != NULL) {
		EventHandler::getSingleton().onJoinRoomError(*ptrErrorMessage);
	}
}

void SFSConnector::OnUserExitRoom(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	//get the user parameter of the event
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueUser = (*ptrEventParams)["user"];
	boost::shared_ptr<User> ptrNotifiedUser = ((boost::static_pointer_cast<User>))(ptrEventParamValueUser);

	//get the room parameter of the event
	boost::shared_ptr<void> ptrEventParamValueRoom = (*ptrEventParams)["room"];
	boost::shared_ptr<Room> ptrNotifiedRoom = ((boost::static_pointer_cast<Room>))(ptrEventParamValueRoom);

	//CCLOG("%s leaved room %s", ptrNotifiedUser->Name()->c_str(), ptrNotifiedRoom->Name()->c_str());
	if (EventHandler::getSingleton().onUserExitRoom != NULL) {
		EventHandler::getSingleton().onUserExitRoom(ptrNotifiedUser->Id());
	}
}

void SFSConnector::OnUserEnterRoom(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	//get the user parameter of the event
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueUser = (*ptrEventParams)["user"];
	boost::shared_ptr<User> ptrNotifiedUser = ((boost::static_pointer_cast<User>))(ptrEventParamValueUser);

	//get the room parameter of the event
	boost::shared_ptr<void> ptrEventParamValueRoom = (*ptrEventParams)["room"];
	boost::shared_ptr<Room> ptrNotifiedRoom = ((boost::static_pointer_cast<Room>))(ptrEventParamValueRoom);

	//CCLOG("%s enter room %s", ptrNotifiedUser->Name()->c_str(), ptrNotifiedRoom->Name()->c_str());
}

void SFSConnector::OnExtensionResponse(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent)
{
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueCmd = (*ptrEventParams)["cmd"];
	boost::shared_ptr<string> ptrNotifiedCmd = ((boost::static_pointer_cast<string>)(ptrEventParamValueCmd));

	boost::shared_ptr<void> ptrEventParamValueParams = (*ptrEventParams)["params"];
	boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject = ((boost::static_pointer_cast<ISFSObject>)(ptrEventParamValueParams));

	CCLOG("OnExtensionResponse::cmd::%s", (*ptrNotifiedCmd).c_str());
	/*vector<string> vt = *(ptrNotifiedISFSObject->GetKeys());
	for (string s : vt) {
		CCLOG("%s", s.c_str());
	}*/
	SFSResponse::getSingleton().onExtensionResponse(ptrNotifiedCmd, ptrNotifiedISFSObject);
}

void SFSConnector::OnPublicMessage(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent)
{
	//get the user parameter of the event
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueUser = (*ptrEventParams)["sender"];
	boost::shared_ptr<User> ptrNotifiedUser = ((boost::static_pointer_cast<User>))(ptrEventParamValueUser);

	//get the message parameter of the event
	boost::shared_ptr<void> ptrEventParamValueMsg = (*ptrEventParams)["message"];
	boost::shared_ptr<string> ptrNotifiedMsg = ((boost::static_pointer_cast<string>))(ptrEventParamValueMsg);

	if(EventHandler::getSingleton().onPublicMessage != NULL) {
		EventHandler::getSingleton().onPublicMessage(ptrNotifiedUser->Id(), *ptrNotifiedMsg);
	}
}

void SFSConnector::OnPingPong(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent)
{
	//get the user parameter of the event
	boost::shared_ptr<map<string, boost::shared_ptr<void> > > ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValue = (*ptrEventParams)["lagValue"];
	boost::shared_ptr<long> ptrValue = ((boost::static_pointer_cast<long>))(ptrEventParamValue);
	//CCLOG("Measured lag is: %dms", *ptrValue);
	if (EventHandler::getSingleton().onPingPong != NULL) {
		EventHandler::getSingleton().onPingPong(*ptrValue);
	}
}

void SFSConnector::Connect(std::string host, int port)
{
    CCLOG("Connect to SmartFox: %s %d", host.c_str(), port);
	SFSConnector::getSingleton().InitializeSmartFox();
	try {
		mSmartFox->Connect(host.c_str(), port);
	} catch (exception e) {
		CCLOG("SFSConnector::Connect::Exception: %s", e.what());
		if (EventHandler::getSingleton().onConnectionFailed != NULL) {
			EventHandler::getSingleton().onConnectionFailed();
		}
	}
}

void SFSConnector::Disconnect()
{
	CCLOG("SFSConnector::Disconnect");
	try {
		if (mSmartFox) {
			mSmartFox->Disconnect();
		}
	} catch (exception e) {
		CCLOG("SFSConnector::Disconnect::Exception: %s", e.what());
		//Utils::getSingleton().goToLoginScene();
	}
}

void SFSConnector::ProcessEvents()
{
	if (mSmartFox) {
		mSmartFox->ProcessEvents();
	}
}

void SFSConnector::LoginZone(string username, string password, string zone) 
{
	boost::shared_ptr<IRequest> request(new LoginRequest(username, password, zone));
	SendRequest(request);
}

void SFSConnector::LogoutZone()
{
	boost::shared_ptr<IRequest> request(new LogoutRequest());
	SendRequest(request);
}

void SFSConnector::RequestLeaveRoom()
{
	boost::shared_ptr<IRequest> request(new LeaveRoomRequest());
	SendRequest(request);
}

void SFSConnector::EnableLagMonitor()
{
	mSmartFox->EnableLagMonitor(true, 3);
}

void SFSConnector::ForceIPv6(bool value)
{
    this->useIPv6 = value;
}

void SFSConnector::SendPublicMessage(std::string msg, boost::shared_ptr<ISFSObject> params, boost::shared_ptr<Room> room)
{
	boost::shared_ptr<IRequest> request(new PublicMessageRequest(msg, params, room));
	SendRequest(request);
}

void SFSConnector::SendExtensionRequest(std::string cmd, boost::shared_ptr<ISFSObject> params, boost::shared_ptr<Room> room)
{
	CCLOG("SendExtensionRequest: %s", cmd.c_str());
	boost::shared_ptr<IRequest> request(new ExtensionRequest(cmd, params, room));
	SendRequest(request);
}

void SFSConnector::SendRequest(boost::shared_ptr<IRequest> request)
{
	try {
		if (mSmartFox) {
			mSmartFox->Send(request);
		}
	} catch (exception e) {
		CCLOG("SFSConnector::SendRequest::Exception: %s", e.what());
	}
}
