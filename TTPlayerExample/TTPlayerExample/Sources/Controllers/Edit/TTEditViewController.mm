//
//  TTEditViewController.m
//  TTPlayerExample
//
//  Created by liang on 21/1/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "easylogging++.h"
#include "TTEdit.h"
#include "TTProcess.h"

#import "TTCapture.h"

#import "TTEditViewController.h"
#import "TTPreviewCell.h"

static NSString *kPreviewCellIdentifier = @"previewCell";

@interface TTEditViewController () <
  UICollectionViewDelegate,
  UICollectionViewDataSource,
  UICollectionViewDelegateFlowLayout>
{
    std::shared_ptr<TT::EditGroup> _editGroup;
    
    std::shared_ptr<TT::FilterGroup> _filterGroup;
    std::shared_ptr<TT::Y420ToRGBFilter> _filterTexture;
    
    TTImageView *_imageView;
    UICollectionView *_previewBar;
}

@property (nonatomic, strong, readwrite) UIButton *doneButton;

- (void)video:(TT::Video *)video statusCallback:(TT::VideoStatus)status;
- (void)video:(TT::Video *)video eventCallback:(TT::VideoEvent)event;
- (void)video:(TT::Video *)video readFrameCallback:(size_t)size;

@end

void VideoStatusCallback(void *opaque, TT::Video *video, TT::VideoStatus status) {
    if (opaque == nullptr) {
        return;
    }
    
    LOG(DEBUG) << "Edit status change:" << (int)status;

    TTEditViewController *vc = (__bridge TTEditViewController *)opaque;
    [vc video:video statusCallback:status];
}

void VideoEventCallback(void *opaque, TT::Video *video, TT::VideoEvent event) {
    if (opaque == nullptr) {
        return;
    }
    
    LOG(DEBUG) << "Edit event callback:" << (int)event;
    
    TTEditViewController *vc = (__bridge TTEditViewController *)opaque;
    [vc video:video eventCallback:event];
}

void VideoReadFrameCallback(void *opaque, TT::Video *video, size_t size) {
    if (opaque == nullptr) {
        return;
    }
    
//    LOG(DEBUG) << "Edit decode frame size:" << size;
    
    TTEditViewController *vc = (__bridge TTEditViewController *)opaque;
    [vc video:video readFrameCallback:size];
}

@implementation TTEditViewController

- (instancetype)initWithURLs:(NSArray<NSURL *> *)urls {
    self = [super init];
    if (self) {
        _editGroup = std::make_shared<TT::EditGroup>();
        _urls = [urls mutableCopy];
    }
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self setupFilter];
    
    [_urls enumerateObjectsUsingBlock:^(NSURL * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        [self addMaterial:obj];
    }];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

#pragma mark -
#pragma mark UI

- (void)setupUI {
    self.view.backgroundColor = [UIColor whiteColor];
    
    _imageView = [TTImageView new];
    _imageView.contentMode = UIViewContentModeScaleAspectFit;
    [self.view addSubview:_imageView];
    [_imageView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(_imageView.superview);
    }];
    
    self.doneButton.backgroundColor = [UIColor greenColor];
    [self.doneButton addTarget:self action:@selector(onDoneButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.doneButton];
    [self.doneButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(100));
        make.height.equalTo(@(44));
        make.center.equalTo(self.doneButton.superview);
    }];
    
    UICollectionViewFlowLayout *layout = [UICollectionViewFlowLayout new];
    layout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
    _previewBar = [[UICollectionView alloc] initWithFrame:CGRectZero collectionViewLayout:layout];
    _previewBar.delegate = self;
    _previewBar.dataSource = self;
    [_previewBar registerClass:[TTPreviewCell class] forCellWithReuseIdentifier:kPreviewCellIdentifier];
    [self.view addSubview:_previewBar];
    [_previewBar mas_makeConstraints:^(MASConstraintMaker *make) {
        make.right.equalTo(_previewBar.superview);
        make.left.equalTo(_previewBar.superview);
        make.bottom.equalTo(_previewBar.superview);
        make.height.equalTo(@(150));
    }];
}

- (void)setupFilter {
    _filterGroup = std::make_shared<TT::FilterGroup>();
    _filterGroup->addFilter([_imageView filter]);
    
    _filterTexture = std::make_shared<TT::Y420ToRGBFilter>();
    _filterTexture->addFilter(_filterGroup);
}

