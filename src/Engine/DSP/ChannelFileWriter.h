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

#ifndef __NEUROMORE_CHANNELFILEWRITER_H
#define __NEUROMORE_CHANNELFILEWRITER_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../EngineManager.h"
#include <edflib/edflib.h>
#include "ChannelBase.h"


// (de)serialize channels and multichannels
class ENGINE_API ChannelFileWriter
{
	public:

		// the supported container formats
		enum EFormat
		{
			FORMAT_CSV_SIMPLE,
			FORMAT_CSV_TIMESTAMP,
			FORMAT_EDF_PLUS,
			//FORMAT_RAW_DOUBLE,
			NUM_FORMATS
		};

		// constructor & destructor
		ChannelFileWriter() : mSamples(0), mSampleRate(0) { memset(mBuffer, 0, sizeof(mBuffer)); }
		~ChannelFileWriter()		{}

		static const char* GetFormatName(EFormat format);
		static const char* GetFormatExtension(EFormat format);

		//
		// main methods
		//

		// Write all samples contained in a set of channels to a file in the specified format. If write fails false will be returned and the file will hold the data that was sucessfully written before the failure occured.
		bool Write(EFormat format, const Core::Array<Channel<double>*>& channels, FILE* file, int handle, double phyiscalMin, double phyiscalMax);
		
		// write only the file header only
		bool WriteHeader(EFormat format, const Core::Array<Channel<double>*>& channels, FILE* file, int handle, double phyiscalMin, double phyiscalMax);
		
		// appends the last N sampels to the file
		bool WriteSamples(EFormat format, const Core::Array<Channel<double>*>& channels, uint32 numSamples, FILE* file, int edfHandler);

	private:

		Core::String mTempString;	// for formatting stuff

		// EDF+ buffer for 1s block-writes
		// holding up to 64 double channels at 512hz
		static constexpr uint32_t MAXCHANNELS = 64;
		static constexpr uint32_t MAXSAMPLERATE = 512;
		double   mBuffer[MAXCHANNELS*MAXSAMPLERATE];
		uint32_t mSampleRate;
		uint32_t mSamples;

		//
		// individual formats
		//

		// CSV
		bool WriteHeaderCSV(const Core::Array<Channel<double>*>& inChannels, bool useTimestamps, FILE* outFile);
		bool WriteSamplesCSV(const Core::Array<Channel<double>*>& inChannels, uint32 numSamples, bool useTimestamps, uint32 numDigits, FILE* outFile);

		// .nmd
		bool WriteSamplesRawDouble(const Core::Array<Channel<double>*>& inChannels, uint32 numSamples, FILE* outFile);

		// edf plus
		bool WriteHeaderEDF(const Core::Array<Channel<double>*>& inChannels, int handle, double phyiscalMin, double phyiscalMax);
		bool WriteSamplesEDF(const Core::Array<Channel<double>*>& inChannels, uint32 numSamples, int handle);
};


#endif
