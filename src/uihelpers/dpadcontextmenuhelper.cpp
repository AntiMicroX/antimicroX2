/* antimicrox Gamepad to KB+M event mapper
 * Copyright (C) 2015 Travis Nickles <nickles.travis@gmail.com>
 * Copyright (C) 2020 Jagoda Górska <juliagoda.pl@protonmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dpadcontextmenuhelper.h"

#include "joybuttonslot.h"

#include <QDebug>
#include <QHashIterator>
#include <QListIterator>

DPadContextMenuHelper::DPadContextMenuHelper(JoyDPad *dpad, QObject *parent)
    : QObject(parent)
{
    Q_ASSERT(dpad);

    this->dpad = dpad;
}

void DPadContextMenuHelper::setPendingSlots(QHash<JoyDPadButton::JoyDPadDirections, JoyButtonSlot *> *tempSlots)
{
    pendingSlots.clear();

    QHashIterator<JoyDPadButton::JoyDPadDirections, JoyButtonSlot *> iter(*tempSlots);
    while (iter.hasNext())
    {
        iter.next();

        JoyButtonSlot *slot = iter.value();
        JoyDPadButton::JoyDPadDirections tempDir = iter.key();
        pendingSlots.insert(tempDir, slot);
    }
}

void DPadContextMenuHelper::clearPendingSlots() { pendingSlots.clear(); }

void DPadContextMenuHelper::setFromPendingSlots()
{
    if (!getPendingSlots().isEmpty())
    {
        QHashIterator<JoyDPadButton::JoyDPadDirections, JoyButtonSlot *> iter(getPendingSlots());
        while (iter.hasNext())
        {
            iter.next();

            JoyButtonSlot *slot = iter.value();
            if (slot)
            {
                JoyDPadButton::JoyDPadDirections tempDir = iter.key();
                JoyDPadButton *button = dpad->getJoyButton(tempDir);
                button->clearSlotsEventReset(false);
                button->setAssignedSlot(slot->getSlotCode(), slot->getSlotCodeAlias(), slot->getSlotMode());
                slot->deleteLater();
            }
        }
    }
}

void DPadContextMenuHelper::clearButtonsSlotsEventReset()
{
    QHash<int, JoyDPadButton *> *buttons = dpad->getButtons();
    QHashIterator<int, JoyDPadButton *> iter(*buttons);
    while (iter.hasNext())
    {
        JoyDPadButton *button = iter.next().value();
        button->clearSlotsEventReset();
    }
}

QHash<JoyDPadButton::JoyDPadDirections, JoyButtonSlot *> const &DPadContextMenuHelper::getPendingSlots()
{
    return pendingSlots;
}
