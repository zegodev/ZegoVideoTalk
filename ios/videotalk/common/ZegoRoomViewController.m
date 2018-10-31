//
//  ZegoRoomViewController.m
//  
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import "ZegoRoomViewController.h"
#import "ZegoRoomInfo.h"
#import "ZegoSetting.h"

NSString *const zegoDomain      = @"zego.im";
NSString *const alphaBaseUrl    = @"https://alpha-liveroom-api.zego.im";
NSString *const testBaseUrl     = @"https://test2-liveroom-api.zego.im";

@implementation ZegoRoomTableViewCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self)
    {
        self.textLabel.lineBreakMode = NSLineBreakByTruncatingTail;
    }
    return self;
}

@end

@interface ZegoRoomViewController ()

@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray<ZegoRoomInfo *> *roomList;
@property (nonatomic, strong) UIRefreshControl *refreshControl;

@end

@implementation ZegoRoomViewController

#pragma mark - Life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
   
    [self.refreshControl addTarget:self action:@selector(handleRefresh:) forControlEvents:UIControlEventValueChanged];
    [self.tableView addSubview:self.refreshControl];

//    self.navigationItem.title = @"ZEGO";
//    UIBarButtonItem *rightButtonItem = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"刷新", nil)
//                                                                        style:UIBarButtonItemStylePlain
//                                                                       target:self
//                                                                       action:@selector(onRightBarButton:)];
//    self.navigationItem.rightBarButtonItem = rightButtonItem;
//    
    // tableViewCell 配置
    [self.tableView registerClass:[ZegoRoomTableViewCell class] forCellReuseIdentifier:@"zegoRoomCellID"];
    self.tableView.dataSource = self;
    self.tableView.delegate = self;
    self.tableView.tableFooterView = [[UIView alloc] init];
    
    // 房间页加载即刷新房间信息
    [self getLiveRoom];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationActive:) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onRoomInstanceClear:) name:@"RoomInstanceClear" object:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Event response

- (void)handleRefresh:(UIRefreshControl *)sender {
    [self.roomList removeAllObjects];
    [self getLiveRoom];
}

- (void)onApplicationActive:(NSNotification *)notification {
    [self handleRefresh:self.refreshControl];
}

- (void)onRoomInstanceClear:(NSNotification *)notification {
    [self getLiveRoom];
}

#pragma mark - Public

- (void)refreshRoomList {
    if ([self.refreshControl isRefreshing]) {
        return;
    }
    [self.roomList removeAllObjects];
    [self getLiveRoom];
}

#pragma mark - Private

- (void)getLiveRoom {
    [self.refreshControl beginRefreshing];
    
    NSString *baseUrl = nil;
    if ([ZegoSetting sharedInstance].useAlphaEnv) {
        baseUrl = @"https://alpha-liveroom-api.zego.im";
    } else if ([ZegoSetting sharedInstance].useTestEnv) {
        baseUrl = @"https://test2-liveroom-api.zego.im";
    } else {
        baseUrl = [NSString stringWithFormat:@"https://liveroom%u-api.%@", [ZegoSetting sharedInstance].appID, zegoDomain];
    }
    
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/demo/roomlist?appid=%u", baseUrl, [ZegoSetting sharedInstance].appID]];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    
    NSLog(@"[GetLiveRoom] url: %@", url.absoluteString);
    
    NSURLSessionConfiguration *configuration = [NSURLSessionConfiguration defaultSessionConfiguration];
    configuration.timeoutIntervalForRequest = 10;
    
    NSURLSession *session = [NSURLSession sessionWithConfiguration:configuration];
    NSURLSessionDataTask *task = [session dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if ([self.refreshControl isRefreshing]) {
                [self.refreshControl endRefreshing];
            }
            
            if ([self.delegate respondsToSelector:@selector(onRefreshRoomListFinished)]) {
                [self.delegate onRefreshRoomListFinished];
            }
            [self.roomList removeAllObjects];
            
            if (error) {
                NSLog(@"[GetLiveRoom] error: %@", error);
                return;
            }
            
            if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
                NSError *jsonError;
                NSDictionary *jsonResponse = [NSJSONSerialization JSONObjectWithData:data options:0 error:&jsonError];
                if (jsonError) {
                    NSLog(@"[GetLiveRoom] parsing json error");
                    return;
                } else {
                    NSLog(@"[GetLiveRoom] response: %@", jsonResponse);
                    NSUInteger code = [jsonResponse[@"code"] integerValue];
                    if (code != 0) {
                        return;
                    }
                    NSArray *roomList = jsonResponse[@"data"][@"room_list"];
                    for (int idx = 0; idx < roomList.count; idx++) {
                        ZegoRoomInfo *info = [[ZegoRoomInfo alloc] init];
                        NSDictionary *infoDict = roomList[idx];
                        info.roomID = infoDict[@"room_id"];
                        if (info.roomID.length == 0) {
                            return;
                        }
                        
                        // 过滤掉没有 stream_info 的房间
                        if ([infoDict objectForKey:@"stream_info"]) {
                            NSArray *streamList = infoDict[@"stream_info"];
                            if (streamList.count == 0) {
                                continue;
                            }
                        }
                        info.anchorID = infoDict[@"anchor_id_name"];
                        info.anchorName = infoDict[@"anchor_nick_name"];
                        info.roomName = infoDict[@"room_name"];
                        info.streamInfo = [[NSMutableArray alloc] initWithCapacity:1];
                        for (NSDictionary *dict in infoDict[@"stream_info"]) {
                            [info.streamInfo addObject:dict[@"stream_id"]];
                        }
                        
                        [self.roomList addObject:info];
                    }
                    
                    [self.tableView reloadData];
                }
            }
        });
    }];
    
    [task resume];
    
}

#pragma mark - UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.roomList.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    ZegoRoomTableViewCell *cell = (ZegoRoomTableViewCell *)[tableView dequeueReusableCellWithIdentifier:@"zegoRoomCellID" forIndexPath:indexPath];
    
    if (indexPath.row >= self.roomList.count) {
        return cell;
    }
    
    ZegoRoomInfo *info = self.roomList[indexPath.row];
    
    if (info.roomName.length == 0) {
        if (info.anchorName.length == 0) {
            cell.textLabel.text = info.roomID;
        } else {
            cell.textLabel.text = info.anchorName;
        }
    } else {
        cell.textLabel.text = info.roomName;
    }
    
    return cell;
}

#pragma mark - UITableViewDelegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:NO];
    
    if (indexPath.row >= self.roomList.count) {
        return;
    }
    
    ZegoRoomInfo *info = [self.roomList objectAtIndex:indexPath.row];
    
    UIViewController *controller = [[ZegoSetting sharedInstance] getViewControllerFromRoomInfo:info];
    if (controller) {
        [self presentViewController:controller animated:YES completion:nil];
    }
}

#pragma mark - Getter and setter

- (NSMutableArray *)roomList {
    if (!_roomList) {
        _roomList = [NSMutableArray array];
    }
    return _roomList;
}

- (UIRefreshControl *)refreshControl {
    if (!_refreshControl) {
        _refreshControl = [[UIRefreshControl alloc] init];
    }
    return _refreshControl;
}

@end
