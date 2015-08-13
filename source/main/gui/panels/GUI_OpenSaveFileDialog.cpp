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
	@date		08/2008
*/

#include "GUI_OpenSaveFileDialog.h"

#include "AngelScriptSetupHelper.h"
#include "FileSystemInfo.h"

using namespace RoR;
using namespace GUI;

OpenSaveFileDialog::OpenSaveFileDialog() :
	Dialog("OpenSaveFileDialog.layout"),
	mWindow(nullptr),
	mListFiles(nullptr),
	mEditFileName(nullptr),
	mButtonUp(nullptr),
	mCurrentFolderField(nullptr),
	mButtonOpenSave(nullptr),
	mFileMask("*.*"),
	mAngelScriptRefCount(0),
	mFolderMode(false)
{
	assignWidget(mListFiles, "ListFiles");
	assignWidget(mEditFileName, "EditFileName");
	assignWidget(mButtonUp, "ButtonUp");
	mButtonUp->setCaption("[^] Up");
	assignWidget(mCurrentFolderField, "CurrentFolder");
	assignWidget(mButtonOpenSave, "ButtonOpenSave");

	mListFiles->eventListChangePosition += MyGUI::newDelegate(this, &OpenSaveFileDialog::notifyListChangePosition);
	mListFiles->eventListSelectAccept += MyGUI::newDelegate(this, &OpenSaveFileDialog::notifyListSelectAccept);
	mEditFileName->eventEditSelectAccept += MyGUI::newDelegate(this, &OpenSaveFileDialog::notifyEditSelectAccept);
	mButtonUp->eventMouseButtonClick += MyGUI::newDelegate(this, &OpenSaveFileDialog::notifyUpButtonClick);
	mCurrentFolderField->eventComboAccept += MyGUI::newDelegate(this, &OpenSaveFileDialog::notifyDirectoryComboAccept);
	mCurrentFolderField->eventComboChangePosition += MyGUI::newDelegate(this, &OpenSaveFileDialog::notifyDirectoryComboChangePosition);
	mButtonOpenSave->eventMouseButtonClick += MyGUI::newDelegate(this, &OpenSaveFileDialog::notifyMouseButtonClick);

	mWindow = mMainWidget->castType<MyGUI::Window>();
	mWindow->eventWindowButtonPressed += MyGUI::newDelegate(this, &OpenSaveFileDialog::notifyWindowButtonPressed);

	mCurrentFolder = FileSystem::getSystemCurrentFolder();

	mMainWidget->setVisible(false);

	// Selection finished callback: Bind to itself, will be forwarded to AngelScript
	this->eventEndDialog = MyGUI::newDelegate(this, &OpenSaveFileDialog::NotifyFileSelectorEnded);

	update();
}

void OpenSaveFileDialog::NotifyFileSelectorEnded(GUI::Dialog* dialog, bool result)
{
	if (!mAsDialogFinishedCallback.IsBound())
	{
		return;
	}
	// TODO: Full unicode solution
	std::string folder   = this->getCurrentFolder();
	std::string filename = this->getFileName();

	auto* ctx = mAsDialogFinishedCallback.PrepareContext();
	mAsDialogFinishedCallback.SetArgBool    (ctx, 0, result);
	mAsDialogFinishedCallback.SetArgObject  (ctx, 1, static_cast<void*>(&folder));
	mAsDialogFinishedCallback.SetArgObject  (ctx, 2, static_cast<void*>(&filename));

	mAsDialogFinishedCallback.ExecuteContext(ctx);
}


void OpenSaveFileDialog::notifyWindowButtonPressed(MyGUI::Window* _sender, const std::string& _name)
{
	if (_name == "close")
		eventEndDialog(this, false);
}

void OpenSaveFileDialog::notifyEditSelectAccept(MyGUI::EditBox* _sender)
{
	accept();
}

void OpenSaveFileDialog::notifyMouseButtonClick(MyGUI::Widget* _sender)
{
	accept();
}

void OpenSaveFileDialog::notifyUpButtonClick(MyGUI::Widget* _sender)
{
	upFolder();
}

void OpenSaveFileDialog::AS_ConfigureDialog(std::string title, std::string select_btn_text, bool use_folder_mode)
{
	this->setDialogInfo(title, select_btn_text, use_folder_mode);
}

void OpenSaveFileDialog::setDialogInfo(MyGUI::UString _caption, MyGUI::UString _button, bool _folderMode)
{
	mFolderMode = _folderMode;
	mWindow->setCaption(_caption);
	mButtonOpenSave->setCaption(_button);
	mEditFileName->setVisible(!_folderMode);
}

void OpenSaveFileDialog::notifyListChangePosition(MyGUI::ListBox* _sender, size_t _index)
{
	if (_index == MyGUI::ITEM_NONE)
	{
		mEditFileName->setCaption("");
	}
	else
	{
		FileSystem::FileInfo info = *_sender->getItemDataAt<FileSystem::FileInfo>(_index);
		if (!info.folder)
			mEditFileName->setCaption(info.name);
	}
}

void OpenSaveFileDialog::notifyListSelectAccept(MyGUI::ListBox* _sender, size_t _index)
{
	if (_index == MyGUI::ITEM_NONE) return;

	FileSystem::FileInfo info = *_sender->getItemDataAt<FileSystem::FileInfo>(_index);
	if (info.folder)
	{
		if (info.name == L"..")
		{
			upFolder();
		}
		else
		{
			mCurrentFolder = FileSystem::concatenatePath (mCurrentFolder.asWStr(), info.name);
			update();
		}
	}
	else
	{
		accept();
	}
}

