#pragma once
#include "cocos2d.h"

struct ErrorData
{
	unsigned char Code;
	std::string Message;
};

struct ZoneData
{
	unsigned char Id;
	std::string ZoneName;
	std::string ZoneIpIos;
	std::string ZoneIp;
	long ZonePort;
	unsigned char Status;
	unsigned char Money;
};

struct UserData
{
	long UserID;
	long SfsUserId;
	long Exp;
	long Win;
	long Total;
	short Level;
	double MoneyFree;
	double MoneyReal;
	unsigned char MoneyType;
	unsigned char Device;
	unsigned char IsActived;
	std::string Name;
	std::string DisplayName;
	std::string GroupAvatar;
};

struct PlayerData
{
	UserData Info;
	bool Ready;
	unsigned char Index;
	short UType;
	std::string Ip;
	double TMoney;
	double PMoney;
	std::vector<char> SingleCards;
	std::vector<std::vector<char>> PairCards;
};

struct RoomData
{
	unsigned char TimeStart;
	unsigned char TimeDeal;
	unsigned char TimeChooseHost;
	std::vector<PlayerData> Players;
};

struct CardHandData
{
	std::vector<unsigned char> ThienKhai;
	std::vector<unsigned char> Chan;
	std::vector<unsigned char> BaDau;
	std::vector<unsigned char> Ca;
	std::vector<unsigned char> Que;
};

struct PlayerCardHandData
{
	long UId;
	CardHandData CardHand;
};

struct StartGameData
{
	unsigned char CardStilt;
	long LastWinner;
	long CurrentTurn;
	bool CanWin;
	CardHandData CardHand;
};

struct BashData
{
	long UId;
	long TurnId;
	bool CanPenet;
	bool CanPenetWin;
	unsigned char CardId;
	unsigned char SoundId;
	CardHandData CardHand;
};

struct BashBackData
{
	long UId;
	long TurnId;
	long BackId;
	bool IsPicked;
	bool CanPenet;
	bool CanPenetWin;
	unsigned char Status;
	unsigned char CardId;
	unsigned char SoundId;
	CardHandData CardHand;
};

struct HoldData
{
	long UId;
	long TurnId;
	bool IsPicked;
	unsigned char CardId;
	unsigned char CardIdHold;
	unsigned char SoundId;
	CardHandData CardHand;
};

struct PickData
{
	long UId;
	long TurnId;
	bool CanPenet;
	bool CanWin;
	unsigned char CardId;
	unsigned char SoundId;
	unsigned char StiltCount;
};

struct ForwardData
{
	long UId;
	long TurnId;
	bool CanPenet;
	bool CanPenetWin;
	unsigned char CardId;
	unsigned char SoundId;
};

struct PenetData
{
	long UId;
	bool IsPicked;
	unsigned char CardId;
	unsigned char SoundId;
	CardHandData CardHand;
};

struct CrestResponseData
{
	long UId;
	short Ga;
	short Diem;
	long DenLangId;
	unsigned char Status;
	std::vector<unsigned char> CuocHo;
	std::vector<unsigned char> CuocSai;
	std::string Msg;
	std::string MsgAnGa;
	std::vector<long> ListUDe;
};

struct EndMatchData
{
	long WinId;
	unsigned char SoundId;
	std::vector<unsigned char> ListChanU;
	std::vector<unsigned char> ListCaU;
	std::vector<unsigned char> ListCardU;
	std::vector<unsigned char> ListHandCardU;
	std::vector<unsigned char> ListStiltCard;
	std::vector<PlayerCardHandData> ListPlayerCard;
};

struct EndMatchMoneyData
{
	std::vector<long> ListUserId;
	std::vector<double> ListUserAmount;
};

struct LobbyPlayerData
{
	long UserId;
	double Money;
	std::string Name;
};

struct LobbyTableData
{
	long Money;
	bool IsAdvance;
	bool IsU411;
	unsigned char HasPot;
	unsigned char Time;
	unsigned char RoomId;
	std::vector<LobbyPlayerData> ListPlayer;
};

struct LobbyListTable
{
	unsigned char Size;
	long Money;
	std::vector<LobbyTableData> ListTable;
};

struct LobbyRoomType
{
	unsigned char Id;
	std::string Group;
	std::string Name;
	std::string Description;
};

struct LobbyListRoomType
{
	bool IsRealMoney;
	std::vector<LobbyRoomType> ListRoomType;
};

struct GameTableData
{
	long Money;
	unsigned char Time;
	bool IsAdvance;
	bool IsU411;
	bool IsQuick;
};

struct InviteData
{
	std::string InviterName;
	long InviterId;
	double Money;
	unsigned char RoomTime;
	std::string Msg;
	std::string RoomName;
	std::string Password;
};

struct PlayingTableData
{
	bool IsReconnect;
	long TurnId;
	unsigned char StiltCount;
	std::vector<PlayerData> Players;
};

struct GameConfig
{
	bool paymentEnabled;
	bool paymentEnabledIOS;
	bool canUpdate;
	int port;
	int websocket;
	int version;
	int versionIOS;
	std::string zone;
	std::string host;
	std::string ip_rs;
	std::string phone;
	std::string smsVT;
	std::string smsVNPVMS;
	std::string smsKH;
	std::string smsMK;
	std::string linkFb;
	std::string linkAndroid;
	std::string linkIOS;
    std::string inapp;
};

struct RankData
{
	std::string Name;
	double Money;
	long Uid;
	long Exp;
};

struct RankWinData
{
	long Uid;
	long Point;
	std::string Name;
	std::string Cuoc;
	std::string Date;
};

struct PlayLogData
{
	std::string Date;
	std::string Info;
	double Balance;
	long Money;
	long GameId;
};

struct GameReconnectData
{
	unsigned char PlayerState;
	short IndexCard;
	CardHandData CardHand;
};

struct UserReconnectData
{
	long UserId;
	long SfsUserId;
	std::string Name;
	std::string DisplayName;
};

struct ShopHistoryData
{
	long UserId;
	unsigned char Id;
	unsigned char ItemId;
	unsigned char Price;
	unsigned char Status;
	std::string Name;
	std::string CreateDate;
	std::string UpdateDate;
	std::string Content;
};

struct ShopItemData
{
	unsigned char Id;
	std::string Name;
	std::string Image;
	long Price;
	long PriceChange;
	short ItemType;
	short IsActived;
};

struct MailData
{
	long Id;
	std::string Sender;
	std::string Date;
	std::string Title;
	bool IsRead;
};

struct NewsData
{
	unsigned char Id;
	unsigned char Type;
	std::string Title;
	std::string Content;
	std::string Image;
	std::string Date;
};

struct EventData
{
	unsigned char Id;
	std::string Content;
	std::string Date;
};

struct TableReconnectData
{
	std::string Room;
	std::string Message;
};

struct ProductData
{
    double Price;
    std::string Id;
    std::string Description;
    std::string CurrencySymbol;
};
