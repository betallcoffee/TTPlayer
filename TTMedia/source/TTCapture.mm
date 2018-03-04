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
    AVCaptureDeviceInput *_videoInput;
    AVCaptureVideoDataOutput *_videoOutput;
    
    AVCaptureDevice *_microphone;
    AVCaptureDeviceInput *_audioInput;
    AVCaptureAudioDataOutput *_audioOutput;
    
    BOOL _captrueAsYUV;

    dispatch_queue_t _captureSessionQueue;
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
        _captureSessionQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
        _cameraProcessingQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
        _cameraPosition = AVCaptureDevicePositionFront;
        _audioProcessingQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
        _captureSessionPreset = AVCaptureSessionPreset640x480;
        _captrueAsYUV = NO;
    }
    return self;
}

- (void)addFilter:(std::shared_ptr<TT::Filter>)filter {
    _filter.addFilter(filter);
}

- (BOOL)setupVideoCapture {
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
    
    [_captureSession beginConfiguration];
    
    // Add the video input
    NSError *error = nil;
    _videoInput = [[AVCaptureDeviceInput alloc] initWithDevice:_inputCamera error:&error];
    if (_videoInput == nil) {
        LOG(ERROR) << "Create video input error: " << [error.description cStringUsingEncoding:NSUTF8StringEncoding];
        [_captureSession commitConfiguration];
        return NO;
    }
    
    if ([_captureSession canAddInput:_videoInput]) {
        [_captureSession addInput:_videoInput];
    } else {
        LOG(ERROR) << "Can't add video input";
        [_captureSession commitConfiguration];
        return NO;
    }
    
    // Add the video frame output
    _videoOutput = [[AVCaptureVideoDataOutput alloc] init];
    [_videoOutput setAlwaysDiscardsLateVideoFrames:NO];
    
    if (_captrueAsYUV) {
        BOOL supportsFullYUVRange = NO;
        NSArray *supportedPixelFormats = _videoOutput.availableVideoCVPixelFormatTypes;
        for (NSNumber *currentPixelFormat in supportedPixelFormats) {
            if ([currentPixelFormat intValue] == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange) {
                supportsFullYUVRange = YES;
            }
        }
        
        if (supportsFullYUVRange) {
            [_videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8PlanarFullRange] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
        } else {
            [_videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
        }
    } else {
        [_videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
    }

    
    [_videoOutput setSampleBufferDelegate:self queue:_cameraProcessingQueue];
    if ([_captureSession canAddOutput:_videoOutput]) {
        [_captureSession addOutput:_videoOutput];
    } else {
        LOG(ERROR) << "Couldn't add video output";
        [_captureSession commitConfiguration];
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

- (bool)setupAudioCapture {
    [_captureSession beginConfiguration];
    
    _microphone = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
    
    NSError *error;
    _audioInput = [AVCaptureDeviceInput deviceInputWithDevice:_microphone error:&error];
    if (_audioInput == nil) {
        LOG(ERROR) << "Create audio input error: " << [error.description cStringUsingEncoding:NSUTF8StringEncoding];
        [_captureSession commitConfiguration];
        return NO;
    }
    
    if ([_captureSession canAddInput:_audioInput]) {
        [_captureSession addInput:_audioInput];
    }

    _audioOutput = [[AVCaptureAudioDataOutput alloc] init];
    if ([_captureSession canAddOutput:_audioOutput]) {
        [_captureSession addOutput:_audioOutput];
    } else {
        LOG(ERROR) << "Couldn't add video output";
        [_captureSession commitConfiguration];
        return NO;
    }
    [_audioOutput setSampleBufferDelegate:self queue:_audioProcessingQueue];
    
    [_captureSession commitConfiguration];
    
    return YES;
}

#pragma mark -
#pragma mark Manage the camera video stream

- (BOOL)isRunning
{
    return [_captureSession isRunning];
}

- (void)startCaptureVideo:(BOOL)hasVideo andAudio:(BOOL)hasAudio {
    if (![_captureSession isRunning]) {
        _startingCaptureTime = [NSDate date];
        // Create the capture session
        _captureSession = [[AVCaptureSession alloc] init];
        dispatch_async(_captureSessionQueue, ^{
            if (hasVideo) {
                [self setupVideoCapture];
            }
            if (hasAudio) {
                [self setupAudioCapture];
            }
            
            [_captureSession startRunning];
        });
    }
}

- (void)stopCameraCapture
{
    dispatch_async(_captureSessionQueue, ^{
        if ([_captureSession isRunning]) {
            [_captureSession stopRunning];
        }
    });
}

#pragma mark -
#pragma mark AVCaptureVideoDataOutputSampleBufferDelegate

- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection
{
    if (![self isRunning]) {
        return;
    } else if (captureOutput == _videoOutput) {
        _filter.processFrame(sampleBuffer);
    } else if (captureOutput == _audioOutput) {
        [_movieWriter processAudioBuffer:sampleBuffer];
    }
}

#pragma mark -
#pragma mark getter/setter

- (void)setMovieWriter:(TTMovieWriter *)movieWriter {
    _movieWriter = movieWriter;
}

- (void)setOutputImageOrientation:(AVCaptureVideoOrientation)outputImageOrientation {
    _outputImageOrientation = outputImageOrientation;
    self.videoOutputConnection.videoOrientation = _outputImageOrientation;
}

- (AVCaptureConnection *)videoOutputConnection {
//    if (_videoOutputConnection == nil) {
//        for (AVCaptureConnection *connection in [_videoOutput connections] ) {
//            for ( AVCaptureInputPort *port in [connection inputPorts] ) {
//                if ( [[port mediaType] isEqual:AVMediaTypeVideo] ) {
//                    _videoOutputConnection = connection;
//                }
//            }
//        }
//    }
    return _videoOutputConnection;
}

@end