#pragma mark getter/setter
- (UIButton *)doneButton {
    if (_doneButton == nil) {
        _doneButton = [UIButton new];
    }
    return _doneButton;
}

#pragma mark target/action
- (void)onDoneButton:(UIButton *)button {
//    std::string url = _url->scheme() + ":///" + _url->dir() + "/liangliang.mp4";
//    std::shared_ptr<TT::URL> _outUrl = std::make_shared<TT::URL>(url);
//    _video->save(_outUrl);
}

#pragma mark Edit Material
- (void)addMaterial:(NSURL *)url {
    std::shared_ptr<TT::Video> video = std::make_shared<TT::Video>();
    video->setStatusCallback(std::bind(VideoStatusCallback, (__bridge void *)self, std::placeholders::_1, std::placeholders::_2));
    video->setEventCallback(std::bind(VideoEventCallback, (__bridge void *)self, std::placeholders::_1, std::placeholders::_2));
    video->setReadFrameCallback(std::bind(VideoReadFrameCallback, (__bridge void *)self, std::placeholders::_1, std::placeholders::_2));
    _editGroup->addMaterial(video);
    const char *str = [url.absoluteString cStringUsingEncoding:NSUTF8StringEncoding];
    video->open(std::make_shared<TT::URL>(str));
}

#pragma mark Edit callback
- (void)video:(TT::Video *)video statusCallback:(TT::eEditStatus)status {
}

- (void)video:(TT::Video *)video eventCallback:(TT::VideoEvent)event {
    if (event == TT::VideoEvent::kReadEnd) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [_previewBar reloadData];
        });
    }
}

- (void)video:(TT::Video *)video readFrameCallback:(size_t)size {
}

#pragma mark -- UICollectionViewDataSource
//定义展示的UICollectionViewCell的个数
-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    std::shared_ptr<TT::Material> material = _editGroup->material(section);
    if (material) {
        return material->frameCount();
    }
    return 0;
}

//定义展示的Section的个数
-(NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return _editGroup->materialCount();
}

//每个UICollectionView展示的内容
-(UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    TTPreviewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:kPreviewCellIdentifier
                                                                    forIndexPath:indexPath];
    
    cell.backgroundColor = [UIColor colorWithRed:((10 * indexPath.row) / 255.0)
                                           green:((20 * indexPath.row)/255.0)
                                            blue:((30 * indexPath.row)/255.0)
                                           alpha:1.0f];
    
    std::shared_ptr<TT::Material> material = _editGroup->material(indexPath.section);
    if(material) {
        std::shared_ptr<TT::Frame> frame = material->frame(indexPath.row);
        [cell setupUI];
        [cell showFrame:frame];
        
        _filterTexture->processFrame(frame);
    }
    
    return cell;
}

#pragma mark --UICollectionViewDelegateFlowLayout
//定义每个UICollectionView 的大小
- (CGSize)collectionView:(UICollectionView *)collectionView
                  layout:(UICollectionViewLayout*)collectionViewLayout
  sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
    return CGSizeMake(96, 100);
}

//定义每个UICollectionView 的 margin
-(UIEdgeInsets)collectionView:(UICollectionView *)collectionView
                       layout:(UICollectionViewLayout *)collectionViewLayout
       insetForSectionAtIndex:(NSInteger)section {
    return UIEdgeInsetsMake(5, 5, 5, 5);
}

#pragma mark --UICollectionViewDelegate
- (void)scrollViewDidScroll:(UIScrollView *)scrollView {
//    LOG(DEBUG) << "scrolling: " << scrollView.contentOffset.x;
}

//UICollectionView被选中时调用的方法
-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    UICollectionViewCell * cell = (UICollectionViewCell *)[collectionView cellForItemAtIndexPath:indexPath];
    cell.backgroundColor = [UIColor whiteColor];
    
    std::shared_ptr<TT::Material> material = _editGroup->material(indexPath.section);
    if(material) {
        std::shared_ptr<TT::Frame> frame = material->frame(indexPath.row);
        _filterTexture->processFrame(frame);
    }
}

//返回这个UICollectionView是否可以被选择
-(BOOL)collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    return YES;
}

@end
