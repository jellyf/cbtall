//
//  IOSHelper.h
//  TestSmartfox
//
//  Created by thanhphonglan2003 on 12/9/16.
//
//

#ifndef IOSHelper_h
#define IOSHelper_h

#import "IOSHelperCPlus.h"
#import <UIKit/UIKit.h>

@interface IOSNDKHelper : NSObject

+ (void) SetNDKReciever:(NSObject*)reciever;
+ (void) OnLoginFacebookCallback:(NSString*)token;
+ (void) OnPurchaseSuccess:(NSString*)token;
+ (void) OnFacebookInvite:(NSString*)token;

@end

@interface NSDictionary (BVJSONString)
-(NSString*) bv_jsonStringWithPrettyPrint:(BOOL) prettyPrint;
@end

#endif /* IOSHelper_h */
