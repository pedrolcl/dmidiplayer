/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2020, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDebug>
#include <drumstick/settingsfactory.h>
#include <drumstick/configurationdialogs.h>
#include "connections.h"

using namespace drumstick::rt;
using namespace drumstick::widgets;

Connections::Connections(QWidget *parent)
    : QDialog(parent),
      m_advanced(false),
      m_settingsChanged(false),
      m_midiOut(0)
{
    ui.setupUi(this);
    connect(ui.m_advanced, &QCheckBox::clicked, this, &Connections::clickedAdvanced);
    connect(ui.m_outputBackends, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &Connections::refreshOutputs);
    connect(ui.btnOutputDriverCfg, &QToolButton::clicked, this, &Connections::configureOutputDriver);
    ui.m_advanced->setChecked(m_advanced);
}

void Connections::setOutputs(QList<MIDIOutput *> outs)
{
    ui.m_outputBackends->disconnect();
    foreach(MIDIOutput *o, outs) {
        //qDebug() << Q_FUNC_INFO << o->backendName();
        ui.m_outputBackends->addItem(o->backendName(), qVariantFromValue((void *) o));
    }
    connect(ui.m_outputBackends, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &Connections::refreshOutputs);
}

void Connections::accept()
{
    MIDIConnection conn;
    SettingsFactory settings;
    m_advanced = ui.m_advanced->isChecked();
    if (m_midiOut != 0) {
        conn = ui.m_outputPorts->currentData().value<MIDIConnection>();
        if (conn != m_midiOut->currentConnection() || m_settingsChanged) {
            m_midiOut->close();
            if (!conn.first.isEmpty()) {
                m_midiOut->initialize(settings.getQSettings());
                m_midiOut->open(conn);
            }
        }
    }
    m_settingsChanged = false;
    QDialog::accept();
}

void Connections::refresh()
{
    m_advanced = ui.m_advanced->isChecked();
    if (m_midiOut != 0) {
        //qDebug() << Q_FUNC_INFO << m_midiOut->backendName();
        ui.m_outputBackends->setCurrentText(m_midiOut->backendName());
        refreshOutputs(m_midiOut->backendName());
    }
}

void Connections::refreshOutputs(QString id)
{
    ui.btnOutputDriverCfg->setEnabled(drumstick::widgets::outputDriverIsConfigurable(id));
    if (m_midiOut != 0 && m_midiOut->backendName() != id) {
        m_midiOut->close();
        int idx = ui.m_outputBackends->findText(id, Qt::MatchStartsWith);
        if (idx > -1) {
            m_midiOut = (MIDIOutput *) ui.m_outputBackends->itemData(idx).value<void *>();
        } else {
            m_midiOut = 0;
        }
    }
    ui.m_outputPorts->clear();
    if (m_midiOut != 0) {
        for(const auto& conn : m_midiOut->connections(m_advanced)) {
            ui.m_outputPorts->addItem(conn.first, QVariant::fromValue(conn));
        }
        ui.m_outputPorts->setCurrentText(m_midiOut->currentConnection().first);
    }
}

void Connections::setAdvanced(bool value)
{
    ui.m_advanced->setChecked(value);
    refresh();
}

void Connections::clickedAdvanced(bool value)
{
    m_advanced = value;
    refresh();
}

bool Connections::advanced()
{
    return ui.m_advanced->isChecked();
}

void Connections::configureOutputDriver()
{
    QString driver = ui.m_outputBackends->currentText();
    m_settingsChanged |= drumstick::widgets::configureOutputDriver(driver, this);
}
