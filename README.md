![Screenshot](ss.png)

[![Build Status](https://travis-ci.org/hiroMTB/ofxNuitrack.svg?branch=master)](https://travis-ci.org/hiroMTB/ofxNuitrack)
[![Build status](https://ci.appveyor.com/api/projects/status/u39imujns0w1r9sr?svg=true)](https://ci.appveyor.com/project/hiroMTB/ofxnuitrack)

# ofxNuitrack
OpenFrameworks addon for Nuitrack body tracking SDK. Ported from [Cinder-Nuitrack block](https://github.com/daanvanhasselt/Cinder-Nuitrack).


## Requirements
- Windows 10 or Nuitrack supported OS (Linux, Android)
- openFrameworks 0.10.0

## Setup
Follow the instruction [here](http://download.3divi.com/Nuitrack/doc/Installation_page.html) and install Nuitrack SDK to your system first. And check prebuild example app under $NUITRACK_HOME/bin works fine with your camera.

## 3 min playback limitation
Nuitrack SDK has ONI file and BAG file playback feature. But only works 3 minite because of license issue. Probabry this is same even if you purchaced Nuitrack license. You can record depth and color stream to ONI file via NiViewer. And Realsense Viewer for BAG file. This repository does not include any files. Please record it by yourself.

## TODO
- [x] Windows support (under development)
- [ ] Linux support (no plan so far)
- [ ] Android support (no plan so far)
