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

#ifndef __CHANNELREFNODE_H__
#define __CHANNELREFNODE_H__


#include <ProcessorHeaders.h>

#define BUFFER_SIZE 1024

class ReferenceMatrix;


/**

  ChannelRefNode

  Allows the user to select arbitrary (virtual) reference channels for each 
  recording channel.

  @see GenericProcessor

*/

class ChannelRefNode : public GenericProcessor

{
public:

    ChannelRefNode();
    ~ChannelRefNode();

    void process(AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    void setParameter(int parameterIndex, float newValue);

    AudioProcessorEditor* createEditor();

    bool hasEditor() const
    {
        return true;
    }

    void updateSettings();

	ReferenceMatrix* getReferenceMatrix();

private:

	ReferenceMatrix* refMat;
	AudioSampleBuffer channelBuffer;
	AudioSampleBuffer avgBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelRefNode);

};


/**

  Reference matrix

  Each row indicates the selected reference channels for each channel.

  1: selected
  0: not selected

  TODO allow values between 0 and 1 to set the gain of each reference channel

  @see ChannelRefNode

*/

class ReferenceMatrix
{
public:

    ReferenceMatrix(int nChan);
    ~ReferenceMatrix();

	void setNumberOfChannels(int n);
	int getNumberOfChannels();

	void update();

	void setValue(int rowIndex, int colIndex, float value);
	float getValue(int rowIndex, int colIndex);

	float* getChannel(int index);
	bool allChannelReferencesActive(int index);

	void setAll(float value);
	void setAll(float value, int maxChan);
	void clear();

	void print();

private:

	int nChannels;
	int nChannelsBefore;
	float* values;

};


#endif  //__CHANNELREFNODE_H__