void OpenSaveFileDialog::accept()
{
	if (!mFolderMode)
	{
		mFileName = mEditFileName->getOnlyText();
		if (!mFileName.empty())
			eventEndDialog(this, true);
	}
	else
	{
		if (mListFiles->getIndexSelected() != MyGUI::ITEM_NONE)
		{
			FileSystem::FileInfo info = *mListFiles->getItemDataAt<FileSystem::FileInfo>(mListFiles->getIndexSelected());
			if (!FileSystem::isParentDir(info.name.c_str()))
				mCurrentFolder = FileSystem::concatenatePath (mCurrentFolder.asWStr(), info.name);
		}
		eventEndDialog(this, true);
	}
}

void OpenSaveFileDialog::upFolder()
{
	size_t index = mCurrentFolder.find_last_of(L"\\/");
	if (index != std::string::npos)
	{
		mCurrentFolder = mCurrentFolder.substr(0, index);
	}
	update();
}

void OpenSaveFileDialog::setCurrentFolder(const MyGUI::UString& _folder)
{
	mCurrentFolder = _folder.empty() ? MyGUI::UString(FileSystem::getSystemCurrentFolder()) : _folder;

	update();
}

void OpenSaveFileDialog::update()
{
	if (mCurrentFolder.empty())
		mCurrentFolder = "/";
	mCurrentFolderField->setCaption(mCurrentFolder);

	mListFiles->removeAllItems();

	// add all folders first
	FileSystem::VectorFileInfo infos;
	getSystemFileList(infos, mCurrentFolder, L"*.*");

	for (FileSystem::VectorFileInfo::iterator item = infos.begin(); item != infos.end(); ++item)
	{
		if ((*item).folder)
			mListFiles->addItem(L"[" + (*item).name + L"]", *item);
	}

	if (!mFolderMode)
	{
		// add files by given mask
		infos.clear();
		getSystemFileList(infos, mCurrentFolder, mFileMask);

		for (FileSystem::VectorFileInfo::iterator item = infos.begin(); item != infos.end(); ++item)
		{
			if (!(*item).folder)
				mListFiles->addItem((*item).name, *item);
		}
	}
}

void OpenSaveFileDialog::setFileName(const MyGUI::UString& _value)
{
	mFileName = _value;
	mEditFileName->setCaption(_value);
}

void OpenSaveFileDialog::setFileMask(const MyGUI::UString& _value)
{
	mFileMask = _value;
	update();
}

const MyGUI::UString& OpenSaveFileDialog::getFileMask() const
{
	return mFileMask;
}

void OpenSaveFileDialog::onDoModal()
{
	update();

	MyGUI::IntSize windowSize = mMainWidget->getSize();
	MyGUI::IntSize parentSize = mMainWidget->getParentSize();

	mMainWidget->setPosition((parentSize.width - windowSize.width) / 2, (parentSize.height - windowSize.height) / 2);
}

void OpenSaveFileDialog::onEndModal()
{
}

void OpenSaveFileDialog::notifyDirectoryComboAccept(MyGUI::ComboBox* _sender, size_t _index)
{
	setCurrentFolder(_sender->getOnlyText());
}

const MyGUI::UString& OpenSaveFileDialog::getCurrentFolder() const
{
	return mCurrentFolder;
}

const MyGUI::UString& OpenSaveFileDialog::getFileName() const
{
	return mFileName;
}

void OpenSaveFileDialog::setRecentFolders(const VectorUString& _listFolders)
{
	mCurrentFolderField->removeAllItems();

	for (VectorUString::const_iterator item = _listFolders.begin(); item != _listFolders.end(); ++ item)
		mCurrentFolderField->addItem((*item));
}

void OpenSaveFileDialog::notifyDirectoryComboChangePosition(MyGUI::ComboBox* _sender, size_t _index)
{
	if (_index != MyGUI::ITEM_NONE)
		setCurrentFolder(_sender->getItemNameAt(_index));
}

void OpenSaveFileDialog::BindToAngelScript(RoR::AngelScriptSetupHelper* A)
{
	auto proxy = A->RegisterObjectWithProxy("GUI_OpenSaveFileDialog", 0, asOBJ_REF);
	proxy.AddBehavior(asBEHAVE_ADDREF,  "void f()", asMETHOD(OpenSaveFileDialog, AS_RefCountIncrease));
	proxy.AddBehavior(asBEHAVE_RELEASE, "void f()", asMETHOD(OpenSaveFileDialog, AS_RefCountDecrease));
	proxy.AddBehavior(asBEHAVE_FACTORY, "GUI_OpenSaveFileDialog@ f()", asFUNCTION(OpenSaveFileDialog::AS_CreateInstance), asCALL_CDECL);

	proxy.AddMethod("void StartModal()", asMETHOD(OpenSaveFileDialog, doModal));
	proxy.AddMethod("void EndModal()",   asMETHOD(OpenSaveFileDialog, endModal));

	proxy.AddMethod("void ConfigureDialog(string title, string select_button_text, bool use_folder_mode)",
		asMETHOD(OpenSaveFileDialog, AS_ConfigureDialog));

	A->RegisterInterface("GUI_IOpenSaveFileDialogListener");
	A->RegisterFuncdef("void FOpenSaveFileDialogFinishedCallback(bool success, string folder, string filename)");

	proxy.AddMethod("void RegisterDialogFinishedCallback(GUI_IOpenSaveFileDialogListener@ dialog, string method_name)",
		asMETHOD(OpenSaveFileDialog, AS_RegisterDialogFinishedCallback));
}

void OpenSaveFileDialog::AS_RegisterDialogFinishedCallback(AngelScript::asIScriptObject* object, std::string method_name)
{
	mAsDialogFinishedCallback.RegisterCallback(object, method_name);
}
