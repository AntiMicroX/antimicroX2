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

#include "xmlconfigreader.h"

#include "common.h"
#include "globalvariables.h"
#include "inputdevice.h"
#include "joystick.h"
#include "xmlconfigmigration.h"
#include "xmlconfigwriter.h"

#include "common.h"
#include "gamecontroller/gamecontroller.h"
#include "gamecontroller/xml/gamecontrollerxml.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QXmlStreamReader>

XMLConfigReader::XMLConfigReader(QObject *parent)
    : QObject(parent)
{
    xml = new QXmlStreamReader();
    configFile = nullptr;
    m_joystick = nullptr;
    initDeviceTypes();
}

XMLConfigReader::~XMLConfigReader()
{
    if (configFile != nullptr)
    {
        if (configFile->isOpen())
            configFile->close();

        delete configFile;
        configFile = nullptr;
    }

    if (xml != nullptr)
    {
        delete xml;
        xml = nullptr;
    }

    if (!m_joystickXml.isNull())
        delete m_joystickXml;
}

void XMLConfigReader::setJoystick(InputDevice *joystick) { m_joystick = joystick; }

void XMLConfigReader::setFileName(QString filename)
{
    QFile *temp = new QFile(filename);

    if (temp->exists())
    {
        configFile = temp;
    } else
    {
        delete temp;
        temp = nullptr;
    }
}

void XMLConfigReader::configJoystick(InputDevice *joystick)
{
    m_joystick = joystick;
    read();
}

bool XMLConfigReader::read()
{
    bool error = false;

    if ((configFile != nullptr) && configFile->exists() && (m_joystick != nullptr))
    {
        xml->clear();

        if (!configFile->isOpen())
        {
            configFile->open(QFile::ReadOnly | QFile::Text);
            xml->setDevice(configFile);
        }

        xml->readNextStartElement();

        if (!deviceTypes.contains(xml->name().toString()))
        {
            xml->raiseError("Root node is not a joystick or controller");
        } else if (xml->name() == GlobalVariables::Joystick::xmlName)
        {
            XMLConfigMigration migration(xml);

            if (migration.requiresMigration())
            {
                QString migrationString = migration.migrate();

                if (migrationString.length() > 0)
                {
                    xml->clear();                                     // Remove QFile from reader and clear state
                    xml->addData(migrationString);                    // Add converted XML string to reader
                    xml->readNextStartElement();                      // Skip joystick root node
                    configFile->close();                              // Close current config file
                    configFile->open(QFile::WriteOnly | QFile::Text); // Write converted XML to file

                    if (configFile->isOpen())
                    {
                        configFile->write(migrationString.toLocal8Bit());
                        configFile->close();
                    } else
                    {
                        xml->raiseError(tr("Could not write updated profile XML to file %1.").arg(configFile->fileName()));
                    }
                }
            }
        }

        while (!xml->atEnd())
        {
            if (xml->isStartElement() && deviceTypes.contains(xml->name().toString()))
            {
                m_joystickXml = new InputDeviceXml(m_joystick);
                m_joystickXml->readConfig(xml);
                // if (!m_joystickXml.isNull()) delete m_joystickXml;
            } else
            {
                // If none of the above, skip the element
                xml->skipCurrentElement();
            }

            xml->readNextStartElement();
        }

        if (configFile->isOpen())
            configFile->close();

        if (xml->hasError() && (xml->error() != QXmlStreamReader::PrematureEndOfDocumentError))
        {
            error = true;
        } else if (xml->hasError() && (xml->error() == QXmlStreamReader::PrematureEndOfDocumentError))
        {
            xml->clear();
        }
    }

    return error;
}

const QString XMLConfigReader::getErrorString()
{
    QString temp = QString();

    if (xml->hasError())
        temp = xml->errorString();

    return temp;
}

bool XMLConfigReader::hasError() { return xml->hasError(); }

void XMLConfigReader::initDeviceTypes()
{
    deviceTypes.clear();
    deviceTypes.append(GlobalVariables::Joystick::xmlName);
    deviceTypes.append(GlobalVariables::GameController::xmlName);
}

const QXmlStreamReader *XMLConfigReader::getXml() { return xml; }

QString const &XMLConfigReader::getFileName() { return fileName; }

const QFile *XMLConfigReader::getConfigFile() { return configFile; }

const InputDevice *XMLConfigReader::getJoystick() { return m_joystick; }

QStringList const &XMLConfigReader::getDeviceTypes() { return deviceTypes; }
