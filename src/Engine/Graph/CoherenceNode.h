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

#ifndef __NEUROMORE_COHERECENODE_H
#define __NEUROMORE_COHERECENODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/CoherenceProcessor.h"


class ENGINE_API CoherenceNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x003b };

		static const char* Uuid() { return "3b5f505b-756f-402c-aab4-8bdaa3eec8b0"; }

		enum
		{
			ATTRIB_WINDOWSIZE			= 0,
		};

		enum
		{
			INPUTPORT_X			= 0,
			INPUTPORT_Y			= 1,
			OUTPUTPORT_RESULT	= 0
		};

		// constructor & destructor
		CoherenceNode(Graph* graph);
		~CoherenceNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		Core::Color GetColor() const override								{ return Core::RGBA(204, 255, 51); }
		uint32 GetType() const override										{ return TYPE_ID; }
		const char* GetTypeUuid() const override final						{ return Uuid(); }
		const char* GetReadableType() const override						{ return "Coherence"; }
		const char* GetRuleName() const override final						{ return "NODE_Coherence"; }
		uint32 GetPaletteCategory() const override							{ return CATEGORY_DSP; }
		GraphObject* Clone(Graph* graph) override							{ CoherenceNode* clone = new CoherenceNode(graph); return clone; }

		virtual void OnAttributesChanged() override;

		const ChannelProcessor::Settings& GetSettings() override			{ return mSettings; }

	// settings
	protected:
		CoherenceProcessor::CoherenceSettings  		mSettings;

		bool mNeedResetNode = false;
};


#endif
