#pragma once
#include "BaseScene.h"

class UserNode;
enum GameState {
	WAIT,
	NONE,
	READY,
	START,
	DEAL,
	CHOOSE_STILT,
	CHOOSE_HOST,
	PLAY,
	CREST,
	ENDING
};
class GameScene : public BaseScene
{
public:
	CREATE_FUNC(GameScene);
	virtual void onInit();
	virtual void registerEventListenner();
	virtual void unregisterEventListenner();
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* _event);

	virtual void onApplicationDidEnterBackground();
    virtual void onApplicationWillEnterForeground();

	void onConnectionFailed();
	void onJoinRoom(long roomId, std::string roomName);
	void onUserDataResponse(UserData data);
	void onUserExitRoom(long sfsUId);
	void onPublicMessage(long uid, std::string msg);
	void onRoomDataResponse(RoomData roomData);
	void onRoomDataGaResponse(bool isGa, double gaMoney);
	void onStartGameDataResponse(StartGameData data);
	void onChooseStilt(unsigned char stilt);
	void onChooseHost(unsigned char stilt1, unsigned char stilt2, unsigned char host);
	void onUserBash(BashData data);
	void onUserBashBack(BashBackData data);
	void onUserHold(HoldData data);
	void onUserPick(PickData data);
	void onUserPenet(PenetData data);
	void onUserForward(ForwardData data);
	void onUserWin(long uId, unsigned char sound);
	void onCrestResponse(CrestResponseData data);
	void onEndMatch(EndMatchData data);
	void onEndMatchMoney(EndMatchMoneyData data);
	void onEndMatchTie(std::vector<unsigned char> stiltCards);
	void onPunishResponse(long UiD, std::string msg);
	void onUserReadyResponse(long UiD, bool isReady);
	void onTableResponse(GameTableData data);
	void onLobbyUserResponse(std::vector<UserData> listUser);
	void onGamePlayingDataResponse(PlayingTableData data);
	void onGameSpectatorDataResponse(std::vector<PlayerData> spectators);
	void onGameMyReconnectDataResponse(GameReconnectData data);
	void onGameUserReconnectDataResponse(std::vector<UserReconnectData> list);
	void onLobbyListTableResponse(LobbyListTable data);
	void onTableReconnectDataResponse(TableReconnectData data);
	void onTourRoomMatch(long totalMatch);
	void onTourTimeWaitPlayer(long timeWait);
protected:
	virtual bool onKeyBack();
	virtual void onKeyHome();
	virtual void onBackScene();
	virtual bool onConnectionLost(std::string reason);
	virtual bool onErrorResponse(unsigned char code, std::string msg);
	virtual void joinIntoTour();
