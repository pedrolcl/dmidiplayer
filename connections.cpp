/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2023, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QMessageBox>
#include <drumstick/settingsfactory.h>
#include <drumstick/configurationdialogs.h>
#include "connections.h"
#include "iconutils.h"

using namespace drumstick::rt;
using namespace drumstick::widgets;

Connections::Connections(QWidget *parent)
    : QDialog(parent),
      m_advanced(false),
      m_settingsChanged(false),
      m_midiOut(nullptr),
      m_savedOut(nullptr)
{
    ui.setupUi(this);
    ui.btnOutputDriverCfg->setIcon(IconUtils::GetIcon("wrench"));
    connect(ui.m_advanced, &QCheckBox::clicked, this, &Connections::clickedAdvanced);
    connect(ui.m_outputBackends, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Connections::refreshOutputs);
    connect(ui.btnOutputDriverCfg, &QToolButton::clicked, this, &Connections::configureOutputDriver);
    ui.m_advanced->setChecked(m_advanced);
}

void Connections::setOutput(drumstick::rt::MIDIOutput *out)
{
    m_savedOut = m_midiOut = out;
    if (out != nullptr) {
        m_connOut = out->currentConnection();
    }
}

void Connections::setOutputs(QList<MIDIOutput *> outs)
{
    ui.m_outputBackends->disconnect();
    foreach(MIDIOutput *o, outs) {
        ui.m_outputBackends->addItem(o->backendName(), QVariant::fromValue((void *) o));
    }
    connect(ui.m_outputBackends, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Connections::refreshOutputs);
}

MIDIOutput *Connections::getOutput()
{
    return m_midiOut;
}

void Connections::reopen()
{
    SettingsFactory settings;
    m_advanced = ui.m_advanced->isChecked();
    if (m_midiOut != 0) {
        if (m_connOut != m_midiOut->currentConnection() || m_settingsChanged) {
            m_midiOut->close();
            if (!m_connOut.first.isEmpty()) {
                m_midiOut->initialize(settings.getQSettings());
                m_midiOut->open(m_connOut);
                auto metaObj = m_midiOut->metaObject();
                if ((metaObj->indexOfProperty("status") != -1) &&
                    (metaObj->indexOfProperty("diagnostics") != -1)) {
                    auto status = m_midiOut->property("status");
                    if (status.isValid() && !status.toBool()) {
                        auto diagnostics = m_midiOut->property("diagnostics");
                        if (diagnostics.isValid()) {
                            auto text = diagnostics.toStringList().join(QChar::LineFeed).trimmed();
                            QMessageBox::warning(this, tr("MIDI Output"), text);
                        }
                    }
                }
            }
        }
    }
    m_settingsChanged = false;
}

void Connections::accept()
{
    m_connOut = ui.m_outputPorts->currentData().value<MIDIConnection>();
    reopen();
    QDialog::accept();
}

void Connections::reject()
{
    m_midiOut = m_savedOut;
    reopen();
    QDialog::reject();
}

void Connections::refresh()
{
    m_advanced = ui.m_advanced->isChecked();
    if (m_midiOut != 0) {
        ui.m_outputBackends->setCurrentText(m_midiOut->backendName());
        refreshOutputs(ui.m_outputBackends->currentIndex());
    }
}

void Connections::refreshOutputs(int idx)
{
    QString id = ui.m_outputBackends->itemText(idx);
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
        auto connections = m_midiOut->connections(m_advanced);
        foreach(const auto& conn, connections) {
            ui.m_outputPorts->addItem(conn.first, QVariant::fromValue(conn));
        }
        QString conn = m_midiOut->currentConnection().first;
        if (conn.isEmpty() && !connections.isEmpty()) {
            conn = connections.first().first;
        }
        ui.m_outputPorts->setCurrentText(conn);
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

void Connections::retranslateUi()
{
    ui.retranslateUi(this);
}

void Connections::configureOutputDriver()
{
    QString driver = ui.m_outputBackends->currentText();
    m_settingsChanged |= drumstick::widgets::configureOutputDriver(driver, this);
}
