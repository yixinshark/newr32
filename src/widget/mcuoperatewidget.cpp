//
// Created by yixin on 2023/5/31.
//

#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

#include "mcuoperatewidget.h"

McuOperateWidget::McuOperateWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
}

McuOperateWidget::~McuOperateWidget()
{

}

void McuOperateWidget::initUI()
{
    auto *mainLayout = new QVBoxLayout(this);

    // 第一行
    auto *row1Layout = new QHBoxLayout;
    auto *powerButton = new QPushButton("箱体开关", this);
    auto *powerButtonGroup = new QButtonGroup(this);
    auto *onRadioButton = new QRadioButton("开", this);
    auto *offRadioButton = new QRadioButton("关", this);
    powerButtonGroup->addButton(onRadioButton);
    powerButtonGroup->addButton(offRadioButton);
    row1Layout->addWidget(powerButton);
    row1Layout->addWidget(onRadioButton);
    row1Layout->addWidget(offRadioButton);
    mainLayout->addLayout(row1Layout);

    // 第二行
    auto *row2Layout = new QHBoxLayout;
    auto *openValveButton = new QPushButton("打开电磁阀", this);
    auto *valve1CheckBox = new QCheckBox("电磁阀1", this);
    auto *valve2CheckBox = new QCheckBox("电磁阀2", this);
    auto *valve3CheckBox = new QCheckBox("电磁阀3", this);
    auto *valve4CheckBox = new QCheckBox("电磁阀4", this);
    auto *valve5CheckBox = new QCheckBox("电磁阀5", this);

    row2Layout->addWidget(openValveButton);
    row2Layout->addWidget(valve1CheckBox);
    row2Layout->addWidget(valve2CheckBox);
    row2Layout->addWidget(valve3CheckBox);
    row2Layout->addWidget(valve4CheckBox);
    row2Layout->addWidget(valve5CheckBox);
    mainLayout->addLayout(row2Layout);

    // 第三行
    auto *row3Layout = new QHBoxLayout;
    auto *fanBtn = new QPushButton("风扇", this);
    auto *fan1CheckBox = new QCheckBox("风扇1", this);
    auto *fan2CheckBox = new QCheckBox("风扇2", this);
    auto *fan3CheckBox = new QCheckBox("风扇3", this);
    auto *fan4CheckBox = new QCheckBox("风扇4", this);

    row3Layout->addWidget(fanBtn);
    row3Layout->addWidget(fan1CheckBox);
    row3Layout->addWidget(fan2CheckBox);
    row3Layout->addWidget(fan3CheckBox);
    row3Layout->addWidget(fan4CheckBox);
    mainLayout->addLayout(row3Layout);

    // 第四行
    auto *row4Layout = new QHBoxLayout;
    auto *controlButton = new QPushButton("浓度控制", this);
    auto *concentrationLabel = new QLabel("浓度:", this);
    auto *concentrationLineEdit = new QLineEdit(this);
    row4Layout->addWidget(controlButton);
    row4Layout->addWidget(concentrationLabel);
    row4Layout->addWidget(concentrationLineEdit);
    mainLayout->addLayout(row4Layout);

    // 第五行
    auto *row5Layout = new QHBoxLayout;
    auto *channelLabel = new QLabel("通道选择:", this);
    auto *channelBox = new QComboBox(this);
    for (int i = 1; i <= 64; ++i) {
        channelBox->addItem(QString::number(i));
    }

    row5Layout->addWidget(channelLabel);
    row5Layout->addWidget(channelBox);
    mainLayout->addLayout(row5Layout);

    // 第六行
    auto *row6Layout = new QHBoxLayout;
    auto *alarmLabel = new QLabel("通道:", this);
    auto *alarmLineEdit = new QLineEdit(this);
    auto *alarmOnRadioButton = new QRadioButton("开");
    auto *alarmOffRadioButton = new QRadioButton("关");

    auto *alarmButtonGroup = new QButtonGroup(this);
    alarmButtonGroup->addButton(alarmOnRadioButton);
    alarmButtonGroup->addButton(alarmOffRadioButton);

    row6Layout->addWidget(alarmLabel);
    row6Layout->addWidget(alarmLineEdit);
    row6Layout->addWidget(alarmOnRadioButton);
    row6Layout->addWidget(alarmOffRadioButton);

    mainLayout->addLayout(row6Layout);
}
