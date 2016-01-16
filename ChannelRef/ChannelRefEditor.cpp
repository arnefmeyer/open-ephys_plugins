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


#include "ChannelRefEditor.h"
#include "ChannelRefNode.h"


ChannelRefEditor::ChannelRefEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors=true)
    : VisualizerEditor(parentNode, useDefaultParameterEditors), chanRefCanvas(nullptr)
{	
    tabText = "Channel Ref";
    desiredWidth = 180;
}

ChannelRefEditor::~ChannelRefEditor()
{
	deleteAllChildren();
}

Visualizer* ChannelRefEditor::createNewCanvas()
{

    ChannelRefNode* processor = (ChannelRefNode*) getProcessor();
	chanRefCanvas = new ChannelRefCanvas(processor);
    return chanRefCanvas;
}

void ChannelRefEditor::updateSettings()
{
	if (chanRefCanvas != nullptr)
	{
		chanRefCanvas->update();
	}
}

void ChannelRefEditor::saveCustomParameters(XmlElement* xml)
{
	ChannelRefNode* p = dynamic_cast<ChannelRefNode*>(getProcessor());
	ReferenceMatrix* refMat = p->getReferenceMatrix();
	int nChannels = refMat->getNumberOfChannels();

    xml->setAttribute("Type", "ChannelRefEditor");

	/* global gain */
    XmlElement* paramXml = xml->createNewChildElement("PARAMETERS");
    paramXml->setAttribute("GlobalGain", p->getGlobalGain());

	/* references for each channel */
	XmlElement* channelsXml = xml->createNewChildElement("REFERENCES");

    for (int i=0; i<nChannels; i++)
    {
		float* ref = refMat->getChannel(i);
 
        XmlElement* channelXml = channelsXml->createNewChildElement("CHANNEL");
        channelXml->setAttribute("Index", i+1);
		for (int j=0; j<nChannels; j++)
		{
			if (ref[j] > 0)
			{
				XmlElement* refXml = channelXml->createNewChildElement("REFERENCE");
				refXml->setAttribute("Index", j+1);
				refXml->setAttribute("Value", ref[j]);
			}
		}
    }
}

void ChannelRefEditor::loadCustomParameters(XmlElement* xml)
{
	ChannelRefNode* p = dynamic_cast<ChannelRefNode*>(getProcessor());
	ReferenceMatrix* refMat = p->getReferenceMatrix();

	forEachXmlChildElementWithTagName(*xml,	paramXml, "PARAMETERS")
	{
    	float globGain = (float)paramXml->getDoubleAttribute("GlobalGain");
		p->setGlobalGain(globGain);
	}

	forEachXmlChildElementWithTagName(*xml,	channelsXml, "REFERENCES")
	{
		forEachXmlChildElementWithTagName(*channelsXml,	channelXml, "CHANNEL")
		{
			int channelIndex = channelXml->getIntAttribute("Index");

			forEachXmlChildElementWithTagName(*channelXml,	refXml, "REFERENCE")
			{
				int refIndex = refXml->getIntAttribute("Index");
				float gain = (float)refXml->getDoubleAttribute("Value");
				refMat->setValue(channelIndex - 1, refIndex - 1, gain);
			}
		}
	}

	updateSettings();
}

void ChannelRefEditor::saveParametersDialog()
{
    if (!acquisitionIsActive)
    {
        FileChooser fc("Choose the file name...",
                       File::getCurrentWorkingDirectory(),
                       "*",
                       true);

        if (fc.browseForFileToSave(true))
        {
            File fileToSave = fc.getResult();

			XmlElement* xml = new XmlElement("SETTINGS");
			saveCustomParameters(xml);
			if(!xml->writeToFile(fileToSave, String::empty))
			{
				CoreServices::sendStatusMessage("Couldn't save channel reference data to file.");
			}
			else
			{
				CoreServices::sendStatusMessage("Saved channel reference data to file " + fileToSave.getFullPathName());
			}
			delete xml;
        }
    } else
	{
		CoreServices::sendStatusMessage("Stop acquisition before saving channel references.");
    }
}

void ChannelRefEditor::loadParametersDialog()
{
    if (!acquisitionIsActive)
    {
        FileChooser fc("Choose the file name...",
                       File::getCurrentWorkingDirectory(),
                       "*",
                       true);

        if (fc.browseForFileToOpen())
        {
            File fileToOpen = fc.getResult();

			XmlElement* xml = XmlDocument::parse(fileToOpen);
			loadCustomParameters(xml);
			CoreServices::sendStatusMessage("Loaded channel reference data from file." + fileToOpen.getFullPathName());
			delete xml;
        }
    } else
	{
		CoreServices::sendStatusMessage("Stop acquisition before loading channel references.");
    }
}

