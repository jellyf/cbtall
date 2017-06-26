#include "SFSGEvent.h"
#include "EventHandler.h"
#include "SFSResponse.h"
#include "SFSRequest.h"
#include "Utils.h"

#define MAX_QUEUE 100

template<> SFSGEvent* SingLeton<SFSGEvent>::mSingleton = 0;
SFSGEvent* SFSGEvent::getSingletonPtr(void)
{
	return mSingleton;
}

SFSGEvent& SFSGEvent::getSingleton(void)
{
	assert(mSingleton);  return (*mSingleton);
}

SFSGEvent::SFSGEvent()
{
	Reset();
	mapFunctions[*SFSEvent::CONNECTION] = std::bind(&SFSGEvent::OnConnection, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::CONNECTION_LOST] = std::bind(&SFSGEvent::OnConnectionLost, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::LOGIN] = std::bind(&SFSGEvent::OnLoginZone, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::LOGIN_ERROR] = std::bind(&SFSGEvent::OnLoginZoneError, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::LOGOUT] = std::bind(&SFSGEvent::OnLogoutZone, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::ROOM_JOIN] = std::bind(&SFSGEvent::OnJoinRoom, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::ROOM_JOIN_ERROR] = std::bind(&SFSGEvent::OnJoinRoomError, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::USER_ENTER_ROOM] = std::bind(&SFSGEvent::OnUserEnterRoom, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::USER_EXIT_ROOM] = std::bind(&SFSGEvent::OnUserExitRoom, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::EXTENSION_RESPONSE] = std::bind(&SFSGEvent::OnExtensionResponse, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::PUBLIC_MESSAGE] = std::bind(&SFSGEvent::OnPublicMessage, this, std::placeholders::_1);
	mapFunctions[*SFSEvent::PING_PONG] = std::bind(&SFSGEvent::OnPingPong, this, std::placeholders::_1);
}

SFSGEvent::~SFSGEvent()
{
}

void SFSGEvent::Reset()
{
	ptrQueue = 0;
	sizeQueue = 0;
	if (eventQueue.size() == 0) {
		for (int i = 0; i < MAX_QUEUE; i++) {
			eventQueue.push_back(pair<string, map<string, boost::shared_ptr<void>>>());
		}
	}
}

void SFSGEvent::ProcessEvents()
{
	if (ptrQueue != sizeQueue) {
		string key = eventQueue[ptrQueue].first;
		if (mapFunctions.find(key) != mapFunctions.end()) {
			mapFunctions[key](eventQueue[ptrQueue].second);
		}
		ptrQueue++;
		if (ptrQueue == MAX_QUEUE) ptrQueue = 0;
	}
}

void SFSGEvent::DoWork(bool work)
{
    isWork = work;
}

void SFSGEvent::OnSFSEvent(unsigned long long ptrContext, boost::shared_ptr<Sfs2X::Core::BaseEvent> ptrEvent)
{
    if(!isWork) return;
	eventQueue[sizeQueue].first = *ptrEvent->Type();
	eventQueue[sizeQueue].second = *ptrEvent->Params();
	sizeQueue++;
	if (sizeQueue == MAX_QUEUE) sizeQueue = 0;
}

void SFSGEvent::OnConnection(map<string, boost::shared_ptr<void>> params)
{
	boost::shared_ptr<void> ptrEventParamValueSuccess = params["success"];
	boost::shared_ptr<bool> ptrErrorMessage = ((boost::static_pointer_cast<bool>))(ptrEventParamValueSuccess);

	if (*ptrErrorMessage == true)
	{
		CCLOG("OnSmartFoxConnection: successful");
		if (EventHandler::getSingleton().onConnected != NULL) {
			EventHandler::getSingleton().onConnected();
		}
	} else {
		CCLOG("OnSmartFoxConnection: failed");
		SFSRequest::getSingleton().InitSmartFox();
		if (EventHandler::getSingleton().onConnectionFailed != NULL) {
			EventHandler::getSingleton().onConnectionFailed();
		}
	}
}

