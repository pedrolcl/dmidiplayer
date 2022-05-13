/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2022, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
#include <QVBoxLayout>

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
    FramelessWindow(parent)
{
    setObjectName(QString::fromUtf8("HelpWindow"));
    setWindowIcon(QIcon(":/dmidiplayer.png"));

    QToolBar* tbar = new QToolBar(this);
    tbar->setObjectName("toolbar");
    tbar->setMovable(false);
    tbar->setFloatable(false);
    tbar->setIconSize(QSize(22,22));
    tbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    addToolBar(tbar);
    setPseudoCaption(tbar);
    
    m_titleLabel = new QLabel(this);
    m_titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_textBrowser = new QTextBrowser(this);
    m_home = new QAction(tr("&Home"), this);
    m_back = new QAction(tr("&Back"), this);
    m_close = new QAction(tr("Close"), this);
    m_zoomIn = new QAction(tr("Zoom In"), this);
    m_zoomOut = new QAction(tr("Zoom Out"), this);
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    tbar->addWidget(m_titleLabel);
    tbar->addAction(m_home);
    tbar->addAction(m_back);
    tbar->addAction(m_zoomIn);
    tbar->addAction(m_zoomOut);
    tbar->addWidget(spacer);
    tbar->addAction(m_close);
    
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(5,5,5,5);
    vlayout->setSizeConstraint(QLayout::SetNoConstraint);
    vlayout->addWidget(m_textBrowser);
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(vlayout);
    setCentralWidget(centralWidget);

    connect(m_home, &QAction::triggered, m_textBrowser, &QTextBrowser::home);
    connect(m_back, &QAction::triggered, m_textBrowser, &QTextBrowser::backward);
    connect(m_zoomIn, &QAction::triggered, this, [=]{ m_textBrowser->zoomIn(); });
    connect(m_zoomOut, &QAction::triggered, this, [=]{ m_textBrowser->zoomOut(); });
    connect(m_close, &QAction::triggered, this, &HelpWindow::close);
    connect(m_textBrowser, &QTextBrowser::sourceChanged, this, &HelpWindow::updateWindowTitle);

    m_textBrowser->setOpenExternalLinks(true);

    retranslateUi();
}

void HelpWindow::readSettings()
{
    const QByteArray geometry = Settings::instance()->helpWindowGeometry();
    const QByteArray state = Settings::instance()->helpWindowState();
    const int fontSize = Settings::instance()->helpWindowFontSize();

    if (geometry.isEmpty()) {
        const QRect availableGeometry =
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
                QApplication::desktop()->availableGeometry(this);
#else
                screen()->availableGeometry();
#endif
        QSize size(640,480);
        setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                        size, availableGeometry));
    } else {
        restoreGeometry(geometry);
    }
    if (!state.isEmpty()) {
        restoreState(state);
    }
    if (fontSize > 0) {
        //qDebug() << Q_FUNC_INFO << fontSize;
        auto font = m_textBrowser->font();
        font.setPointSize(fontSize);
        m_textBrowser->setFont(font);
    }
}

void HelpWindow::writeSettings()
{
    auto fontSize = m_textBrowser->font().pointSize();
    Settings::instance()->setHelpWindowGeometry(saveGeometry());
    Settings::instance()->setHelpWindowState(saveState());
    Settings::instance()->setHelpWindowFontSize(fontSize);
    //qDebug() << Q_FUNC_INFO << fontSize;
}

void HelpWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void HelpWindow::updateWindowTitle()
{
    //setWindowTitle(m_textBrowser->documentTitle());
    m_titleLabel->setText(m_textBrowser->documentTitle());
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

void HelpWindow::showEvent(QShowEvent *event)
{
    static bool firstTime = true;
    QMainWindow::showEvent(event);
    if (firstTime) {
        readSettings();
        firstTime = false;
    }
}

void HelpWindow::retranslateUi()
{
    QString lang = Settings::instance()->language();
    if (lang == "C") {
        lang = "en";
    }
    m_path = QStringLiteral(":/help");
    m_page = QStringLiteral("%1/index.html").arg(lang);
    m_textBrowser->setSearchPaths({m_path,":/help/en",":/"});
    m_textBrowser->setSource(m_page);
    updateWindowTitle();
    m_home->setText(tr("&Home"));
    m_back->setText(tr("&Back"));
    m_close->setText(tr("Close"));
    m_zoomIn->setText(tr("Zoom In"));
    m_zoomOut->setText(tr("Zoom Out"));
}

void HelpWindow::applySettings()
{
    m_home->setIcon(IconUtils::GetIcon("go-home"));
    m_back->setIcon(IconUtils::GetIcon("go-previous"));
    m_close->setIcon(IconUtils::GetIcon("window-close"));
    m_zoomIn->setIcon(IconUtils::GetIcon("format-font-size-more"));
    m_zoomOut->setIcon(IconUtils::GetIcon("format-font-size-less"));
	FramelessWindow::applySettings();
}
