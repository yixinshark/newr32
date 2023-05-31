//
// Created by yixin on 2023/5/31.
//

#ifndef R32_MCUOPERATEWIDGET_H
#define R32_MCUOPERATEWIDGET_H

#include <QWidget>

class McuOperateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit McuOperateWidget(QWidget *parent = nullptr);
    ~McuOperateWidget() override;

private:
    void initUI();
};


#endif //R32_MCUOPERATEWIDGET_H
