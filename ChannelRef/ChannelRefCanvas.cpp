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


#include "ChannelRefCanvas.h"
#include "ChannelRefEditor.h"


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


ChannelRefCanvas::ChannelRefCanvas(ChannelRefNode* n) :
    processor(n)
{

    displayViewport = new Viewport("ChannelRefDisplay");
	display = new ChannelRefDisplay(processor, this, displayViewport);
	displayViewport->setViewedComponent(display, false);
    displayViewport->setScrollBarsShown(true, true);
	addAndMakeVisible(displayViewport);

	scrollBarThickness = displayViewport->getScrollBarThickness();
	scrollDistance = 0;

    resetButton = new UtilityButton("Reset", Font("Small Text", 13, Font::plain));
    resetButton->setRadius(3.0f);
    resetButton->addListener(this);
    addAndMakeVisible(resetButton);

    selectModeButton = new UtilityButton("Single mode", Font("Small Text", 13, Font::plain));
    selectModeButton->setRadius(4.0f);
    selectModeButton->setClickingTogglesState(true);
    selectModeButton->setToggleState(false, sendNotification);
    selectModeButton->addListener(this);
    addAndMakeVisible(selectModeButton);

    saveButton = new UtilityButton("Save", Font("Small Text", 13, Font::plain));
    saveButton->setRadius(3.0f);
    saveButton->addListener(this);
    addAndMakeVisible(saveButton);

    loadButton = new UtilityButton("Load", Font("Small Text", 13, Font::plain));
    loadButton->setRadius(3.0f);
    loadButton->addListener(this);
    addAndMakeVisible(loadButton);

    gainSlider = new Slider("Gain");
	gainSlider->setSliderStyle(Slider::Rotary);
    gainSlider->setRange(0.0f, 2.0f);
    gainSlider->setTextBoxStyle(Slider::TextBoxRight, false, 50, 30);
    gainSlider->setValue(1.0f);
    gainSlider->addListener(this);
    addAndMakeVisible(gainSlider);

	presetNamesLabel = new Label("PresetLabel", "Preset");
	addAndMakeVisible(presetNamesLabel);

	presetNames.add("None");
	presetNames.add("Common average reference");
	presetNames.add("Avg of other tetrodes");
	presetNames.add("Avg of next tetrode");

    presetNamesBox = new ComboBox("Presets");
    presetNamesBox->addItemList(presetNames, 1);
    presetNamesBox->setSelectedId(1, sendNotification);
    presetNamesBox->setEditableText(false);
    presetNamesBox->addListener(this);
    addAndMakeVisible(presetNamesBox);

	channelCountLabel = new Label("ChanCountLabel", "# channels");
	addAndMakeVisible(channelCountLabel);

	for (int i=1; i<=32; i++)
	{
		channelCounts.add(String(i*16));
	}

    channelCountBox = new ComboBox("Channels");
    channelCountBox->addItemList(channelCounts, 1);
    channelCountBox->setSelectedId(1, sendNotification);
    channelCountBox->setEditableText(false);
    channelCountBox->addListener(this);
    addAndMakeVisible(channelCountBox);

    update();
}

ChannelRefCanvas::~ChannelRefCanvas()
{
}

void ChannelRefCanvas::beginAnimation()
{
//    startCallbacks();
}

void ChannelRefCanvas::endAnimation()
{
//    stopCallbacks();
}


void ChannelRefCanvas::paint(Graphics& g)
{
    g.fillAll(Colours::grey);
}

void ChannelRefCanvas::refresh()
{
    repaint();
}

void ChannelRefCanvas::refreshState()
{
	update();
    resized();
}

