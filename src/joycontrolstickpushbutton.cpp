/* antimicrox Gamepad to KB+M event mapper
 * Copyright (C) 2015 Travis Nickles <nickles.travis@gmail.com>
 * Copyright (C) 2020 Jagoda Górska <juliagoda.pl@protonmail>
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

#include "joycontrolstickpushbutton.h"

#include "joycontrolstick.h"
#include "joycontrolstickcontextmenu.h"

#include <QDebug>

JoyControlStickPushButton::JoyControlStickPushButton(JoyControlStick *stick, bool displayNames, QWidget *parent)
    : FlashButtonWidget(displayNames, parent)
{
    this->stick = stick;

    refreshLabel();

    tryFlash();

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &JoyControlStickPushButton::customContextMenuRequested, this, &JoyControlStickPushButton::showContextMenu);

    connect(stick, &JoyControlStick::active, this, &JoyControlStickPushButton::flash, Qt::QueuedConnection);
    connect(stick, &JoyControlStick::released, this, &JoyControlStickPushButton::unflash, Qt::QueuedConnection);
    connect(stick, &JoyControlStick::stickNameChanged, this, &JoyControlStickPushButton::refreshLabel);
}

JoyControlStick *JoyControlStickPushButton::getStick() const { return stick; }

/**
 * @brief Generate the string that will be displayed on the button
 * @return Display string
 */
QString JoyControlStickPushButton::generateLabel()
{
    QString temp = QString();
    if (!stick->getStickName().isEmpty() && ifDisplayNames())
    {
        temp.append(stick->getPartialName(false, true));
    } else
    {
        temp.append(stick->getPartialName(false));
    }

    qDebug() << "Name of joy control stick push button: " << temp;

    return temp;
}

void JoyControlStickPushButton::disableFlashes()
{
    disconnect(stick, &JoyControlStick::active, this, &JoyControlStickPushButton::flash);
    disconnect(stick, &JoyControlStick::released, this, &JoyControlStickPushButton::unflash);
    this->unflash();
}

void JoyControlStickPushButton::enableFlashes()
{
    connect(stick, &JoyControlStick::active, this, &JoyControlStickPushButton::flash, Qt::QueuedConnection);
    connect(stick, &JoyControlStick::released, this, &JoyControlStickPushButton::unflash, Qt::QueuedConnection);
}

void JoyControlStickPushButton::showContextMenu(const QPoint &point)
{
    QPoint globalPos = this->mapToGlobal(point);
    JoyControlStickContextMenu *contextMenu = new JoyControlStickContextMenu(stick, this);
    contextMenu->buildMenu();
    contextMenu->popup(globalPos);
}

void JoyControlStickPushButton::tryFlash()
{
    if (stick->getCurrentDirection() != JoyControlStick::StickCentered)
    {
        flash();
    }
}