void SFSGEvent::OnConnectionLost(map<string, boost::shared_ptr<void>> params)
{
	boost::shared_ptr<void> ptrEventParamReason = params["reason"];
	boost::shared_ptr<string> ptrReason = ((boost::static_pointer_cast<string>))(ptrEventParamReason);

	CCLOG("OnSmartFoxConnectionLost: %s", (*ptrReason).c_str());
	if (EventHandler::getSingleton().onConnectionLost != NULL) {
		EventHandler::getSingleton().onConnectionLost(*ptrReason);
	}
}

void SFSGEvent::OnLoginZone(map<string, boost::shared_ptr<void>> params)
{
	CCLOG("OnSmartFoxLoginZone");
	boost::shared_ptr<void> ptrEventParamValueZone = params["zone"];
	boost::shared_ptr<string> ptrNotifiedZone = ((boost::static_pointer_cast<string>))(ptrEventParamValueZone);

	SFSRequest::getSingleton().EnableLagMonitor();
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
void SFSGEvent::OnLoginZoneError(map<string, boost::shared_ptr<void>> params)
{
	boost::shared_ptr<void> ptrEventParamValueErrorMessage = params["errorMessage"];
	boost::shared_ptr<string> ptrErrorMessage = ((boost::static_pointer_cast<string>))(ptrEventParamValueErrorMessage);

	boost::shared_ptr<void> ptrEventParamValueErrorCode = params["errorCode"];
	boost::shared_ptr<short int> ptrErrorCode = ((boost::static_pointer_cast<short int>))(ptrEventParamValueErrorCode);

	if (EventHandler::getSingleton().onLoginZoneError != NULL) {
		EventHandler::getSingleton().onLoginZoneError(*ptrErrorCode, *ptrErrorMessage);
	}
	string message = "OnSmartFoxLoginZoneError: " + *ptrErrorMessage;
	CCLOG("%d %s", *ptrErrorCode, message.c_str());
}

void SFSGEvent::OnLogoutZone(map<string, boost::shared_ptr<void>> params)
{
	CCLOG("OnSmartFoxLogoutZone");
	Utils::getSingleton().logoutZone();
	if (EventHandler::getSingleton().onLogoutZone != NULL) {
		EventHandler::getSingleton().onLogoutZone();
	}
}

void SFSGEvent::OnJoinRoom(map<string, boost::shared_ptr<void>> params)
{
	//room joined
	boost::shared_ptr<void> ptrEventParamValueRoom = params["room"];
	boost::shared_ptr<Room> ptrNotifiedRoom = ((boost::static_pointer_cast<Room>))(ptrEventParamValueRoom);

	Utils::getSingleton().currentRoomId = ptrNotifiedRoom->Id();
	Utils::getSingleton().currentRoomName = *(ptrNotifiedRoom->Name());
	CCLOG("RoomJoin: %d %s", ptrNotifiedRoom->Id(), ptrNotifiedRoom->Name()->c_str());
	if (EventHandler::getSingleton().onJoinRoom != NULL) {
		EventHandler::getSingleton().onJoinRoom(ptrNotifiedRoom->Id(), *(ptrNotifiedRoom->Name()));
	}
}

void SFSGEvent::OnJoinRoomError(map<string, boost::shared_ptr<void>> params)
{
	boost::shared_ptr<void> ptrEventParamValueErrorMessage = params["errorMessage"];
	boost::shared_ptr<string> ptrErrorMessage = ((boost::static_pointer_cast<string>))(ptrEventParamValueErrorMessage);

	CCLOG("RoomJoinError: %s", (*ptrErrorMessage).c_str());
	if (EventHandler::getSingleton().onJoinRoomError != NULL) {
		EventHandler::getSingleton().onJoinRoomError(*ptrErrorMessage);
	}
}

void SFSGEvent::OnUserExitRoom(map<string, boost::shared_ptr<void>> params)
{
	boost::shared_ptr<void> ptrEventParamValueUser = params["user"];
	boost::shared_ptr<User> ptrNotifiedUser = ((boost::static_pointer_cast<User>))(ptrEventParamValueUser);

	//get the room parameter of the event
	boost::shared_ptr<void> ptrEventParamValueRoom = params["room"];
	boost::shared_ptr<Room> ptrNotifiedRoom = ((boost::static_pointer_cast<Room>))(ptrEventParamValueRoom);

	//CCLOG("%s leaved room %s", ptrNotifiedUser->Name()->c_str(), ptrNotifiedRoom->Name()->c_str());
	if (EventHandler::getSingleton().onUserExitRoom != NULL) {
		EventHandler::getSingleton().onUserExitRoom(ptrNotifiedUser->Id());
	}
}

void SFSGEvent::OnUserEnterRoom(map<string, boost::shared_ptr<void>> params)
{
	boost::shared_ptr<void> ptrEventParamValueUser = params["user"];
	boost::shared_ptr<User> ptrNotifiedUser = ((boost::static_pointer_cast<User>))(ptrEventParamValueUser);

	//get the room parameter of the event
	boost::shared_ptr<void> ptrEventParamValueRoom = params["room"];
	boost::shared_ptr<Room> ptrNotifiedRoom = ((boost::static_pointer_cast<Room>))(ptrEventParamValueRoom);

	//CCLOG("%s enter room %s", ptrNotifiedUser->Name()->c_str(), ptrNotifiedRoom->Name()->c_str());
}

void SFSGEvent::OnExtensionResponse(map<string, boost::shared_ptr<void>> params)
{
	boost::shared_ptr<void> ptrEventParamValueCmd = params["cmd"];
	boost::shared_ptr<string> ptrNotifiedCmd = ((boost::static_pointer_cast<string>)(ptrEventParamValueCmd));

	boost::shared_ptr<void> ptrEventParamValueParams = params["params"];
	boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject = ((boost::static_pointer_cast<ISFSObject>)(ptrEventParamValueParams));

	CCLOG("OnExtensionResponse::cmd::%s", (*ptrNotifiedCmd).c_str());
	/*vector<string> vt = *(ptrNotifiedISFSObject->GetKeys());
	for (string s : vt) {
	CCLOG("%s", s.c_str());
	}*/
	SFSResponse::getSingleton().onExtensionResponse(ptrNotifiedCmd, ptrNotifiedISFSObject);
}

void SFSGEvent::OnPublicMessage(map<string, boost::shared_ptr<void>> params)
{
	boost::shared_ptr<void> ptrEventParamValueUser = params["sender"];
	boost::shared_ptr<User> ptrNotifiedUser = ((boost::static_pointer_cast<User>))(ptrEventParamValueUser);

	//get the message parameter of the event
	boost::shared_ptr<void> ptrEventParamValueMsg = params["message"];
	boost::shared_ptr<string> ptrNotifiedMsg = ((boost::static_pointer_cast<string>))(ptrEventParamValueMsg);

	if (EventHandler::getSingleton().onPublicMessage != NULL) {
		EventHandler::getSingleton().onPublicMessage(ptrNotifiedUser->Id(), *ptrNotifiedMsg);
	}
}

void SFSGEvent::OnPingPong(map<string, boost::shared_ptr<void>> params)
{
	boost::shared_ptr<void> ptrEventParamValue = params["lagValue"];
	boost::shared_ptr<long> ptrValue = ((boost::static_pointer_cast<long>))(ptrEventParamValue);
	//CCLOG("Measured lag is: %dms", *ptrValue);
	if (EventHandler::getSingleton().onPingPong != NULL) {
		EventHandler::getSingleton().onPingPong(*ptrValue);
	}
}

void SFSGEvent::OnConnectionException(std::exception e)
{
    if(EventHandler::getSingleton().onConnectionException != NULL){
        EventHandler::getSingleton().onConnectionException();
    }
}
