/*!
	@file
	@author		Albert Semenov
	@date		04/2008
	@module
*/
/*
	This file is part of MyGUI.

	MyGUI is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	MyGUI is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with MyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "pch.h"
#include "MyGUI_Precompiled.h"
#include "MultiList2.h"
#include "MyGUI_MultiListItem.h"
#include "MyGUI_ResourceSkin.h"
#include "MyGUI_Button.h"
#include "MyGUI_ImageBox.h"
#include "MyGUI_ListBox.h"
#include "MyGUI_Gui.h"
#include "MyGUI_WidgetManager.h"

namespace MyGUI
{

	MultiList2::MultiList2() :
		mHeightButton(0),
		mWidthBar(0),
		mWidgetEmpty(nullptr),
		mLastMouseFocusIndex(ITEM_NONE),
		mSortUp(true),
		mSortColumnIndex(ITEM_NONE),
		mWidthSeparator(0),
		mOffsetButtonSeparator(2),
		mItemSelected(ITEM_NONE),
		mFrameAdvise(false),
		mClient(nullptr),
		mHeaderPlace(nullptr),
		mSortUpOld(true),
		mSortColumnIndexOld(ITEM_NONE)
	{
	}
	
	/// custom method ---------------------------------
	void MultiList2::beginToItemAt(size_t id)
	{
		for (VectorColumnInfo::iterator iter=mVectorColumnInfo.begin(); iter!=mVectorColumnInfo.end(); ++iter)
		{
			//(*iter).list->setTextColour(Colour(1,0,1));
			(*iter).list->beginToItemAt(id);  //setScrollPosition(800);
		}
	}

	void MultiList2::initialiseOverride()
	{
		Base::initialiseOverride();

		std::string skinButtonEmpty;

		if (isUserString("SkinButton"))
			mSkinButton = getUserString("SkinButton");
		if (isUserString("SkinList"))
			mSkinList = getUserString("SkinList");
		if (isUserString("SkinSeparator"))
			mSkinSeparator = getUserString("SkinSeparator");
		if (isUserString("WidthSeparator"))
			mWidthSeparator = utility::parseValue<int>(getUserString("WidthSeparator"));

		// OBSOLETE
		if (isUserString("HeightButton"))
			mHeightButton = utility::parseValue<int>(getUserString("HeightButton"));
		if (mHeightButton < 0)
			mHeightButton = 0;

		assignWidget(mHeaderPlace, "HeaderPlace");

		assignWidget(mClient, "Client");
		if (mClient != nullptr)
			setWidgetClient(mClient);

		if (nullptr == mClient)
			mClient = this;

		assignWidget(mWidgetEmpty, "Empty");

		if (mWidgetEmpty == nullptr)
		{
			if (isUserString("SkinButtonEmpty"))
				skinButtonEmpty = getUserString("SkinButtonEmpty");
			if (!skinButtonEmpty.empty())
				mWidgetEmpty = mClient->createWidget<Widget>(skinButtonEmpty, IntCoord(0, 0, mClient->getWidth(), getButtonHeight()), Align::Default);
		}

		if (getUpdateByResize())
			updateColumns();
	}

	void MultiList2::shutdownOverride()
	{
		mClient = nullptr;

		Base::shutdownOverride();
	}

	void MultiList2::setColumnNameAt(size_t _column, const UString& _name)
	{
		MYGUI_ASSERT_RANGE(_column, mVectorColumnInfo.size(), "MultiList2::setColumnNameAt");
		mVectorColumnInfo[_column].name = _name;
		// обновляем кэпшен сначала
		redrawButtons();
		updateColumns();
	}

	void MultiList2::setColumnWidthAt(size_t _column, int _width)
	{
		MYGUI_ASSERT_RANGE(_column, mVectorColumnInfo.size(), "MultiList2::setColumnWidthAt");
		mVectorColumnInfo[_column].width = _width;
		updateColumns();
	}

	const UString& MultiList2::getColumnNameAt(size_t _column)
	{
		MYGUI_ASSERT_RANGE(_column, mVectorColumnInfo.size(), "MultiList2::getColumnNameAt");
		return mVectorColumnInfo[_column].name;
	}

	int MultiList2::getColumnWidthAt(size_t _column)
	{
		MYGUI_ASSERT_RANGE(_column, mVectorColumnInfo.size(), "MultiList2::getColumnWidthAt");
		return mVectorColumnInfo[_column].width;
	}

	void MultiList2::removeAllColumns()
	{
		while (!mVectorColumnInfo.empty())
			removeColumnAt(0);
	}

	void MultiList2::sortByColumn(size_t _column, bool _backward)
	{
		mSortColumnIndex = _column;
		if (_backward)
		{
			mSortUp = !mSortUp;
			redrawButtons();
			// если было недосортированно то сортируем
			if (mFrameAdvise)
				sortList();

			flipList();
		}
		else
		{
			mSortUp = true;
			redrawButtons();
			sortList();
		}
	}

	size_t MultiList2::getItemCount() const
	{
		if (mVectorColumnInfo.empty())
			return 0;
		return mVectorColumnInfo.front().list->getItemCount();
	}

	void MultiList2::removeAllItems()
	{
		BiIndexBase::removeAllItems();
		for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
		{
			(*iter).list->removeAllItems();
		}

		mItemSelected = ITEM_NONE;
	}

	void MultiList2::updateBackSelected(size_t _index)
	{
		if (_index == ITEM_NONE)
		{
			for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
			{
				(*iter).list->clearIndexSelected();
			}
		}
		else
		{
			for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
			{
				(*iter).list->setIndexSelected(_index);
			}
		}
	}

	void MultiList2::setIndexSelected(size_t _index)
	{
		if (_index == mItemSelected)
			return;

		MYGUI_ASSERT(!mVectorColumnInfo.empty(), "MultiList2::setIndexSelected");
		MYGUI_ASSERT_RANGE_AND_NONE(_index, mVectorColumnInfo.begin()->list->getItemCount(), "MultiList2::setIndexSelected");

		mItemSelected = _index;
		updateBackSelected(BiIndexBase::convertToBack(mItemSelected));
	}

	void MultiList2::setSubItemNameAt(size_t _column, size_t _index, const UString& _name)
	{
		MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.begin()->list->getItemCount(), "MultiList2::setSubItemAt");

		size_t index = BiIndexBase::convertToBack(_index);
		getSubItemAt(_column)->setItemNameAt(index, _name);

		// если мы попортили список с активным сортом, надо пересчитывать
		if (_column == mSortColumnIndex)
			frameAdvise(true);
	}

	const UString& MultiList2::getSubItemNameAt(size_t _column, size_t _index)
	{
		MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.begin()->list->getItemCount(), "MultiList2::getSubItemNameAt");

		size_t index = BiIndexBase::convertToBack(_index);
		return getSubItemAt(_column)->getItemNameAt(index);
	}

	size_t MultiList2::findSubItemWith(size_t _column, const UString& _name)
	{
		size_t index = getSubItemAt(_column)->findItemIndexWith(_name);
		return BiIndexBase::convertToFace(index);
	}

	int MultiList2::getButtonHeight() const
	{
		if (mHeaderPlace != nullptr)
			return mHeaderPlace->getHeight();
		return mHeightButton;
	}

	void MultiList2::updateOnlyEmpty()
	{
		if (nullptr == mWidgetEmpty)
			return;

		// кнопка, для заполнения пустоты
		if (mWidthBar >= mClient->getWidth())
			mWidgetEmpty->setVisible(false);
		else
		{
			mWidgetEmpty->setCoord(mWidthBar, 0, mClient->getWidth() - mWidthBar, getButtonHeight());
			mWidgetEmpty->setVisible(true);
		}
	}

	void MultiList2::notifyListChangePosition(ListBox* _sender, size_t _position)
	{
		for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
		{
			if (_sender != (*iter).list)
				(*iter).list->setIndexSelected(_position);
		}

		updateBackSelected(_position);

		mItemSelected = BiIndexBase::convertToFace(_position);

		// наш евент
		eventListChangePosition(this, mItemSelected);
	}

	void MultiList2::notifyListSelectAccept(ListBox* _sender, size_t _position)
	{
		// наш евент
		eventListSelectAccept(this, BiIndexBase::convertToFace(_position));
	}

	void MultiList2::notifyListChangeFocus(ListBox* _sender, size_t _position)
	{
		for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
		{
			if (_sender != (*iter).list)
			{
				if (ITEM_NONE != mLastMouseFocusIndex)
					(*iter).list->_setItemFocus(mLastMouseFocusIndex, false);
				if (ITEM_NONE != _position)
					(*iter).list->_setItemFocus(_position, true);
			}
		}
		mLastMouseFocusIndex = _position;
	}

	void MultiList2::notifyListChangeScrollPosition(ListBox* _sender, size_t _position)
	{
		for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
		{
			if (_sender != (*iter).list)
				(*iter).list->setScrollPosition(_position);
		}
	}

	void MultiList2::notifyButtonClick(MyGUI::Widget* _sender)
	{
		size_t index = *_sender->_getInternalData<size_t>();
		sortByColumn(index, index == mSortColumnIndex);
	}

	void MultiList2::redrawButtons()
	{
		size_t pos = 0;
		for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
		{
			if (pos == mSortColumnIndex)
			{
				if (mSortUp)
					(*iter).button->setImageName("Up");
				else
					(*iter).button->setImageName("Down");
			}
			else
				(*iter).button->setImageName("None");

			(*iter).button->setCaption((*iter).name);
			pos++;
		}
	}

	void MultiList2::frameEntered(float _frame)
	{
		sortList();
	}

	void MultiList2::frameAdvise(bool _advise)
	{
		if (_advise)
		{
			if (!mFrameAdvise)
			{
				MyGUI::Gui::getInstance().eventFrameStart += MyGUI::newDelegate( this, &MultiList2::frameEntered );
				mFrameAdvise = true;
			}
		}
		else
		{
			if (mFrameAdvise)
			{
				MyGUI::Gui::getInstance().eventFrameStart -= MyGUI::newDelegate( this, &MultiList2::frameEntered );
				mFrameAdvise = false;
			}
		}
	}

	Widget* MultiList2::getSeparator(size_t _index)
	{
		if (!mWidthSeparator || mSkinSeparator.empty())
			return nullptr;
		// последний столбик
		if (_index == mVectorColumnInfo.size() - 1)
			return nullptr;

		while (_index >= mSeparators.size())
		{
			Widget* separator = mClient->createWidget<Widget>(mSkinSeparator, IntCoord(), Align::Default);
			mSeparators.push_back(separator);
		}

		return mSeparators[_index];
	}

	void MultiList2::flipList()
	{
		return;
		if (ITEM_NONE == mSortColumnIndex)
			return;

		size_t last = mVectorColumnInfo.front().list->getItemCount();
		if (0 == last)
			return;
		last --;
		size_t first = 0;

		while (first < last)
		{
			BiIndexBase::swapItemsBackAt(first, last);
			for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
			{
				(*iter).list->swapItemsAt(first, last);
			}

			first++;
			last--;
		}

		updateBackSelected(BiIndexBase::convertToBack(mItemSelected));
	}

	bool MultiList2::compare(ListBox* _list, size_t _left, size_t _right)
	{
		bool result = false;
		if (mSortUp)
			std::swap(_left, _right);
		if (requestOperatorLess.empty())
			result = _list->getItemNameAt(_left) < _list->getItemNameAt(_right);
		else
			requestOperatorLess(this, mSortColumnIndex, _list->getItemNameAt(_left), _list->getItemNameAt(_right), result);
		return result;
	}

	void MultiList2::sortList()
	{
		return;
		if (ITEM_NONE == mSortColumnIndex)
			return;

		ListBox* list = mVectorColumnInfo[mSortColumnIndex].list;

		size_t count = list->getItemCount();
		if (0 == count)
			return;

		// shell sort
		int first;
		size_t last;
		for (size_t step = count >> 1; step > 0 ; step >>= 1)
		{
			for (size_t i = 0; i < (count - step); ++i)
			{
				first = (int)i;
				while (first >= 0)
				{
					last = first + step;
					if (compare(list, first, last))
					{
						BiIndexBase::swapItemsBackAt(first, last);
						for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
						{
							(*iter).list->swapItemsAt(first, last);
						}
					}
					first--;
				}
			}
		}

		frameAdvise(false);

		updateBackSelected(BiIndexBase::convertToBack(mItemSelected));
	}

	void MultiList2::insertItemAt(size_t _index, const UString& _name, Any _data)
	{
		MYGUI_ASSERT(!mVectorColumnInfo.empty(), "MultiList2::insertItemAt");
		MYGUI_ASSERT_RANGE_INSERT(_index, mVectorColumnInfo.front().list->getItemCount(), "MultiList2::insertItemAt");
		if (ITEM_NONE == _index)
			_index = mVectorColumnInfo.front().list->getItemCount();

		// если надо, то меняем выделенный элемент
		// при сортировке, обновится
		if ((mItemSelected != ITEM_NONE) && (_index <= mItemSelected))
			mItemSelected ++;

		size_t index = BiIndexBase::insertItemAt(_index);

		// вставляем во все поля пустые, а потом присваиваем первому
		for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
		{
			(*iter).list->insertItemAt(index, "");
		}
		mVectorColumnInfo.front().list->setItemNameAt(index, _name);
		mVectorColumnInfo.front().list->setItemDataAt(index, _data);

		frameAdvise(true);
	}

	void MultiList2::removeItemAt(size_t _index)
	{
		MYGUI_ASSERT(!mVectorColumnInfo.empty(), "MultiList2::removeItemAt");
		MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.begin()->list->getItemCount(), "MultiList2::removeItemAt");

		size_t index = BiIndexBase::removeItemAt(_index);

		for (VectorColumnInfo::iterator iter = mVectorColumnInfo.begin(); iter != mVectorColumnInfo.end(); ++iter)
		{
			(*iter).list->removeItemAt(index);
		}

		// если надо, то меняем выделенный элемент
		size_t count = mVectorColumnInfo.begin()->list->getItemCount();
		if (count == 0)
			mItemSelected = ITEM_NONE;
		else if (mItemSelected != ITEM_NONE)
		{
			if (_index < mItemSelected)
				mItemSelected --;
			else if ((_index == mItemSelected) && (mItemSelected == count))
				mItemSelected --;
		}
		updateBackSelected(BiIndexBase::convertToBack(mItemSelected));
	}

	void MultiList2::swapItemsAt(size_t _index1, size_t _index2)
	{
		MYGUI_ASSERT(!mVectorColumnInfo.empty(), "MultiList2::removeItemAt");
		MYGUI_ASSERT_RANGE(_index1, mVectorColumnInfo.begin()->list->getItemCount(), "MultiList2::swapItemsAt");
		MYGUI_ASSERT_RANGE(_index2, mVectorColumnInfo.begin()->list->getItemCount(), "MultiList2::swapItemsAt");

		// при сортированном, меняем только индексы
		BiIndexBase::swapItemsFaceAt(_index1, _index2);

		// при несортированном, нужно наоборот, поменять только данные
		// FIXME
	}

	void MultiList2::setColumnDataAt(size_t _index, Any _data)
	{
		MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.size(), "MultiList2::setColumnDataAt");
		mVectorColumnInfo[_index].data = _data;
	}

	void MultiList2::setSubItemDataAt(size_t _column, size_t _index, Any _data)
	{
		MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.begin()->list->getItemCount(), "MultiList2::setSubItemDataAt");

		size_t index = BiIndexBase::convertToBack(_index);
		getSubItemAt(_column)->setItemDataAt(index, _data);
	}

	size_t MultiList2::getColumnCount() const
	{
		return mVectorColumnInfo.size();
	}

	void MultiList2::addColumn(const UString& _name, int _width, Any _data)
	{
		insertColumnAt(ITEM_NONE, _name, _width, _data);
	}

	void MultiList2::clearColumnDataAt(size_t _index)
	{
		setColumnDataAt(_index, Any::Null);
	}

	void MultiList2::addItem(const UString& _name, Any _data)
	{
		insertItemAt(ITEM_NONE, _name, _data);
	}

	void MultiList2::setItemNameAt(size_t _index, const UString& _name)
	{
		setSubItemNameAt(0, _index, _name);
	}

	const UString& MultiList2::getItemNameAt(size_t _index)
	{
		return getSubItemNameAt(0, _index);
	}

	size_t MultiList2::getIndexSelected() const
	{
		return mItemSelected;
	}

	void MultiList2::clearIndexSelected()
	{
		setIndexSelected(ITEM_NONE);
	}

	void MultiList2::setItemDataAt(size_t _index, Any _data)
	{
		setSubItemDataAt(0, _index, _data);
	}

	void MultiList2::clearItemDataAt(size_t _index)
	{
		setItemDataAt(_index, Any::Null);
	}

	void MultiList2::clearSubItemDataAt(size_t _column, size_t _index)
	{
		setSubItemDataAt(_column, _index, Any::Null);
	}

	ListBox* MultiList2::getSubItemAt(size_t _column)
	{
		MYGUI_ASSERT_RANGE(_column, mVectorColumnInfo.size(), "MultiList2::getSubItemAt");
		return mVectorColumnInfo[_column].list;
	}

	size_t MultiList2::_getItemCount()
	{
		return getColumnCount();
	}

	void MultiList2::_addItem(const MyGUI::UString& _name)
	{
		addColumn(_name);
		setColumnResizingPolicyAt(getColumnCount() - 1, ResizingPolicy::Auto);
	}

	void MultiList2::_removeItemAt(size_t _index)
	{
		removeColumnAt(_index);
	}

	void MultiList2::_setItemNameAt(size_t _index, const UString& _name)
	{
		setColumnNameAt(_index, _name);
	}

	const UString& MultiList2::_getItemNameAt(size_t _index)
	{
		return getColumnNameAt(_index);
	}

	void MultiList2::insertColumnAt(size_t _column, const UString& _name, int _width, Any _data)
	{
		MYGUI_ASSERT_RANGE_INSERT(_column, mVectorColumnInfo.size(), "MultiList2::insertColumnAt");
		if (_column == ITEM_NONE)
			_column = mVectorColumnInfo.size();

		createWidget<MultiListItem>("", IntCoord(), Align::Default);

		mVectorColumnInfo.back().width = _width;
		mVectorColumnInfo.back().sizeType = ResizingPolicy::Fixed;
		mVectorColumnInfo.back().name = _name;
		mVectorColumnInfo.back().data = _data;
		mVectorColumnInfo.back().button->setCaption(_name);

		if (_column == (mVectorColumnInfo.size() - 1))
		{
			updateColumns();

			mVectorColumnInfo.back().list->setScrollVisible(true);
		}
		else
		{
			_swapColumnsAt(_column, mVectorColumnInfo.size() - 1);
		}
	}

	void MultiList2::removeColumnAt(size_t _column)
	{
		MYGUI_ASSERT_RANGE(_column, mVectorColumnInfo.size(), "MultiList2::removeColumnAt");

		ColumnInfo& info = mVectorColumnInfo[_column];

		WidgetManager::getInstance().destroyWidget(info.item);
	}

	void MultiList2::swapColumnsAt(size_t _index1, size_t _index2)
	{
		MYGUI_ASSERT_RANGE(_index1, mVectorColumnInfo.size(), "MultiList2::swapColumnsAt");
		MYGUI_ASSERT_RANGE(_index2, mVectorColumnInfo.size(), "MultiList2::swapColumnsAt");

		_swapColumnsAt(_index1, _index2);
	}

	void MultiList2::_swapColumnsAt(size_t _index1, size_t _index2)
	{
		if (_index1 == _index2)
			return;

		mVectorColumnInfo[_index1].list->setScrollVisible(false);
		mVectorColumnInfo[_index2].list->setScrollVisible(false);

		std::swap(mVectorColumnInfo[_index1], mVectorColumnInfo[_index2]);

		updateColumns();

		mVectorColumnInfo.back().list->setScrollVisible(true);
	}

	void MultiList2::onWidgetCreated(Widget* _widget)
	{
		Base::onWidgetCreated(_widget);

		MultiListItem* child = _widget->castType<MultiListItem>(false);
		if (child != nullptr)
		{
			_wrapItem(child);
		}
	}

	void MultiList2::onWidgetDestroy(Widget* _widget)
	{
		Base::onWidgetDestroy(_widget);

		MultiListItem* child = _widget->castType<MultiListItem>(false);
		if (child != nullptr)
		{
			_unwrapItem(child);
		}
		else
		{
			for (VectorColumnInfo::iterator item = mVectorColumnInfo.begin(); item != mVectorColumnInfo.end(); ++item)
			{
				if ((*item).button == _widget)
					(*item).button = nullptr;
			}
		}
	}

	void MultiList2::_wrapItem(MultiListItem* _item)
	{
		// скрываем у крайнего скролл
		if (!mVectorColumnInfo.empty())
			mVectorColumnInfo.back().list->setScrollVisible(false);
		else
			mSortColumnIndex = ITEM_NONE;

		ColumnInfo column;
		column.width = 0;
		column.sizeType = ResizingPolicy::Auto;

		column.item = _item;
		column.list = _item->createWidget<ListBox>(mSkinList, IntCoord(0, 0, _item->getWidth(), _item->getHeight()), Align::Stretch);
		column.list->eventListChangePosition += newDelegate(this, &MultiList2::notifyListChangePosition);
		column.list->eventListMouseItemFocus += newDelegate(this, &MultiList2::notifyListChangeFocus);
		column.list->eventListChangeScroll += newDelegate(this, &MultiList2::notifyListChangeScrollPosition);
		column.list->eventListSelectAccept += newDelegate(this, &MultiList2::notifyListSelectAccept);

		if (mHeaderPlace != nullptr)
			column.button = mHeaderPlace->createWidget<Button>(mSkinButton, IntCoord(), Align::Default);
		else
			column.button = mClient->createWidget<Button>(mSkinButton, IntCoord(), Align::Default);

		column.button->eventMouseButtonClick += newDelegate(this, &MultiList2::notifyButtonClick);

		// если уже были столбики, то делаем то же колличество полей
		if (!mVectorColumnInfo.empty())
		{
			size_t count = mVectorColumnInfo.front().list->getItemCount();
			for (size_t pos = 0; pos < count; ++pos)
				column.list->addItem("");
		}

		mVectorColumnInfo.push_back(column);

		updateColumns();

		// показываем скролл нового крайнего
		mVectorColumnInfo.back().list->setScrollVisible(true);
	}

	void MultiList2::_unwrapItem(MultiListItem* _item)
	{
		for (VectorColumnInfo::iterator item = mVectorColumnInfo.begin(); item != mVectorColumnInfo.end(); ++item)
		{
			if ((*item).item == _item)
			{
				if ((*item).button != nullptr)
					WidgetManager::getInstance().destroyWidget((*item).button);

				mVectorColumnInfo.erase(item);
				break;
			}
		}

		if (mVectorColumnInfo.empty())
		{
			mSortColumnIndex = ITEM_NONE;
			mItemSelected = ITEM_NONE;
		}
		else
		{
			mSortColumnIndex = ITEM_NONE;
			mSortUp = true;
			sortList();
		}

		updateColumns();

		if (!mVectorColumnInfo.empty())
			mVectorColumnInfo.back().list->setScrollVisible(true);
	}

	Widget* MultiList2::_getItemAt(size_t _index)
	{
		MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.size(), "MultiList2::_getItemAt");
		return mVectorColumnInfo[_index].item;
	}

	void MultiList2::setColumnName(MultiListItem* _item, const UString& _name)
	{
		setColumnNameAt(getColumnIndex(_item), _name);
	}

	const UString& MultiList2::getColumnName(MultiListItem* _item)
	{
		return getColumnNameAt(getColumnIndex(_item));
	}

	size_t MultiList2::getColumnIndex(MultiListItem* _item)
	{
		for (size_t index = 0; index < mVectorColumnInfo.size(); ++ index)
		{
			if (mVectorColumnInfo[index].item == _item)
				return index;
		}

		return ITEM_NONE;
	}

	void MultiList2::setColumnResizingPolicy(MultiListItem* _item, ResizingPolicy _value)
	{
		setColumnResizingPolicyAt(getColumnIndex(_item), _value);
	}

	void MultiList2::setColumnResizingPolicyAt(size_t _index, ResizingPolicy _value)
	{
		MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.size(), "MultiList2::setColumnWidthAt");
		mVectorColumnInfo[_index].sizeType = _value;
		updateColumns();
	}

	void MultiList2::setColumnWidth(MultiListItem* _item, int _width)
	{
		setColumnWidthAt(getColumnIndex(_item), _width);
	}

	void MultiList2::setPosition(const IntPoint& _point)
	{
		Base::setPosition(_point);
	}

	void MultiList2::setSize(const IntSize& _size)
	{
		Base::setSize(_size);

		if (getUpdateByResize())
			updateColumns();
	}

	void MultiList2::setCoord(const IntCoord& _coord)
	{
		Base::setCoord(_coord);

		if (getUpdateByResize())
			updateColumns();
	}

	void MultiList2::setPosition(int _left, int _top)
	{
		setPosition(IntPoint(_left, _top));
	}

	void MultiList2::setSize(int _width, int _height)
	{
		setSize(IntSize(_width, _height));
	}

	void MultiList2::setCoord(int _left, int _top, int _width, int _height)
	{
		setCoord(IntCoord(_left, _top, _width, _height));
	}

	bool MultiList2::getUpdateByResize()
	{
		if (mWidgetEmpty != nullptr)
			return true;

		for (VectorColumnInfo::iterator item = mVectorColumnInfo.begin(); item != mVectorColumnInfo.end(); ++item)
		{
			if ((*item).sizeType == ResizingPolicy::Fill)
				return true;
		}
		return false;
	}

	int MultiList2::getColumnWidth(size_t _index, int _freeSpace, size_t _countStars, size_t _lastIndexStar, int _starWidth) const
	{
		const ColumnInfo& info = mVectorColumnInfo[_index];

		if (info.sizeType == ResizingPolicy::Auto)
		{
			return info.realWidth;
		}
		else if (info.sizeType == ResizingPolicy::Fixed)
		{
			return info.realWidth;
		}
		else if (info.sizeType == ResizingPolicy::Fill)
		{
			if (_lastIndexStar == _index)
			{
				return _starWidth + _freeSpace - (_starWidth * _countStars);
			}
			else
			{
				return _starWidth;
			}
		}
		return 0;
	}

	int MultiList2::updateWidthColumns(size_t& _countStars, size_t& _lastIndexStar)
	{
		_countStars = 0;
		_lastIndexStar = ITEM_NONE;

		int width = 0;

		for (size_t index = 0; index < mVectorColumnInfo.size(); ++ index)
		{
			ColumnInfo& info = mVectorColumnInfo[index];

			if (info.sizeType == ResizingPolicy::Auto)
			{
				info.realWidth = info.button->getWidth() - info.button->getTextRegion().width + info.button->getTextSize().width;
			}
			else if (info.sizeType == ResizingPolicy::Fixed)
			{
				info.realWidth =  info.width < 0 ? 0 : info.width;
			}
			else if (info.sizeType == ResizingPolicy::Fill)
			{
				info.realWidth = 0;
				_countStars ++;
				_lastIndexStar = index;
			}
			else
			{
				info.realWidth = 0;
			}

			width += info.realWidth;
		}

		return width;
	}

	void MultiList2::updateColumns()
	{
		size_t countStars = 0;
		size_t lastIndexStar = ITEM_NONE;

		int allColumnsWidth = updateWidthColumns(countStars, lastIndexStar);
		int clientWidth = mClient->getWidth();
		int separatorsWidth = mVectorColumnInfo.empty() ? 0 : (mVectorColumnInfo.size() - 1) * mWidthSeparator;
		int freeSpace = clientWidth - separatorsWidth - allColumnsWidth;
		int starWidth = (countStars != 0 && freeSpace > 0) ? (freeSpace / countStars) : 0;

		mWidthBar = 0;
		for (size_t index = 0; index < mVectorColumnInfo.size(); ++ index)
		{
			ColumnInfo& info = mVectorColumnInfo[index];

			int columnWidth = getColumnWidth(index, freeSpace, countStars, lastIndexStar, starWidth);

			if (mHeaderPlace != nullptr)
			{
				info.item->setCoord(mWidthBar, 0, columnWidth, mClient->getHeight());
			}
			else
			{
				info.item->setCoord(mWidthBar, mHeightButton, columnWidth, mClient->getHeight() - mHeightButton);
			}

			info.button->setCoord(mWidthBar, 0, columnWidth, getButtonHeight());
			info.button->_setInternalData(index);

			mWidthBar += columnWidth;

			// промежуток между листами
			Widget* separator = getSeparator(index);
			if (separator)
			{
				separator->setCoord(mWidthBar, 0, mWidthSeparator, mClient->getHeight());
			}

			mWidthBar += mWidthSeparator;
		}

		redrawButtons();
		updateOnlyEmpty();
	}

} // namespace MyGUI
