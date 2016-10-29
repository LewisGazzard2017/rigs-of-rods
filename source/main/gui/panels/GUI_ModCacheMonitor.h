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

#pragma once

#include "ForwardDeclarations.h"
#include "GUI_ModCacheMonitorLayout.h"

namespace RoR {
namespace GUI {

class ModCacheMonitor: public ModCacheMonitorLayout
{
public:
    ModCacheMonitor();

    void SetVisible(bool v);
    bool IsVisible();
    void SetTextLine1(const char* label, const char* value);
    void SetTextLine2(const char* label, const char* value);
    void SetCaption(const char* str);
    void PositionOnScreen();

};

} // namespace GUI
} // namespace RoR
