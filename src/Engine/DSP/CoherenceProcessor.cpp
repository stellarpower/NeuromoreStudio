/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/

// include precompiled header
#include <Engine/Precompiled.h>

// include required files
#include "CoherenceProcessor.h"
#include "../Core/LogManager.h"
#include "Channel.h"
#include "ChannelProcessor.h"
#include "WindowFunction.h"

#include <numeric>

using namespace Core;

// constructor
CoherenceProcessor::CoherenceProcessor()
{
	Init();
}


// destructor
CoherenceProcessor::~CoherenceProcessor()
{
}


// init DFT processor 
void CoherenceProcessor::Init()
{
	AddInput<double>();
	AddInput<double>();
	AddOutput<double>();

	mWindow.reserve(mNperseg);

	// Use Hann window by default.
	WindowFunction hannWinFunc;
	hannWinFunc.SetType(WindowFunction::EWindowFunction::WINDOWFUNCTION_HANN);
	for (uint32 i = 0; i < mNperseg; ++i) {
		mWindow.push_back(hannWinFunc.Evaluate(i, mNperseg));
	}
}

// reinit processor internals
void CoherenceProcessor::ReInit()
{
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;

	ChannelBase* inputX = GetInput(0);
    ChannelBase* inputY = GetInput(1);
	ChannelBase* output = GetOutput(0);

	// nothing to do until both channels are connected
	if (inputX == NULL || inputY == NULL)
		return;

	const double samplesRate = inputX->GetSampleRate();

	// nothing to do if channels sample rate is not the same
	if (samplesRate != inputY->GetSampleRate())
		return;


	if (mSettings.mNumSamples == 0)
	{
		mIsInitialized = false;
		return;
	}

	// init FFT
	mFFT.Init(mNperseg);
	
	// set output sample rate
	output->SetSampleRate(samplesRate);

	mIsInitialized = true;
}


void CoherenceProcessor::Update()
{
	if (mIsInitialized == false)
		return;
			
	// update base
	ChannelProcessor::Update();
	
	// channel inputs reader
	ChannelReader* inputX = GetInputReader(0);
	ChannelReader* inputY = GetInputReader(1);

	Channel<double>* coherenceOutput = GetOutput(0)->AsType<double>();

	const uint32 fftResLength = mNperseg / 2 + 1;

	// number of new epochs we can process
	const uint32 numNewEpochs = inputX->GetNumEpochs();

	if (numNewEpochs != inputY->GetNumEpochs()) {
		return;
	}

	for (uint32 e = 0; e < numNewEpochs; ++e)
	{
		// get the inputs epoch
		Epoch inputEpochX = inputX->PopOldestEpoch();
		Epoch inputEpochY = inputY->PopOldestEpoch();

		std::vector<double> dataX;
		std::vector<double> dataY;

		const uint32 numSamples = inputEpochX.GetLength();

		// nothing to do if epochs length is not same.
		if (numSamples != inputEpochY.GetLength()) {
			continue;
		}

		for (uint32 i = 0; i < numSamples; ++i)
		{
			dataX.push_back(inputEpochX.GetSample(i));
			dataY.push_back(inputEpochY.GetSample(i));
		}

		double* inputBuffer = mFFT.GetInput();

		std::vector<Core::Complex> fftX(fftResLength);
		std::vector<Core::Complex> fftY(fftResLength);
		std::vector<Core::Complex> pXX(fftResLength);
		std::vector<Core::Complex> pYY(fftResLength);
		std::vector<Core::Complex> pXY(fftResLength);

		// use Welch's method
		for (uint32 segIdx = 0; segIdx < dataX.size() / mNperseg; ++segIdx) {
			std::vector<double> newDatax(dataX.begin() + segIdx*mNperseg, dataX.begin() + (segIdx+1)*mNperseg);
			std::vector<double> newDatay(dataY.begin() + segIdx*mNperseg, dataY.begin() + (segIdx+1)*mNperseg);
			const auto count = static_cast<float>(newDatax.size());
	    	const auto meanx = std::reduce(newDatax.begin(), newDatax.end()) / count;

			for (uint32 i = 0; i < newDatax.size(); ++i) {
				inputBuffer[i] = (newDatax[i] - meanx)*mWindow[i];
			}
			mFFT.CalcFFT();

			auto fftRes = mFFT.GetOutput();

			// calculate power spectral density for the X and Y signals
			for (uint32 i = 0; i < fftResLength; ++i) {
				fftX[i] = fftRes[i];
				pXX[i] += ComplexMath::Conjugate(fftRes[i])*fftRes[i];
			}

			const auto meany = std::reduce(newDatay.begin(), newDatay.end()) / count;

			for (uint32 i = 0; i < newDatay.size(); ++i) {
				inputBuffer[i] = (newDatay[i] - meany)*mWindow[i];
			}
			mFFT.CalcFFT();
			fftRes = mFFT.GetOutput();

			// calculate power spectral density for the Y signal and cross spectral density for X and Y signals

			for (uint32 i = 0; i < fftResLength; ++i) {
				fftY[i] = fftRes[i];
				pYY[i] += ComplexMath::Conjugate(fftRes[i])*fftRes[i];
				pXY[i] += ComplexMath::Conjugate(fftX[i])*fftY[i];
			}
		}

		// calculate coherence and add its average to output.
		std::vector<double> coh(fftResLength);

		for (uint32 i = 0; i < pXX.size(); ++i) {
			if (pXX[i].mReal == 0 || pYY[i].mReal == 0) {
				coh[i] = 0;
			} else {
				coh[i] = (ComplexMath::Pow(pXY[i].Norm(), 2) / pXX[i].mReal / pYY[i].mReal).mReal;
			}
		}
		coherenceOutput->AddSample(std::reduce(coh.begin(), coh.end()) / coh.size());
	}
}


void CoherenceProcessor::Setup(const ChannelProcessor::Settings& settings)
{
	mSettings = static_cast<const CoherenceSettings&>(settings);

	// configure inputs reader epoching
	ChannelReader*	inputReaderX = GetInputReader(0);
	inputReaderX->SetEpochLength(mSettings.mNumSamples);
	inputReaderX->SetEpochShift(1);

	ChannelReader*	inputReaderY = GetInputReader(1);
	inputReaderY->SetEpochLength(mSettings.mNumSamples);
	inputReaderY->SetEpochShift(1);
}

