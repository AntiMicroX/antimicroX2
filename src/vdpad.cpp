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

#include "vdpad.h"

#include "globalvariables.h"

#include <QDebug>

VDPad::VDPad(int index, int originset, SetJoystick *parentSet, QObject *parent)
    : JoyDPad(index, originset, parentSet, parent)
{
    this->upButton = nullptr;
    this->downButton = nullptr;
    this->leftButton = nullptr;
    this->rightButton = nullptr;

    pendingVDPadEvent = false;
}

VDPad::VDPad(JoyButton *upButton, JoyButton *downButton, JoyButton *leftButton, JoyButton *rightButton, int index,
             int originset, SetJoystick *parentSet, QObject *parent)
    : JoyDPad(index, originset, parentSet, parent)
{
    this->upButton = upButton;
    upButton->setVDPad(this);

    this->downButton = downButton;
    downButton->setVDPad(this);

    this->leftButton = leftButton;
    leftButton->setVDPad(this);

    this->rightButton = rightButton;
    rightButton->setVDPad(this);

    pendingVDPadEvent = false;
}

VDPad::~VDPad()
{
    if (upButton != nullptr)
    {
        upButton->removeVDPad();
        upButton = nullptr;
    }

    if (downButton != nullptr)
    {
        downButton->removeVDPad();
        downButton = nullptr;
    }

    if (leftButton != nullptr)
    {
        leftButton->removeVDPad();
        leftButton = nullptr;
    }

    if (rightButton != nullptr)
    {
        rightButton->removeVDPad();
        rightButton = nullptr;
    }
}

QString VDPad::getXmlName() { return GlobalVariables::VDPad::xmlName; }

QString VDPad::getName(bool forceFullFormat, bool displayName)
{
    QString label = QString();

    if (!getDpadName().isEmpty() && displayName)
    {
        if (forceFullFormat)
            label.append(tr("VDPad")).append(" ");

        label.append(getDpadName());
    } else if (!getDefaultDpadName().isEmpty())
    {
        if (forceFullFormat)
            label.append(tr("VDPad")).append(" ");

        label.append(getDefaultDpadName());
    } else
    {
        label.append(tr("VDPad")).append(" ");
        label.append(QString::number(getRealJoyNumber()));
    }

    return label;
}

void VDPad::joyEvent(bool pressed, bool ignoresets)
{
    Q_UNUSED(pressed)

    int tempDirection = static_cast<int>(JoyDPadButton::DpadCentered);

    // Check which buttons are currently active

    if ((upButton != nullptr) && upButton->getButtonState())
        tempDirection |= JoyDPadButton::DpadUp;

    if (downButton && downButton->getButtonState())
        tempDirection |= JoyDPadButton::DpadDown;

    if (leftButton && leftButton->getButtonState())
        tempDirection |= JoyDPadButton::DpadLeft;

    if (rightButton && rightButton->getButtonState())
        tempDirection |= JoyDPadButton::DpadRight;

    JoyDPad::joyEvent(tempDirection, ignoresets);

    pendingVDPadEvent = false;
}

void VDPad::addVButton(JoyDPadButton::JoyDPadDirections direction, JoyButton *button)
{
    switch (direction)
    {
    case JoyDPadButton::DpadUp: {
        if (upButton != nullptr)
            upButton->removeVDPad();

        upButton = button;
        upButton->setVDPad(this);

        break;
    }
    case JoyDPadButton::DpadDown: {
        if (downButton != nullptr)
            downButton->removeVDPad();

        downButton = button;
        downButton->setVDPad(this);

        break;
    }
    case JoyDPadButton::DpadLeft: {
        if (leftButton != nullptr)
            leftButton->removeVDPad();

        leftButton = button;
        leftButton->setVDPad(this);

        break;
    }
    case JoyDPadButton::DpadRight: {
        if (rightButton != nullptr)
            rightButton->removeVDPad();

        rightButton = button;
        rightButton->setVDPad(this);

        break;
    }
    default:
        break;
    }
}

void VDPad::removeVButton(JoyDPadButton::JoyDPadDirections direction)
{
    if ((direction == JoyDPadButton::DpadUp) && (upButton != nullptr))
    {
        upButton->removeVDPad();
        upButton = nullptr;
    } else if ((direction == JoyDPadButton::DpadDown) && (downButton != nullptr))
    {
        downButton->removeVDPad();
        downButton = nullptr;
    } else if ((direction == JoyDPadButton::DpadLeft) && (leftButton != nullptr))
    {
        leftButton->removeVDPad();
        leftButton = nullptr;
    } else if ((direction == JoyDPadButton::DpadRight) && (rightButton != nullptr))
    {
        rightButton->removeVDPad();
        rightButton = nullptr;
    }
}

void VDPad::removeVButton(JoyButton *button)
{
    if ((button != nullptr) && (button == upButton))
    {
        upButton->removeVDPad();
        upButton = nullptr;
    } else if ((button != nullptr) && (button == downButton))
    {
        downButton->removeVDPad();
        downButton = nullptr;
    } else if ((button != nullptr) && (button == leftButton))
    {
        leftButton->removeVDPad();
        leftButton = nullptr;
    } else if ((button != nullptr) && (button == rightButton))
    {
        rightButton->removeVDPad();
        rightButton = nullptr;
    }
}

bool VDPad::isEmpty()
{
    bool empty = true;

    if ((upButton != nullptr) || (downButton != nullptr) || (leftButton != nullptr) || (rightButton != nullptr))
    {
        empty = false;
    }

    return empty;
}

JoyButton *VDPad::getVButton(JoyDPadButton::JoyDPadDirections direction)
{
    JoyButton *button = nullptr;

    switch (direction)
    {
    case JoyDPadButton::DpadUp: {
        button = upButton;
        break;
    }
    case JoyDPadButton::DpadDown: {
        button = downButton;
        break;
    }
    case JoyDPadButton::DpadLeft: {
        button = leftButton;
        break;
    }
    case JoyDPadButton::DpadRight: {
        button = rightButton;
        break;
    }
    default:
        break;
    }

    return button;
}

bool VDPad::hasPendingEvent() { return pendingVDPadEvent; }

void VDPad::queueJoyEvent(bool ignoresets)
{
    Q_UNUSED(ignoresets)

    pendingVDPadEvent = true;
}

void VDPad::activatePendingEvent()
{
    if (pendingVDPadEvent)
    {
        // Always use true. The proper direction value will be determined
        // in the joyEvent method.

        joyEvent(true);
        pendingVDPadEvent = false;
    }
}

void VDPad::clearPendingEvent() { pendingVDPadEvent = false; }

JoyButton *VDPad::getUpButton() const { return upButton; }

JoyButton *VDPad::getDownButton() const { return downButton; }

JoyButton *VDPad::getLeftButton() const { return leftButton; }

JoyButton *VDPad::getRightButton() const { return rightButton; }

bool VDPad::getPendingVDPadEvent() const { return pendingVDPadEvent; }
