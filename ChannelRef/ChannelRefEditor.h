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

#ifndef __CHANNELREFEDITOR_H__
#define __CHANNELREFEDITOR_H__

#include <EditorHeaders.h>
#include <VisualizerWindowHeaders.h>
#include <VisualizerEditorHeaders.h>


#include "ChannelRefCanvas.h"


/**

  User interface for the Channel Refence processor.

  @see ChannelRefNode

*/

class ChannelRefEditor : public VisualizerEditor,
    public DragAndDropContainer

{
public:

    ChannelRefEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    virtual ~ChannelRefEditor();

	Visualizer* createNewCanvas();
    void updateSettings();

    void saveCustomParameters(XmlElement* xml);
    void loadCustomParameters(XmlElement* xml);

    void saveParametersDialog();
    void loadParametersDialog();

    String writePrbFile(File filename);
    String loadPrbFile(File filename);

private:

	ChannelRefCanvas* chanRefCanvas;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelRefEditor);

};

#endif  // __CHANNELREFEDITOR_H__
