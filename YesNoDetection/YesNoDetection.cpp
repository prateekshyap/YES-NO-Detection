// YesNoDetection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

float findDCShift(char* fileName)
{
	char line[100]; //string to read lines from the input file
	int x = 0; //stores the x_i values
	long noOfSamples = 0; //stores the total number of samples
	float DCShift = 0.0f; //to store the DC shift value

	if (fileName == NULL) return 0; //return 0 if no DC shift test file is given

	FILE* file = fopen(fileName,"r"); //open input file in read mode

	//till end of fileis not reached
	while(!feof(file))
	{
		fgets(line, 100, file); //read one line from the file
		if(!isalpha(line[0])) //if it is a numerical data
		{
			x = atoi(line); //convert it into integer
			DCShift += x; //add it to dc shift
			++noOfSamples; //increment sample count
		}
	}

	DCShift /= noOfSamples; //find out the average
	if (abs(DCShift) < 0.0005) return 0; //if average is less than 0.0005 then return 0
	return DCShift; //otherwise return DC Shift value
}

int performDataExtraction(char* inputFileName, char* outputFileName, float normalizationFactor, float DCShift)
{
	FILE* inputFile = fopen(inputFileName,"r"); //open input file in read mode
	FILE* outputFile = fopen(outputFileName,"w"); //open output file in write mode
	FILE* tempDataFile = fopen("tempData.txt","w"); //open a file to store the normalized data points
	int sampleCount = 0; //to count the total number of energy and zcr values
	char line[100]; //string to read the lines from the input file
	/*
	iterations stores the number of x values read from the input file
	prevX stores x_(i-1)
	x stores x_(i)
	*/
	int iterations = 0, prevX = 0, x = 0;
	/*
	energy stores the total energy calculated
	zcr stores the zcr value calculated
	*/
	long energy = 0, zcr = 0;

	//if any of the file pointers are null then print the message
	if (inputFile == NULL)
		printf("Input file not found");
	else if (outputFile == NULL)
		printf("Output file not found");
	else if (tempDataFile == NULL)
		printf("Temporary file not found");

	//otherwise
	else
	{
		//till end of file is not reached
		while (!feof(inputFile))
		{
			fgets(line,100,inputFile); //read one line from file
			if (!isalpha(line[0])) //if it is a numerical data
			{
				if (iterations == 100) //for each 100 data
				{
					//write the energy and zcr into a file in the format - "energy\nzcr\n"
					fprintf(outputFile,"%ld",energy);
					fprintf(outputFile,"\n");
					fprintf(outputFile,"%ld",zcr);
					fprintf(outputFile,"\n");
					++sampleCount; //increase sample count
					
					//reset energy, zcr and iterations
					energy = 0;
					zcr = 0;
					iterations = 0;
				}

				x = atoi(line); //convert the line to numerical data
				if (DCShift != 0) x -= DCShift; //subtract the DCShift if it is non-zero
				zcr += ((prevX < 0 && x > 0) || (prevX > 0 && x < 0)) ? 1 : 0; //update zcr
				prevX = x; //store the current x in prevX for next iteration
				x *= normalizationFactor; //multiply the normalization factor
				//add the normalized data to a file for reference in the format "x\n"
				fprintf(tempDataFile,"%d",x);
				fprintf(tempDataFile,"\n");
				energy += (x*x); //update energy
				++iterations; //increment iterations
			}
		}
	}

	//close all the files
	fclose(inputFile);
	fclose(outputFile);
	fclose(tempDataFile);

	//return the sample count
	return sampleCount;
}

int getMaxEnergyFromNoise(char* fileName)
{
	/*
	result stores the final result
	energy stores the current energy in each iteration 
	i is the index variable
	flag is a toggle or indicator variable
	*/
	int result = 0, energy = 0, i = 0, flag = 0;
	char line[20]; //string to read the lines from the input file
	FILE* file = fopen(fileName,"r"); //open input file in read mode

	if (file == NULL)
		printf("File not found");
	else
	{
		//till the end of file is not reached
		while (!feof(file))
		{
			flag = 1-flag; //toggle the flag
			fgets(line,20,file); //read next energy
			if (flag > 0) //if flag indicates 1
			{
				energy = atoi(line); //store current energy
				if (energy > result)
					result = energy; //update the result
			}
		}
	}

	fclose(file); //close the file
	return result; //return result
}

