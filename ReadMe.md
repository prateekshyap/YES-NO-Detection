# Assumptions and calculations

1. DC Shift
-----------

DC shift is found out by taking the average of the sample data where microphone is turned off. <br>
(I have considered the DC shift to be 0 if the average is less than 0.0005, since this value is too small.)

2. Normalization Factor
-----------------------

Normalization factor is found out by dividing the average of the two extreme values by 5000. For example - if the maximum value is 12500 and the minimum value is -12600 then normalization factor will be 5000/((12500+12600)/2) i.e. 0.398406 . <br>
(To make things simpler, I am taking only one digit after decimal point. That means 0.398406 becomes 0.3 . It doesn't affect the values much. It also ensures that the range remains 5000 to -5000 since we are reducing the value from 0.398406 to 0.3 .)

3. Ambient Noise
----------------

Ambient noise is the background noise which is calculated from silence.<br>
(I have taken it to be the maximum energy value from the file where nothing is spoken. It makes word detection safer.)

4. YES/NO detection
-------------------

From all the energy and zcr samples available for a single word, the last 40% of the zcr data should be checked to detect the fricative of YES. The threshold of zcr should be 50. <br>
(I have taken the threshold to be 40. Since I am taking the average of all ZCR values of the last 40% of the sample, it is possible to have a few samples(e.g. 5-10 samples out of 50 samples) with zcr lower than 50 which pulls the average down, but the word still remains YES. Also the average zcr of NO is very low e.g. 1-10. So reducing the threshold by 10 doesn't affect the detection much.)

# Functions and their Functionalities

1. float findDCShift(char \* fileName)
------------------------------------

This function is finding out the DC shift from \<fileName\> file. This file is recorded keeping the microphone off.

2. int performDataExtraction(char \* inputFileName, char \* outputFileName, float normalizationFactor, float DCShift)
-------------------------------------------------------------------------------------------------

This function is reading the x values from \<inputFileName\> and finding out the energy and zcr values and storing them in \<outputFileName\> for reference. DC shift correction and normalization are also done in this function.

3. int getMaxEnergyFromNoise(char \* fileName)
--------------------------------------------

This function finds out the ambient noise from \<fileName\> file. This file is recorded in silence with microphone turned on.

4. void storeData(char \* fileName, long \* energy, long \* zcr)
----------------------------------------------------------

This function reads the data from \<fileName\> which contains the energy and zcr values stored in the \<outputFileName\> in performDataExtraction function and stores them in two arrays i.e. energy and zcr.

5. float getNormalizationFactor(char \* fileName, float range)
--------------------------------------------------------------

This function finds out the normalization factor for the given speech data. This factor is also used to normalize the ambient noise.

6. int \_tmain(int argc, \_TCHAR \* argv[])
-------------------------------------------

This is the main function. Here other functions are called sequentially as per the requirement. Then the starting and ending indices of the energy and zcr arrays are spotted using the ambient noise found. This also works when multiple yes and no are spoken in a single file. Then the word is detected by checking the last 40% of the samples.

# How to create the input files

1. DC Shift
-----------

For this, the microphone should be turned off and recorded. The file should be stored as _DCShiftTest.txt_.

2. Ambient Noise
----------------

For this, the microphone should be kept on and recorded. The file should be stored as _noise.txt_. If you are using a speech sample which is recorded by a different system (than mine) you need to find out the first few x values from the speech sample, copy them to a file named _sampleNoise.txt_. In the main function, the file name inputs should be taken as per requirement.

3. Speech sample
----------------

This can be recorded by speaking a YES or a NO or by speaking several YESs and NOs in a sequence. This code can detect both type of speech.

# Testcases Description

1. DC shift test sample
-----------------------

DCShiftTest.txt

2. Ambient noise sample
-----------------------

_noise.txt_ contains the sample recorded by my system. Noise sample for the given speech sample is stored in _sampleNoise.txt_.

3. Single YES or Single NO
--------------------------

The speech sample with a single YES or NO are named as _yesn.txt_ or _non.txt_ respectively where n is an integer. More files can be added with similar convention. <br><br>

Already added files- <br>
	1) recorded by my system- <br>
	_yes1.txt_ to _yes3.txt_ <br>
	_no1.txt_ to _no3.txt_ <br>
	2) given sample- <br>
	_yes4.txt_ to _yes7.txt_ <br>
	_no4.txt_ to _no7.txt_<br>

4. Multiple YESs and NOs
------------------------

The speech sample with mulitple YESs and NOs are named as per the sequence of speech. That means, if "YES NO NO YES YES NO" is spoken, the file name should be _ynnyyn.txt_. More files can be added with
similar convention. <br><br>

Already added files- <br>
	1) recorded by my system- <br>
	_yyynnn.txt_ <br>
	_ynynyn.txt_ <br>
	_ynnyyn.txt_ <br>
	_yynnyn.txt_ <br>
	_nnynyy.txt_ <br>
	2) given sample- <br>
	_yyyynnnn.txt_ <br>

5. Output files can be named with any name. Any convention can be followed for easier understanding. For example - noise output can be stored in _noiseoutput.txt_. Output for single YES or NO files can be stored in _yesoutput.txt_ or _nooutput.txt_ respectively. Output for multiple YESs and NOs can be stored in _yesnooutput.txt_. Intermediate data such as normalized x values for the speech file can be stored in _tempData.txt_.

# Instructions to execute the code

1. The project should be opened in visual studio.

2. The variables which should be decided before execution are listed separately within main function at line number - 237 in [Main cpp file](https://github.com/prateekshyap/YES-NO-Detection/blob/main/YesNoDetection/YesNoDetection.cpp).

2. The names of the speech input and output files should be decided first and written for the respective variables in the main function in the same file.

3. The names of the noise input and output files should be decided according to the speech samples.

e.g.-
If dataInputFile = _yes1.txt_ then noiseInputFile = _noise.txt_ <br>
If dataInputFile = _yes4.txt_ then noiseInputFile = _sampleNoise.txt_ <br>
Because _yes1.txt_ is recorded in my system and _yes4.txt_ is cropped from the given speech sample.

4. DCShiftFile name should be kept NULL if the given speech sample is taken. Otherwise it should be set to the required file name.

5. *startBuffer* and *endBuffer* should be initialized with integers according to the number of continuous frames that needs to be checked to decide the starting point and the ending point respectively.

e.g.-
If startBuffer = 5 and endBuffer = 10 <br>
The program will check for continuous 5 frames with energy higher than the ambient noise and set starting point to current index - 5.
Similarly it will check for continuous 10 frames with energy lower than the ambient noise and set ending point to current index - 10.

6. zcrThreshold should be decided according to the requirement of fricative identification.

7. throwFrames should be decided according to the amount of noise present in the initial frames.

8. If the energy should be higher than 115% of the ambient noise then deviationPerc should be set to 1.15. Accordingly the numbers can be changed.

9. If the last 40% of the frames should be considered for detecting whether a fricative has been spoken or not, framesPerc should be set to 0.4 .

10. The amplitude range should be set at range variable. Only positive range should be set.

11. To see the console window, a breakpoint should be put at line number - 381. For this, the shortcut is to place the cursor at 381 and press f9 key. (Fn+f9 for some systems)

12. f7 can be pressed for building the project and f5 should be pressed for execution.

13. Output can be observed in the console window.

14. The output files can be checked within the same folder where [YesNoDetection.cpp](https://github.com/prateekshyap/YES-NO-Detection/blob/main/YesNoDetection/YesNoDetection.cpp) is located.

# Want to improve the project?

Fork this repository. Make required changes and create a pull request.