void ChannelRefCanvas::resized()
{
    displayViewport->setBounds(20, 20, getWidth() - 40, getHeight()-90);
    resetButton->setBounds(10, getHeight()-60, 100, 20);
	selectModeButton->setBounds(110, getHeight()-60, 100, 20);
	loadButton->setBounds(10, getHeight()-30, 100, 20);
	saveButton->setBounds(110, getHeight()-30, 100, 20);

	gainSlider->setBounds(240, getHeight()-65, 120, 65);

	channelCountLabel->setBounds(380, getHeight()-30, 100, 20);
	channelCountBox->setBounds(460, getHeight()-30, 250, 20);
	presetNamesLabel->setBounds(380, getHeight()-60, 100, 20);
	presetNamesBox->setBounds(460, getHeight()-60, 250, 20);
}

void ChannelRefCanvas::update()
{
	display->update();
	gainSlider->setValue(processor->getGlobalGain());
}

void ChannelRefCanvas::mouseDown(const MouseEvent& event)
{

}

void ChannelRefCanvas::buttonClicked(Button* b)
{
	UtilityButton *button = dynamic_cast<UtilityButton*>(b);

	if (button == resetButton)
	{
		display->reset();
	}
	else if (button == selectModeButton)
	{
		display->setEnableSingleSelectionMode(button->getToggleState());
		display->grabKeyboardFocus();
	}
	else if (button == loadButton)
	{
		ChannelRefEditor* editor = dynamic_cast<ChannelRefEditor*>(processor->getEditor());
		editor->loadParametersDialog();
	}
	else if (button == saveButton)
	{
		ChannelRefEditor* editor = dynamic_cast<ChannelRefEditor*>(processor->getEditor());
		editor->saveParametersDialog();
	}
}

void ChannelRefCanvas::comboBoxChanged(ComboBox* cb)
{
	if (cb == presetNamesBox || cb == channelCountBox)
	{
		String presetName = presetNames[presetNamesBox->getSelectedId()-1];
		String s = channelCounts[channelCountBox->getSelectedId()-1];
		int numChannels = s.getIntValue();
		display->applyPreset(presetName, numChannels);
	}
}

void ChannelRefCanvas::sliderValueChanged(Slider* slider)
{
    if (slider == gainSlider)
	{
		processor->setGlobalGain(gainSlider->getValue());
	}
}



// ----------------------------------------------------------------

ChannelRefDisplay::ChannelRefDisplay(ChannelRefNode* n, ChannelRefCanvas* c, Viewport* v, bool selectMode) :
    processor(n), canvas(c), viewport(v), nChannelsBefore(-1), singleSelectMode(selectMode)
{
	addKeyListener(this);
	drawTable();
}

ChannelRefDisplay::~ChannelRefDisplay()
{
}

