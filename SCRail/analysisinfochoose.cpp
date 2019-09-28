#include "analysisinfochoose.h"
#include "ui_analysisinfochoose.h"

analysisInfoChoose::analysisInfoChoose(QVector<QString> info,QWidget *parent):
    QDialog(parent),
    ui(new Ui::analysisInfoChoose)
{
this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::Tool);
this->setAttribute(Qt::WA_TranslucentBackground);
ui->setupUi(this);
ui->frame->setStyleSheet("#frame{border-image: url(./../../SCRail/importData/ImageFolder/bgBox1.png);}");
    for (int i = 0; i < info.size(); i++)
    {
       _info.push_back(info.at(i).section('_',0,0));
	   _info1.push_back(info.at(i).section('_',1,1));
	   _info2.push_back(info.at(i).section('_',2,2));
	   _info3.push_back(info.at(i).section('_',3,3));
    }
	if (_info3.at(1) == NULL)
	{
		ui->comboBox1->addItems(_info);
		ui->comboBox2->addItems(_info1);
		ui->comboBox3->addItems(_info2);
		ui->comboBox4->setVisible(false);
		ui->label_5->setVisible(false);
		state = 1;
	}
	else
	{
		ui->comboBox1->addItems(_info);
		ui->comboBox2->addItems(_info1);
		ui->comboBox3->addItems(_info2);
		ui->comboBox4->addItems(_info3);
		state = 0;
	}
	
	ui->btn_sure->installEventFilter(this);
	ui->btn_cancel->installEventFilter(this);
    connect(ui->btn_sure,SIGNAL(clicked()),this,SLOT(on_sureful()));
    connect(ui->btn_cancel,SIGNAL(clicked()),this,SLOT(on_cancel()));
}

analysisInfoChoose::~analysisInfoChoose()
{
    delete ui;
}
void analysisInfoChoose::on_sureful()
{
    accept();
}
void analysisInfoChoose::on_cancel()
{
    reject();
}

QString analysisInfoChoose::inforeturn()
{
	if (state)
		return (ui->comboBox1->currentText() + QString("_") + ui->comboBox2->currentText() + QString("_") + ui->comboBox3->\
			currentText());
	else
		return (ui->comboBox1->currentText() + QString("_") + ui->comboBox2->currentText() + QString("_") + ui->comboBox3->\
			currentText() + QString("_") + ui->comboBox4->currentText());
}
bool analysisInfoChoose::eventFilter(QObject * watched, QEvent * event)
{
	if (watched == ui->btn_sure)
	{
		if (event->type() == QEvent::Enter)
		{
			ui->btn_sure->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j1.png);border:none;");
		}
		else if (event->type() == QEvent::Leave)
		{
			ui->btn_sure->setStyleSheet("color: white;background-color: transparent;border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j2.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			ui->btn_sure->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j1.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			ui->btn_sure->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j2.png);border:none;");
		}
	}
	else if (watched == ui->btn_cancel)
	{

		if (event->type() == QEvent::Enter)
		{
			ui->btn_cancel->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j1.png);border:none;");
		}
		else if (event->type() == QEvent::Leave)
		{
			ui->btn_cancel->setStyleSheet("color: white; background-color: transparent;border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j2.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			ui->btn_cancel->setStyleSheet("color: white; background-color: transparent;border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j1.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			ui->btn_cancel->setStyleSheet("color: white;background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/button/j2.png);border:none;");
		}
	}

	return QWidget::eventFilter(watched, event);
}