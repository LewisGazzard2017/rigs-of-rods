/*
    This source file is part of Rigs of Rods
    Copyright 2016+ Petr Ohlidal & contributors

    For more information, see http://www.rigsofrods.org/

    Rigs of Rods is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3, as
    published by the Free Software Foundation.

    Rigs of Rods is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

#include "GUI_ModCacheMonitor.h"

#include "RoRPrerequisites.h"

#include "Application.h"
#include "GUIManager.h"
#include "Language.h"
#include "RoRVersion.h"
#include "Utils.h"
#include "rornet.h"

#include <MyGUI.h>


namespace RoR {
namespace GUI {

#define CLASS        ModCacheMonitor
#define MAIN_WIDGET  ((MyGUI::Window*)m_ModCacheMonitor)

CLASS::CLASS()
{
    MAIN_WIDGET->setVisible(false);
    MAIN_WIDGET->setMovable(false);
}

void CLASS::PositionOnScreen()
{
    MyGUI::IntSize windowSize = MAIN_WIDGET->getSize();
    MyGUI::IntSize parentSize = MAIN_WIDGET->getParentSize();

    MAIN_WIDGET->setPosition((parentSize.width - windowSize.width - 20), 20);
}

bool CLASS::IsVisible()
{
    return MAIN_WIDGET->isVisible();
}

void CLASS::SetCaption(const char* str)
{
    MAIN_WIDGET->setCaption(str);
}

void CLASS::SetTextLine1(const char* label, const char* value)
{
    m_label_1->setCaption(label);
    m_info_1->setCaption(value);
}

void CLASS::SetTextLine2(const char* label, const char* value)
{
    m_label_2->setCaption(label);
    m_info_2->setCaption(value);
}

void CLASS::SetVisible(bool v)
{
    MAIN_WIDGET->setVisible(v);
}

} // namespace GUI
} // namespace RoR

