/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2021, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
#include <QApplication>
#include <QToolBar>
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
#include <QDesktopWidget>
#else
#include <QScreen>
#endif

#include <drumstick/settingsfactory.h>
#include "settings.h"
#include "iconutils.h"
#include "helpwindow.h"

HelpWindow::HelpWindow(QWidget *parent):
    QMainWindow(parent)
{
    setObjectName(QString::fromUtf8("HelpWindow"));
    setWindowIcon(QIcon(":/dmidiplayer.png"));
#if defined(Q_OS_MACOS)
    setUnifiedTitleAndToolBarOnMac(true);
    setAttribute(Qt::WA_MacMiniSize, true);
#else
    setWindowFlag(Qt::Tool, true);
#endif
    setAttribute(Qt::WA_DeleteOnClose, false);

    QToolBar* tbar = new QToolBar(this);
    tbar->setObjectName("toolbar");
    tbar->setMovable(false);
    tbar->setFloatable(false);
    tbar->setIconSize(QSize(22,22));
    addToolBar(tbar);

    m_textBrowser = new QTextBrowser(this);
    m_homeButton = new QPushButton(tr("&Home"), this);
    m_homeButton->setIcon(IconUtils::GetIcon("go-home"));
    m_backButton = new QPushButton(tr("&Back"), this);
    m_backButton->setIcon(IconUtils::GetIcon("go-previous"));
    m_closeButton = new QPushButton(tr("Close"), this);
    m_closeButton->setShortcut(tr("Esc"));
    m_closeButton->setIcon(IconUtils::GetIcon("window-close"));
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    tbar->addWidget(m_homeButton);
    tbar->addWidget(m_backButton);
    tbar->addWidget(spacer);
    tbar->addWidget(m_closeButton);

    setCentralWidget(m_textBrowser);

    connect(m_homeButton, &QAbstractButton::clicked, m_textBrowser, &QTextBrowser::home);
    connect(m_backButton, &QAbstractButton::clicked, m_textBrowser, &QTextBrowser::backward);
    connect(m_closeButton, &QAbstractButton::clicked, this, &QWidget::close);
    connect(m_textBrowser, &QTextBrowser::sourceChanged, this, &HelpWindow::updateWindowTitle);

    m_textBrowser->setOpenExternalLinks(true);

    resize(640,480);
    readSettings();
    retranslateUi();
#if defined(Q_OS_WINDOWS)
    m_snapper.SetEnabled(Settings::instance()->winSnap());
#endif
}

void HelpWindow::readSettings()
{
    using namespace drumstick::widgets;
    SettingsFactory settings;
    settings->beginGroup("HelpWindow");
    const QByteArray geometry = settings->value("Geometry", QByteArray()).toByteArray();
    settings->endGroup();

    if (geometry.isEmpty()) {
        const QRect availableGeometry =
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
                QApplication::desktop()->availableGeometry(this);
#else
                screen()->availableGeometry();
#endif
        //qDebug() << Q_FUNC_INFO << availableGeometry;
        setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                        size(), availableGeometry));
    } else {
        restoreGeometry(geometry);
    }
}

void HelpWindow::writeSettings()
{
    using namespace drumstick::widgets;
    SettingsFactory settings;
    settings->beginGroup("HelpWindow");
    settings->setValue("Geometry", saveGeometry());
    settings->endGroup();
}


void HelpWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

bool HelpWindow::nativeEvent(const QByteArray &eventType, void *message,
 #if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                          long *result
 #else
                          qintptr *result
 #endif
                         )
{
#if defined(Q_OS_WINDOWS)
    if (m_snapper.HandleMessage(message)) {
        result = 0;
        return true;
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void HelpWindow::updateWindowTitle()
{
    setWindowTitle(tr("Help: %1").arg(m_textBrowser->documentTitle()));
}

void HelpWindow::showPage(const QString &path, const QString &page)
{
    //qDebug() << Q_FUNC_INFO << path << page;
    m_textBrowser->clear();
    m_textBrowser->clearHistory();
    m_textBrowser->setSearchPaths({m_path = path,":/help/en",":/"});
    m_textBrowser->setSource(m_page = page);
    show();
}

void HelpWindow::retranslateUi()
{
    m_path = QStringLiteral(":/help");
    m_page = QStringLiteral("%1/index.html").arg(Settings::instance()->language());
    m_textBrowser->setSearchPaths({m_path,":/help/en",":/"});
    m_textBrowser->setSource(m_page);
    updateWindowTitle();
    m_homeButton->setText(tr("&Home"));
    m_backButton->setText(tr("&Back"));
    m_closeButton->setText(tr("Close"));
    m_closeButton->setShortcut(tr("Esc"));
}
