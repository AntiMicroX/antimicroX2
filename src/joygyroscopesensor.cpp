/* antimicrox Gamepad to KB+M event mapper
 * Copyright (C) 2022 Max Maisel <max.maisel@posteo.de>
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

#include "joygyroscopesensor.h"
#include "globalvariables.h"
#include "joybuttontypes/joygyroscopebutton.h"

JoyGyroscopeSensor::JoyGyroscopeSensor(int originset, SetJoystick *parent_set, QObject *parent)
    : JoySensor(GYROSCOPE, originset, parent_set, parent)
{
    reset();
    populateButtons();
}

JoyGyroscopeSensor::~JoyGyroscopeSensor() {}

/**
 * @brief Get the value for the corresponding X axis.
 * @return X axis value in °/s
 */
float JoyGyroscopeSensor::getXCoordinate() const { return radToDeg(m_current_value[0]); }

/**
 * @brief Get the value for the corresponding Y axis.
 * @return Y axis value in °/s
 */
float JoyGyroscopeSensor::getYCoordinate() const { return radToDeg(m_current_value[1]); }

/**
 * @brief Get the value for the corresponding Z axis.
 * @return Z axis value in °/s
 */
float JoyGyroscopeSensor::getZCoordinate() const { return radToDeg(m_current_value[2]); }

/**
 * @brief Get the translated sensor type name
 * @returns Translated sensor type name
 */
QString JoyGyroscopeSensor::sensorTypeName() const { return tr("Gyroscope"); }

/**
 * @brief Reads the calibration values of the sensor
 * @param[out] offsetX Offset value for X axis
 * @param[out] offsetY Offset value for Y axis
 * @param[out] offsetZ Offset value for Z axis
 */
void JoyGyroscopeSensor::getCalibration(double *offsetX, double *offsetY, double *offsetZ) const
{
    *offsetX = m_calibration_value[0];
    *offsetY = m_calibration_value[1];
    *offsetZ = m_calibration_value[2];
}

/**
 * @brief Sets the sensor calibration values and sets the calibration flag.
 * @param[in] offsetX Offset value for X axis
 * @param[in] offsetY Offset value for Y axis
 * @param[in] offsetZ Offset value for Z axis
 */
void JoyGyroscopeSensor::setCalibration(double offsetX, double offsetY, double offsetZ)
{
    m_calibration_value[0] = offsetX;
    m_calibration_value[1] = offsetY;
    m_calibration_value[2] = offsetZ;
    m_calibrated = true;
}

/**
 * @brief Resets internal variables back to default
 */
void JoyGyroscopeSensor::reset()
{
    JoySensor::reset();
    m_max_zone = degToRad(GlobalVariables::JoySensor::GYRO_MAX);
}

/**
 * @brief Initializes the JoySensorButton objects for this sensor.
 */
void JoyGyroscopeSensor::populateButtons()
{
    JoySensorButton *button = nullptr;
    button = new JoyGyroscopeButton(this, SENSOR_LEFT, m_originset, getParentSet(), this);
    m_buttons.insert(SENSOR_LEFT, button);

    button = new JoyGyroscopeButton(this, SENSOR_RIGHT, m_originset, getParentSet(), this);
    m_buttons.insert(SENSOR_RIGHT, button);

    button = new JoyGyroscopeButton(this, SENSOR_UP, m_originset, getParentSet(), this);
    m_buttons.insert(SENSOR_UP, button);

    button = new JoyGyroscopeButton(this, SENSOR_DOWN, m_originset, getParentSet(), this);
    m_buttons.insert(SENSOR_DOWN, button);

    button = new JoyGyroscopeButton(this, SENSOR_FWD, m_originset, getParentSet(), this);
    m_buttons.insert(SENSOR_FWD, button);

    button = new JoyGyroscopeButton(this, SENSOR_BWD, m_originset, getParentSet(), this);
    m_buttons.insert(SENSOR_BWD, button);
}

/**
 * @brief Applies calibration to queued input values
 */
void JoyGyroscopeSensor::applyCalibration()
{
    m_pending_value[0] -= m_calibration_value[0];
    m_pending_value[1] -= m_calibration_value[1];
    m_pending_value[2] -= m_calibration_value[2];
}
