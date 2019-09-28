#pragma once

#include <QWidget>
#include "ui_CheckInface.h"

class HSRail;

class CheckInface : public QWidget
{
	Q_OBJECT
public:
    CheckInface(QWidget* parent = Q_NULLPTR);
	~CheckInface();

private:
	Ui::CheckInface ui;
};
