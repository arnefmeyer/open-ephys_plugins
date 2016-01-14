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

#ifndef ChannelRefCanvas_H_
#define ChannelRefCanvas_H_

#include <EditorHeaders.h>
#include <VisualizerWindowHeaders.h>
#include <VisualizerEditorHeaders.h>

#include "ChannelRefNode.h"

class ElectrodeTableButton;
class ChannelRefDisplay;

class ChannelRefCanvas : public Visualizer, public Button::Listener, public ComboBox::Listener

{
public:
    ChannelRefCanvas(ChannelRefNode* n);
    ~ChannelRefCanvas();

	void paint(Graphics& g);

    void refresh();

	void beginAnimation();
    void endAnimation();

    void refreshState();
	void resized();

    void setParameter(int, float) {}
    void setParameter(int, int, int, float) {}

	void update();

    void buttonClicked(Button* button);

    void startRecording() { }
    void stopRecording() { }

	bool updateNeeded = false;

	void mouseDown(const MouseEvent& event);

	void comboBoxChanged(ComboBox* cb);

private:

	ScopedPointer<ChannelRefDisplay> display;
	ChannelRefNode* processor;
	ScopedPointer<Viewport> displayViewport;

	ScopedPointer<ComboBox> presetSelection;
	StringArray presets;

	ScopedPointer<UtilityButton> resetButton;
	ScopedPointer<UtilityButton> selectModeButton;
	ScopedPointer<UtilityButton> saveButton;
	ScopedPointer<UtilityButton> loadButton;

	OwnedArray<ElectrodeTableButton> electrodeButtons;

	int scrollBarThickness;
	int scrollDistance;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelRefCanvas);

};


class ElectrodeTableButton : public ElectrodeButton
{
public:

	ElectrodeTableButton(int index, int rowIndex_, int colIndex_) : ElectrodeButton(index), rowIndex(rowIndex_), colIndex(colIndex_)
	{
	}

	int getRowIndex()
	{
		return rowIndex;
	}

	int getColIndex()
	{
		return colIndex;
	}

private:

	int rowIndex;
	int colIndex;

};


class CarButton : public ElectrodeButton
{
public:

	CarButton(String label_, int channelIndex_) : ElectrodeButton(channelIndex_)
	{
		setButtonText(label_);
	}
};


class ChannelRefDisplay : public Component, public Button::Listener, public KeyListener
{
public:

    ChannelRefDisplay(ChannelRefNode*, ChannelRefCanvas*, Viewport*, bool selectMode = false);
    ~ChannelRefDisplay();

    void paint(Graphics& g);

    void resized();
	void update();

    void mouseDown(const MouseEvent& event);

	void buttonEvent(Button* button);
	void buttonClicked(Button* button);

	bool keyPressed(const KeyPress &key, Component *originatingComponent);

	void drawTable();
	void reset();
	void setEnableSingleSelectionMode(bool b);

	void applyPreset(String name);

private:

    int nChannelsBefore;
	bool singleSelectMode;
	int selectedRow;
	int selectedColumn;

	ChannelRefNode* processor;
	ChannelRefCanvas* canvas;
    Viewport* viewport;
	ScopedPointer<Label> staticLabel;

	OwnedArray<ElectrodeTableButton> electrodeButtons;
	OwnedArray<CarButton> carButtons;
	OwnedArray<Label> rowLabels;
	OwnedArray<Label> headerLabels;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelRefDisplay);

};


#endif
