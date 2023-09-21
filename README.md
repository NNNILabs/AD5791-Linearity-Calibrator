[![Upload to - AISLER](https://img.shields.io/badge/Upload_to_-AISLER-ff8000)](https://aisler.net/p/new?url=https://raw.githubusercontent.com/NNNILabs/AD5791-Linearity-Calibrator/main/HW/AD5791%20Linearity%20Calibrator.kicad_pcb&ref=github)
# AD5791 Linearity Calibrator
## Introduction
Portable voltage, current and resistance standards exist and can be bought for a hole in your pocket, but linearity has exclusively been the domain of high-end bench multimeters. This project contains files for a linearity calibrator based around the AD5791BRUZ 20 bit DAC and a few assisting components. 
## Project Motivation
The gold standard for DMM linearity is the HP/Agilent/Keysight 3458A. This meter costs upwards of several thousand Euros and is out of reach for most people.
While the AD5791 is not even close in terms of absolute linearity, the fact that it can be bought for much less (around 100 Euros) and has decent long-term INL stability (0.19LSB) makes it a good choice for a linearity transfer standard.
## Usage
## List of Files
- Hardware: KiCAD files
- Software: C/C++ code for the Raspberry Pi Pico, UF2 file for direct upload
- Resources: Images
## Application Examples
## Notes
## Links
## Special Note: upload.bat
This file, originally created by <utsuho>, makes it easier to upload code to the Pi Pico without using a debugger or having to unplug and replug the board. PLug in the Pico to find out which COM port it connects to, and modify line 7 accordingly. After building, simply run upload.bat. The Pico should automatically enter boot mode and the .uf2 from the build folder should automatically be uploaded. Make sure the COM port is not being accessed from another program. 
