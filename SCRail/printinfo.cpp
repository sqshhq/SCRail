#include "printinfo.h"
#include "ui_printinfo.h"
#include<QDateTime>
#include<qDebug>
#include<QFileDialog>
#include<QMessageBox>
extern const char*table_info[3][8];
PrintInfo::PrintInfo(QWidget *parent, QString name,QString type, QString typeName, QString track,QString oversize, QString mileage, int oversizeState, QString halfWear, QString thirdsWear, QString horWear, QString totalWear,QString maxOffset,QString minOffset,QString GQI,QString *path,float *diff) :
	QDialog(parent), _lineName(name),
	_type(type),
	_typeName(typeName),
	_track(track),
	_oversize(oversize),
	_checkMileage(mileage),
	_oversizeState(oversizeState),
	_verticalHalfWear(halfWear),
	_verticalThirdsWear(thirdsWear),
	_horizontalWear(horWear),
	_GQI(GQI),
	_maxOffset(maxOffset),
	_minOffset(minOffset),
	_totalWear(totalWear),
    ui(new Ui::PrintInfo)
{
	QString _path[3] = { "\0" };
	for (int i = 0; i < 3; i++)
	{
		_path[i] = path[i];
	}
	for (int i = 0; i < 21; i++)
	{
		diffValue[i] = diff[i];
	}
	this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::Tool);
	this->setAttribute(Qt::WA_TranslucentBackground);
     ui->setupUi(this);
	 ui->frame->setStyleSheet("#frame{border-image: url(./../../SCRail/importData/ImageFolder/bgBox.png);border-radius: 8px ;}");
	 ui->Lab_lineName->setText(_lineName);
	 if (_type == QString::fromLocal8Bit("行别:"))
	 {
		 _type = QString::fromLocal8Bit("行别");
		 ui->lab_type->setText(QString::fromLocal8Bit(" 行    别:"));
	 }
	 else
	 {
		 _type = QString::fromLocal8Bit("车站");
		 ui->lab_type->setText(QString::fromLocal8Bit(" 车    站:"));
	 }
		
	 ui->Lab_typeName->setText(_typeName);
	 if (QString("") == _track)
		 ui->Lab_track->setText(QString::fromLocal8Bit("无"));
	 else
		 ui->Lab_track->setText(_track);
	 ui->Lab_oversize->setText(_oversize);
	 ui->Lab_mileage->setText(_checkMileage);
	 ui->Lab_verticalHalfWear->setText(_verticalHalfWear);
	 ui->Lab_verticalThirdsWear->setText(_verticalThirdsWear);
	 ui->Lab_horizontalWear->setText(_horizontalWear);
	 ui->lab_totalWear->setText(_totalWear);
	 ui->Lab_maxOffset->setText(_maxOffset);
	 ui->Lab_minOffset->setText(_minOffset);
	 ui->Lab_GQIValue->setText(_GQI);
	 if (!_oversizeState)
	 {
		 ui->label->setText(QString::fromLocal8Bit("左轨钢轨轨头廓型图"));
		 ui->label_2->setText(QString::fromLocal8Bit("左轨钢轨轨头磨耗分析比较图"));
		 ui->label_3->setText(QString::fromLocal8Bit("左轨实测廓形与目标廓形差值表"));
		 ui->label_4->setText(QString::fromLocal8Bit("左股偏差图"));
	 }
	 else
	 {
		 ui->label->setText(QString::fromLocal8Bit("右轨钢轨轨头廓型图"));
		 ui->label_2->setText(QString::fromLocal8Bit("右轨钢轨轨头磨耗分析比较图"));
		 ui->label_3->setText(QString::fromLocal8Bit("右轨实测廓形与目标廓形差值表"));
		 ui->label_4->setText(QString::fromLocal8Bit("右股偏差图"));
	 }
	 ui->Lab_image->setStyleSheet(QString("border-image: url(") + _path[0] + QString(")"));
	 ui->Lab_image2->setStyleSheet(QString("border-image: url(") + _path[1] + QString(")"));
	 ui->Lab_image3->setStyleSheet(QString("border-image: url(") + _path[2] + QString(")"));
	 set_table();

	 ui->btn_printInfo->installEventFilter(this);
	 ui->Btn_close->installEventFilter(this);
	 ui->btn_saveWith->installEventFilter(this);
	 connect(ui->Btn_close, SIGNAL(clicked()), this, SLOT(on_close()));
	 connect(ui->btn_printInfo, SIGNAL(clicked()), this, SLOT(on_print()));
	 connect(ui->btn_saveWith, SIGNAL(clicked()), this, SLOT(on_saveWith()));

}

