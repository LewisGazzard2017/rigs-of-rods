/*
	This source file is part of Rigs of Rods
	Copyright 2005-2012 Pierre-Michel Ricordel
	Copyright 2007-2012 Thomas Fischer
	Copyright 2013-2014 Petr Ohlidal

	For more information, see http://www.rigsofrods.com/

	Rigs of Rods is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 3, as
	published by the Free Software Foundation.

	Rigs of Rods is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

/*
	This file was ported from MyGUI project (MIT licensed)
	https://github.com/MyGUI/mygui
	http://mygui.info/
*/

/*!
	@file
	@author		Albert Semenov
	@date		09/2008
*/

#pragma once

#include <MyGUI.h>
#include "Dialog.h"

//#include "AngelScriptCallbackSocket.h"

#include <string>

namespace RoR
{

// Forward decl...
//class AngelScriptSetupHelper;

namespace GUI
{
	
class OpenSaveFileDialog :
	public Dialog
{
public:
	OpenSaveFileDialog();

	void setDialogInfo(MyGUI::UString _caption, MyGUI::UString _button, bool _folderMode);

	void setCurrentFolder(const MyGUI::UString& _value);
	const MyGUI::UString& getCurrentFolder() const;

	void setFileName(const MyGUI::UString& _value);
	const MyGUI::UString& getFileName() const;

	typedef std::vector<MyGUI::UString> VectorUString;
	void setRecentFolders(const VectorUString& _listFolders);

	void setFileMask(const MyGUI::UString& _value);
	const MyGUI::UString& getFileMask() const;

	// AngelScript interface

	static OpenSaveFileDialog* AS_CreateInstance()
	{
		return new OpenSaveFileDialog();
	}

	void AS_RefCountIncrease() { mAngelScriptRefCount++; }
	void AS_RefCountDecrease() { mAngelScriptRefCount--; }

	//void AS_RegisterDialogFinishedCallback(AngelScript::asIScriptObject* object, std::string method_name);

	void AS_ConfigureDialog(std::string title, std::string select_btn_text, bool use_folder_mode);

	// END AngelScript interface

	//static void BindToAngelScript(RoR::AngelScriptSetupHelper* A);

protected:
	virtual void onDoModal();
	virtual void onEndModal();

	// GUI callbacks
	void NotifyFileSelectorEnded(RoR::GUI::Dialog* dialog, bool result);

private:
	void notifyWindowButtonPressed(MyGUI::Window* _sender, const std::string& _name);
	void notifyDirectoryComboAccept(MyGUI::ComboBox* _sender, size_t _index);
	void notifyDirectoryComboChangePosition(MyGUI::ComboBox* _sender, size_t _index);
	void notifyListChangePosition(MyGUI::ListBox* _sender, size_t _index);
	void notifyListSelectAccept(MyGUI::ListBox* _sender, size_t _index);
	void notifyEditSelectAccept(MyGUI::EditBox* _sender);
	void notifyMouseButtonClick(MyGUI::Widget* _sender);
	void notifyUpButtonClick(MyGUI::Widget* _sender);

	void update();
	void accept();

	void upFolder();

private:
	MyGUI::Window* mWindow;
	MyGUI::ListBox* mListFiles;
	MyGUI::EditBox* mEditFileName;
	MyGUI::Button* mButtonUp;
	MyGUI::ComboBox* mCurrentFolderField;
	MyGUI::Button* mButtonOpenSave;

	MyGUI::UString mCurrentFolder;
	MyGUI::UString mFileName;
	MyGUI::UString mFileMask;

	bool mFolderMode;
	int  mAngelScriptRefCount;

	//RoR::AngelScriptCallbackSocket mAsDialogFinishedCallback;
};

} // namespace GUI

} // namespace RoR
