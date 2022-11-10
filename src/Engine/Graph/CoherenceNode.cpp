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

// include required headers
#include "CoherenceNode.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
CoherenceNode::CoherenceNode(Graph* graph) : ProcessorNode(graph, new CoherenceProcessor())
{
	mSettings.mIntervalDuration = 1.0;
	mSettings.mNumSamples = 0.0;
}


// destructor
CoherenceNode::~CoherenceNode()
{
}


// initialize the node
void CoherenceNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// SETUP PORTS

	GetInputPort(INPUTPORT_X).SetupAsChannels<double>("x", "x1", INPUTPORT_X);
	GetInputPort(INPUTPORT_Y).SetupAsChannels<double>("y", "x2", INPUTPORT_Y);
	GetOutputPort(OUTPUTPORT_RESULT).SetupAsChannels<double>("out", "y1", OUTPUTPORT_RESULT);
	
	// SETUP ATTRIBUTES

	// interval length
	Core::AttributeSettings* numInputsAttr = RegisterAttribute( "Window size", "WindowSize", "The length of the interval that will be looked at. Set to 0 to use an infinite interval.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSLIDER );
	numInputsAttr->SetDefaultValue(Core::AttributeFloat::Create( mSettings.mIntervalDuration ));
	numInputsAttr->SetMinValue(Core::AttributeFloat::Create( 1.0 ));
	numInputsAttr->SetMaxValue(Core::AttributeFloat::Create( 5.0 ));
}


void CoherenceNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	// calculate the interval length in number of samples
	const double sampleRate = mInputChannels.GetSampleRate(); 
	mSettings.mNumSamples = mSettings.mIntervalDuration * sampleRate;

	MultiChannel* channelsX = GetInputPort(INPUTPORT_X).GetChannels();
	MultiChannel* channelsY = GetInputPort(INPUTPORT_Y).GetChannels();

	if (channelsX != NULL && channelsY != NULL)
	{
		const double sampleRateX = channelsX->GetSampleRate();
		const double sampleRateY = channelsY->GetSampleRate();

		if (sampleRateX != sampleRateY) {
			mIsInitialized = false;
			mNeedResetNode = true;
			SetError(ERROR_INPUT_MATCHING_SAMPLERATES, "Input sample rates are incompatible.");
			return;
		} else {
			ClearError(ERROR_INPUT_MATCHING_SAMPLERATES);
		}
	} else {
		mNeedResetNode = true;
		mIsInitialized = false;
		return;
	}

	// reset the node if needed
	if (mNeedResetNode) {
		mNeedResetNode = false;
		ResetAsync();
	}

	PostReInit(elapsed, delta);
}

// update the node
void CoherenceNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);
}


// attributes have changed
void CoherenceNode::OnAttributesChanged()
{
	// need reset node if interval duration has changed (input epoch length increases)
	const double intervalLength = GetFloatAttribute(ATTRIB_WINDOWSIZE);
	if (mSettings.mIntervalDuration != intervalLength)
	{
		mSettings.mIntervalDuration = intervalLength;
		ResetAsync();
		
	}
	else
	{
		// reconfigure processors on the fly
		SetupProcessors();
	}
}
