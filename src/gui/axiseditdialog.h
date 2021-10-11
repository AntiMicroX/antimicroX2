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

#ifndef AXISEDITDIALOG_H
#define AXISEDITDIALOG_H

#include <QDialog>

class JoyAxis;
class SetAxisThrottleDialog;
class QWidget;

namespace Ui {
class AxisEditDialog;
}

class AxisEditDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit AxisEditDialog(JoyAxis *axis, bool keypadUnlocked, QWidget *parent = 0);
    AxisEditDialog(QWidget *parent = 0);
    ~AxisEditDialog();

  protected:
    void selectAxisCurrentPreset();
    void selectTriggerPreset();

    void buildTriggerPresetsMenu();
    void buildAxisPresetsMenu();

  private:
    Ui::AxisEditDialog *ui;

    JoyAxis *m_axis;
    bool keypadUnlocked;
    SetAxisThrottleDialog *setAxisThrottleConfirm;
    int initialThrottleState;

  private slots:
    void implementAxisPresets(int index);
    void implementTriggerPresets(int index);
    void implementPresets(int index);
    void presetForThrottleChange(int index);

    void updateDeadZoneBox(int value);
    void updateMaxZoneBox(int value);
    void updateThrottleUi(int index);
    void updateJoyValue(int value);
    void updateDeadZoneSlider(QString value);
    void updateMaxZoneSlider(QString value);
    void openAdvancedPDialog();
    void openAdvancedNDialog();

    void refreshPButtonLabel();
    void refreshNButtonLabel();
    void refreshPreset();

    void checkFinalSettings();
    void openMouseSettingsDialog();
    void enableMouseSettingButton();
    void updateWindowTitleAxisName();
};

#endif // AXISEDITDIALOG_H