private:
	void reset();
	void initChatTable();
	void initCrestTable();
	void initEndMatchTable();
	void initInviteTable();
	void initSettingsPopup();
	void initTableInfo();
	void initCofferEffects();

	bool bashCardDown(int from, int to, int cardId, bool isMe);
	void autoBash(int card, int group);

	void onUserBashToMe(BashData data);
	void onUserBashBackToMe(BashBackData data);
	void onUserPickToMe(PickData data);

	void dealCards();
	void dropWin();
    void dropPenet();
	void disconnectToSync();
	void processWaitAction();
	void syncHandCard(CardHandData cardHand);
	void showMyCards(bool runEffect = true);
	void runTimeWaiting(long uid, float time);
	void getCardSpriteToHand(int id, int group, int zorder);
	void updateCardHand(CardHandData handData);
	void showStiltCards();
	void showWinnerCards();
	void showSettings();
	void showError(std::string msg);
	void showSystemNotice(std::string msg);
	void showCofferEffects(std::string money);
	void showMenuButtons();
	void hideMenuButtons();
	void updateTourMatch();
	void playSoundAction(unsigned char soundId);
	void playSoundCuoc(unsigned char cuocId);
	void changeZOrderAfterFly(cocos2d::Sprite* card, int zorder);
	void beatenNodeAndHide(cocos2d::Node* node, float scale1, float scale2, float timeToBeaten, float timeToHide);
	void getCardCount(CardHandData cardHand, std::vector<int>& counts);
	bool isCardHandDataSync(CardHandData cardHand);
	int getCardName(unsigned char cardId);
	int getNextPlayerIndexFrom(int index);
	cocos2d::Sprite* getCardSprite(int id);
	cocos2d::Vec2 getScaleScenePosition(cocos2d::Vec2 pos);
	cocos2d::Vec2 getScaleSceneDistance(cocos2d::Vec2 pos);

	std::map<long, int> userIndexs;
	std::map<long, int> userIndexs2;
	std::vector<UserNode*> vecUsers;
	std::vector<std::string> vecChats;
	std::vector<std::string> vecSoundActions;
	std::vector<std::string> vecSoundCuocs;
	std::vector<cocos2d::Vec2> vecUserPos;
	std::vector<cocos2d::Vec2> tableCardPos;
	std::vector<cocos2d::Vec2> tableCardDistance;
	std::vector<cocos2d::Vec2> dealPos;
	std::vector<cocos2d::Vec2> handPos;
	std::vector<cocos2d::Node*> vecStilts;
	std::vector<cocos2d::Label*> lbSanSangs;
	std::vector<cocos2d::Label*> lbBatBaos;
	std::vector<cocos2d::Label*> lbWinMoneys;
	std::vector<cocos2d::Label*> lbTourPoints;
	std::vector<cocos2d::Sprite*> spCards;
	std::vector<cocos2d::Sprite*> spHandCards;
	std::vector<cocos2d::Sprite*> spDealCards;
	std::vector<cocos2d::Sprite*> spSanSangs;
	//std::vector<cocos2d::Sprite*> spBatBaos;
	std::vector<cocos2d::Sprite*> spInvites;
	std::vector<cocos2d::Sprite*> runningCards;
	std::vector<cocos2d::ui::Button*> vecCrests;
	std::vector<cocos2d::ui::Button*> vecMenuBtns;
	std::vector<cocos2d::ui::Button*> vecBtnKicks;
	std::vector<char> tableCardNumb;
	std::vector<char> maxTableCardNumb;
	std::vector<char> chosenCuocs;
	std::vector<char> chosenCuocNumbs;
	std::vector<char> maxChosenCuocs;
	cocos2d::Layer* playLayer;
	cocos2d::Layer* endLayer;
	cocos2d::ProgressTimer* progressTimer;
	cocos2d::ui::Button* btnUp;
	cocos2d::ui::Button* btnDown;
	cocos2d::ui::Button* btnBack;
	cocos2d::ui::Button* btnChat;
	cocos2d::ui::Button* btnSettings;
	cocos2d::ui::Button* btnReady;
	cocos2d::ui::Button* btnCancelReady;
	cocos2d::ui::Button* btnBash;
	cocos2d::ui::Button* btnBashBack;
	cocos2d::ui::Button* btnForward;
	cocos2d::ui::Button* btnHold;
	cocos2d::ui::Button* btnPick;
    cocos2d::ui::Button* btnPenet;
    cocos2d::ui::Button* btnDropPenet;
	cocos2d::ui::Button* btnWin;
	cocos2d::ui::Button* btnDropWin;
	cocos2d::ui::Button* btnXemNoc;
	cocos2d::ui::Button* btnDongNoc;
	cocos2d::ui::Button* btnCrest;
	cocos2d::ui::Scale9Sprite* gameSplash;
	cocos2d::ui::Scale9Sprite* cofferSplash;
	cocos2d::Sprite* hostCard = NULL;
	cocos2d::Sprite* runningSpCard;
	cocos2d::Sprite* iconGa;
	cocos2d::Sprite* spChuPhong;
	//cocos2d::Sprite* spChonCai;
	cocos2d::Label* lbChonCai;
	cocos2d::Label* lbError;
	cocos2d::Label* lbNoticeAction;
	cocos2d::Label* lbCardNoc;
	cocos2d::Label* lbWinner;
	cocos2d::Label* lbCuocSai;
	cocos2d::Label* lbDiem;
	cocos2d::Label* lbNoted;
	cocos2d::Label* lbNotedGa;
	cocos2d::Label* lbCrestTime;
	cocos2d::Label* lbChonCuoc;
	cocos2d::Label* lbMoneyGa;
	cocos2d::Label* lbCrestWin;
	cocos2d::Node* tableCrest;
	cocos2d::Node* tableEndMatch;
	cocos2d::Node* tableInvite;
	cocos2d::Node* tableInfo;
	cocos2d::Node* tableChat;
	cocos2d::Node* popupSettings;
	cocos2d::Node* nodeWebview;
	cocos2d::Node* nodeStilt;
	cocos2d::Node* cofferEffect;
	cocos2d::Texture2D* textureHu;
	cocos2d::Texture2D* textureLight;

	int timeStart;
	int timeDeal;
	int timeChooseHost;
	int timeTurn;
	int myServerSlot;
	int chosenStilt = -1;
	int chosenStiltHost = -1;
	int chosenHost = -1;
	int chosenCard = -1;
	int noaction = 0;
	int soundDealId = -1;
	int waitAction = -1;
	float cardSpeed = .35f;
	float cardScaleTable = .85f;
	float cardScaleTableNew = .9f;
    double pauseTimeInSecs = 0;
	double tourTimeRemain = 0;
	long timeWaitPlayer = 0;
	long playIdMe;
	long sfsIdMe;
	bool isBatBao;
	bool hasClickInvite = false;
	bool hasRegisterOut = false;
	bool isKickForNotReady = false;
	bool isKickForNotEnoughMoney = false;
	bool isPause = false;
	bool isU411 = false;
	bool mustGoToLobby = false;
	bool isSolo = false;
	bool isAutoReady = false;
	bool isSynchronizing = false;
	bool isAutoBash = false;
	bool isTourGame = false;
	bool isMatchTimeEnd = false;
	bool isPrepareToTour = false;
	GameState state;
	StartGameData startGameData;
	CardHandData myCardHand;
	EndMatchData endMatchData;
	EndMatchMoneyData winMoneyData;
	PlayerData playerMe;
	std::vector<PlayerData> vecPlayers;
	std::vector<PlayerData> vecSpectators;
	//std::string tmpMessage;
};

