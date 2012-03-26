/*
 * Copyright (C) 2012 Rajendran Thirupugalsamy
 * See LICENSE for full copyright and license information.
 * See COPYING for distribution information.
 */

#include "GuiSettingsWindow.h"
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QDebug>
#include "GuiMainWindow.h"
#include "QtCommon.h"
extern "C" {
#include "putty.h"
}

GuiSettingsWindow::GuiSettingsWindow(QWidget *parent) :
    QDialog(parent)
{
    QGroupBox *groupbox;
    QGridLayout *layout;
    QVBoxLayout *mainLayout;
    mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    groupbox = new QGroupBox(tr("Specify the destination you want to connect to"));
    mainLayout->addWidget(groupbox);
    layout = new QGridLayout;
    groupbox->setLayout(layout);
    txtHostName = new QLineEdit;
    txtPort = new QLineEdit;
    layout->addWidget(new QLabel(tr("Host Name (or IP address)")), 0, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Port")), 0, 2, 1, 1);
    layout->addWidget(txtHostName, 1, 0, 1, 2);
    layout->addWidget(txtPort, 1, 2, 1, 1);
    layout->addWidget(new QLabel(tr("Connection type:")), 2, 0);
    btnConnType = new QButtonGroup;
    QRadioButton *radiobtn = new QRadioButton(tr("Telnet"));
    layout->addWidget(radiobtn, 3, 0);
    btnConnType->addButton(radiobtn, PROT_TELNET);
    radiobtn = new QRadioButton(tr("SSH"));
    layout->addWidget(radiobtn, 3, 1);
    btnConnType->addButton(radiobtn, PROT_SSH);

    groupbox = new QGroupBox(tr("Load, save or delete a stored session"));
    mainLayout->addWidget(groupbox);
    layout = new QGridLayout;
    groupbox->setLayout(layout);
    layout->addWidget(new QLabel(tr("Saved Sessions")), 0, 0);

    groupbox = new QGroupBox();
    mainLayout->addWidget(groupbox);
    layout = new QGridLayout;
    groupbox->setLayout(layout);
    layout->addWidget(new QLabel(tr("Close window on exit:")), 0, 0, 1, -1);
    layout->addWidget(new QRadioButton(tr("Always")), 1, 0);
    layout->addWidget(new QRadioButton(tr("Never")), 1, 1);
    layout->addWidget(new QRadioButton(tr("Only on clean exit")), 1, 2);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Open
                                          | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(newTerminal()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    //groupbox->setLayout(mainLayout);

    setWindowTitle(tr("[New Session]"));

    txtHostName->setText("192.168.1.103");
    txtPort->setText("23");
}

void GuiSettingsWindow::newTerminal()
{
    qDebug()<<"newTerminal"<<txtHostName->text()<<txtPort->text()<<btnConnType->checkedId();
    QByteArray hostname = txtHostName->text().toUtf8();
    QByteArray port = txtPort->text().toUtf8();
    mainWindow->newTelnetTerminal(hostname.constData(), port.constData(), btnConnType->checkedId());
    this->close();
}