//
//  TTCapture.m
//  TTPlayerExample
//
//  Created by liang on 1/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"
#include "TTTextureFilter_ios.hpp"
#import "TTCapture.h"


@interface TTCapture () <AVCaptureVideoDataOutputSampleBufferDelegate, AVCaptureAudioDataOutputSampleBufferDelegate>
{
    AVCaptureSession *_captureSession;
    AVCaptureDevice *_inputCamera;
    AVCaptureDevicePosition _cameraPosition;
    AVCaptureDevice *_microphone;
    AVCaptureDeviceInput *_videoInput;
    AVCaptureVideoDataOutput *_videoOutput;
    BOOL _captrueAsYUV;

    dispatch_queue_t _cameraProcessingQueue;
    dispatch_queue_t _audioProcessingQueue;
    
    TT::TextureFilter_ios _filter;
    
    NSDate *_startingCaptureTime;
    NSUInteger _numberOfFramesCaptured;
    CGFloat _totalFrameTimeDuringCapture;
}

@property (nonatomic, strong) AVCaptureConnection *videoOutputConnection;

@end

@implementation TTCapture

- (instancetype)init {
    self = [super init];
    if (self) {
        _videoLayer = [TTVideoLayer new];
        _cameraProcessingQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH,0);
        _cameraPosition = AVCaptureDevicePositionBack;
        _captureSessionPreset = AVCaptureSessionPreset640x480;
        _captrueAsYUV = NO;
    }
    return self;
}

- (void)addFilter:(std::shared_ptr<TT::Filter>)filter {
    _filter.addFilter(filter);
}

- (BOOL)setUpVideoCapture {
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *device in devices)
    {
        if ([device position] == _cameraPosition)
        {
            _inputCamera = device;
        }
    }
    
    if (!_inputCamera) {
        LOG(ERROR) << "Coundn't find camera";
        return NO;
    }
    
    // Create the capture session
    _captureSession = [[AVCaptureSession alloc] init];
    
    [_captureSession beginConfiguration];
    
    // Add the video input
    NSError *error = nil;
    _videoInput = [[AVCaptureDeviceInput alloc] initWithDevice:_inputCamera error:&error];
    if ([_captureSession canAddInput:_videoInput])
    {
        [_captureSession addInput:_videoInput];
    }
    
    // Add the video frame output
    _videoOutput = [[AVCaptureVideoDataOutput alloc] init];
    [_videoOutput setAlwaysDiscardsLateVideoFrames:NO];
    
    if (_captrueAsYUV) {
        BOOL supportsFullYUVRange = NO;
        NSArray *supportedPixelFormats = _videoOutput.availableVideoCVPixelFormatTypes;
        for (NSNumber *currentPixelFormat in supportedPixelFormats)
        {
            if ([currentPixelFormat intValue] == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange)
            {
                supportsFullYUVRange = YES;
            }
        }
        
        if (supportsFullYUVRange)
        {
            [_videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8PlanarFullRange] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
        }
        else
        {
            [_videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
        }
    } else {
        [_videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
    }

    
    [_videoOutput setSampleBufferDelegate:self queue:_cameraProcessingQueue];
    if ([_captureSession canAddOutput:_videoOutput])
    {
        [_captureSession addOutput:_videoOutput];
    }
    else
    {
        LOG(ERROR) << "Couldn't add video output";
        return NO;
    }
    
    [_captureSession setSessionPreset:_captureSessionPreset];
    
    // This will let you get 60 FPS video from the 720p preset on an iPhone 4S, but only that device and that preset
    //    AVCaptureConnection *conn = [videoOutput connectionWithMediaType:AVMediaTypeVideo];
    //
    //    if (conn.supportsVideoMinFrameDuration)
    //        conn.videoMinFrameDuration = CMTimeMake(1,60);
    //    if (conn.supportsVideoMaxFrameDuration)
    //        conn.videoMaxFrameDuration = CMTimeMake(1,60);
    
    self.videoOutputConnection.videoOrientation = _outputImageOrientation;
    
    [_captureSession commitConfiguration];
    
    return YES;
}

#pragma mark -
#pragma mark Manage the camera video stream

- (BOOL)isRunning
{
    return [_captureSession isRunning];
}

- (void)startCameraCapture
{
    if (![_captureSession isRunning])
    {
        _startingCaptureTime = [NSDate date];
        [self setUpVideoCapture];
        [_captureSession startRunning];
    };
}

- (void)stopCameraCapture
{
    if ([_captureSession isRunning])
    {
        [_captureSession stopRunning];
    }
}

#pragma mark -
#pragma mark AVCaptureVideoDataOutputSampleBufferDelegate

- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection
{
    if (![self isRunning])
    {
        return;
    }
    else if (captureOutput == _videoOutput)
    {
        _filter.processFrame(sampleBuffer);
    }
}

#pragma mark -
#pragma getter/setter

- (void)setOutputImageOrientation:(AVCaptureVideoOrientation)outputImageOrientation {
    _outputImageOrientation = outputImageOrientation;
    self.videoOutputConnection.videoOrientation = _outputImageOrientation;
}

- (AVCaptureConnection *)videoOutputConnection {
    if (_videoOutputConnection == nil) {
        for (AVCaptureConnection *connection in [_videoOutput connections] ) {
            for ( AVCaptureInputPort *port in [connection inputPorts] ) {
                if ( [[port mediaType] isEqual:AVMediaTypeVideo] ) {
                    _videoOutputConnection = connection;
                }
            }
        }
    }
    return _videoOutputConnection;
}

@end
