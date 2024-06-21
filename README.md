[![Upload to - AISLER](https://img.shields.io/badge/Upload_to_-AISLER-ff8000)](https://aisler.net/p/new?url=https://raw.githubusercontent.com/NNNILabs/AD5791-Linearity-Calibrator/main/HW/AD5791%20Linearity%20Calibrator.kicad_pcb&ref=github)
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
The C/C++ SDK for the Raspberry Pi Pico can be installed using the one-click installer found [here](https://github.com/raspberrypi/pico-setup/blob/master/pico_setup.sh). The SW folder is opened using 'Pico - Visual Studio Code', and built using the 'gcc-arm-none-eabi' tool. The build file (AD5791LinCal.uf2) can be uploaded to a connected Pico by using upload.bat after suitable configuration (see notes).  

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

### Linearity
An example of the AD5791 DAC board was constructed and linearity was measured against a HP3458A DMM. Pre-endpoint fit was used to process the raw data from the DMM to provide a linearity chart. The starting and ending points of the linearity run (in this case, 0 and 1032192) were measured first, and used to pre-calculate m and c values that are applied to the complete run that follows. This provides an indication of drift, and if its effect on the run was larger than the peak non-linearity, in which case the run can be discarded. In this example, the drift across all five runs was around 0.3ppm, corresponding to 3uV.

<p align="center">
  <img src="https://github.com/NNNILabs/AD5791-Linearity-Calibrator/blob/main/Resources/Linearity.png">
</p>

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
- PCB template is a modified version of [Mark's Protocard template](https://github.com/macaba/Nuts/tree/main/kicad-templates).
- [upload.bat](https://github.com/NNNILabs/AD5791-Linearity-Calibrator/blob/main/SW/upload.bat) was created by Utsuho, and faciliates hands-free upload to the Pico. The COM port the Pico is connected to and the drive letter are entered in lines 7 and 11 respectively. After the software is built, upload.bat is run from the terminal by doing '.\upload.bat' on Windows. The Pico enters boot mode and the .uf2 from the build folder is automatically uploaded. Make sure the COM port is not being accessed by other programs.
## Links
- [AD5791 Datasheet](https://github.com/NNNILabs/AD5791-Linearity-Calibrator/blob/main/Resources/AD5791%20Datasheet.pdf)
- [ISSCC Paper](https://github.com/NNNILabs/AD5791-Linearity-Calibrator/blob/main/Resources/AD5791%20ISSCC.pdf)
- [Reference voltage cap](https://www.printables.com/model/746207-voltage-reference-cap)
