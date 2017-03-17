#pragma once
#include "cocos2d.h"
#include "SingLeton.h"

class Tracker : public SingLeton<Tracker>
{
public:
	Tracker();
	~Tracker();
	static Tracker& getSingleton(void);
	static Tracker* getSingletonPtr(void);

	void trackActiveAccount();
	void trackRegistration(std::string method);
	void trackPurchaseSuccess(std::string type, std::string id, std::string currency, double price);
};

#define __DECLARE_STRING_PROPERTY(name, value) const std::string name(value);
#define __DECLARE_INT_PROPERTY(name, value) const int name(value);
namespace tracker
{
	__DECLARE_STRING_PROPERTY(REGISTER_METHOD_NORMAL, "Normal");
	__DECLARE_STRING_PROPERTY(REGISTER_METHOD_FACEBOOK, "Facebook");
}