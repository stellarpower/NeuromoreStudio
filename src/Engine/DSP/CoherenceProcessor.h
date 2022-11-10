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

#ifndef __NEUROMORE_COHERENCEPROCESSOR_H
#define __NEUROMORE_COHERENCEPROCESSOR_H

// include required headers
#include "../Config.h"
#include "ChannelProcessor.h"
#include "FFT.h"
#include "Channel.h"

// FFT processor
class ENGINE_API CoherenceProcessor : public ChannelProcessor
{
	public:
		enum { TYPE_ID = 0x003b };

		class CoherenceSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x003b };

				CoherenceSettings()									{ mNumSamples = 0; mIntervalDuration = 1.0; }
				virtual ~CoherenceSettings()					    {}
			
				uint32 GetType() const override					{ return CoherenceProcessor::TYPE_ID; }

				double	mIntervalDuration;			// length of the interval, expressed in seconds
				uint32	mNumSamples;				// length of the interval, expressed in samples
		};

		// constructors & destructor
		CoherenceProcessor();
		virtual ~CoherenceProcessor();

		uint32 GetType() const override											{ return TYPE_ID; }
		ChannelProcessor* Clone() override										{ CoherenceProcessor* clone = new CoherenceProcessor(); return clone; }

		void Init() override;
		void ReInit() override;
		void Update() override;

		// settings
		void Setup(const ChannelProcessor::Settings& settings) override;
		const Settings& GetSettings() const	override							{ return mSettings; }
	private:
		// processor settins
		CoherenceSettings			mSettings;	

		// FFT
		FFT					mFFT;
		std::vector<double> mWindow;

		// Length of each segment. Defaults to 64.
		const uint32 mNperseg = 64;
};

#endif