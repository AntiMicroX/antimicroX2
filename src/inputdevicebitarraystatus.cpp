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

#include "inputdevicebitarraystatus.h"

#include "inputdevice.h"
#include "joybutton.h"
#include "joydpad.h"
#include "joystick.h"
#include "setjoystick.h"

#include <QDebug>
#include <QMutex>

InputDeviceBitArrayStatus::InputDeviceBitArrayStatus(InputDevice *device, bool readCurrent, QObject *parent)
    : QObject(parent)
{
    QMutex mutex;
    QMutexLocker locker(&mutex);

    for (int i = 0; i < device->getNumberRawAxes(); i++)
    {
        SetJoystick *currentSet = device->getActiveSetJoystick();
        JoyAxis *axis = currentSet->getJoyAxis(i);

        if ((axis != nullptr) && readCurrent)
        {
            axesStatus.append(!axis->inDeadZone(axis->getCurrentRawValue()) ? true : false);
        } else
        {
            axesStatus.append(false);
        }
    }

    for (int i = 0; i < device->getNumberRawHats(); i++)
    {
        SetJoystick *currentSet = device->getActiveSetJoystick();
        JoyDPad *dpad = currentSet->getJoyDPad(i);

        if ((dpad != nullptr) && readCurrent)
        {
            hatButtonStatus.append(dpad->getCurrentDirection() != JoyDPadButton::DpadCentered ? true : false);
        } else
        {
            hatButtonStatus.append(false);
        }
    }

    getButtonStatusLocal().resize(device->getNumberRawButtons());
    getButtonStatusLocal().fill(0);

    for (int i = 0; i < device->getNumberRawButtons(); i++)
    {
        SetJoystick *currentSet = device->getActiveSetJoystick();
        JoyButton *button = currentSet->getJoyButton(i);

        if ((button != nullptr) && readCurrent)
        {
            getButtonStatusLocal().setBit(i, button->getButtonState());
        }
    }
}

void InputDeviceBitArrayStatus::changeAxesStatus(int axisIndex, bool value)
{
    QMutex mutex;
    QMutexLocker locker(&mutex);

    if ((axisIndex >= 0) && (axisIndex <= axesStatus.size()))
    {
        axesStatus.replace(axisIndex, value);
    }
}

void InputDeviceBitArrayStatus::changeButtonStatus(int buttonIndex, bool value)
{
    QMutex mutex;
    QMutexLocker locker(&mutex);

    if ((buttonIndex >= 0) && (buttonIndex <= getButtonStatusLocal().size()))
    {
        getButtonStatusLocal().setBit(buttonIndex, value);
    }
}

void InputDeviceBitArrayStatus::changeHatStatus(int hatIndex, bool value)
{
    QMutex mutex;
    QMutexLocker locker(&mutex);

    if ((hatIndex >= 0) && (hatIndex <= hatButtonStatus.size()))
    {
        hatButtonStatus.replace(hatIndex, value);
    }
}

QBitArray InputDeviceBitArrayStatus::generateFinalBitArray()
{
    QMutex mutex;
    QMutexLocker locker(&mutex);

    int totalArraySize = 0;
    totalArraySize = axesStatus.size() + hatButtonStatus.size() + getButtonStatusLocal().size();
    QBitArray aggregateBitArray(totalArraySize, false);
    int currentBit = 0;

    for (int i = 0; i < axesStatus.size(); i++)
    {
        aggregateBitArray.setBit(currentBit, axesStatus.at(i));
        currentBit++;
    }

    for (int i = 0; i < hatButtonStatus.size(); i++)
    {
        aggregateBitArray.setBit(currentBit, hatButtonStatus.at(i));
        currentBit++;
    }

    for (int i = 0; i < getButtonStatusLocal().size(); i++)
    {
        aggregateBitArray.setBit(currentBit, getButtonStatusLocal().at(i));
        currentBit++;
    }

    return aggregateBitArray;
}

void InputDeviceBitArrayStatus::clearStatusValues()
{
    QMutex mutex;
    QMutexLocker locker(&mutex);

    for (int i = 0; i < axesStatus.size(); i++)
        axesStatus.replace(i, false);

    for (int i = 0; i < hatButtonStatus.size(); i++)
        hatButtonStatus.replace(i, false);

    getButtonStatusLocal().fill(false);
}

QBitArray &InputDeviceBitArrayStatus::getButtonStatusLocal() { return buttonStatus; }
