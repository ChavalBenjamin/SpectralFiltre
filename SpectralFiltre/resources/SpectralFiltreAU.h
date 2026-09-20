
#include <TargetConditionals.h>
#if TARGET_OS_IOS == 1 || TARGET_OS_VISION == 1
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#define IPLUG_AUVIEWCONTROLLER IPlugAUViewController_vSpectralFiltre
#define IPLUG_AUAUDIOUNIT IPlugAUAudioUnit_vSpectralFiltre
#import <SpectralFiltreAU/IPlugAUViewController.h>
#import <SpectralFiltreAU/IPlugAUAudioUnit.h>

//! Project version number for SpectralFiltreAU.
FOUNDATION_EXPORT double SpectralFiltreAUVersionNumber;

//! Project version string for SpectralFiltreAU.
FOUNDATION_EXPORT const unsigned char SpectralFiltreAUVersionString[];

@class IPlugAUViewController_vSpectralFiltre;