void storeData(char* fileName, long* energy, long* zcr)
{
	char line[20]; //string to read the lines from the input file
	/*
	i is the index variable
	flag is a toggle variable
	*/
	int i = 0, flag = 0;

	FILE* file = fopen(fileName,"r"); //open input file in read mode

	if (file == NULL)
		printf("File not found");
	else
	{	
		//till the end of file is not reached
		while (!feof(file))
		{
			flag = 1-flag; //toggle the flag
			fgets(line,20,file); //read next energy
			if (flag == 0) //if flag indicates 0, store zcr and increment i
				zcr[i++] = _atoi64(line);
			else if (flag == 1) //if flag indicates 1, store energy
				energy[i] = _atoi64(line);
		}
	}

	fclose(file); //close the file
}

float getNormalizationFactor(char* fileName, float range)
{
	float result = 0.0f; //to store the final result
	char line[100]; //string to read lines from the input file
	/*
	data stores the data read from the file
	maxData and minData store the maximum and minimum data point respectively
	*/
	int data = 0, maxData = 0, minData = 0;

	//some variables used to truncate the float result
	int shift = 0, temp = 0;
	float copy = 0.0f;

	FILE* file = fopen(fileName,"r"); //open input file in read mode

	if (file == NULL)
		printf("File not found");
	else
	{
		//till the end of file is not reached
		while (!feof(file))
		{
			fgets(line,100,file); //read next line
			if (!isalpha(line[0])) //if it is numerical
			{
				data = atoi(line); //convert into integer
				if (data > maxData) maxData = data; //update maxData
				if (data < minData) minData = data; //update minData
			}
		}

		minData = abs(minData); //get the absolute value of the minimum
		result = range/((maxData+minData)/2); //get the normalization factor
		
		//truncate the result to one decimal digit
		copy = result; //store the result in copy
		while (copy < 1) //till copy is less than 1
		{
			++shift; //increment shift
			copy *= 10; //keep multiplying copy with 10
		}
		temp = (int)copy; //store the integer part of copy in temp
		copy -= temp; //subtract temp from copy
		while (shift-- > 0) //reverse the shift process
			copy /= 10;
		/*the above process helps in getting all the digits except the first decimal digit*/
		result -= copy; //subtract copy from result
		/*this will ensure that only one digit remains after decimal point*/
	}

	fclose(file); //close the file

	//return the result
	return result;
}