void ChannelRefDisplay::drawTable()
{
	int nChannels = processor->getReferenceMatrix()->getNumberOfChannels();

	if (nChannels != nChannelsBefore)
	{
		/* Move these values somewhere else? */
		int xOffset = 50;
		int yOffset = 1;
		int cellWidth = 19;
		int cellHeight = 15;
		int vSpace = 1;
		int headerHeight = 20;
		int carWidth = 35;

		int totalWidth = xOffset + carWidth + nChannels * cellWidth;
		int totalHeigth = yOffset + headerHeight + nChannels * (cellHeight + vSpace);

		headerLabels.clear();
		rowLabels.clear();
		electrodeButtons.clear();
		carButtons.clear();

		/* Create header */
		Font font(14, Font::plain);

		Label* header1 = new Label("headerCol1", "Target");
		header1->setJustificationType(Justification::horizontallyCentred);
		header1->setBounds(0, yOffset, xOffset, headerHeight);
		header1->setFont(font);
		addAndMakeVisible(header1);

		headerLabels.add(header1);

		Label* header2 = new Label("headerCol2", "CAR");
		header2->setJustificationType(Justification::horizontallyCentred);
		header2->setBounds(xOffset, yOffset, carWidth, headerHeight);
		header2->setFont(font);
		addAndMakeVisible(header2);

		headerLabels.add(header2);

		Label* header3 = new Label("headerCol3", "Reference(s)");
		header3->setJustificationType(Justification::horizontallyCentred);
		header3->setBounds(xOffset + carWidth, yOffset, totalWidth - xOffset - carWidth, headerHeight);
		header3->setFont(font);
		addAndMakeVisible(header3);

		headerLabels.add(header3);

		/* Add button matrix */
		for (int i=0; i<nChannels; i++)
		{
			/* Row label */
			Label* label = new Label(String::formatted("row_label_%d", i), String::formatted("%d", i+1));
			label->setJustificationType(Justification::horizontallyCentred);
			label->setBounds(0, yOffset + headerHeight + i*(cellHeight + vSpace), xOffset, cellHeight);
			label->setFont(font);
			addAndMakeVisible(label);

			rowLabels.add(label);

			/* Button to select all channels as reference (aka common average reference) */
			CarButton* cb = new CarButton("all", i);
			cb->setToggleState(false, dontSendNotification);
			cb->setBounds(xOffset + 5, yOffset + headerHeight + i*(cellHeight + vSpace), carWidth - 10, cellHeight);
			cb->addListener(this);
			cb->setRadioGroupId(0);
			addAndMakeVisible(cb);

			carButtons.add(cb);

			for (int j=0; j<nChannels; j++)
			{
				bool state = processor->getReferenceMatrix()->getValue(i, j) > 0;

				ElectrodeTableButton* button = new ElectrodeTableButton(j+1, i, j);
				button->setToggleState(state, dontSendNotification);
				button->setRadioGroupId(0);
				button->setBounds(xOffset + carWidth + j*cellWidth, yOffset + headerHeight + i*(cellHeight + vSpace), cellWidth, cellHeight);
				button->addListener(this);
				addAndMakeVisible(button);

				electrodeButtons.add(button);
			}
		}

		setSize(totalWidth, totalHeigth);
		setBounds(0, 0, totalWidth, totalHeigth);
	}
	else
	{
		/* Only update toggle states of buttons */
		for (int i=0; i<nChannels; i++)
		{
			for (int j=0; j<nChannels; j++)
			{
				bool state = processor->getReferenceMatrix()->getValue(i, j) > 0;

				ElectrodeTableButton* button = electrodeButtons[i*nChannels + j];
				button->setToggleState(state, dontSendNotification);
			}
		}
	}
}


void ChannelRefDisplay::update()
{
	drawTable();
}

void ChannelRefDisplay::reset()
{
	processor->getReferenceMatrix()->clear();
	update();
}

void ChannelRefDisplay::setEnableSingleSelectionMode(bool b)
{
	singleSelectMode = b;
}

void ChannelRefDisplay::paint(Graphics& g)
{
	g.fillAll(Colours::grey);
}

void ChannelRefDisplay::resized()
{
}

void ChannelRefDisplay::mouseDown(const MouseEvent& event)
{

}

void ChannelRefDisplay::buttonEvent(Button* button)
{
}

void ChannelRefDisplay::buttonClicked(Button* b)
{
	
	selectedRow = -1;
	selectedColumn = -1;

	if (b->getButtonText().startsWith("all"))
	{
		CarButton* button = dynamic_cast<CarButton*>(b);
		int channelIndex = button->getChannelNum();

		float* chan = processor->getReferenceMatrix()->getChannel(channelIndex);

		float value;
		button->getToggleState() ? value = 1 : value = 0;
			
		for (int i=0; i<processor->getReferenceMatrix()->getNumberOfChannels(); i++)
		{
			chan[i] = value;
		}

		update();
	}
	else
	{
		ElectrodeTableButton* button = dynamic_cast<ElectrodeTableButton*>(b);

		int rowIndex = button->getRowIndex();
		int colIndex = button->getColIndex();
		bool state = button->getToggleState();

		if (singleSelectMode)
		{
			int nChannels = processor->getReferenceMatrix()->getNumberOfChannels();
			for (int i=0; i<nChannels; i++)
			{
				processor->getReferenceMatrix()->setValue(rowIndex, i, 0);
			}
			processor->getReferenceMatrix()->setValue(rowIndex, colIndex, 1.);

			selectedRow = rowIndex;
			selectedColumn = colIndex;

			update();
		}
		else
		{
			processor->getReferenceMatrix()->setValue(rowIndex, colIndex, (float)state);
		}
	}
}

