/****************************************************************************
 Copyright (c) 2013      cocos2d-x.org
 Copyright (c) 2013-2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#import "RootViewController.h"
#import "cocos2d.h"
#import "platform/ios/CCEAGLView-ios.h"
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
#import "IOSHelper.h"
#import "Data.h"
#import "Utils.h"

@implementation RootViewController

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
    cocos2d::Application *app = cocos2d::Application::getInstance();
    
    // Initialize the GLView attributes
    app->initGLContextAttrs();
    cocos2d::GLViewImpl::convertAttrs();
    
    // Initialize the CCEAGLView
    CCEAGLView *eaglView = [CCEAGLView viewWithFrame: [UIScreen mainScreen].bounds
                                         pixelFormat: (__bridge NSString *)cocos2d::GLViewImpl::_pixelFormat
                                         depthFormat: cocos2d::GLViewImpl::_depthFormat
                                  preserveBackbuffer: NO
                                          sharegroup: nil
                                       multiSampling: NO
                                     numberOfSamples: 0 ];
    
    // Enable or disable multiple touches
    [eaglView setMultipleTouchEnabled:NO];
    
    // Set EAGLView as view of RootViewController
    self.view = eaglView;
    
    cocos2d::GLView *glview = cocos2d::GLViewImpl::createWithEAGLView((__bridge void *)self.view);
    
    //set the GLView as OpenGLView of the Director
    cocos2d::Director::getInstance()->setOpenGLView(glview);
    
    //run the cocos2d-x game scene
    app->run();
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
    [IOSNDKHelper SetNDKReciever:self];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
}


// For ios6, use supportedInterfaceOrientations & shouldAutorotate instead
#ifdef __IPHONE_6_0
- (NSUInteger) supportedInterfaceOrientations{
    return UIInterfaceOrientationMaskAllButUpsideDown;
}
#endif

- (BOOL) shouldAutorotate {
    return YES;
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {
    [super didRotateFromInterfaceOrientation:fromInterfaceOrientation];

    auto glview = cocos2d::Director::getInstance()->getOpenGLView();

    if (glview)
    {
        CCEAGLView *eaglview = (__bridge CCEAGLView *)glview->getEAGLView();

        if (eaglview)
        {
            CGSize s = CGSizeMake([eaglview getWidth], [eaglview getHeight]);
            cocos2d::Application::getInstance()->applicationScreenSizeChanged((int) s.width, (int) s.height);
        }
    }
}

//fix not hide status on ios7
- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)loginFacebook{
    if ([FBSDKAccessToken currentAccessToken]) {
        [IOSNDKHelper OnLoginFacebookCallback:[[FBSDKAccessToken currentAccessToken] tokenString]];
    }else{
        FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];
        [login
         logInWithReadPermissions: @[@"public_profile"]
         fromViewController:self
         handler:^(FBSDKLoginManagerLoginResult *result, NSError *error) {
             if (error) {
                 NSLog(@"Process error");
                 [IOSNDKHelper OnLoginFacebookCallback:@""];
             } else if (result.isCancelled) {
                 NSLog(@"Cancelled");
                 [IOSNDKHelper OnLoginFacebookCallback:@"cancel"];
             } else {
                 NSLog(@"Logged in");
                 [IOSNDKHelper OnLoginFacebookCallback:[[FBSDKAccessToken currentAccessToken] tokenString]];
             }
         }];
    }
}

- (void)logoutFacebook{
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];
    [login logOut];
}

- (void)purchaseItem:(NSString*)sku{
    for(SKProduct *product in self.products){
        if([product.productIdentifier isEqualToString:sku]){
            [self purchaseProduct:product];
            break;
        }
    }
}

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];

    // Release any cached data, images, etc that aren't in use.
}

-(void)queryIAPProducts:(NSSet*)productIdentifiers{
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
    productsRequest.delegate = self;
    [productsRequest start];
}

- (BOOL)canMakePurchases
{
    return [SKPaymentQueue canMakePayments];
}
- (void)purchaseProduct:(SKProduct*)product{
    if ([self canMakePurchases]) {
        SKPayment *payment = [SKPayment paymentWithProduct:product];
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    }
    else{
        UIAlertView *alertView = [[UIAlertView alloc]initWithTitle:
                                  @"Purchases are disabled in your device" message:nil delegate:
                                  self cancelButtonTitle:@"Ok" otherButtonTitles: nil];
        [alertView show];
    }
}

#pragma mark StoreKit Delegate

-(void)paymentQueue:(SKPaymentQueue *)queue
updatedTransactions:(NSArray *)transactions {
    for (SKPaymentTransaction *transaction in transactions) {
        switch (transaction.transactionState) {
            case SKPaymentTransactionStatePurchasing:
                NSLog(@"Purchasing");
                break;
            case SKPaymentTransactionStateDeferred:
                NSLog(@"Deferred");
                break;
            case SKPaymentTransactionStatePurchased:
                NSLog(@"Purchased");
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                [IOSNDKHelper OnPurchaseSuccess:transaction.transactionReceipt.base64Encoding];
                break;
            case SKPaymentTransactionStateRestored:
                NSLog(@"Restored ");
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                NSLog(@"Purchase failed ");
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                [IOSNDKHelper OnPurchaseSuccess:@""];
                break;
            default:
                break;
        }
    }
}

-(void)productsRequest:(SKProductsRequest *)request
    didReceiveResponse:(SKProductsResponse *)response
{
    if (response.products.count > 0) {
        NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
        [formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [formatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        
        std::vector<ProductData> vecProducts;
        self.products = response.products;
        for(SKProduct *p in self.products){
            [formatter setLocale:p.priceLocale];
            ProductData data;
            data.Price = p.price.doubleValue;
            data.Id = p.productIdentifier.UTF8String;
            data.Description = p.localizedDescription.UTF8String;
            data.CurrencySymbol = formatter.currencySymbol.UTF8String;
            vecProducts.push_back(data);
        }
        Utils::getSingleton().setIAPProducts(vecProducts);
        [formatter release];
    } else {
//        UIAlertView *tmp = [[UIAlertView alloc]
//                            initWithTitle:@"Not Available"
//                            message:@"No products to purchase"
//                            delegate:self
//                            cancelButtonTitle:nil
//                            otherButtonTitles:@"Ok", nil];
//        [tmp show];
    }
    
//    for (NSString *invalidProductId in response.invalidProductIdentifiers)
//    {
//        NSLog(@"Invalid product id: %@" , invalidProductId);
//    }
}

@end
