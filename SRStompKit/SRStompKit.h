//
//  SRStompKit.h
//  SocketDemo
//
//  Created by JimmyOu on 2018/6/1.
//  Copyright © 2018年 JimmyOu. All rights reserved.
//

#import <Foundation/Foundation.h>

#pragma mark Frame headers

#define kHeaderAcceptVersion @"accept-version"
#define kHeaderAck           @"ack"
#define kHeaderContentLength @"content-length"
#define kHeaderDestination   @"destination"
#define kHeaderHeartBeat     @"heart-beat"
#define kHeaderHost          @"host"
#define kHeaderID            @"id"
#define kHeaderLogin         @"login"
#define kHeaderMessage       @"message"
#define kHeaderPasscode      @"passcode"
#define kHeaderReceipt       @"receipt"
#define kHeaderReceiptID     @"receipt-id"
#define kHeaderSession       @"session"
#define kHeaderSubscription  @"subscription"
#define kHeaderTransaction   @"transaction"

#pragma mark Ack Header Values

#define kAckAuto             @"auto"
#define kAckClient           @"client"
#define kAckClientIndividual @"client-individual"

@class STOMPFrame;
@class STOMPMessage;

typedef void (^STOMPFrameHandler)(STOMPFrame *frame);
typedef void (^STOMPMessageHandler)(STOMPMessage *message);

#pragma mark STOMP Frame

@interface STOMPFrame : NSObject

@property (nonatomic, copy, readonly) NSString *command;
@property (nonatomic, copy, readonly) NSDictionary *headers;
@property (nonatomic, copy, readonly) NSString *body;
- (NSString *)toString;

@end

#pragma mark STOMP Message

@interface STOMPMessage : STOMPFrame

- (void)ack;
- (void)ack:(NSDictionary *)theHeaders;
- (void)nack;
- (void)nack:(NSDictionary *)theHeaders;

@end

#pragma mark STOMP Subscription

@interface STOMPSubscription : NSObject

@property (nonatomic, copy, readonly) NSString *identifier;

- (void)unsubscribe;

@end

#pragma mark STOMP Transaction

@interface STOMPTransaction : NSObject

@property (nonatomic, copy, readonly) NSString *identifier;

- (void)commit;
- (void)abort;

@end

@protocol STOMPClientDelegate

@optional
- (void) websocketDidDisconnect: (NSError *)error;
@end


@interface STOMPClient : NSObject
//execute on mainThread
@property (nonatomic, copy) STOMPFrameHandler receiptHandler;
//socket被中断：execute on mainThread
@property (nonatomic, copy) void (^errorHandler)(NSError *error);
@property (nonatomic, assign) BOOL connected;
@property (nonatomic, readonly) BOOL heartbeatActivated;
@property (nonatomic, weak) NSObject<STOMPClientDelegate> *delegate;

/**
 初始化

 @param theUrl url
 @param headers connected的头部信息
 @param heartbeat 是否发送心跳
 */
- (id)initWithURL:(NSURL *)theUrl webSocketHeaders:(NSDictionary *)headers useHeartbeat:(BOOL)heartbeat;

/**
 以 login 和password 的形式connected
 @param login loginStr
 @param passcode passCodeStr
 @param completionHandler completionHandler execute on mainThread
 */
- (void)connectWithLogin:(NSString *)login
                passcode:(NSString *)passcode
       completionHandler:(void (^)(STOMPFrame *connectedFrame, NSError *error))completionHandler;

/**
 以自定义头的形式connected

 @param headers customs headers
 @param completionHandler completionHandler execute on mainThread
 */
- (void)connectWithHeaders:(NSDictionary *)headers
         completionHandler:(void (^)(STOMPFrame *connectedFrame, NSError *error))completionHandler;

/**
 给某个destination发送message
 @param destination destination
 @param body messageBody
 */
- (void)sendTo:(NSString *)destination
          body:(NSString *)body;

/**
 给某个destination发送message

 @param destination destination
 @param headers custom headers
 @param body messageBody
 */
- (void)sendTo:(NSString *)destination
       headers:(NSDictionary *)headers
          body:(NSString *)body;

/**
 订阅某个 destination 的 message

 @param destination destination
 @param handler handler
 @return 订阅对象
 */
- (STOMPSubscription *)subscribeTo:(NSString *)destination
                    messageHandler:(STOMPMessageHandler)handler;
/**
 订阅某个 destination 的 message
 
 @param destination destination
 @param handler handler
 @return 订阅对象。 STOMPSubscription进行取消订阅。
 */
- (STOMPSubscription *)subscribeTo:(NSString *)destination
                           headers:(NSDictionary *)headers
                    messageHandler:(STOMPMessageHandler)handler;


/**
 开始一个事务
 @return 事务对象 transcation commmit进行事务提交
 */
- (STOMPTransaction *)begin;
- (STOMPTransaction *)begin:(NSString *)identifier;

/**
 手动断开链接
 */
- (void)disconnect;
- (void)disconnect:(void (^)(NSError *error))completionHandler;

@end