PrintInfo::~PrintInfo()
{
    delete ui;
}
bool PrintInfo::eventFilter(QObject * watched, QEvent * event)
{
	if (watched == ui->Btn_close)
	{
		if (event->type() == QEvent::Enter)
		{
			ui->Btn_close->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j1.png);border:none;");
		}
		else if (event->type() == QEvent::Leave)
		{
			ui->Btn_close->setStyleSheet("color: white;background-color: transparent;border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j2.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			ui->Btn_close->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j1.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			ui->Btn_close->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j2.png);border:none;");
		}
	}
	else if (watched == ui->btn_printInfo)
	{

		if (event->type() == QEvent::Enter)
		{
			ui->btn_printInfo->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j1.png);border:none;");
		}
		else if (event->type() == QEvent::Leave)
		{
			ui->btn_printInfo->setStyleSheet("color: white; background-color: transparent;border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j2.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			ui->btn_printInfo->setStyleSheet("color: white; background-color: transparent;border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j1.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			ui->btn_printInfo->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j2.png);border:none;");
		}
	}
	else if (watched == ui->btn_saveWith)
	{

		if (event->type() == QEvent::Enter)
		{
			ui->btn_saveWith->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j1.png);border:none;");
		}
		else if (event->type() == QEvent::Leave)
		{
			ui->btn_saveWith->setStyleSheet("color: white; background-color: transparent;border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j2.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			ui->btn_saveWith->setStyleSheet("color: white; background-color: transparent;border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j1.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			ui->btn_saveWith->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j2.png);border:none;");
		}
	}

	return QWidget::eventFilter(watched, event);
}
void PrintInfo::on_print()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString curDate = currentTime.toString("yyyy-MM-dd-hh-mm-ss");
	QString path = "";
	if(_track == QString(""))
		path = QString("./../../SCRail/importData/printInfo/") + _lineName + QString::fromLocal8Bit("_")+ _type + QString::fromLocal8Bit("_") + _typeName + QString::fromLocal8Bit("_") + QString::number(0) + QString::fromLocal8Bit("_") + QString("K") + _checkMileage + QString("m") + QString::fromLocal8Bit("_") + curDate + QString(".png");
	else
		path = QString("./../../SCRail/importData/printInfo/") + _lineName + QString::fromLocal8Bit("_") + _type + QString::fromLocal8Bit("_") + _typeName + QString::fromLocal8Bit("_") + _track + QString::fromLocal8Bit("_") +QString("K") +_checkMileage + QString("m") + QString::fromLocal8Bit("_") + curDate + QString(".png");
	ui->printAxis->setStyleSheet("background-color:rgb(56,67,102);");
	QRect rect = ui->printAxis->geometry();
	QPixmap pix = this->grab(rect);
	//QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存图片"), path, "PNG (*.png);BMP (*.bmp);JPEG (*.jpg *.jpeg)");
	QMessageBox msg(NULL);
	msg.setWindowTitle("Warnning");
	msg.setText(QString::fromLocal8Bit("确定要保存该廓形信息吗？"));
	msg.setWindowIcon(QIcon(QPixmap("./../../SCRail/importData/ImageFolder/XT.ico")));
	msg.setIcon(QMessageBox::NoIcon);
	msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	if (msg.exec() == QMessageBox::Yes)
	{
		pix.save(path);
		ui->printAxis->setStyleSheet("background-color:transparent;");
	}
	else
	{
		ui->printAxis->setStyleSheet("background-color:transparent;");
	}
}
void PrintInfo::on_saveWith()
{
	
	QDateTime currentTime = QDateTime::currentDateTime();
	QString curDate = currentTime.toString("yyyy-MM-dd-hh-mm-ss");
	QString path = "";
	if(_track == QString(""))
		path = QString("./../../SCRail/importData/printInfo/") + _lineName + QString::fromLocal8Bit("_")+ _type + QString::fromLocal8Bit("_") + _typeName + QString::fromLocal8Bit("_") + QString::number(0) + QString::fromLocal8Bit("_") + QString("K") + _checkMileage + QString("m") + QString::fromLocal8Bit("_") + curDate + QString(".png");
	else
		path = QString("./../../SCRail/importData/printInfo/") + _lineName + QString::fromLocal8Bit("_") + _type + QString::fromLocal8Bit("_") + _typeName + QString::fromLocal8Bit("_") + _track + QString::fromLocal8Bit("_") +QString("K") +_checkMileage + QString("m") + QString::fromLocal8Bit("_") + curDate + QString(".png");
	ui->printAxis->setStyleSheet("background-color:rgb(56,67,102);");
	QRect rect = ui->printAxis->geometry();
	QPixmap pix = this->grab(rect);
	QString fileName = QFileDialog::getSaveFileName(this,
		QString::fromLocal8Bit("文件另存为"),
		path,
		tr("Config Files (*.png)"));
	pix.save(fileName);
	ui->printAxis->setStyleSheet("background-color:transparent;");

}
void PrintInfo::set_table()
{
	ui->tableWidget->setRowCount(6);
	ui->tableWidget->setColumnCount(8);
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (0 == i % 2)
			{
				ui->tableWidget->setItem(i, j, new QTableWidgetItem(QString::fromLocal8Bit(table_info[i / 2][j])));
				ui->tableWidget->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui->tableWidget->item(i, j)->setTextColor(QColor(255, 255, 255));
				
			}
			else
			{
				if (0 == j)
				{
					ui->tableWidget->setItem(i, j, new QTableWidgetItem(QString::fromLocal8Bit("差值")));
					ui->tableWidget->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui->tableWidget->item(i, j)->setTextColor(QColor(255, 255, 255));

				}
				else
				{
					QString num = "\0";
					if (0 == (diffValue[i / 2 * 7 + j - 1] - int(diffValue[i / 2 * 7 + j - 1])))
					{
						num = QString::number(diffValue[i / 2 * 7 + j - 1]) + QString(".00");
					}
					else
					{
						num = QString::number(diffValue[i / 2 * 7 + j - 1]);
					}
					ui->tableWidget->setItem(i, j, new QTableWidgetItem(num));
					ui->tableWidget->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					ui->tableWidget->item(i, j)->setTextColor(QColor(255, 255, 255));
				}

			}
		}
	}
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setStyleSheet("alternate-background-color:rgb(56,67,102);background: rgb(51, 51, 51);");
	ui->tableWidget->verticalHeader()->setVisible(false);
	ui->tableWidget->horizontalHeader()->setVisible(false);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//平均分配列宽
	ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);//平均分配行宽
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
void PrintInfo::on_close()
{
	close();
}
