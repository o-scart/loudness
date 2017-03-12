# Detecting people flows with distributed loudness sensors

Information about people flows can be useful for planning events or buildings, evaluating emergency routes, optimising paths etc. In my bachelor's thesis I am analysing if this information can be extracted from loudness data.  

## The hardware

For the task of collecting the loudness data I am using an "Particle Photon" single-board microcontroller and a "Grove" loudness sensor. Two AA batteries and a "Sparkfun" 3.3V step-up are used as Power supply für the microcontroller. The Photon communicates via local Wi-Fi and sends the data to a data server. 

## The software

The Photon can programmed by flashing compiled C++ Code onto it. As soon as the Photon is powered, the code is being executed. The source code for my loudness measurement can be viewed here in my GitHub repository. 

## The measurement data

The Photon sends only one value per second. It is the analog value that is provided by the loudness sensor. This value can be converted to a dB-value with a look-up table. It is not possible to extract a waveform from this data and for instance actually see sounds or conversations.