void ChannelRefDisplay::applyPreset(String name, int numChannels)
{
	ReferenceMatrix* refMat = processor->getReferenceMatrix();
	int nChannels = refMat->getNumberOfChannels();

	if (name.equalsIgnoreCase("Other tetrode electrodes"))
	{
		nChannels = MIN(nChannels, numChannels);
		refMat->clear();

		int nTetrodes = nChannels / 4;
		for (int i=0; i<nTetrodes; i++)
		{
			for (int j=0; j<4; j++)
			{
				int channelIndex = i*4 + j;
				for (int k=0; k<4; k++)
				{
					if (j != k)
					{
						refMat->setValue(channelIndex, i*4 + k, 1);
					}
				}
			}
		}

		drawTable();
	}
	else if (name.equalsIgnoreCase("All tetrode electrodes"))
	{
		nChannels = MIN(nChannels, numChannels);
		refMat->clear();

		int nTetrodes = nChannels / 4;
		for (int i=0; i<nTetrodes; i++)
		{
			for (int j=0; j<4; j++)
			{
				int channelIndex = i*4 + j;
				for (int k=0; k<4; k++)
				{
					refMat->setValue(channelIndex, i*4 + k, 1);
				}
			}
		}

		drawTable();
	}
	else if (name.equalsIgnoreCase("Common average reference"))
	{
		nChannels = MIN(nChannels, numChannels);
		refMat->clear();
		for (int i=0; i<nChannels; i++)
		{
			for (int j=0; j<nChannels; j++)
			{
				refMat->setValue(i, j, 1);
			}
		}

		drawTable();
	}
	else if (name.equalsIgnoreCase("Avg of other tetrodes"))
	{
		nChannels = MIN(nChannels, numChannels);
		refMat->clear();

		/* Activate all channels and deselect channels at the same tetrode */
		int nTetrodes = nChannels / 4;
		refMat->setAll(1, nChannels);
		for (int i=0; i<nTetrodes; i++)
		{
			for (int j=0; j<4; j++)
			{
				int channelIndex = i*4 + j;
				for (int k=0; k<4; k++)
				{
					refMat->setValue(channelIndex, i*4 + k, 0);
				}
			}
		}

		drawTable();
	}
	else if (name.equalsIgnoreCase("Avg of next tetrode"))
	{
		nChannels = MIN(nChannels, numChannels);
		refMat->clear();

		int nTetrodes = nChannels / 4;

		for (int i=0; i<nTetrodes; i++)
		{
			for (int j=0; j<4; j++)
			{
				for (int k=0; k<4; k++)
				{
					if (i < nTetrodes - 1)
					{
						refMat->setValue(i*4+j, (i+1)*4+k, 1);
					}
					else
					{
						refMat->setValue(i*4+j, (i-1)*4+k, 1);
					}
				}
			}
		}

		drawTable();
	}
}

/*
	using keyboard in single channel mode not implemented yet!
*/
bool ChannelRefDisplay::keyPressed(const KeyPress &key, Component *originatingComponent)
{
//	std::cout << "ChannelRefDisplay::keyPressed key code = " << key.getKeyCode() << " | description = " << key.getTextDescription() << " | singleSelectMode = " << singleSelectMode << std::endl;
	if (singleSelectMode)
	{
		if (selectedRow > -1 && selectedColumn > -1)
		{
			int nChannels = processor->getReferenceMatrix()->getNumberOfChannels();
			if (key.getTextDescription().compare("cursor left") == 0 && selectedColumn > 0)
			{
//				std::cout << ":::Move left:::" << std::endl;
			}
			else if (key.getTextDescription().compare("cursor right") == 0 && selectedColumn < nChannels - 1)
			{
//				std::cout << ":::Move right:::" << std::endl;
			}
		}
	}
}

