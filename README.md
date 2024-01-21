[![Upload to - AISLER](https://img.shields.io/badge/Upload_to_-AISLER-ff8000)](https://aisler.net/p/new?url=https://raw.githubusercontent.com/NNNILabs/AD5791-Linearity-Calibrator/main/HW/AD5791%20Linearity%20Calibrator.kicad_pcb&ref=github)
### Work in Progress! 
# AD5791 Linearity Calibrator

<p align="center">
  <img width="400" src="https://github.com/NNNILabs/AD5791-Linearity-Calibrator/blob/main/Resources/AD5791LinearityCalibrator.jpg">
</p>

## Introduction
Portable voltage, current and resistance standards exist and can be bought for a hole in your pocket, but linearity has exclusively been the domain of high-end bench multimeters. This project contains files for a linearity calibrator based around the AD5791BRUZ 20 bit DAC and a few assisting components. 
## Project Motivation
The gold standard for DMM linearity is the HP/Agilent/Keysight 3458A. This meter costs upwards of several thousand Euros and is out of reach for most people.
While the AD5791 is not even close in terms of absolute linearity, the fact that it can be bought for much less (around 100 Euros) and has decent long-term INL stability (0.19LSB) makes it a good choice for a linearity transfer standard.
## Usage
The C/C++ SDK for the Raspberry Pi Pico can be installed using the one-click installer found [here](https://github.com/raspberrypi/pico-setup/blob/master/pico_setup.sh). The SW folder is opened using 'Pico - Visual Studio Code', and built using the 'gcc-arm-none-eabi' tool. The build file (AD5791LinCal.uf2) can be uploaded to a connected Pico by doing './upload.bat' in the terminal after [suitably configuring upload.bat](https://github.com/NNNILabs/AD5791-Linearity-Calibrator/tree/main?tab=readme-ov-file#special-note-uploadbat).  
Power is supplied through the 3.5mm terminal block with the labels 'IN+' and 'IN-'. +/-18V is an ideal nominal supply. The DAC board is connected to a computer through the Raspberry Pi Pico controller's micro USB port. Communicating with the DAC is possible through any serial terminal. The binary code representing the desired output voltage is typed in, and the enter key is pressed to update the DAC output. The buffered DAC output is available on the 3.5mm terminal block with the labels 'OUT+' and 'OUT-'. Raw DAC output is available on the 'DAC' test point.  
  
The DAC's transfer function is as follows:
```math
V_{OUT} = \frac{(V_{REFP} - V_{REFN}) D}{2^{20} - 1} + V_{REFN}
```
Where:  
V<sub>OUT</sub> is the desired output voltage  
V<sub>REFP</sub> is the positive reference voltage (in this case approximately 10.5V)  
V<sub>REFN</sub> is the negative reference voltage (in this case approximately -10.5V)  
D is the integer digital code (0 to 1048575)  
  
For example, 0 would result in V<sub>REFN</sub> on the output, 524288 would result in ~0V, and 1048575 would result in V<sub>REFP</sub>. Each step is therefore approximately 20uV.
## List of Files
- Hardware: KiCAD schematic and PCB files, GERBER files, BOM
- Software: C/C++ code for the Raspberry Pi Pico, .uf2 file for direct upload, upload.bat upload assistant
- Resources: Images, AD5791 datasheet and ISSCC paper mirror
## Application Examples
### Determining Linearity of a DMM
Integral Non-Linearity (INL) describes how the input/output transfer function of a multimeter differs from the ideal straight line. Test methodology involves connecting a voltage source with a known linearity to the meter, and measuring meter readings at various points. Since the meter and source gains and offsets are not guaranteed to be the same, the source gain and offset has to be "normalized" to the meter gain and offset through a simple linear endpoint-fit method. Although some information at the endpoints is lost, it is most often the "shape" of the INL curve that is of interest. The data could possibly be used to derive a transfer function to further increase DAC/ADC linearity.
### Stable, Precise Voltage Source
Given the high resolution of the AD5791 (20 bits), fine voltage steps of 20uV can be achieved. This facilitates experiments that need ultrafine voltage steps. In some cases, the DAC, with its 1us settling time, can be used to generate precision waveforms through additional code modification (Wave table -> DMA -> PIO). 
## Notes
- By initializing GPIO 23 as an output and setting it high, the Raspberry Pi Pico's internal SMPS switches to PWM mode, where radiated EMI and output ripple is lower. 
## Links
- [AD5791 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ad5791.pdf)
- [ISSCC Paper](https://github.com/NNNILabs/AD5791-Linearity-Calibrator/blob/main/Resources/isscc.2013.6487734.pdf)
## Special Note: upload.bat
This file, originally created by utsuho, makes it easier to upload code to the Pi Pico without using a debugger or having to unplug and replug the board. Plug in the Pico to find out which COM port it connects to, and modify line 7 accordingly. In some cases in Windows, the drive letter might have to be changed on line 11. After building, simply run upload.bat. The Pico should automatically enter boot mode and the .uf2 from the build folder should automatically be uploaded. Make sure the COM port is not being accessed from another program. 
