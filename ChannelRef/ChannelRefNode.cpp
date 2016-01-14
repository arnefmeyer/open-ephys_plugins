/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
Copyright (C) 2014 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include "ChannelRefNode.h"
#include "ChannelRefEditor.h"


#define MIN(a,b) (((a)<(b))?(a):(b))


ChannelRefNode::ChannelRefNode()
    : GenericProcessor("Channel Ref"), channelBuffer(1, BUFFER_SIZE), avgBuffer(1, BUFFER_SIZE)
{
	int nChannels = getNumInputs();
	refMat = new ReferenceMatrix(nChannels);
}

ChannelRefNode::~ChannelRefNode()
{
	delete refMat;
}

AudioProcessorEditor* ChannelRefNode::createEditor()
{
    editor = new ChannelRefEditor(this, true);
    return editor;
}



void ChannelRefNode::updateSettings()
{
	int nChannels = getNumInputs();

	if (refMat == nullptr)
	{
		refMat = new ReferenceMatrix(nChannels);
	}
	else
	{
		refMat->setNumberOfChannels(nChannels);
	}

	if (editor != nullptr)
	{
		editor->updateSettings();
	}
}


void ChannelRefNode::setParameter(int parameterIndex, float newValue)
{

}

void ChannelRefNode::process(AudioSampleBuffer& buffer,
                             MidiBuffer& midiMessages)
{
	float* ref;
	float gain;
	int numRefs;
	int numChan = refMat->getNumberOfChannels();

	channelBuffer = buffer;

	for (int i=0; i<numChan; i++)
	{
		avgBuffer.clear();

		ref = refMat->getChannel(i);
		numRefs = 0;
		for (int j=0; j<numChan; j++)
		{
			if (ref[j] > 0)
			{
				numRefs++;
			}
		}

		for (int j=0; j<numChan; j++)
		{
			if (ref[j] > 0)
			{
				gain = 1.0f / float(numRefs);
				avgBuffer.addFrom(0, 0, channelBuffer, j, 0, channelBuffer.getNumSamples(), gain);
			}
		}
		

		if (numRefs > 0)
		{
			buffer.addFrom(i, 			// destChannel
						0, 				// destStartSample
						avgBuffer, 	// source
						0,  			// sourceChannel
						0, 				// sourceStartSample
						buffer.getNumSamples(), // numSamples
						-1.0f); 	    // gain to apply 
		}
	}
}

ReferenceMatrix* ChannelRefNode::getReferenceMatrix()
{
	return refMat;
}


ReferenceMatrix::ReferenceMatrix(int nChan)
{
	nChannels = nChan;
	nChannelsBefore = -1;
	values = nullptr;
	update();
}

ReferenceMatrix::~ReferenceMatrix()
{
	if (values != nullptr)
		delete[] values;
}

void ReferenceMatrix::setNumberOfChannels(int n)
{
	nChannels = n;
	update();
}

int ReferenceMatrix::getNumberOfChannels()
{
	return nChannels;
}

void ReferenceMatrix::update()
{
	if (nChannels != nChannelsBefore)
	{
		if (values != nullptr)
			delete[] values;

		values = new float[nChannels * nChannels];
		for (int i=0; i<nChannels * nChannels; i++)
			values[i] = 0;

		nChannelsBefore = nChannels;
	}
}

void ReferenceMatrix::setValue(int rowIndex, int colIndex, float value)
{
	if (rowIndex >= 0 && rowIndex < nChannels && colIndex >= 0 && colIndex < nChannels)
	{
		values[rowIndex * nChannels + colIndex] = value;
	}
	else
	{
		std::cout << "RefMatrix::setValue INDEX OUT OF BOUNDS! (rowIndex=" << rowIndex << ", colIndex=" << colIndex << ")" << std::endl;
	}
}

float ReferenceMatrix::getValue(int rowIndex, int colIndex)
{
	float value = -1;
	if (rowIndex >= 0 && rowIndex < nChannels && colIndex >= 0 && colIndex < nChannels)
	{
		value = values[rowIndex * nChannels + colIndex];
	}

	return value;
}

float* ReferenceMatrix::getChannel(int index)
{
	if (index >= 0 && index < nChannels)
		return &values[index * nChannels];
	else
		return nullptr;
}

bool ReferenceMatrix::allChannelReferencesActive(int index)
{
	float* chan = getChannel(index);

	int nActive = 0;

	if (chan != nullptr)
	{
		for (int i=0; i<nChannels; i++)
		{
			if (chan[i] > 0)
			{
				nActive++;
			}
		}
	}

	return nActive == nChannels;
}

void ReferenceMatrix::setAll(float value)
{
	if (values != nullptr)
	{
		for (int i=0; i<nChannels; i++)
		{
			for (int j=0; j<nChannels; j++)
			{
				values[i*nChannels + j] = value;
			}
		}
	}
}

void ReferenceMatrix::setAll(float value, int maxChan)
{
	if (values != nullptr)
	{
		maxChan = MIN(nChannels, maxChan);
		for (int i=0; i<maxChan; i++)
		{
			for (int j=0; j<maxChan; j++)
			{
				values[i*nChannels + j] = value;
			}
		}
	}
}

void ReferenceMatrix::clear()
{
	if (values != nullptr)
	{
		for (int i=0; i<nChannels; i++)
		{
			for (int j=0; j<nChannels; j++)
			{
				values[i*nChannels + j] = 0;
			}
		}
	}
}

void ReferenceMatrix::print()
{
	for (int i=0; i<nChannels; i++)
	{
		float* chan = getChannel(i);
		for (int j=0; j<nChannels; j++)
		{
			std::cout << chan[j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