int _tmain(int argc, _TCHAR* argv[])
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*variables which should be decided before execution*/

	/*
	noiseInputFile and noiseOutputFile are for calculating the ambient noise
	dataInputFile and dataOutputFile are for the speech files
	*/
	char* dataInputFile = "yyynnn.txt", * dataOutputFile = "nooutput.txt";
	char* noiseInputFile = "noise.txt", * noiseOutputFile = "noiseoutput.txt";
	char* DCShiftTestFile = "DCShiftTest.txt";

	/*
	These two buffer variables store the number of continuous frames which we shall check
	before deciding the starting or ending point respectively
	zcrThreshold stores the decided threshold value of zcr after which everything will be considered as high
	throwFrames stores how many frames from the beginning should be thrown away to avoid unnecessary noise
	*/
	int startBuffer = 10, endBuffer = 5, zcrThreshold = 40, throwFrames = 5;

	/*
	framesPerc stores the %age of frames to be checked for high or low zcr values while detecting a word
	deviationPerc stores the amount of deviation we shall check for detecting the starting point
	range stores the maximum amplitude to be considered
	*/
	float framesPerc = 0.4, deviationPerc = 1.1, range = 5000.0f;
	
	
	/*variables which should not be touched*/

	/*
	normalizationFactor stores the normalization factor
	DCShift stores the DC shift value
	*/
	float normalizationFactor = 0.0f, DCShift = 0;
	
	/*
	ambientNoise stores the maximum ambient noise
	start stores the starting point of the word
	end stores the ending point of the word
	buffer is used to check whether the deviation should be considered or not
	basically it helps to cut the word more precisely
	distinguisher stores the number of frames that belong to the last 40%
	totalZcr stores the zcr value of the last 40% frames
	noOfSamples stores the total number of energy and zcr samples obtained from dataInputFile
	flag indicates whether a new word has been found or the pointers are still pointing to the older word
	*/
	int ambientNoise = 0, start = 0, end = 0, buffer = 0, distinguisher = 0, totalZcr = 0, noOfSamples = 0, flag = 0;
	
	//required index variables
	int i = 0, j = 0;
	long* energyArr = NULL, * zcrArr = NULL;//two arrays to store the energy values and zcr values

	


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*data processing and information gathering*/

	//find DC shift
	DCShift = findDCShift(DCShiftTestFile);

	//read the file where nothing is spoken(noiseInputFile) and write the energy and zcr to another file(noiseOutputFile)
	performDataExtraction(noiseInputFile,noiseOutputFile,1,DCShift);
	
	//find out the normalization factor of dataInputFile
	normalizationFactor = getNormalizationFactor(dataInputFile,range);

	//read the file where something is spoken(dataInputFile) and write the energy and zcr to another file(dataOutputFile)
	noOfSamples = performDataExtraction(dataInputFile,dataOutputFile,normalizationFactor,DCShift);

	//find out the maximum energy in the ambient noise case
	ambientNoise = getMaxEnergyFromNoise(noiseOutputFile);

	


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*finding out the word*/
	
	//initialize the arrays
	energyArr = new long[noOfSamples];
	zcrArr = new long[noOfSamples];

	storeData(dataOutputFile,energyArr,zcrArr); //copy the data from file to arrays

	ambientNoise *= normalizationFactor; //multiply the normalization factor to the ambient noise
	i = throwFrames+1; //initialize the index variable to 6
	buffer = 0; //reset the buffer variable
	while (i < noOfSamples) //till we reach the end of the arrays
	{
		flag = 0; //set flag to 0
		while (i < noOfSamples) //traverse from ith index
		{
			if (energyArr[i] > (deviationPerc)*ambientNoise) //if energy is increased by more than 10%
				++buffer; //increase the buffer
			else //if the flow of deviated energy is broken
				buffer = 0; //reset the buffer
			/*
 			if buffer becomes 10 that means we have found 10 continuous frames
 			with energy greater than ambient noise
			*/
			if (buffer >= startBuffer)
			{
				start = i-startBuffer; //set start to i-10
				flag = 1; //toggle flag
				break; //break from loop
			}
			++i; //increment i
		}

		if (flag == 0) break; //if no new starting points found, then stop the process
		buffer = 0; //reset the buffer
		flag = 0; //reset flag
		while (i < noOfSamples) //traverse from ith index
		{
			if (energyArr[i] <= ambientNoise) //if energy becomes less than ambient noise
				++buffer; //increase the buffer
			else //if the flow of reduced energy is broken
				buffer = 0; //reset buffer
			/*
			if buffer becomes 5 that means we have found 5 continuous frames
			with energy less than or equal to ambient noise
			*/
			if (buffer >= endBuffer)
			{
				end = i-endBuffer; //set end to i-5
				flag = 1; //toggle flag
				break; //break from loop
			}
			++i; //increment i
		}
		if (flag == 0) end = noOfSamples-1; //if end point is not found, set it to the last index
		distinguisher = (end-start+1)*(framesPerc); //find out the number of frames that belong to 40% of word size
		for (j = end; j > end-distinguisher; --j) //run the loop for the last 40% data
			totalZcr += zcrArr[j]; //keep adding the zcr
		totalZcr /= distinguisher; //find out the average zcr
		if (totalZcr > zcrThreshold) //if average zcr is more than the threshold
			printf("YES\n"); //fricative is spoken
		else //otherwise
			printf("NO\n"); //no fricative
	}

	//Breakpoint
	printf("Breakpoint");
	
	return 0;
}