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

#include "extraprofilesettingsdialog.h"
#include "ui_extraprofilesettingsdialog.h"

#include "inputdevice.h"

#include <QDebug>

ExtraProfileSettingsDialog::ExtraProfileSettingsDialog(InputDevice *device, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExtraProfileSettingsDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    this->device = device;
    ui->pressValueLabel->setText(QString::number(0.10, 'g', 3).append("").append(tr("s")));

    if (device->getDeviceKeyPressTime() > 0)
    {
        int temppress = device->getDeviceKeyPressTime();
        ui->keyPressHorizontalSlider->setValue(device->getDeviceKeyPressTime() / 10);
        ui->pressValueLabel->setText(QString::number(temppress / 1000.0, 'g', 3).append("").append(tr("s")));
    }

    if (!device->getProfileName().isEmpty())
        ui->profileNameLineEdit->setText(device->getProfileName());

    connect(ui->keyPressHorizontalSlider, &QSlider::valueChanged, this, &ExtraProfileSettingsDialog::changeDeviceKeyPress);
    connect(ui->profileNameLineEdit, &QLineEdit::textChanged, device, &InputDevice::setProfileName);
}

ExtraProfileSettingsDialog::~ExtraProfileSettingsDialog() { delete ui; }

void ExtraProfileSettingsDialog::changeDeviceKeyPress(int value)
{
    int temppress = value * 10;
    device->setDeviceKeyPressTime(temppress);
    ui->pressValueLabel->setText(QString::number(temppress / 1000.0, 'g', 3).append("").append(tr("s")));
}

InputDevice *ExtraProfileSettingsDialog::getDevice() const { return device; }
