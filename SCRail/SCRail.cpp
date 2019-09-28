#include "SCRail.h"
#include<QFile>
#include<qDebug>
#include<QMessageBox>
#include<QDateTime>
#include <QProcess>
#include<cmath>
#include <QtPrintSupport/QPrinter>
using namespace std;
const char *conf_path[8] = { 
"./../../SCRail/standardValue/60.txt" ,
"./../../SCRail/standardValue/60N.txt",
"./../../SCRail/standardValue/60D.txt",
"./../../SCRail/standardValue/75.txt" ,
"./../../SCRail/standardValue/60P.txt",
"./../../SCRail/standardValue/60NP.txt",
"./../../SCRail/standardValue/60DP.txt",
"./../../SCRail/standardValue/75P.txt"};
const char*table_info[3][8] = { {"角度","-10°","-5°","-2°","-1°","0°","1°","2°"}
					,{"角度","3°","4°","5°","10°","15°","20°","25°"}
					,{"角度","30°","35°","40°","45°","50°","55°","60°"} };
SCRail::SCRail(QWidget *parent)
	: QMainWindow(parent), _temp(0),_version(0), _state(0),x(200),y(200),_grind(NULL),_quality(NULL),_offset(NULL),_path(NULL),_row(0),_diffValue(0)
{
	ui.setupUi(this);
	
	this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg1.png)}");
	
	QFile file("./../../SCRail/importData/TextFolder/SHCRail.qss");
	if (!file.open(QFile::ReadOnly))
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开参数(HSRail.qss)文件失败！"), QMessageBox::Yes);
		return;
	}
	qApp->setStyleSheet(QString(file.readAll()));
	file.close();
	init();
	//实时检测
	connect(ui.Tbn_checkRealTime, SIGNAL(clicked()), this, SLOT(setReal_check()));
	connect(ui.Tbn_returnMainHome, SIGNAL(clicked()), this, SLOT(return_mainFace()));
	connect(ui.tbn_checkRealtime, SIGNAL(clicked()), this, SLOT(setReal_check()));
	connect(ui.btn_startCheck, SIGNAL(clicked()), this, SLOT(start_check()));
	connect(ui.btn_abraAnalysis, SIGNAL(clicked()),this, SLOT(abrasion_analysis()));
	connect(ui.btn_grindAdvice, SIGNAL(clicked()), this, SLOT(grind_advice()));
	connect(ui.btn_qualityAnalysis, SIGNAL(clicked()), this, SLOT(quality_analysis()));
	connect(ui.btn_returnCheckInterface, SIGNAL(clicked()), this, SLOT(return_checkFace()));
	connect(ui.btn_abraAnalysis2, SIGNAL(clicked()), this, SLOT(interface_switch()));
	connect(ui.btn_grindAdvice2, SIGNAL(clicked()), this, SLOT(interface_switch2()));
	connect(ui.btn_qualityAnalysis2, SIGNAL(clicked()), this, SLOT(interface_switch3()));
	connect(ui.btn_connectState, SIGNAL(clicked()), this, SLOT(set_connectState()));
	connect(ui.btn_gatherState, SIGNAL(clicked()), this, SLOT(set_gatherState()));
	connect(ui.lineCBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(gatherStateChange(int)));
	connect(ui.nameType, SIGNAL(currentIndexChanged(int)), this, SLOT(gatherStateChange(int)));
	connect(ui.trackBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(gatherStateChange(int)));
	connect(ui.MileageBox_m_2, SIGNAL(currentIndexChanged(int)), this, SLOT(gatherStateChange(int)));
	connect(ui.MileageBox_sw_2, SIGNAL(currentIndexChanged(int)), this, SLOT(gatherStateChange(int)));
	connect(ui.MileageBox_wan_2, SIGNAL(currentIndexChanged(int)), this, SLOT(gatherStateChange(int)));
	connect(ui.MileageBox_s_2, SIGNAL(currentIndexChanged(int)), this, SLOT(gatherStateChange(int)));
	connect(ui.MileageBox_h_2, SIGNAL(currentIndexChanged(int)), this, SLOT(gatherStateChange(int)));
	connect(ui.MileageBox_ten_2, SIGNAL(currentIndexChanged(int)), this, SLOT(gatherStateChange(int)));
	connect(ui.MileageBox_one_2, SIGNAL(currentIndexChanged(int)), this, SLOT(gatherStateChange(int)));
	connect(groupMilNum, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
	connect(ui.rowCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(curRowSeparation(int)));
	connect(ui.btn_save, SIGNAL(clicked()), this, SLOT(set_save()));
	connect(ui.qCustomPlot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(slot_mouseRelease(QMouseEvent *)));
	connect(ui.qCustomPlot2, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(slot_mouseRelease2(QMouseEvent *)));
	connect(ui.qCustomPlot3, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(slot_mouseRelease3(QMouseEvent *)));
	connect(ui.qCustomPlot4, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(slot_mouseRelease4(QMouseEvent *)));
	connect(ui.btn_generate, SIGNAL(clicked()), this, SLOT(on_generate()));
	//connect(ui.qCustomPlot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(slot_mouseRelease(QMouseEvent *)));

	//状态指示
	connect(ui.close, SIGNAL(clicked()), this, SLOT(unclose()));
	connect(ui.mini, SIGNAL(clicked(bool)), this, SLOT(showMinimized()));
	connect(ui.osk, SIGNAL(clicked()), this, SLOT(oskShow()));

	//网络通信
	connect(groupDiffValue, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(on_grindIsVisble(QAbstractButton*)));
	connect(tcpClient, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(ReadError(QAbstractSocket::SocketError)));

	//分析报表
	connect(ui.Tbn_analysisList, SIGNAL(clicked()), this, SLOT(set_reportAnalysis()));
	connect(ui.tableWidget_2, SIGNAL(cellClicked(int ,int)), this, SLOT(on_cellValueChanged(int ,int )));
	connect(ui.tableWidget_2, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(on_cellValueDelete(int, int)));
	connect(ui.btn_choose, SIGNAL(clicked()), this, SLOT(choose_menu()));
	connect(ui.btn_startAnalysis, SIGNAL(clicked()), this, SLOT(on_startAnalysis()));
	connect(ui.tbn_statistical, SIGNAL(clicked()), this, SLOT(set_reportAnalysis()));
	connect(ui.tableWidget_2->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn(int)));
	connect(ui.tableWidget_4->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn2(int)));
	connect(ui.tableWidget_6->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn3(int)));
	connect(ui.tableWidget_8->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn4(int)));
	connect(ui.tableWidget_4, SIGNAL(cellClicked(int, int)), this, SLOT(on_choiceRowPlot(int, int)));
	connect(ui.tableWidget_6, SIGNAL(cellClicked(int, int)), this, SLOT(on_choiceRowPlotDiffPoint(int, int)));
	connect(ui.tableWidget_8, SIGNAL(cellClicked(int, int)), this, SLOT(on_choiceRowPlotDiffRail(int, int)));
	connect(ui.btn_generate1, SIGNAL(clicked()), this, SLOT(on_analysisGenerate()));
	connect(ui.btn_generate2, SIGNAL(clicked()), this, SLOT(on_analysisGenerateRailPoint()));
	connect(ui.btn_generate3, SIGNAL(clicked()), this, SLOT(on_analysisGenerateRailDiffPoint()));
	connect(ui.btn_generate4, SIGNAL(clicked()), this, SLOT(on_analysisGenerateDiffRail()));


	ui.mini->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/mini.png);");
	ui.close->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/close.png);");
	ui.osk->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/keyBroad.png);");
	ui.Tbn_checkRealTime->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/checkImage1.png);");
	ui.Tbn_analysisList->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/huifang1.png);");
	ui.Tbn_returnMainHome->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/returnMain.png);");
	updataTime();
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updataTime()));
	timer->start(1000);
	qRegisterMetaType<contral_j>("contral_j");
}

void SCRail::setReal_check()
{
	this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg2_1.png)}");
	ui.stackedWidget->setCurrentIndex(1);
	ui.funcInterface->setCurrentIndex(0);

	ui.Tbn_analysisList->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/huifang1.png);");
	ui.Tbn_checkRealTime->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/checkImage2.png);");
}

SCRail::~SCRail()
{
	delete checkIn;
	checkIn = NULL;
	delete railData;
	railData = NULL;
	delete metricalInfo;
	metricalInfo = NULL;
	delete groupMilNum;
	groupMilNum = NULL;
	delete groupDiffValue;
	groupDiffValue = NULL;
	delete checkdata;
	checkdata = NULL;
	delete groupAnalysisType;
	groupAnalysisType = NULL;
	ui.qCustomPlot->clearPlottables();
	ui.qCustomPlot2->clearPlottables();
	ui.qCustomPlot3->clearPlottables();
	ui.qCustomPlot4->clearPlottables();
	ui.qCustomPlot5->clearPlottables();
	ui.qCustomPlot6->clearPlottables();
	ui.qCustomPlot7->clearPlottables();
	ui.qCustomPlot8->clearPlottables();
	/*delete fermatSpiral2;
	fermatSpiral2 = NULL;
	for (int i = 0; i < 21; i++)
	{
		delete fermatSpiral[i];
		fermatSpiral[i] = NULL;
	}*/
}

void SCRail::start_check()
{
	
	this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg2.png)}");
	ui.btn_gatherState->setText(QString::fromLocal8Bit("采    集"));
	save_checkInfo();
	set_checkInfo();
	//set_plotNormal(checkIn->_targetOversizeIndex);//检测界面及磨耗分析
	//set_plot(ui.qCustomPlot3, checkIn->_targetOversizeIndex + 4);//打磨建议
	//set_plot(checkIn->_targetOversizeIndex + 4,ui.qCustomPlot4,ui.qCustomPlot5,checkdata);//质量分析
	//set_checkInterfaceTitle();
	compuComple = 1;
	if (0 == ui.funcInterface->currentIndex())
		ui.funcInterface->setCurrentIndex(1);
	//set_data();
	set_checkInterfaceTitle();
	strcpy_s(contral.wayname, ui.lineCBox->currentText().toStdString().c_str());
	if (ui.rowCBox->currentIndex() != 5)
		strcpy_s(contral.linename, ui.rowCBox->currentText().toStdString().c_str());
	else
		strcpy_s(contral.linename, ui.staCBox->currentText().toStdString().c_str());
	contral.railtype = ui.oversizeBox->currentIndex();
	contral.status = 1;
	writeData();
	Sleep(1);

	if (_objRaildata.dotcount == 0)
	{
		set_plotNormal(checkIn->_targetOversizeIndex);//检测界面及磨耗分析
		set_plot(ui.qCustomPlot3, checkIn->_targetOversizeIndex + 4);//打磨建议
		set_plot(checkIn->_targetOversizeIndex + 4, ui.qCustomPlot4, ui.qCustomPlot5, checkdata);//质量分析
		char temp[50] = "\0";
		sprintf(temp, "%.2f", 0.0);
		ui.Lab_verticalHalfWear->setText(QString("%1").arg(temp) + QString("mm"));
		
		sprintf(temp, "%.2f", 0.0);
		ui.Lab_horizontalWear->setText(QString("%1").arg(temp) + QString("mm"));
		sprintf(temp, "%.2f", 0.0);
		ui.Lab_verticalHalfWear2->setText(QString("%1").arg(temp) + QString("mm"));
		sprintf(temp, "%.2f", 0.0);
		ui.Lab_verticalThirdsWear->setText(QString("%1").arg(temp) + QString("mm"));
		sprintf(temp, "%.2f", 0.0);
		ui.Lab_horizontalWear2->setText(QString("%1").arg(temp) + QString("mm"));
		sprintf(temp, "%.2f", 0.0);
		ui.Lab_maxOffset->setText(QString("%1").arg(temp) + QString("mm"));
		ui.Lab_maxOffset2->setText(QString("%1").arg(temp) + QString("mm"));
		sprintf(temp, "%.2f", 0.0);
		ui.Lab_minOffset->setText(QString("%1").arg(temp) + QString("mm"));
		ui.Lab_minOffset2->setText(QString("%1").arg(temp) + QString("mm"));
		ui.Lab_GQIValue->setText(QString::number(0));	
		ui.Lab_GQIValue2->setText(QString::number(0));	
	}
}

void SCRail::abrasion_analysis()

{
	if (_objRaildata.dotcount == 0)
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("未采集到廓形信息"), QMessageBox::Yes);
		return;
	}
	this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg2.1.png)}");
	_temp = 1;
	set_Title(_temp);
	set_Info(ui.lineName_2,ui.typeTitle,ui.typeName,ui.trackName,ui.mileageName,ui.target_oversizeName);
	//set_plotNormal(checkIn->_targetOversizeIndex);
	ui.funcInterface->setCurrentIndex(2);
	if (0 != ui.infoAnalysis->currentIndex())
	{
		ui.infoAnalysis->setCurrentIndex(0);

	}
	ui.btn_abraAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n1.png);border:none;color:white;");
	ui.btn_grindAdvice2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
	ui.btn_qualityAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
}

void SCRail::grind_advice()
{
	if (_objRaildata.dotcount == 0)
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("未采集到廓形信息"), QMessageBox::Yes);
		return;
	}
	this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg2.1.png)}");
	_temp = 2;
	set_Title(_temp);
	//set_plot(ui.qCustomPlot3, checkIn->_targetOversizeIndex + 4);
	if(2 != ui.funcInterface->currentIndex())
		ui.funcInterface->setCurrentIndex(2);
	if (1 != ui.infoAnalysis->currentIndex())
	{
		ui.infoAnalysis->setCurrentIndex(1);
	}
	set_Info(ui.lineName_3, ui.typeTitle_2, ui.typeName_2, ui.trackName_2, ui.mileageName_2, ui.target_oversizeName_2);
	ui.btn_grindAdvice2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n1.png);border:none;color:white;");
	ui.btn_abraAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
	ui.btn_qualityAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
	
}

void SCRail::quality_analysis()
{
	if (_objRaildata.dotcount == 0)
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("未采集到廓形信息"), QMessageBox::Yes);
		return;
	}
	this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg2.1.png)}");
	_temp = 3;
	set_Title(_temp);
	set_plot(checkIn->_targetOversizeIndex + 4, ui.qCustomPlot4, ui.qCustomPlot5,checkdata);
	if (2 != ui.funcInterface->currentIndex())
		ui.funcInterface->setCurrentIndex(2);

	if (2 != ui.infoAnalysis->currentIndex())
	{

		ui.infoAnalysis->setCurrentIndex(2);

	}
	set_Info(ui.lineName_4, ui.typeTitle_3, ui.typeName_3, ui.trackName_3, ui.mileageName_3, ui.target_oversizeName_3);
	ui.btn_abraAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
	ui.btn_grindAdvice2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
	ui.btn_qualityAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n1.png);border:none;color:white;");
}
void SCRail::return_checkFace()
{
	this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg2.png)}");
	ui.funcInterface->setCurrentIndex(1);
	
}


void SCRail::set_stateStyle(int state)
{
	if (state)
	{
		ui.stateLight->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lighting.png);");
		ui.stateLight_2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lighting.png);");
		ui.stateLight_3->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lighting.png);");
		return;
	}
	ui.stateLight->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lightOut.png);");
	ui.stateLight_2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lightOut.png);");
	ui.stateLight_3->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lightOut.png);");
		
	
}
bool SCRail::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == ui.mini)
	{
		if (event->type() == QEvent::Enter)
		{
			ui.mini->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/mini2.png);border:none;");
		}
		else if (event->type() == QEvent::Leave)
		{
			ui.mini->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/mini.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			ui.mini->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/mini2.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			ui.mini->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/mini.png);border:none;");
		}
	}
	else if (watched == ui.close)
	{
		if (event->type() == QEvent::Enter)
		{
			ui.close->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/close2.png);border:none;");
		}
		else if (event->type() == QEvent::Leave)
		{
			ui.close->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/close.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			ui.close->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/close2.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			ui.close->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/close.png);border:none;");
		}
	}
	else if (watched == ui.osk)
	{
		if (event->type() == QEvent::Enter)
		{
			ui.osk->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/keyBroad2.png);border:none;");
		}
		else if (event->type() == QEvent::Leave)
		{
			ui.osk->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/keyBroad.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			ui.osk->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/keyBroad2.png);border:none;");
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			ui.osk->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/keyBroad.png);border:none;");
		}
	}
	else if(watched == ui.btn_startCheck|| watched == ui.btn_qualityAnalysis || watched == ui.btn_grindAdvice || watched == ui.btn_abraAnalysis || watched == ui.btn_returnCheckInterface || \
		watched == ui.btn_abraAnalysis2 || watched == ui.btn_grindAdvice2 || watched == ui.btn_qualityAnalysis2 || watched == ui.btn_generate || watched == ui.btn_startAnalysis || watched == ui.btn_choose\
		|| watched == ui.btn_generate1 || watched == ui.btn_generate2 || watched == ui.btn_generate3 || watched == ui.btn_generate4)
	{
		QPushButton *watched1 = qobject_cast<QPushButton*>(watched);
		if ((watched1 == ui.btn_abraAnalysis2 && 1 == _temp) || (watched1 == ui.btn_grindAdvice2 && 2 == _temp) || (watched1 == ui.btn_qualityAnalysis2 && 3 == _temp))
			return 0;

		if (event->type() == QEvent::Enter)
		{
			if (watched == ui.btn_generate || watched == ui.btn_startAnalysis || watched == ui.btn_generate1|| watched == ui.btn_generate2|| watched == ui.btn_generate3 || watched == ui.btn_generate4)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/connect.png);border:none;color:white;");
				return 0;
			}
			else if(watched == ui.btn_choose)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/chooseOn.png);");
				return 0;
			}
			else if (watched == ui.btn_returnCheckInterface)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/returnCheck2.png);border:none;color:white;");
				return 0;
			}
			watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n1.png);border:none;color:white;");
		}
		else if (event->type() == QEvent::Leave)
		{
			if (watched == ui.btn_generate || watched == ui.btn_startAnalysis || watched == ui.btn_generate1 || watched == ui.btn_generate2 || watched == ui.btn_generate3 || watched == ui.btn_generate4)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/disconnect.png);border:none;color:white;");
				return 0;
			}
			else if (watched == ui.btn_choose)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/chooseOut.png);");
				return 0;
			}
			else if (watched == ui.btn_returnCheckInterface)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/returnCheck.png);border:none;color:white;");
				return 0;
			}
			watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			if (watched == ui.btn_generate || watched == ui.btn_startAnalysis || watched == ui.btn_generate1 || watched == ui.btn_generate2 || watched == ui.btn_generate3 || watched == ui.btn_generate4)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/connect.png);border:none;color:white;");
				return 0;
			}
			else if (watched == ui.btn_choose)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/chooseOn.png);");
				return 0;
			}
			else if (watched == ui.btn_returnCheckInterface)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/returnCheck2.png);border:none;color:white;");
				return 0;
			}
			watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n1.png);border:none;color:white;");
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			if (watched == ui.btn_generate || watched == ui.btn_startAnalysis || watched == ui.btn_generate1 || watched == ui.btn_generate2 || watched == ui.btn_generate3 || watched == ui.btn_generate4)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/disconnect.png);border:none;color:white;");
				return 0;
			}
			else if (watched == ui.btn_choose)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/chooseOut.png);");
				return 0;
			}
			else if (watched == ui.btn_returnCheckInterface)
			{
				watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/returnCheck.png);border:none;color:white;");
				return 0;
			}
			watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
		}
	}
	else if ( watched == ui.btn_connectState || watched == ui.btn_gatherState || watched == ui.btn_save)
	{
		QPushButton *watched1 = qobject_cast<QPushButton*>(watched);
		if (event->type() == QEvent::Enter)
		{
			watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/connect.png);border:none;color:white;");
		}
		else if (event->type() == QEvent::Leave)
		{
			watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/disconnect.png);border:none;color:white;");
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/connect.png);border:none;color:white;");
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			watched1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/disconnect.png);border:none;color:white;");
		}
	}
	else if (watched == ui.Lab_state1 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[0])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<进曲线>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d1.png);");
			_oversizeStateIndex[0] = 0;
			checkdata->railState -= pow(2, 0);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<进曲线>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/b1.png);");
		_oversizeStateIndex[0] = 1;
		checkdata->railState += pow(2, 0);
	}
	else if (watched == ui.Lab_state2 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[1])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<出曲线>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d2.png);");
			_oversizeStateIndex[1] = 0;
			checkdata->railState -= pow(2, 1);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<出曲线>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/b2.png);");
		_oversizeStateIndex[1] = 1;
		checkdata->railState += pow(2, 1);
	}
	else if (watched == ui.Lab_state3 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[2])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<进隧道>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state3->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d3.png);");
			_oversizeStateIndex[2] = 0;
			checkdata->railState -= pow(2, 2);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<进隧道>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state3->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/b3.png);");
		_oversizeStateIndex[2] = 1;
		checkdata->railState += pow(2, 2);
	}
	else if (watched == ui.Lab_state4 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[3])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<出隧道>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state4->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d4.png);");
			_oversizeStateIndex[3] = 0;
			checkdata->railState -= pow(2, 3);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<出隧道>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state4->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/b4.png);");
		_oversizeStateIndex[3] = 1;
		checkdata->railState += pow(2, 3);
	}
	else if (watched == ui.Lab_state5 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[4])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<上桥梁>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state5->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d5.png);");
			_oversizeStateIndex[4] = 0;
			checkdata->railState -= pow(2, 4);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<上桥梁>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state5->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/b5.png);");
		_oversizeStateIndex[4] = 1;
		checkdata->railState += pow(2, 4);
	}
	else if (watched == ui.Lab_state6 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[5])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<下桥梁>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state6->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d6.png);");
			_oversizeStateIndex[5] = 0;
			checkdata->railState -= pow(2, 5);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<下桥梁>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state6->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/b6.png);");
		_oversizeStateIndex[5] = 1;
		checkdata->railState += pow(2, 5);
	}
	else if (watched == ui.Lab_state7 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[6])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<进站信号机>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state7->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d7.png);");
			_oversizeStateIndex[6] = 0;
			checkdata->railState -= pow(2, 6);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<进站信号机>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state7->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/b7.png);");
		_oversizeStateIndex[6] = 1;
		checkdata->railState += pow(2, 6);
	}
	else if (watched == ui.Lab_state8 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[7])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<出站信号机>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state8->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d8.png);");
			_oversizeStateIndex[7] = 0;
			checkdata->railState -= pow(2, 7);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<出站信号机>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state8->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/b8.png);");
		_oversizeStateIndex[7] = 1;
		checkdata->railState += pow(2, 7);
	}
	else if (watched == ui.Lab_state9 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[8])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<焊接接头>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state9->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c1.png);");
			_oversizeStateIndex[8] = 0;
			checkdata->railState -= pow(2, 8);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<焊接接头>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state9->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/a1.png);");
		_oversizeStateIndex[8] = 1;
		checkdata->railState += pow(2, 8);
	}
	else if (watched == ui.Lab_state10 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[9])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<轨面严重擦伤>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state10->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c2.png);");
			_oversizeStateIndex[9] = 0;
			checkdata->railState -= pow(2, 9);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<轨面严重擦伤>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state10->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/a2.png);");
		_oversizeStateIndex[9] = 1;
		checkdata->railState += pow(2, 9);
	}
	else if (watched == ui.Lab_state11 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[10])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<轨面掉块>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state11->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c3.png);");
			_oversizeStateIndex[10] = 0;
			checkdata->railState -= pow(2, 10);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<轨面掉块>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state11->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/a3.png);");
		_oversizeStateIndex[10] = 1;
		checkdata->railState += pow(2, 10);
	}
	else if (watched == ui.Lab_state12 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[11])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<侧磨严重>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state12->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c4.png);");
			_oversizeStateIndex[11] = 0;
			checkdata->railState -= pow(2, 11);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<侧磨严重>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state12->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/a4.png);");
		_oversizeStateIndex[11] = 1;
		checkdata->railState += pow(2, 11);
	}
	else if (watched == ui.Lab_state13 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[12])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<下颚变形>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state13->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c5.png);");
			_oversizeStateIndex[12] = 0;
			checkdata->railState -= pow(2, 12);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<下颚变形>的状态吗？"));
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state13->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/a5.png);");
		_oversizeStateIndex[12] = 1;
		checkdata->railState += pow(2, 12);
	}
	else if (watched == ui.Lab_state14 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[13])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<道岔>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state14->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c6.png);");
			_oversizeStateIndex[13] = 0;
			checkdata->railState -= pow(2, 13);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<道岔>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state14->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/a6.png);");
		_oversizeStateIndex[13] = 1;
		checkdata->railState += pow(2, 13);
	}
	else if (watched == ui.Lab_state15 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[14])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<千米里程标>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state15->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c7.png);");
			_oversizeStateIndex[14] = 0;
			checkdata->railState -= pow(2, 14);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<千米里程标>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state15->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/a7.png);");
		_oversizeStateIndex[14] = 1;
		checkdata->railState += pow(2, 14);
	}
	else if (watched == ui.Lab_state16 && event->type() == QEvent::MouseButtonPress)
	{
		if (_oversizeStateIndex[15])
		{
			QMessageBox msg(this);
			msg.setWindowTitle("Warnning");
			msg.setText(QString::fromLocal8Bit("确定要删除<百米里程标>的状态吗？"));
			msg.setIcon(QMessageBox::NoIcon);
			msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
			msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
			if (msg.exec() == QMessageBox::No)
				return 0;
			ui.Lab_state1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c8.png);");
			_oversizeStateIndex[15] = 0;
			checkdata->railState -= pow(2, 15);
			return 0;
		}
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("确定要标记<百米里程标>的状态吗？"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		ui.Lab_state16->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/a8.png);");
		_oversizeStateIndex[15] = 1;
		checkdata->railState += pow(2, 15);
	}
	else if (watched == ui.stateLight && event->type() == QEvent::MouseButtonPress &&  1 == _state)
	{
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("即将关闭下位机，请保存好文件!!!"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		contral.shutdown = 1;
		writeData();
		contral.shutdown = 0;
	}
	else if (watched == ui.stateLight_2 && event->type() == QEvent::MouseButtonPress && 1 == _state)
	{
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("即将关闭下位机，请保存好文件!!!"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		contral.shutdown = 1;
		writeData();
		contral.shutdown = 0;
	}
	else if (watched == ui.stateLight_3 && event->type() == QEvent::MouseButtonPress && 1 == _state)
	{
		QMessageBox msg(this);
		msg.setWindowTitle("Warnning");
		msg.setText(QString::fromLocal8Bit("即将关闭下位机，请保存好文件!!!"));
		msg.setIcon(QMessageBox::NoIcon);
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
		msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
		if (msg.exec() == QMessageBox::No)
			return 0;
		contral.shutdown = 1;
		writeData();
		contral.shutdown = 0;
	}
	if ((watched == ui.qCustomPlot|| watched == ui.qCustomPlot2 || watched == ui.qCustomPlot3 || watched == ui.qCustomPlot4 || watched == ui.qCustomPlot5 || watched == ui.qCustomPlot6 ||\
		watched == ui.qCustomPlot7 || watched == ui.qCustomPlot8 || watched == ui.qCustomPlot9 || watched == ui.qCustomPlot10 || watched == ui.qCustomPlot11 || watched == ui.qCustomPlot12 || \
		watched == ui.qCustomPlot13 || watched == ui.qCustomPlot14 || watched == ui.qCustomPlot15 || watched == ui.qCustomPlot16 || watched == ui.qCustomPlot17 || watched == ui.qCustomPlot18 || \
		watched == ui.qCustomPlot19 || watched == ui.qCustomPlot20))
	{
		if (event->type() == QEvent::MouseButtonDblClick)
		{
			QCustomPlot *watched1 = qobject_cast<QCustomPlot*>(watched);
			watched1->rescaleAxes();
			watched1->replot();
		}
		else if (event->type() == QEvent::MouseButtonPress)
		{
			if (watched == ui.qCustomPlot)
				mouseLocal = 1;
			else if (watched == ui.qCustomPlot2)
				mouseLocal = 2;
			else if (watched == ui.qCustomPlot3)
				mouseLocal = 3;
			else if (watched == ui.qCustomPlot4)
				mouseLocal = 4;
			else if (watched == ui.qCustomPlot5)
				mouseLocal = 5;
			else if (watched == ui.qCustomPlot6)
				mouseLocal = 6;
			else if (watched == ui.qCustomPlot7)
				mouseLocal = 7;
			else if (watched == ui.qCustomPlot8)
				mouseLocal = 8;
			else if (watched == ui.qCustomPlot9)
				mouseLocal = 9;
			else if (watched == ui.qCustomPlot10)
				mouseLocal = 10;
			else if (watched == ui.qCustomPlot11)
				mouseLocal = 11;
			else if (watched == ui.qCustomPlot12)
				mouseLocal = 12;
			else if (watched == ui.qCustomPlot13)
				mouseLocal = 13;
			else if (watched == ui.qCustomPlot14)
				mouseLocal = 14;
			else if (watched == ui.qCustomPlot15)
				mouseLocal = 15;
			else if (watched == ui.qCustomPlot16)
				mouseLocal = 16;
			else if (watched == ui.qCustomPlot17)
				mouseLocal = 17;
			else if (watched == ui.qCustomPlot18)
				mouseLocal = 18;
			else if (watched == ui.qCustomPlot19)
				mouseLocal = 19;
			else if (watched == ui.qCustomPlot20)
				mouseLocal = 20;
		}
	}
	

	return QWidget::eventFilter(watched, event);
}
bool SCRail::event(QEvent * event)
{
	if (event->type() == QEvent::Gesture)
	{
		return gestureEvent(dynamic_cast<QGestureEvent*>(event));
	}
	return QWidget::event(event);
}
bool SCRail::gestureEvent(QGestureEvent * event)
{
	if (QGesture* pinch = event->gesture(Qt::PinchGesture))
		pinchTriggered(static_cast<QPinchGesture *>(pinch));
	return true;
}
void SCRail::pinchTriggered(QPinchGesture * pinch)
{
	int i = 0;
	QPinchGesture::ChangeFlags changeFlags = pinch->changeFlags();
	if (changeFlags & QPinchGesture::ScaleFactorChanged)
	{
		i++;
		if (i == 1)
		{
			scaleFactor = pinch->totalScaleFactor();

		}
	}
	if (pinch->state() == Qt::GestureFinished)
	{
		if (scaleFactor < 1)
		{
			for (int j = 0; j < 20; j++)
			{
				customPlotZoom(mouseLocal, 0);
			}
		}
		else
		{
			for (int j = 0; j < 20; j++)
			{
				customPlotZoom(mouseLocal, 1);
			}
		}
	}
}
void SCRail::customPlotZoom(short local, qreal zoomOffset)
{
	switch (mouseLocal)
	{
	case 1:
		zoomReplot(zoomOffset, ui.qCustomPlot);
		break;
	case 2:
		zoomReplot(zoomOffset, ui.qCustomPlot2);
		break;
	case 3:
		zoomReplot(zoomOffset, ui.qCustomPlot3);
		break;
	case 4:
		zoomReplot(zoomOffset, ui.qCustomPlot4);
		break;
	case 5:
		zoomReplot(zoomOffset, ui.qCustomPlot5);
		break;
	case 6:
		zoomReplot(zoomOffset, ui.qCustomPlot6);
		break;
	case 7:
		zoomReplot(zoomOffset, ui.qCustomPlot7);
		break;
	case 8:
		zoomReplot(zoomOffset, ui.qCustomPlot8);
		break;
	case 9:
		zoomReplot(zoomOffset, ui.qCustomPlot9);
		break;
	case 10:
		zoomReplot(zoomOffset, ui.qCustomPlot10);
		break;
	case 11:
		zoomReplot(zoomOffset, ui.qCustomPlot11);
		break;
	case 12:
		zoomReplot(zoomOffset, ui.qCustomPlot12);
		break;
	case 13:
		zoomReplot(zoomOffset, ui.qCustomPlot13);
		break;
	case 14:
		zoomReplot(zoomOffset, ui.qCustomPlot14);
		break;
	case 15:
		zoomReplot(zoomOffset, ui.qCustomPlot15);
		break;
	case 16:
		zoomReplot(zoomOffset, ui.qCustomPlot16);
		break;
	case 17:
		zoomReplot(zoomOffset, ui.qCustomPlot17);
		break;
	case 18:
		zoomReplot(zoomOffset, ui.qCustomPlot18);
		break;
	case 19:
		zoomReplot(zoomOffset, ui.qCustomPlot19);
		break;
	case 20:
		zoomReplot(zoomOffset, ui.qCustomPlot20);
		break;
	}

}
void SCRail::zoomReplot(qreal zoomOffset, QCustomPlot * plot)
{
	if (zoomOffset < 0.5)
	{
		plot->xAxis->setRange(plot->xAxis->range().lower*1.05, plot->xAxis->range().upper*1.05);
		plot->yAxis->setRange(plot->yAxis->range().lower*1.05, plot->yAxis->range().upper*1.05);
	}
	else
	{
		plot->xAxis->setRange(plot->xAxis->range().lower / 1.05, plot->xAxis->range().upper / 1.05);
		plot->yAxis->setRange(plot->yAxis->range().lower / 1.05, plot->yAxis->range().upper / 1.05);
	}
	plot->replot();
}
void SCRail::set_comBoxItem(const QString & path, QComboBox * combobox)
{
	int s = 0;
	QFile file(path);
	if (file.open(QIODevice::ReadOnly))
	{
		while (!file.atEnd())
		{
			QByteArray Byarr = file.readLine();
			QString str = QString::fromLocal8Bit(Byarr);
			str.remove("\n");
			str.remove("\r");
			combobox->insertItem(s, str);
			s++;
		}
		file.close();
	}

}

void SCRail::import_info()
{
	QString path = "./../../SCRail/importData/TextFolder/lineBox.txt";
	set_comBoxItem(path, ui.lineCBox);
	set_comBoxItem(path, ui.lineCBox_2);
	path.clear();
	path = "./../../SCRail/importData/TextFolder/rowBox.txt";
	set_comBoxItem(path, ui.rowCBox);
	path.clear();
	path = "./../../SCRail/importData/TextFolder/TrackBox.txt";
	set_comBoxItem(path, ui.staCBox);
	path.clear();
	path = "./../../SCRail/importData/TextFolder/vehicleBox.txt";
	set_comBoxItem(path, ui.trackBox);
	//set_comBoxItem(path, ui.trackBox_2);
	path.clear();
	path = "./../../SCRail/importData/TextFolder/oversizeBox.txt";
	set_comBoxItem(path, ui.oversizeBox);
	set_comBoxItem(path, ui.oversizeBox_2);
	path.clear();
	path = "./../../SCRail/importData/TextFolder/wearBox.txt";
	set_comBoxItem(path, ui.value_1);
	set_comBoxItem(path, ui.value_2);
	set_comBoxItem(path, ui.value_3);
	set_comBoxItem(path, ui.value_4);
	path.clear();
	path = "./../../SCRail/importData/TextFolder/GQIBox.txt";
	set_comBoxItem(path, ui.GQIBox);
	set_comBoxItem(path, ui.GQIBox_2);
	path.clear();
	path = "./../../SCRail/importData/TextFolder/maxdiffBox.txt";
	set_comBoxItem(path, ui.maxdiffBox);
	set_comBoxItem(path, ui.maxdiffBox_2);
	path.clear();
	path = "./../../SCRail/importData/TextFolder/mindiffBox.txt";
	set_comBoxItem(path, ui.mindiffBox);
	set_comBoxItem(path, ui.mindiffBox_2);

	ui.value_1->setCurrentIndex(5);
	ui.value_2->setCurrentIndex(5);
	ui.value_3->setCurrentIndex(5);
	ui.value_4->setCurrentIndex(5);
	ui.GQIBox->setCurrentIndex(3);
	ui.GQIBox_2->setCurrentIndex(3);
	ui.maxdiffBox->setCurrentIndex(3);
	ui.maxdiffBox_2->setCurrentIndex(3);
	ui.mindiffBox->setCurrentIndex(3);
	ui.mindiffBox_2->setCurrentIndex(3);
}

void SCRail::init()
{
	QPixmap pixlabImg("./../../SCRail/importData/ImageFolder/NameImg.png");
	ui.labImg->setPixmap(pixlabImg);
	this->setWindowIcon(QIcon(QPixmap("./../../SCRail/importData/ImageFolder/XT.ico")));
	this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	ui.TitleText->setText(QString::fromLocal8Bit("钢轨廓形检测分析仪"));
	WinExec("cmd /C \"del D:\\SHCRail\\SCRail\\SCRail\\importData\\GeneratedImage\\*.png\"", SW_HIDE);
	WinExec("cmd /C \"del .\\..\\..\\SCRail\\importData\\qCustomImage\\*.png\"", SW_HIDE);
	checkIn = new CheckInfo();
	metricalInfo = new metricalInfo_j();
	import_info();
	set_radioGroup();
	set_checkGroup();
	memset(_oversizeStateIndex, 0, sizeof(_oversizeStateIndex));
	railData = new raildata_j();
	_raildata = new railData_j();
	checkdata = new CheckData();
	tcpClient = new QTcpSocket(this);
	tcpClient->abort(); //取消原有连接
	//qDebug() << QNetworkInterface().allAddresses().at(1).toString() << endl;
	FILE *f = fopen("./../../SCRail/IPAddressPort/IP.txt", "r+");
	QString arg[2] = { "\0" };
	char buf[256] = "\0";
	int i = 0;
	int j = 0;
	fseek(f, 0, SEEK_SET);
	while (fgets(buf, 256, f) != NULL)
	{
		if (strlen(buf) < 2)
			break;
		arg[i] = QString("%1").arg(buf);
		i++;
	}
	fclose(f);
	arg[0].chop(1);
	IPAddress = arg[0];
	Iport = arg[1].toInt();
	qDebug() << IPAddress << ":" << Iport << endl;
	tcpClient->connectToHost(QHostAddress(IPAddress), Iport);
	{
		//const QString state_color = state_style + QString("green");
		if (tcpClient->waitForConnected(5000))  // 连接成功则进入if{}
		{
			_state = 1;
			ui.stateLight->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lighting.png);");
			ui.stateLight_2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lighting.png);");
			ui.stateLight_3->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lighting.png);");
			ui.btn_connectState->setText(QString::fromLocal8Bit("断    开"));
		}
		else
		{
			ui.stateLight->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lightOut.png);");
			ui.stateLight_2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lightOut.png);");
			ui.stateLight_3->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lightOut.png);");
			ui.btn_connectState->setText(QString::fromLocal8Bit("连    接"));
		}
		
	}
	ui.staCBox->setAttribute(Qt::WA_TransparentForMouseEvents);
	ui.staCBox->setStyleSheet("color:gray;");
	ui.trackBox->setAttribute(Qt::WA_TransparentForMouseEvents);
	ui.trackBox->setStyleSheet("color:gray;");

	//设置scrollArea无边框
	//ui.scrollArea->setFrameShape(QFrame::NoFrame);

	ui.mini->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/mini.png);");
	ui.close->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/close.png);");
	ui.osk->setStyleSheet("background-color: transparent; border-radius:5px;image:url(./../../SCRail/importData/ImageFolder/keyBroad.png);");
	ui.close->installEventFilter(this);
	ui.mini->installEventFilter(this);
	ui.osk->installEventFilter(this);
	ui.btn_generate->installEventFilter(this);
	ui.btn_generate1->installEventFilter(this);
	ui.btn_generate2->installEventFilter(this);
	ui.btn_generate3->installEventFilter(this);
	ui.btn_generate4->installEventFilter(this);
	ui.btn_abraAnalysis2->installEventFilter(this);
	ui.btn_grindAdvice2->installEventFilter(this);
	ui.btn_qualityAnalysis2->installEventFilter(this);
	ui.btn_returnCheckInterface->installEventFilter(this);
	ui.btn_abraAnalysis->installEventFilter(this);
	ui.btn_grindAdvice->installEventFilter(this);
	ui.btn_qualityAnalysis->installEventFilter(this);
	ui.btn_save->installEventFilter(this);
	ui.btn_gatherState->installEventFilter(this);
	
	ui.btn_connectState->installEventFilter(this);
	ui.btn_startCheck->installEventFilter(this);
	ui.Lab_state1->installEventFilter(this);
	ui.Lab_state2->installEventFilter(this);
	ui.Lab_state3->installEventFilter(this);
	ui.Lab_state4->installEventFilter(this);
	ui.Lab_state5->installEventFilter(this);
	ui.Lab_state6->installEventFilter(this);
	ui.Lab_state7->installEventFilter(this);
	ui.Lab_state8->installEventFilter(this);
	ui.Lab_state9->installEventFilter(this);
	ui.Lab_state10->installEventFilter(this);
	ui.Lab_state11->installEventFilter(this);
	ui.Lab_state12->installEventFilter(this);
	ui.Lab_state13->installEventFilter(this);
	ui.Lab_state14->installEventFilter(this);
	ui.Lab_state15->installEventFilter(this);
	ui.Lab_state16->installEventFilter(this); 
	ui.stateLight->installEventFilter(this);
	ui.stateLight_2->installEventFilter(this);
	ui.stateLight_3->installEventFilter(this);
	ui.Tbn_checkRealTime->installEventFilter(this);
	ui.Tbn_returnMainHome->installEventFilter(this);
	ui.btn_choose->installEventFilter(this);
	ui.btn_startAnalysis->installEventFilter(this);
	ui.qCustomPlot->installEventFilter(this);
	ui.qCustomPlot2->installEventFilter(this);
	ui.qCustomPlot3->installEventFilter(this);
	ui.qCustomPlot4->installEventFilter(this);
	ui.qCustomPlot5->installEventFilter(this);
	ui.qCustomPlot6->installEventFilter(this);
	ui.qCustomPlot7->installEventFilter(this);
	ui.qCustomPlot8->installEventFilter(this);
	ui.qCustomPlot9->installEventFilter(this);
	ui.qCustomPlot10->installEventFilter(this);
	ui.qCustomPlot11->installEventFilter(this);
	ui.qCustomPlot12->installEventFilter(this);
	ui.qCustomPlot13->installEventFilter(this);
	ui.qCustomPlot14->installEventFilter(this);
	ui.qCustomPlot15->installEventFilter(this);
	ui.qCustomPlot16->installEventFilter(this);
	ui.qCustomPlot17->installEventFilter(this);
	ui.qCustomPlot18->installEventFilter(this);
	ui.qCustomPlot19->installEventFilter(this);
	ui.qCustomPlot20->installEventFilter(this);
	//ui.frame_4->setStyleSheet("background-color:rgba(255, 0, 0,0);border-radius:15px;border:1px solid white");
	//ui.frame_5->setStyleSheet("background-color:rgba(255, 0, 0,0);border-radius:15px;border:1px solid white");
	//ui.scrollArea->setStyleSheet("background-color: rgba(255, 0, 0,0)");
	//绘图基本设置
	//设置图例可见与字体
	this->setFocusPolicy(Qt::ClickFocus);
	this->setMouseTracking(true);
	grabGesture(Qt::PanGesture);
	grabGesture(Qt::PinchGesture);
	grabGesture(Qt::SwipeGesture);
	ui.qCustomPlot->legend->setVisible(true);
	ui.qCustomPlot2->legend->setVisible(true);
	ui.qCustomPlot4->legend->setVisible(true);
	ui.qCustomPlot6->legend->setVisible(true);
	ui.qCustomPlot7->legend->setVisible(true);
	ui.qCustomPlot8->legend->setVisible(true);

	ui.qCustomPlot9->legend->setVisible(true);
	ui.qCustomPlot11->legend->setVisible(true);
	ui.qCustomPlot12->legend->setVisible(true);
	ui.qCustomPlot13->legend->setVisible(true);
	ui.qCustomPlot15->legend->setVisible(true);
	ui.qCustomPlot16->legend->setVisible(true);
	ui.qCustomPlot17->legend->setVisible(true);
	ui.qCustomPlot19->legend->setVisible(true);
	ui.qCustomPlot20->legend->setVisible(true);
	ui.qCustomPlot->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot2->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot4->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot6->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot7->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot8->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot9->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot11->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot12->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot13->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot15->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot16->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot17->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot19->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot20->legend->setFont(QFont(QString::fromLocal8Bit("黑体"), 9));
	ui.qCustomPlot->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot->legend->setTextColor(Qt::white);
	ui.qCustomPlot2->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot2->legend->setTextColor(Qt::white);
	ui.qCustomPlot4->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot4->legend->setTextColor(Qt::white);
	ui.qCustomPlot6->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot6->legend->setTextColor(Qt::white);
	ui.qCustomPlot7->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot7->legend->setTextColor(Qt::white);
	ui.qCustomPlot8->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot8->legend->setTextColor(Qt::white);
	ui.qCustomPlot9->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot9->legend->setTextColor(Qt::white);
	ui.qCustomPlot11->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot11->legend->setTextColor(Qt::white);
	ui.qCustomPlot12->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot12->legend->setTextColor(Qt::white);
	ui.qCustomPlot13->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot13->legend->setTextColor(Qt::white);
	ui.qCustomPlot15->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot15->legend->setTextColor(Qt::white);
	ui.qCustomPlot15->legend->setIconSize(40, 10);
	ui.qCustomPlot16->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot16->legend->setTextColor(Qt::white);
	ui.qCustomPlot17->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot17->legend->setTextColor(Qt::white);
	ui.qCustomPlot17->legend->setIconSize(40, 10);
	ui.qCustomPlot19->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot19->legend->setTextColor(Qt::white);
	ui.qCustomPlot19->legend->setIconSize(40, 10);
	ui.qCustomPlot20->legend->setBrush(QColor(255, 255, 255, 0));
	ui.qCustomPlot20->legend->setTextColor(Qt::white);
	//将图例矩形域放到左下角
	ui.qCustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot4->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot6->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot7->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot8->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot9->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot11->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot12->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot13->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot15->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot16->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot17->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot19->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
	ui.qCustomPlot20->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignLeft);
}

void SCRail::on_judgeIsOver(double real, double stand, QLabel * label)
{
	if (abs(real) > abs(stand))
		label->setStyleSheet("color:red;");
	else
		label->setStyleSheet("color: rgb(255, 255, 255);");
}

void SCRail::on_judgeIsOver(double real, double stand, QTableWidgetItem * item)
{
	if (abs(real) > abs(stand))
		item->setForeground(QBrush(QColor(255,0,0)));
	else
		item->setForeground(QBrush(QColor(255,255,255)));
}

void SCRail::set_radioGroup()
{
	groupMilNum = new QButtonGroup(this);
	groupMilNum->addButton(ui.btn_leftRail, 0);
	groupMilNum->addButton(ui.btn_rightRail, 1);
	ui.btn_leftRail->setChecked(true);
	groupMilNum->setId(ui.btn_leftRail, 0);//topTubePositionRadio的Id设为0
	groupMilNum->setId(ui.btn_rightRail, 1);

	groupAnalysisType = new QButtonGroup(this);
	groupAnalysisType->addButton(ui.btn_singlePonit, 0);
	groupAnalysisType->addButton(ui.btn_railWithPoint, 1);
	groupAnalysisType->addButton(ui.btn_railWithDiffPoint, 2);
	groupAnalysisType->addButton(ui.btn_DiffrailWithDiffPoint, 3);
	ui.btn_singlePonit->setChecked(true);
	groupAnalysisType->setId(ui.btn_singlePonit, 0);//topTubePositionRadio的Id设为0
	groupAnalysisType->setId(ui.btn_railWithPoint, 1); ui.btn_leftRail->setChecked(true);
	groupAnalysisType->setId(ui.btn_railWithDiffPoint, 2);//topTubePositionRadio的Id设为0
	groupAnalysisType->setId(ui.btn_DiffrailWithDiffPoint,3);
}

void SCRail::set_checkGroup()
{
	groupDiffValue = new QButtonGroup(this);
	groupDiffValue->setExclusive(false);
	groupDiffValue->addButton(ui.CB_grindTarget,0);
	groupDiffValue->addButton(ui.CB_grindReal,1);
	groupDiffValue->addButton(ui.CB_degreeMinus10,2);
	groupDiffValue->addButton(ui.CB_degreeMinus5,3);
	groupDiffValue->addButton(ui.CB_degreeMinus2,4);
	groupDiffValue->addButton(ui.CB_degreeMinus1,5);
	groupDiffValue->addButton(ui.CB_degree0,6);
	groupDiffValue->addButton(ui.CB_degree1,7);
	groupDiffValue->addButton(ui.CB_degree2,8);
	groupDiffValue->addButton(ui.CB_degree3,9);
	groupDiffValue->addButton(ui.CB_degree4,10);
	groupDiffValue->addButton(ui.CB_degree5,11);
	groupDiffValue->addButton(ui.CB_degree10,12);
	groupDiffValue->addButton(ui.CB_degree15,13);
	groupDiffValue->addButton(ui.CB_degree20,14);
	groupDiffValue->addButton(ui.CB_degree25,15);
	groupDiffValue->addButton(ui.CB_degree30,16);
	groupDiffValue->addButton(ui.CB_degree35,17);
	groupDiffValue->addButton(ui.CB_degree40,18);
	groupDiffValue->addButton(ui.CB_degree45,19);
	groupDiffValue->addButton(ui.CB_degree50,20);
	groupDiffValue->addButton(ui.CB_degree55,21);
	groupDiffValue->addButton(ui.CB_degree60,22);
	for (int i = 0; i < 23; i++)
	{
		groupDiffValue->button(i)->setChecked(true);
	}
}

void SCRail::save_checkInfo()
{
	checkIn->_lineName = ui.lineCBox->currentIndex();
	checkIn->_rowDiff = ui.rowCBox->currentIndex();
	checkIn->_station = ui.staCBox->currentIndex();
	checkIn->_track = ui.trackBox->currentIndex();
	checkIn->_mileage[0] = ui.MileageBox_m->currentIndex();
	checkIn->_mileage[1] = ui.MileageBox_sw->currentIndex();
	checkIn->_mileage[2] = ui.MileageBox_wan->currentIndex();
	checkIn->_mileage[3] = ui.MileageBox_s->currentIndex();
	checkIn->_mileage[4] = ui.MileageBox_h->currentIndex();
	checkIn->_mileage[5] = ui.MileageBox_ten->currentIndex();
	checkIn->_mileage[6] = ui.MileageBox_one->currentIndex();
	checkIn->_checkLocal = groupMilNum->checkedId();
	checkIn->_targetOversize = ui.oversizeBox->currentText();
	checkIn->_targetOversizeIndex = ui.oversizeBox->currentIndex();
	checkIn->_verticalWear = ui.value_1->currentIndex();
	checkIn->_sideWear = ui.value_2->currentIndex();
	checkIn->_GQIValue = ui.GQIBox->currentIndex();
	checkIn->_maxDiff = ui.maxdiffBox->currentIndex();
	checkIn->_minDiff = ui.mindiffBox->currentIndex();
}

void SCRail::set_checkInfo()
{
	ui.lineCBox_2->setCurrentIndex(checkIn->_lineName);
	QString path = "";
	if (5 == checkIn->_rowDiff)
	{
		ui.lab_typeName->setText(QString::fromLocal8Bit("车站:"));
		path.clear();
		ui.nameType->clear();
		path = "./../../SCRail/importData/TextFolder/TrackBox.txt";
		set_comBoxItem(path, ui.nameType);
		ui.nameType->setCurrentIndex(checkIn->_station);
		
		path.clear();
		path = "./../../SCRail/importData/TextFolder/vehicleBox.txt";
		ui.trackBox_2->clear();
		set_comBoxItem(path, ui.trackBox_2);
		ui.trackBox_2->setCurrentIndex(checkIn->_track);
		ui.trackBox_2->setAttribute(Qt::WA_TransparentForMouseEvents, false);
		ui.trackBox_2->setStyleSheet("color:white;");
	}	
	else
	{
		ui.lab_typeName->setText(QString::fromLocal8Bit("行别:"));
		path.clear();
		ui.nameType->clear();
		path = "./../../SCRail/importData/TextFolder/rowBox.txt";
		set_comBoxItem(path, ui.nameType);
		ui.nameType->setCurrentIndex(checkIn->_rowDiff);
		ui.trackBox_2->clear();
		ui.trackBox_2->setAttribute(Qt::WA_TransparentForMouseEvents);
		ui.trackBox_2->setStyleSheet("color:gray;");
		
	}
	ui.MileageBox_m_2->setCurrentIndex(checkIn->_mileage[0]);
	ui.MileageBox_sw_2->setCurrentIndex(checkIn->_mileage[1]);
	ui.MileageBox_wan_2->setCurrentIndex(checkIn->_mileage[2]);
	ui.MileageBox_s_2->setCurrentIndex(checkIn->_mileage[3]);
	ui.MileageBox_h_2->setCurrentIndex(checkIn->_mileage[4]);
	ui.MileageBox_ten_2->setCurrentIndex(checkIn->_mileage[5]);
	ui.MileageBox_one_2->setCurrentIndex(checkIn->_mileage[6]);
		
	
	
}

void SCRail::set_Info(QLabel * label1, QLabel * label2, QLabel * label3, QLabel * label4, QLabel * label5,QLabel *label6)
{
	label1->setText(ui.lineCBox_2->currentText());

	if(QString::fromLocal8Bit("行别:") == ui.lab_typeName->text())
	{
		label2->setText(QString::fromLocal8Bit("行       别:"));
	}
	else
	{
		label2->setText(QString::fromLocal8Bit("车       站:"));
	}
	
	label3->setText(ui.nameType->currentText());
	if(ui.trackBox_2->currentText() == NULL)
		label4->setText(QString::fromLocal8Bit("无"));
	else
		label4->setText(ui.trackBox_2->currentText());
	QString mileage_num = QString::number(ui.MileageBox_m_2->currentText().toDouble() * 1000 + ui.MileageBox_sw_2->currentText().toDouble() * 100 + ui.MileageBox_wan_2->currentText().toDouble() * 10\
		+ ui.MileageBox_s_2->currentText().toDouble()) + QString(" + ");
	label5->setText(mileage_num + QString::number(ui.MileageBox_h_2->currentText().toDouble()*100 + ui.MileageBox_ten_2->currentText().toDouble() * 10 + ui.MileageBox_one_2->currentText().toDouble()));
	label6->setText(checkIn->_targetOversize);
}

void SCRail::set_plotNormal(int index)
{
	
	if (ui.qCustomPlot->plottableCount())
		ui.qCustomPlot->clearPlottables();
	if (ui.qCustomPlot2->plottableCount())
		ui.qCustomPlot2->clearPlottables();
	fermatSpiralF = new QCPCurve(ui.qCustomPlot->xAxis, ui.qCustomPlot->yAxis);
	fermatSpiralF1 = new QCPCurve(ui.qCustomPlot2->xAxis, ui.qCustomPlot2->yAxis);
	fermatSpiral01 = new QCPCurve(ui.qCustomPlot2->xAxis, ui.qCustomPlot2->yAxis);
	

	//向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
	//ui.qCustomPlot->addGraph();
	//添加数据
	//ui.qCustomPlot->graph(0)->setData(x, y);
	//得到标准廓形的斜率 角度，点
	importData(x, y, conf_path[index], railData);
	auto xxmin = std::min_element(std::begin(x), std::end(x));
	auto yymin = y[std::distance(std::begin(x), xxmin)];
	double xmin = *xxmin;
	double slope_stanard = compute_slopeStandard(railData, xmin, yymin);
	

	//根据标准廓形的点，绘制实测廓形并平移
	x.clear();
	y.clear();
	railData_j *data = new railData_j();
	data = _raildata;
	int vNum = data->dotcount;
	checkdata->dotcount = vNum;
	double ymin = 100;
	double xac = 100.0;
	int doue = 0;
	int xdoue = 0;
	for (int i = 0; i < vNum; i++)
	{
		checkdata->datadot[i].data_x = data->dot[i].x;
		checkdata->datadot[i].data_y = data->dot[i].y;
		x.push_back(data->dot[i].x);
		y.push_back(data->dot[i].y);
		
	}

	for (int i = 0; i < x.size(); i++)
	{
		//qDebug() << railData->dot[i].x << endl;
		if (data->dot[i].x < data->dot[x.size() / 2].x)
		{
			ymin = data->dot[i].y < ymin ? data->dot[i].y : ymin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		//qDebug() << railData->dot[i].x << endl;
		if (data->dot[i].x < data->dot[x.size() / 2].x)
		{
			xac = data->dot[i].x < xac ? data->dot[i].x : xac;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (data->dot[i].x < data->dot[x.size() / 2].x)
		{
			if (ymin == data->dot[i].y)
			{
				doue = i;
			}
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (data->dot[i].x < data->dot[x.size() / 2].x)
		{
			if (xac == data->dot[i].x)
			{
				xdoue = i;
			}
		}
	}
	//qDebug() << data->dot[doue].x << "ads sajd sa" << ymin << endl;
	double slope_actual = compute_slopeStandard(data, data->dot[doue].x, ymin);
	double slope_actual1 = compute_slopeStandard(data, xac, data->dot[xdoue].y);
	auto xValue = xmin - data->dot[doue].x;
	auto yValue = yymin - ymin;
	for (auto iter = x.begin(); iter < x.end(); iter++)
	{
		*iter += xValue;
	}
	for (auto iter = y.begin(); iter < y.end(); iter++)
	{
		*iter += yValue;
	}
	for (int i = 0; i < vNum; i++)
	{
		data->dot[i].x = x[i];
		data->dot[i].y = y[i];
	}
	
	int Cant = getFileValue();
	ui.Lab_railCantValue->setText(QString("1:") + QString::number(int(slope_actual1 + Cant)));
	ui.Lab_railCantValue1->setText(QString("1:") + QString::number(int(slope_actual1 + Cant)));
	//ui.Lab_railCantValue2->setText(QString("1:") + QString::number(int(slope_actual + Cant)));
	ui.Lab_railCantValue3->setText(QString("1:") + QString::number(int(slope_actual1 + Cant)));
	fermatSpiralF->addData(x, y);
	fermatSpiralF1->addData(x, y);

	//确定旋转角度 绘制标准廓形
	//importData(x, y, conf_path[index], railData);
	int vNum2 = railData->dotcount;
	//qDebug() << slope_actual << ":::" << slope_stanard << endl;
	double angle = compute_angle(slope_actual, slope_stanard);
	double angle_rotate = 0.0;
	if (slope_actual *slope_stanard > 0)
	{
		if (slope_actual > slope_stanard)
		{
			angle_rotate = angle;
		}
			
		else
		{
			angle_rotate = -angle;
		}
			
	}
	else
	{
		if (slope_actual > slope_stanard)
		{
			angle_rotate = -angle;
		}
			
		else
		{
			angle_rotate = angle;
		}
			
	}
	//for (int i = 0; i < x.size(); i++)
	//{
		//railData->dot[i].x = x[i];
		//railData->dot[i].y = y[i];
	//}
	for (int i = 0; i <vNum2; i++)
	{
		railData->dot[i].x = (railData->dot[i].x - xmin)*cos(angle_rotate) - (railData->dot[i].y - yymin)*sin(angle_rotate) + xmin;
		railData->dot[i].y = (railData->dot[i].y - yymin)*cos(angle_rotate) + (railData->dot[i].x - xmin)*sin(angle_rotate) + yymin;

	}
	x.clear();
	y.clear();
	for (int i = 0; i < vNum2; i++)
	{
		x.push_back(railData->dot[i].x);
		y.push_back(railData->dot[i].y);
		//qDebug() << y[i] <<endl;
	}
	auto yymax = std::max_element(std::begin(y), std::end(y));
	fermatSpiral01->setData(x, y);
	//计算垂侧磨
	
	compute_verWear(railData, data, vNum2, vNum, *yymax);
	//qDebug() << metricalInfo->verticalHalf_wear << ".." << metricalInfo->verticalThirds_wear << ".." << metricalInfo->horizontal_wear << endl;
	//qDebug() << vNum2 << "&&" << vNum << "&&" << *yymax << endl;
	//设置坐标轴显示范围,否则我们只能看到默认的范围
	//设置坐标轴标签名称
	//ui.qCustomPlot->xAxis->setLabel("x");
	//ui.qCustomPlot->yAxis->setLabel("y");

	//设置坐标轴显示范围,否则我们只能看到默认的范围
	
	//ui.qCustomPlot->xAxis->setRange(20, 120);
	//ui.qCustomPlot->yAxis->setRange(30, 80);
	fermatSpiralF->rescaleAxes();
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::green);
	fermatSpiralF->setPen(pen);
	fermatSpiralF->setName(QString::fromLocal8Bit("实测廓形"));
	fermatSpiralF1->rescaleAxes();
	fermatSpiralF1->setPen(pen);
	fermatSpiralF1->setName(QString::fromLocal8Bit("实测廓形"));
	fermatSpiral01->rescaleAxes();
	pen.setColor(Qt::red);
	fermatSpiral01->setPen(pen);
	fermatSpiral01->setName(QString::fromLocal8Bit("标准廓形"));

	//fermatSpiral1->setVisible(0);
	
	//重绘，这里可以不用，官方例子有，执行setData函数后自动重绘
	//我认为应该用于动态显示或者是改变坐标轴范围之后的动态显示，我们以后探索

	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot->yAxis2 << ui.qCustomPlot->xAxis2 << ui.qCustomPlot->yAxis << ui.qCustomPlot->xAxis;
	ui.qCustomPlot->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot->setBackground(QBrush(Qt::black));
	ui.qCustomPlot->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot->yAxis->setTickLabelColor(Qt::white);

	//ui.qCustomPlot->graph(1)->setPen(QPen(Qt::red));
	ui.qCustomPlot->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot->replot();

	QList<QCPAxis*> axes1;
	axes1 << ui.qCustomPlot2->yAxis2 << ui.qCustomPlot2->xAxis2 << ui.qCustomPlot2->yAxis << ui.qCustomPlot2->xAxis;
	ui.qCustomPlot2->axisRect()->setRangeZoomAxes(axes1);
	//设置图形可缩放
	ui.qCustomPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot2->setBackground(QBrush(Qt::black));
	ui.qCustomPlot2->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot2->yAxis->setTickLabelColor(Qt::white);

	//ui.qCustomPlot->graph(1)->setPen(QPen(Qt::red));
	ui.qCustomPlot2->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot2->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot2->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot2->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot2->replot();
	fermatSpiralF = NULL;
	fermatSpiralF1 = NULL;
	fermatSpiral01 = NULL;

		
}

void SCRail::set_plotNormal(int index,QCustomPlot * plot,CheckData *check)
{
	if (plot->plottableCount())
		plot->clearPlottables();
	QCPCurve *fermatSpiral_N = new QCPCurve(plot->xAxis, plot->yAxis);
	QCPCurve *fermatSpiral_B = new QCPCurve(plot->xAxis, plot->yAxis);
	//向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
	//ui.qCustomPlot->addGraph();
	//添加数据
	//ui.qCustomPlot->graph(0)->setData(x, y);
	//得到标准廓形的斜率 角度，点
	importData(x, y, conf_path[index], railData);
	auto xxmin = std::min_element(std::begin(x), std::end(x));
	auto yymin = y[std::distance(std::begin(x), xxmin)];
	double xmin = *xxmin;
	double slope_stanard = compute_slopeStandard(railData, xmin, yymin);


	//根据标准廓形的点，绘制实测廓形并平移
	x.clear();
	y.clear();
	railData_j *data = new railData_j();
	//importData(x, y, analogData, data);
	int vNum = check->dotcount;
	double ymin = 100.0;
	int doue = 0;
	for (int i = 0; i < vNum; i++)
	{
		data->dot[i].x = check->datadot[i].data_x;
		data->dot[i].y = check->datadot[i].data_y;
		x.push_back(data->dot[i].x);
		y.push_back(data->dot[i].y);
	}
	for (int i = 0; i < x.size(); i++)
	{
		//qDebug() << railData->dot[i].x << endl;
		if (data->dot[i].x < data->dot[x.size() / 2].x)
		{
			ymin = data->dot[i].y < ymin ? data->dot[i].y : ymin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (data->dot[i].x < data->dot[x.size() / 2].x)
		{
			if (ymin == data->dot[i].y)
			{
				doue = i;
			}
		}
	}
	//qDebug() << *xxmin << "ads sajd sa" << endl;
	double slope_actual = compute_slopeStandard(data, data->dot[doue].x, ymin);
	auto xValue = xmin - data->dot[doue].x;
	auto yValue = yymin - ymin;
	for (auto iter = x.begin(); iter < x.end(); iter++)
	{
		*iter += xValue;
	}
	for (auto iter = y.begin(); iter < y.end(); iter++)
	{
		*iter += yValue;
	}
	for (int i = 0; i < vNum; i++)
	{
		data->dot[i].x = x[i];
		data->dot[i].y = y[i];
	}
	fermatSpiral_N->addData(x, y);

	//确定旋转角度 绘制标准廓形
	int vNum2 = railData->dotcount;
	double angle = compute_angle(slope_actual, slope_stanard);
	double angle_rotate = 0.0;
	if (slope_actual *slope_stanard > 0)
	{
		if (slope_actual > slope_stanard)
		{
			angle_rotate = angle;
		}

		else
		{
			angle_rotate = -angle;
		}

	}
	else
	{
		if (slope_actual > slope_stanard)
		{
			angle_rotate = -angle;
		}

		else
		{
			angle_rotate = angle;
		}

	}

	for (int i = 0; i < x.size(); i++)
	{
		railData->dot[i].x = (railData->dot[i].x - xmin)*cos(angle_rotate) - (railData->dot[i].y - yymin)*sin(angle_rotate) + xmin;
		railData->dot[i].y = (railData->dot[i].y - yymin)*cos(angle_rotate) + (railData->dot[i].x - xmin)*sin(angle_rotate) + yymin;

	}

	x.clear();
	y.clear();
	for (int i = 0; i < vNum2; i++)
	{
		x.push_back(railData->dot[i].x);
		y.push_back(railData->dot[i].y);
		//qDebug() << y[i] <<endl;
	}
	auto yymax = std::max_element(std::begin(y), std::end(y));
	fermatSpiral_B->setData(x, y);
	
	fermatSpiral_N->rescaleAxes();
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::green);
	fermatSpiral_N->setPen(pen);
	fermatSpiral_N->setName(QString::fromLocal8Bit("实测廓形"));
	fermatSpiral_B->rescaleAxes();
	pen.setColor(Qt::red);
	fermatSpiral_B->setPen(pen);
	fermatSpiral_B->setName(QString::fromLocal8Bit("标准廓形"));

	
	QList<QCPAxis*> axes1;
	axes1 << plot->yAxis2 << plot->xAxis2 << plot->yAxis << plot->xAxis;
	plot->axisRect()->setRangeZoomAxes(axes1);
	//设置图形可缩放
	plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	plot->setBackground(QBrush(Qt::black));
	plot->xAxis->setTickLabelColor(Qt::white);
	plot->yAxis->setTickLabelColor(Qt::white);

	//ui.qCustomPlot->graph(1)->setPen(QPen(Qt::red));
	plot->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->replot();
	fermatSpiral_B = NULL;
	fermatSpiral_N = NULL;


}

void SCRail::set_plot(QCustomPlot * plot, int index)
{
	int plottableCount = plot->plottableCount();
	if (plottableCount != 0)
		plot->clearPlottables();
	fermatSpiral1 = new QCPCurve(plot->xAxis, plot->yAxis);
	fermatSpiral2 = new QCPCurve(plot->xAxis, plot->yAxis);
	//QCPCurve *fermatSpiral3 = new QCPCurve(plot->xAxis, plot->yAxis);
	//向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
	//ui.qCustomPlot->addGraph();
	//添加数据
	//ui.qCustomPlot->graph(0)->setData(x, y);
	importData(x, y, conf_path[index], railData);
	int size = x.size();
	fermatSpiral1->addData(x, y);
	raildata_j *rail = new raildata_j();
	auto ymax = std::max_element(std::begin(y), std::end(y));
	double ymaxx = *ymax;
	for (int i = 0; i < x.size(); i++)
	{
		rail->dot[i].x = x[i];
		rail->dot[i].y = y[i];
	}
	int maxX = 0;
	double xmin = 100.0;
	for (int i = 0; i < x.size(); i++)
	{
		if (*ymax == rail->dot[i].y)
			maxX = i;
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x > rail->dot[maxX].x)
		{
			xmin = fabs(*ymax - rail->dot[i].y - 16) < xmin ? fabs(*ymax - rail->dot[i].y - 16) : xmin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x > rail->dot[maxX].x)
		{
			if (xmin == fabs(*ymax - rail->dot[i].y - 16))
			{
				xmin = rail->dot[i].x;
			}
		}
	}
	
	//设置坐标轴标签名称
	//ui.qCustomPlot->xAxis->setLabel("x");
	//ui.qCustomPlot->yAxis->setLabel("y");
	railData_j *rail2 = new railData_j(_objRaildata);
	int size2 = rail2->dotcount;
	x.clear();
	y.clear();
	for (int i = 0; i < size2; i++)
	{
		x.push_back(rail2->dot[i].x) ;
		y.push_back(rail2->dot[i].y) ;
		//qDebug() << y[i] <<endl;
	}
	auto yymax = std::max_element(std::begin(y), std::end(y));
	int maxXX = 0;
	double xxmin = 100.0;
	for (int i = 0; i < x.size(); i++)
	{
		if (*yymax == rail2->dot[i].y)
			maxXX = i;
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail2->dot[i].x > rail2->dot[maxXX].x)
		{
			xxmin = fabs(*yymax - rail2->dot[i].y - 16) < xxmin ? fabs(*yymax - rail2->dot[i].y - 16) : xxmin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail2->dot[i].x > rail2->dot[maxXX].x)
		{
			if (xxmin == fabs(*yymax - rail2->dot[i].y - 16))
			{
				xxmin = rail2->dot[i].x;
			}
		}
	}
	auto xValue = xmin -xxmin;
	auto yValue = ymaxx - *yymax;
	for (auto iter = x.begin(); iter < x.end(); iter++)
	{
		*iter += xValue;
	}
	for (auto iter = y.begin(); iter < y.end(); iter++)
	{
		*iter += yValue;
	}
	fermatSpiral2->addData(x, y);
	for (int i = 0; i < x.size(); i++)
	{
		rail2->dot[i].x = x[i];
		rail2->dot[i].y = y[i];
		//qDebug() << y[i] <<endl;
	}
	//设置坐标轴显示范围,否则我们只能看到默认的范围
	int angle[21] = { 10,5,2,1,0,-1,-2,-3,-4,-5,-10,-15,-20,-25,-30,-35,-40,-45,-50,-55,-60 };
	//设置坐标轴显示范围,否则我们只能看到默认的范围
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::yellow);
	for (int i = 0; i < 21; i++)
	{
		
		fermatSpiral[i] = new  QCPCurve(plot->xAxis, plot->yAxis);
		diffValue[i] = compute_ARE(railData, rail2, size, size2, angle[i], fermatSpiral[i]);
		fermatSpiral[i]->rescaleAxes();
		fermatSpiral[i]->setPen(pen);
	}
	set_tableView(ui.tableWidget,diffValue);
	//ui.qCustomPlot->xAxis->setRange(20, 120);
	//ui.qCustomPlot->yAxis->setRange(30, 80);
	pen.setColor(Qt::green);
	fermatSpiral2->rescaleAxes();
	fermatSpiral2->setPen(pen);
	fermatSpiral1->rescaleAxes();
	pen.setColor(Qt::red);
	fermatSpiral1->setPen(pen);

	//fermatSpiral1->setVisible(0);

	//重绘，这里可以不用，官方例子有，执行setData函数后自动重绘
	//我认为应该用于动态显示或者是改变坐标轴范围之后的动态显示，我们以后探索
	
	QList<QCPAxis*> axes;
	axes << plot->yAxis2 << plot->xAxis2 << plot->yAxis << plot->xAxis;
	plot->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	plot->setBackground(QBrush(Qt::black));
	plot->xAxis->setTickLabelColor(Qt::white);
	plot->yAxis->setTickLabelColor(Qt::white);

	//ui.qCustomPlot->graph(1)->setPen(QPen(Qt::red));
	plot->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->replot();
	_grind.clear();
	_grind.push_back(fermatSpiral1);
	_grind.push_back(fermatSpiral2);
	for (int i = 0; i < 21; i++)
	{
		_grind.push_back(fermatSpiral[i]);
	}
	delete rail;
	rail = NULL;
	delete rail2;
	rail2 = NULL;
	fermatSpiral1 = NULL;
	fermatSpiral2 = NULL;
	for (int i = 0; i < 21; i++)
	{

		fermatSpiral[i] = NULL;
	}
}

void SCRail::set_plot(int index, CheckStatus * state, CheckData * data)
{
	
	int plottableCount = ui.qCustomPlot12->plottableCount();
	if (plottableCount != 0)
		ui.qCustomPlot12->clearPlottables();
	QCPCurve *fermatSpiralS = new QCPCurve(ui.qCustomPlot12->xAxis, ui.qCustomPlot12->yAxis);
	QCPCurve *fermatSpiralB = new QCPCurve(ui.qCustomPlot12->xAxis, ui.qCustomPlot12->yAxis);
	//QCPCurve *fermatSpiral3 = new QCPCurve(plot->xAxis, plot->yAxis);
	//向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
	//ui.qCustomPlot->addGraph();
	//添加数据
	//ui.qCustomPlot->graph(0)->setData(x, y);
	importData(x, y, conf_path[index], railData);
	int size = x.size();
	fermatSpiralB->addData(x, y);
	raildata_j *rail = new raildata_j();
	auto ymax = std::max_element(std::begin(y), std::end(y));
	double ymaxx = *ymax;
	for (int i = 0; i < x.size(); i++)
	{
		rail->dot[i].x = x[i];
		rail->dot[i].y = y[i];
	}
	int maxX = 0;
	double xmin = 100.0;
	for (int i = 0; i < x.size(); i++)
	{
		if (*ymax == rail->dot[i].y)
			maxX = i;
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x > rail->dot[maxX].x)
		{
			xmin = fabs(*ymax - rail->dot[i].y - 16) < xmin ? fabs(*ymax - rail->dot[i].y - 16) : xmin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x > rail->dot[maxX].x)
		{
			if (xmin == fabs(*ymax - rail->dot[i].y - 16))
			{
				xmin = rail->dot[i].x;
			}
		}
	}

	//设置坐标轴标签名称
	//ui.qCustomPlot->xAxis->setLabel("x");
	//ui.qCustomPlot->yAxis->setLabel("y");
	railData_j *rail2 = new railData_j();
	int size2 = data->dotcount;

	x.clear();
	y.clear();
	for (int i = 0; i < size2; i++)
	{
		x.push_back(data->datadot[i].data_x);
		y.push_back(data->datadot[i].data_y);
		rail2->dot[i].x = data->datadot[i].data_x;
		rail2->dot[i].y = data->datadot[i].data_y;
	}
	auto yymax = std::max_element(std::begin(y), std::end(y));
	int maxXX = 0;
	double xxmin = 100.0;
	for (int i = 0; i < x.size(); i++)
	{
		if (*yymax == rail2->dot[i].y)
			maxXX = i;
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail2->dot[i].x > rail2->dot[maxXX].x)
		{
			xxmin = fabs(*yymax - rail2->dot[i].y - 16) < xxmin ? fabs(*yymax - rail2->dot[i].y - 16) : xxmin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail2->dot[i].x > rail2->dot[maxXX].x)
		{
			if (xxmin == fabs(*yymax - rail2->dot[i].y - 16))
			{
				xxmin = rail2->dot[i].x;
			}
		}
	}
	auto xValue = xmin - xxmin;
	auto yValue = ymaxx - *yymax;
	for (auto iter = x.begin(); iter < x.end(); iter++)
	{
		*iter += xValue;
	}
	for (auto iter = y.begin(); iter < y.end(); iter++)
	{
		*iter += yValue;
	}
	fermatSpiralS->addData(x, y);
	set_tableView(ui.tableWidget_5, state->diffData);
	//ui.qCustomPlot->xAxis->setRange(20, 120);
	//ui.qCustomPlot->yAxis->setRange(30, 80);
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::green);
	fermatSpiralS->rescaleAxes();
	fermatSpiralS->setPen(pen);
	fermatSpiralS->setName(QString::fromLocal8Bit("实测廓形"));
	fermatSpiralB->rescaleAxes();
	pen.setColor(Qt::red);
	fermatSpiralB->setPen(pen);
	fermatSpiralB->setName(QString::fromLocal8Bit("标准廓形"));
	//fermatSpiral1->setVisible(0);

	//重绘，这里可以不用，官方例子有，执行setData函数后自动重绘
	//我认为应该用于动态显示或者是改变坐标轴范围之后的动态显示，我们以后探索
	
	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot12->yAxis2 << ui.qCustomPlot12->xAxis2 << ui.qCustomPlot12->yAxis << ui.qCustomPlot12->xAxis;
	ui.qCustomPlot12->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot12->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot12->setBackground(QBrush(Qt::black));
	ui.qCustomPlot12->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot12->yAxis->setTickLabelColor(Qt::white);

	//ui.qCustomui.qCustomPlot12->graph(1)->setPen(QPen(Qt::red));
	ui.qCustomPlot12->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot12->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot12->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot12->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot12->replot();
	delete rail;
	rail = NULL;
	delete rail2;
	rail2 = NULL;
	fermatSpiral1 = NULL;
	
}

void SCRail::set_plotDiffPoint(int index, CheckStatus * state, CheckData * data)
{
	int plottableCount = ui.qCustomPlot16->plottableCount();
	if (plottableCount != 0)
		ui.qCustomPlot16->clearPlottables();
	QCPCurve *fermatSpiralS = new QCPCurve(ui.qCustomPlot16->xAxis, ui.qCustomPlot16->yAxis);
	QCPCurve *fermatSpiralB = new QCPCurve(ui.qCustomPlot16->xAxis, ui.qCustomPlot16->yAxis);
	//QCPCurve *fermatSpiral3 = new QCPCurve(plot->xAxis, plot->yAxis);
	//向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
	//ui.qCustomPlot->addGraph();
	//添加数据
	//ui.qCustomPlot->graph(0)->setData(x, y);
	importData(x, y, conf_path[index], railData);
	int size = x.size();
	fermatSpiralB->addData(x, y);
	raildata_j *rail = new raildata_j();
	auto ymax = std::max_element(std::begin(y), std::end(y));
	double ymaxx = *ymax;
	for (int i = 0; i < x.size(); i++)
	{
		rail->dot[i].x = x[i];
		rail->dot[i].y = y[i];
	}
	int maxX = 0;
	double xmin = 100.0;
	for (int i = 0; i < x.size(); i++)
	{
		if (*ymax == rail->dot[i].y)
			maxX = i;
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x > rail->dot[maxX].x)
		{
			xmin = fabs(*ymax - rail->dot[i].y - 16) < xmin ? fabs(*ymax - rail->dot[i].y - 16) : xmin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x > rail->dot[maxX].x)
		{
			if (xmin == fabs(*ymax - rail->dot[i].y - 16))
			{
				xmin = rail->dot[i].x;
			}
		}
	}

	//设置坐标轴标签名称
	//ui.qCustomPlot->xAxis->setLabel("x");
	//ui.qCustomPlot->yAxis->setLabel("y");
	railData_j *rail2 = new railData_j();
	int size2 = data->dotcount;

	x.clear();
	y.clear();
	for (int i = 0; i < size2; i++)
	{
		x.push_back(data->datadot[i].data_x);
		y.push_back(data->datadot[i].data_y);
		rail2->dot[i].x = data->datadot[i].data_x;
		rail2->dot[i].y = data->datadot[i].data_y;
	}
	auto yymax = std::max_element(std::begin(y), std::end(y));
	int maxXX = 0;
	double xxmin = 100.0;
	for (int i = 0; i < x.size(); i++)
	{
		if (*yymax == rail2->dot[i].y)
			maxXX = i;
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail2->dot[i].x > rail2->dot[maxXX].x)
		{
			xxmin = fabs(*yymax - rail2->dot[i].y - 16) < xxmin ? fabs(*yymax - rail2->dot[i].y - 16) : xxmin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail2->dot[i].x > rail2->dot[maxXX].x)
		{
			if (xxmin == fabs(*yymax - rail2->dot[i].y - 16))
			{
				xxmin = rail2->dot[i].x;
			}
		}
	}
	auto xValue = xmin - xxmin;
	auto yValue = ymaxx - *yymax;
	for (auto iter = x.begin(); iter < x.end(); iter++)
	{
		*iter += xValue;
	}
	for (auto iter = y.begin(); iter < y.end(); iter++)
	{
		*iter += yValue;
	}
	fermatSpiralS->addData(x, y);
	set_tableView(ui.tableWidget_7, state->diffData);
	//ui.qCustomPlot->xAxis->setRange(20, 120);
	//ui.qCustomPlot->yAxis->setRange(30, 80);
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::green);
	fermatSpiralS->rescaleAxes();
	fermatSpiralS->setPen(pen);
	fermatSpiralB->rescaleAxes();
	pen.setColor(Qt::red);
	fermatSpiralB->setPen(pen);
	fermatSpiralS->setName(QString::fromLocal8Bit("实测廓形"));
	fermatSpiralB->setName(QString::fromLocal8Bit("标准廓形"));
	//fermatSpiral1->setVisible(0);

	//重绘，这里可以不用，官方例子有，执行setData函数后自动重绘
	//我认为应该用于动态显示或者是改变坐标轴范围之后的动态显示，我们以后探索
	
	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot16->yAxis2 << ui.qCustomPlot16->xAxis2 << ui.qCustomPlot16->yAxis << ui.qCustomPlot16->xAxis;
	ui.qCustomPlot16->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot16->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot16->setBackground(QBrush(Qt::black));
	ui.qCustomPlot16->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot16->yAxis->setTickLabelColor(Qt::white);

	//ui.qCustomPlot->graph(1)->setPen(QPen(Qt::red));
	ui.qCustomPlot16->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot16->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot16->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot16->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot16->replot();
	delete rail;
	rail = NULL;
	delete rail2;
	rail2 = NULL;
	fermatSpiral1 = NULL;
}

void SCRail::set_plotDiffRail(int index, CheckStatus * state, CheckData * data)
{
	int plottableCount = ui.qCustomPlot20->plottableCount();
	if (plottableCount != 0)
		ui.qCustomPlot20->clearPlottables();
	QCPCurve *fermatSpiralS = new QCPCurve(ui.qCustomPlot20->xAxis, ui.qCustomPlot20->yAxis);
	QCPCurve *fermatSpiralB = new QCPCurve(ui.qCustomPlot20->xAxis, ui.qCustomPlot20->yAxis);
	//QCPCurve *fermatSpiral3 = new QCPCurve(plot->xAxis, plot->yAxis);
	//向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
	//ui.qCustomPlot->addGraph();
	//添加数据
	//ui.qCustomPlot->graph(0)->setData(x, y);
	importData(x, y, conf_path[index], railData);
	int size = x.size();
	fermatSpiralB->addData(x, y);
	raildata_j *rail = new raildata_j();
	auto ymax = std::max_element(std::begin(y), std::end(y));
	double ymaxx = *ymax;
	for (int i = 0; i < x.size(); i++)
	{
		rail->dot[i].x = x[i];
		rail->dot[i].y = y[i];
	}
	int maxX = 0;
	double xmin = 100.0;
	for (int i = 0; i < x.size(); i++)
	{
		if (*ymax == rail->dot[i].y)
			maxX = i;
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x > rail->dot[maxX].x)
		{
			xmin = fabs(*ymax - rail->dot[i].y - 20) < xmin ? fabs(*ymax - rail->dot[i].y - 20) : xmin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x > rail->dot[maxX].x)
		{
			if (xmin == fabs(*ymax - rail->dot[i].y - 20))
			{
				xmin = rail->dot[i].x;
			}
		}
	}

	//设置坐标轴标签名称
	//ui.qCustomPlot->xAxis->setLabel("x");
	//ui.qCustomPlot->yAxis->setLabel("y");
	railData_j *rail2 = new railData_j();
	int size2 = data->dotcount;

	x.clear();
	y.clear();
	for (int i = 0; i < size2; i++)
	{
		x.push_back(data->datadot[i].data_x);
		y.push_back(data->datadot[i].data_y);
		rail2->dot[i].x = data->datadot[i].data_x;
		rail2->dot[i].y = data->datadot[i].data_y;
	}
	auto yymax = std::max_element(std::begin(y), std::end(y));
	int maxXX = 0;
	double xxmin = 100.0;
	for (int i = 0; i < x.size(); i++)
	{
		if (*yymax == rail2->dot[i].y)
			maxXX = i;
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail2->dot[i].x > rail2->dot[maxXX].x)
		{
			xxmin = fabs(*yymax - rail2->dot[i].y - 20) < xxmin ? fabs(*yymax - rail2->dot[i].y - 20) : xxmin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail2->dot[i].x > rail2->dot[maxXX].x)
		{
			if (xxmin == fabs(*yymax - rail2->dot[i].y - 20))
			{
				xxmin = rail2->dot[i].x;
			}
		}
	}
	auto xValue = xmin - xxmin;
	auto yValue = ymaxx - *yymax;
	for (auto iter = x.begin(); iter < x.end(); iter++)
	{
		*iter += xValue;
	}
	for (auto iter = y.begin(); iter < y.end(); iter++)
	{
		*iter += yValue;
	}
	fermatSpiralS->addData(x, y);
	set_tableView(ui.tableWidget_9, state->diffData);
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::green);
	fermatSpiralS->rescaleAxes();
	fermatSpiralS->setPen(pen);
	fermatSpiralB->rescaleAxes();
	pen.setColor(Qt::red);
	fermatSpiralB->setPen(pen);
	fermatSpiralS->setName(QString::fromLocal8Bit("实测廓形"));
	fermatSpiralB->setName(QString::fromLocal8Bit("标准廓形"));
	
	
	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot20->yAxis2 << ui.qCustomPlot20->xAxis2 << ui.qCustomPlot20->yAxis << ui.qCustomPlot20->xAxis;
	ui.qCustomPlot20->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot20->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot20->setBackground(QBrush(Qt::black));
	ui.qCustomPlot20->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot20->yAxis->setTickLabelColor(Qt::white);

	//ui.qCustomPlot->graph(1)->setPen(QPen(Qt::red));
	ui.qCustomPlot20->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot20->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot20->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot20->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot20->replot();
	delete rail;
	rail = NULL;
	delete rail2;
	rail2 = NULL;
	fermatSpiral1 = NULL;
}

void SCRail::set_plot(int index, QCustomPlot* plot, QCustomPlot* plot1,CheckData *check)
{
	if (plot->plottableCount() != 0)
		plot->clearPlottables();
	if (plot1->plottableCount() != 0)
		plot1->clearPlottables();
	fermatSpiral3 = new QCPCurve(plot->xAxis, plot->yAxis);
	fermatSpiral4 = new QCPCurve(plot->xAxis, plot->yAxis);
	fermatSpiral5 = new QCPCurve(plot1->xAxis, plot1->yAxis);
	fermatSpiral6 = new QCPCurve(plot1->xAxis, plot1->yAxis);
	fermatSpiral7 = new QCPCurve(plot1->xAxis, plot1->yAxis);
	//QCPCurve *fermatSpiral3 = new QCPCurve(plot->xAxis, plot->yAxis);
	//向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
	//ui.qCustomPlot->addGraph();
	//添加数据
	//ui.qCustomPlot->graph(0)->setData(x, y);
	raildata_j *rail = new raildata_j();
	importData(x, y, conf_path[index], rail);
	int size = x.size();
	fermatSpiral4->addData(x, y);
	
	auto ymax = std::max_element(std::begin(y), std::end(y));
	double ymaxx = *ymax;
	for (int i = 0; i < x.size(); i++)
	{
		rail->dot[i].x = x[i];
		rail->dot[i].y = y[i];
	}
	int maxX = 0;
	double xmin = 100.0;
	for (int i = 0; i < x.size(); i++)
	{
		if (*ymax == rail->dot[i].y)
			maxX = i;
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x > rail->dot[maxX].x)
		{
			xmin = fabs(*ymax - rail->dot[i].y - 16) < xmin ? fabs(*ymax - rail->dot[i].y - 16) : xmin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x > rail->dot[maxX].x)
		{
			if (xmin == fabs(*ymax - rail->dot[i].y - 16))
			{
				xmin = rail->dot[i].x;
			}
		}
	}

	//设置坐标轴标签名称
	//ui.qCustomPlot->xAxis->setLabel("x");
	//ui.qCustomPlot->yAxis->setLabel("y");
	railData_j *rail2 = new railData_j();
	int size2 = check->dotcount;
	x.clear();
	y.clear();
	
	for (int i = 0; i < size2; i++)
	{
		rail2->dot[i].x = check->datadot[i].data_x;
		rail2->dot[i].y = check->datadot[i].data_y;
		x.push_back(rail2->dot[i].x);
		y.push_back(rail2->dot[i].y);
		//qDebug() << y[i] <<endl;
	}
	auto yymax = std::max_element(std::begin(y), std::end(y));
	int maxXX = 0;
	double xxmin = 100.0;
	for (int i = 0; i < x.size(); i++)
	{
		if (*yymax == rail2->dot[i].y)
			maxXX = i;
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail2->dot[i].x > rail2->dot[maxXX].x)
		{
			xxmin = fabs(*yymax - rail2->dot[i].y - 16) < xxmin ? fabs(*yymax - rail2->dot[i].y - 16) : xxmin;
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		if (rail2->dot[i].x > rail2->dot[maxXX].x)
		{
			if (xxmin == fabs(*yymax - rail2->dot[i].y - 16))
			{
				xxmin = rail2->dot[i].x;
			}
		}
	}
	auto xValue = xmin - xxmin;
	auto yValue = ymaxx - *yymax;
	for (auto iter = x.begin(); iter < x.end(); iter++)
	{
		*iter += xValue;
	}
	for (auto iter = y.begin(); iter < y.end(); iter++)
	{
		*iter += yValue;
	}
	fermatSpiral3->addData(x, y);
	for (int i = 0; i < x.size(); i++)
	{
		rail2->dot[i].x = x[i];
		rail2->dot[i].y = y[i];
	}
	compute_offset(rail, rail2, size, size2,fermatSpiral5);
	QVector<double>localX(0), localY(0);
	localX.push_back(-25);
	localX.push_back(-25);
	localY.push_back(-5);
	localY.push_back(5);
	fermatSpiral6->addData(localX, localY);
	localX.clear();
	localY.clear();
	localX.push_back(32);
	localX.push_back(32);
	localY.push_back(-5);
	localY.push_back(5);
	fermatSpiral7->addData(localX, localY);
	//设置坐标轴显示范围,否则我们只能看到默认的范围
	//设置坐标轴显示范围,否则我们只能看到默认的范围
	//ui.qCustomPlot->xAxis->setRange(20, 120);
	//ui.qCustomPlot->yAxis->setRange(30, 80);
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::green);
	fermatSpiral3->rescaleAxes();
	fermatSpiral3->setName(QString::fromLocal8Bit("实测廓形"));
	fermatSpiral3->setPen(pen);
	fermatSpiral4->rescaleAxes();
	pen.setColor(Qt::red);
	fermatSpiral4->setName(QString::fromLocal8Bit("标准廓形"));
	fermatSpiral4->setPen(pen);
	fermatSpiral5->setPen(pen);
	fermatSpiral5->setName(QString::fromLocal8Bit("偏差"));
	pen.setColor(Qt::green);
	fermatSpiral6->setPen(pen);
	fermatSpiral6->setName(QString::fromLocal8Bit("-25mm"));
	fermatSpiral7->setPen(pen);
	fermatSpiral7->setName(QString::fromLocal8Bit("+32mm"));
	
	plot1->xAxis->setRange(-28, 34);
	plot1->yAxis->setRange(-1, 1.5);
	//fermatSpiral1->setVisible(0);

	//重绘，这里可以不用，官方例子有，执行setData函数后自动重绘
	//我认为应该用于动态显示或者是改变坐标轴范围之后的动态显示，我们以后探索
	
	QList<QCPAxis*> axes;
	axes << plot->yAxis2 << plot->xAxis2 << plot->yAxis << plot->xAxis;
	plot->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	plot->setBackground(QBrush(Qt::black));
	plot->xAxis->setTickLabelColor(Qt::white);
	plot->yAxis->setTickLabelColor(Qt::white);

	//ui.qCustomPlot->graph(1)->setPen(QPen(Qt::red));
	plot->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot1->replot();
	/*QList<QCPAxis*> axes1;
	axes1 << plot1->yAxis2 << plot1->xAxis2 << plot1->yAxis << plot1->xAxis;
	plot1->axisRect()->setRangeZoomAxes(axes1);*/
	//设置图形可缩放
	plot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	plot1->setBackground(QBrush(Qt::black));
	plot1->xAxis->setTickLabelColor(Qt::white);
	plot1->yAxis->setTickLabelColor(Qt::white);

	//ui.qCustomPlot->graph(1)->setPen(QPen(Qt::red));
	plot1->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot1->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot1->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot1->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->replot();
	_quality.clear();
	_quality.push_front(fermatSpiral3);
	_quality.push_front(fermatSpiral4);
	_offset.clear();
	_offset.push_back(fermatSpiral5);
	_offset.push_back(fermatSpiral6);
	_offset.push_back(fermatSpiral7);
	delete rail;
	rail = NULL;
	delete rail2;
	rail2 = NULL;
	fermatSpiral3 = NULL;
	fermatSpiral4 = NULL;
	fermatSpiral5 = NULL;
	fermatSpiral6 = NULL;
	fermatSpiral7 = NULL;

}
void SCRail::set_plotAnalysis(QVector<QString>time, QVector<double> vertical, QVector<double> hori, QVector<double> max, QVector<double> min, QVector<double> GQI)
{
	if (ui.qCustomPlot9->plottableCount())
		ui.qCustomPlot9->clearPlottables();
	if (ui.qCustomPlot10->plottableCount())
		ui.qCustomPlot10->clearPlottables();
	QCPCurve* fermatSpiralFA = new QCPCurve(ui.qCustomPlot9->xAxis, ui.qCustomPlot9->yAxis);
	QCPCurve* fermatSpiralFB = new QCPCurve(ui.qCustomPlot9->xAxis, ui.qCustomPlot9->yAxis);
	QCPCurve* fermatSpiralFC = new QCPCurve(ui.qCustomPlot9->xAxis, ui.qCustomPlot9->yAxis);
	QCPCurve* fermatSpiralFD = new QCPCurve(ui.qCustomPlot9->xAxis, ui.qCustomPlot9->yAxis);
	QCPCurve* fermatSpiralFE = new QCPCurve(ui.qCustomPlot10->xAxis, ui.qCustomPlot10->yAxis);
	QVector<double>Ticks(0);
	for (int i = 1; i <= time.size(); i++)
	{
		Ticks.push_back(i);
	}
	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	fermatSpiralFC->setData(Ticks, max);
	fermatSpiralFD->setData(Ticks, min);
	fermatSpiralFA->setData(Ticks, vertical);
	fermatSpiralFB->setData(Ticks, hori);
	textTicker->addTicks(Ticks, time);
	ui.qCustomPlot9->xAxis->setTicker(textTicker);
	ui.qCustomPlot10->xAxis->setTicker(textTicker);
	ui.qCustomPlot9->xAxis->setTickLabelRotation(10);
	ui.qCustomPlot10->xAxis->setTickLabelRotation(10);
	fermatSpiralFE->setData(Ticks, GQI);
	fermatSpiralFA->setPen(QPen(Qt::green,2));
	fermatSpiralFB->setPen(QPen(Qt::red,2));
	fermatSpiralFC->setPen(QPen(Qt::yellow,2));
	fermatSpiralFD->setPen(QPen(Qt::cyan,2));
	fermatSpiralFE->setPen(QPen(Qt::green,2));
	QVector<double> sum(0);
	sum += vertical + hori + max + min;
	ui.qCustomPlot9->xAxis->setRange(1, time.size());
	ui.qCustomPlot9->yAxis->setRange(*std::min_element(std::begin(sum), std::end(sum)) - 1, *std::max_element(std::begin(sum), std::end(sum)) + 1);
	ui.qCustomPlot10->xAxis->setRange(1, time.size());
	ui.qCustomPlot10->yAxis->setRange(*std::min_element(std::begin(GQI), std::end(GQI)) - 1, *std::max_element(std::begin(GQI), std::end(GQI)) + 1);
	
	ui.qCustomPlot9->xAxis->setSubTicks(false);
	ui.qCustomPlot10->xAxis->setSubTicks(false);

	
	fermatSpiralFA->setName(QString::fromLocal8Bit("垂磨"));
	fermatSpiralFB->setName(QString::fromLocal8Bit("侧磨"));
	fermatSpiralFC->setName(QString::fromLocal8Bit("最大偏差值"));
	fermatSpiralFD->setName(QString::fromLocal8Bit("最小偏差值"));

	fermatSpiralFE->setName(QString::fromLocal8Bit("GQI值"));
	
	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot9->yAxis2 << ui.qCustomPlot9->xAxis2 << ui.qCustomPlot9->yAxis << ui.qCustomPlot9->xAxis;
	ui.qCustomPlot9->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot9->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot9->setBackground(QBrush(Qt::black));
	ui.qCustomPlot9->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot9->yAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot9->xAxis->setLabel(QString::fromLocal8Bit("检测时间(yy-mm-dd_hh-mm-ss)"));
	ui.qCustomPlot9->yAxis->setLabel(QString::fromLocal8Bit("数值(mm)"));
	ui.qCustomPlot9->xAxis->setLabelColor(Qt::white);
	ui.qCustomPlot9->yAxis->setLabelColor(Qt::white);

	ui.qCustomPlot9->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot9->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot9->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot9->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot9->replot();
	QList<QCPAxis*> axes1;
	axes1 << ui.qCustomPlot10->yAxis2 << ui.qCustomPlot10->xAxis2 << ui.qCustomPlot10->yAxis << ui.qCustomPlot10->xAxis;
	ui.qCustomPlot10->axisRect()->setRangeZoomAxes(axes1);
	//设置图形可缩放
	ui.qCustomPlot10->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot10->setBackground(QBrush(Qt::black));
	ui.qCustomPlot10->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot10->yAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot10->xAxis->setLabel(QString::fromLocal8Bit("检测时间(yy-mm-dd_hh-mm-ss)"));
	ui.qCustomPlot10->yAxis->setLabel(QString::fromLocal8Bit("GQI值(mm)"));
	ui.qCustomPlot10->xAxis->setLabelColor(Qt::white);
	ui.qCustomPlot10->yAxis->setLabelColor(Qt::white);

	ui.qCustomPlot10->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot10->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot10->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot10->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot10->replot();
	fermatSpiralFA = NULL;
	fermatSpiralFB = NULL;
	fermatSpiralFC = NULL;
	fermatSpiralFD = NULL;
	fermatSpiralFE = NULL;


}
void SCRail::set_plotAnalysis(QVector<QString> time, QVector<double> value, QCustomPlot * plot)
{
	if (plot->plottableCount())
		plot->clearPlottables();
	QCPCurve* fermatSpiralFA = new QCPCurve(plot->xAxis, plot->yAxis);
	QVector<double>Ticks(0);
	for (int i = 1; i <= time.size(); i++)
	{
		Ticks.push_back(i);
	}
	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(Ticks, time);
	fermatSpiralFA->setData(Ticks, value);
	
	plot->xAxis->setTicker(textTicker);

	fermatSpiralFA->setPen(QPen(Qt::green, 2));
	fermatSpiralFA->setName(QString::fromLocal8Bit("偏差值"));
	plot->xAxis->setTickLabelRotation(10);
	plot->xAxis->setRange(1, time.size());
	plot->yAxis->setRange(*std::min_element(std::begin(value), std::end(value)) - 1, *std::max_element(std::begin(value), std::end(value)) + 1);

	plot->xAxis->setSubTicks(false);
	
	QList<QCPAxis*> axes;
	axes << plot->yAxis2 << plot->xAxis2 << plot->yAxis << plot->xAxis;
	plot->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	plot->setBackground(QBrush(Qt::black));
	plot->xAxis->setTickLabelColor(Qt::white);
	plot->yAxis->setTickLabelColor(Qt::white);
	plot->xAxis->setLabel(QString::fromLocal8Bit("检测时间(yy-mm-dd_hh-mm-ss)"));
	plot->yAxis->setLabel(QString::fromLocal8Bit("偏差值(mm)"));
	plot->xAxis->setLabelColor(Qt::white);
	plot->yAxis->setLabelColor(Qt::white);

	plot->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->replot();
	fermatSpiralFA = NULL;
}
void SCRail::set_plotAnalysisDiffPoint(QVector<QString> mileage, QVector<double> value, QCustomPlot * plot)
{
	if (plot->plottableCount())
		plot->clearPlottables();
	QCPCurve* fermatSpiralFA = new QCPCurve(plot->xAxis, plot->yAxis);
	QVector<double>Ticks(0);
	for (int i = 1; i <= mileage.size(); i++)
	{
		Ticks.push_back(i);
	}
	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	
	textTicker->addTicks(Ticks, mileage);
	plot->xAxis->setTicker(textTicker);
	fermatSpiralFA->setData(Ticks, value);
	fermatSpiralFA->setPen(QPen(Qt::green, 2));
	fermatSpiralFA->setName(QString::fromLocal8Bit("偏差值"));
	plot->xAxis->setRange(1, mileage.size());
	plot->yAxis->setRange(*std::min_element(std::begin(value), std::end(value)) - 1, *std::max_element(std::begin(value), std::end(value)) + 1);

	plot->xAxis->setSubTicks(false);

	QList<QCPAxis*> axes;
	axes << plot->yAxis2 << plot->xAxis2 << plot->yAxis << plot->xAxis;
	plot->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	plot->setBackground(QBrush(Qt::black));
	plot->xAxis->setTickLabelColor(Qt::white);
	plot->yAxis->setTickLabelColor(Qt::white);
	plot->xAxis->setLabel(QString::fromLocal8Bit("检测里程(K+)"));
	plot->yAxis->setLabel(QString::fromLocal8Bit("偏差值(mm)"));
	plot->xAxis->setLabelColor(Qt::white);
	plot->yAxis->setLabelColor(Qt::white);

	plot->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->replot();
	fermatSpiralFA = NULL;
}
void SCRail::set_plotAnalysisDiffRail(QVector<double> value, QCustomPlot * plot)
{
	if (plot->plottableCount())
		plot->clearPlottables();
	QCPCurve* fermatSpiralFA = new QCPCurve(plot->xAxis, plot->yAxis);
	QVector<double>Ticks(0);
	QVector<QString>number(0);
	for (int i = 1; i <= value.size(); i++)
	{
		Ticks.push_back(i);
		number.push_back(QString::number(i));
	}
	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	
	textTicker->addTicks(Ticks, number);
	plot->xAxis->setTicker(textTicker);
	fermatSpiralFA->setData(Ticks, value);
	fermatSpiralFA->setPen(QPen(Qt::green, 2));
	fermatSpiralFA->setName(QString::fromLocal8Bit("偏差值"));
	plot->xAxis->setRange(1, value.size());
	plot->yAxis->setRange(*std::min_element(std::begin(value), std::end(value)) - 1, *std::max_element(std::begin(value), std::end(value)) + 1);


	plot->xAxis->setSubTicks(false);
	
	QList<QCPAxis*> axes;
	axes << plot->yAxis2 << plot->xAxis2 << plot->yAxis << plot->xAxis;
	plot->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	plot->setBackground(QBrush(Qt::black));
	plot->xAxis->setTickLabelColor(Qt::white);
	plot->yAxis->setTickLabelColor(Qt::white);
	plot->xAxis->setLabel(QString::fromLocal8Bit("检测里程(K+)"));
	plot->yAxis->setLabel(QString::fromLocal8Bit("偏差值(mm)"));
	plot->xAxis->setLabelColor(Qt::white);
	plot->yAxis->setLabelColor(Qt::white);

	plot->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	plot->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	plot->replot();
	fermatSpiralFA = NULL;
}
void SCRail::set_plotAnalysisDiffPoint(QVector<double> mile, QVector<double> vertical, QVector<double> hori, QVector<double> max, QVector<double> min, QVector<double> GQI)
{
	if (ui.qCustomPlot13->plottableCount())
		ui.qCustomPlot13->clearPlottables();
	if (ui.qCustomPlot14->plottableCount())
		ui.qCustomPlot14->clearPlottables();
	QCPCurve* fermatSpiralFA = new QCPCurve(ui.qCustomPlot13->xAxis, ui.qCustomPlot13->yAxis);
	QCPCurve* fermatSpiralFB = new QCPCurve(ui.qCustomPlot13->xAxis, ui.qCustomPlot13->yAxis);
	QCPCurve* fermatSpiralFC = new QCPCurve(ui.qCustomPlot13->xAxis, ui.qCustomPlot13->yAxis);
	QCPCurve* fermatSpiralFD = new QCPCurve(ui.qCustomPlot13->xAxis, ui.qCustomPlot13->yAxis);
	QCPCurve* fermatSpiralFE = new QCPCurve(ui.qCustomPlot14->xAxis, ui.qCustomPlot14->yAxis);
	QVector<double>Ticks(0);
	QVector<QString> _mileage(0);
	for (int i = 1; i <= mile.size(); i++)
	{
		Ticks.push_back(i);
		_mileage.push_back(QString("K") + QString::number(int(mile.at(i-1))) + "+" + QString::number((mile.at(i-1) - int(mile.at(i-1))) * 1000));
	}
	
	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(Ticks, _mileage);
	ui.qCustomPlot13->xAxis->setTicker(textTicker);
	ui.qCustomPlot14->xAxis->setTicker(textTicker);
	fermatSpiralFA->setData(Ticks, vertical);
	fermatSpiralFB->setData(Ticks, hori);
	fermatSpiralFC->setData(Ticks, max);
	fermatSpiralFD->setData(Ticks, min);
	fermatSpiralFE->setData(Ticks, GQI);
	fermatSpiralFA->setPen(QPen(Qt::green, 2));
	fermatSpiralFB->setPen(QPen(Qt::red, 2));
	fermatSpiralFC->setPen(QPen(Qt::yellow, 2));
	fermatSpiralFD->setPen(QPen(Qt::cyan, 2));
	fermatSpiralFE->setPen(QPen(Qt::green, 2));
	QVector<double> sum(0);
	sum += vertical + hori + max + min;
	ui.qCustomPlot13->xAxis->setRange(1, mile.size());
	ui.qCustomPlot13->yAxis->setRange(*std::min_element(std::begin(sum), std::end(sum)) - 1, *std::max_element(std::begin(sum), std::end(sum)) + 1);
	ui.qCustomPlot14->xAxis->setRange(1, mile.size());
	ui.qCustomPlot14->yAxis->setRange(*std::min_element(std::begin(GQI), std::end(GQI)) - 1, *std::max_element(std::begin(GQI), std::end(GQI)) + 1);
	//QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
	//ui.qCustomPlot13->xAxis->setTicker(dateTimeTicker);
	//ui.qCustomPlot14->xAxis->setTicker(dateTimeTicker);
	//dateTimeTicker->setDateTimeSpec(Qt::UTC);//设施世界时间，即不加上时区的时间
	//dateTimeTicker->setTickCount(14);
	//dateTimeTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
	//ui.qCustomPlot13->xAxis->setSubTicks(false);
	//ui.qCustomPlot14->xAxis->setSubTicks(false);
	//dateTimeTicker->setDateTimeFormat("yyyy-MM-dd_hh-mm-ss");
	//ui.qCustomPlot13->xAxis->setTickLabelRotation(14);
	//ui.qCustomPlot14->xAxis->setTickLabelRotation(14);

	fermatSpiralFA->setName(QString::fromLocal8Bit("垂磨"));
	fermatSpiralFB->setName(QString::fromLocal8Bit("侧磨"));
	fermatSpiralFC->setName(QString::fromLocal8Bit("最大偏差值"));
	fermatSpiralFD->setName(QString::fromLocal8Bit("最小偏差值"));
	ui.qCustomPlot13->replot();
	fermatSpiralFE->setName(QString::fromLocal8Bit("GQI值"));
	ui.qCustomPlot14->replot();
	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot13->yAxis2 << ui.qCustomPlot13->xAxis2 << ui.qCustomPlot13->yAxis << ui.qCustomPlot13->xAxis;
	ui.qCustomPlot13->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot13->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot13->setBackground(QBrush(Qt::black));
	ui.qCustomPlot13->xAxis->setLabel(QString::fromLocal8Bit("检测里程(km)"));
	ui.qCustomPlot13->yAxis->setLabel(QString::fromLocal8Bit("数值(mm)"));
	ui.qCustomPlot13->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot13->yAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot13->xAxis->setLabelColor(Qt::white);
	ui.qCustomPlot13->yAxis->setLabelColor(Qt::white);


	ui.qCustomPlot13->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot13->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot13->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot13->yAxis->setTickPen(QPen(QColor(255, 255, 255)));

	QList<QCPAxis*> axes1;
	axes1 << ui.qCustomPlot14->yAxis2 << ui.qCustomPlot14->xAxis2 << ui.qCustomPlot14->yAxis << ui.qCustomPlot14->xAxis;
	ui.qCustomPlot14->axisRect()->setRangeZoomAxes(axes1);
	//设置图形可缩放
	ui.qCustomPlot14->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot14->setBackground(QBrush(Qt::black));
	ui.qCustomPlot14->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot14->yAxis->setTickLabelColor(Qt::white);

	ui.qCustomPlot14->xAxis->setLabelColor(Qt::white);
	ui.qCustomPlot14->yAxis->setLabelColor(Qt::white);
	ui.qCustomPlot14->xAxis->setLabel(QString::fromLocal8Bit("检测里程(km)"));
	ui.qCustomPlot14->yAxis->setLabel(QString::fromLocal8Bit("GQI值"));

	ui.qCustomPlot14->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot14->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot14->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot14->yAxis->setTickPen(QPen(QColor(255, 255, 255)));


	fermatSpiralFA = NULL;
	fermatSpiralFB = NULL;
	fermatSpiralFC = NULL;
	fermatSpiralFD = NULL;
	fermatSpiralFE = NULL;
}
void SCRail::set_plotAnalysisDiffRail(QTableWidget * table)
{
	QVector<double> _verticalThirds(0), _horizontalWear(0), _GQIValue(0), _maxOffset(0), _minOffset(0);
	QVector<double>Ticks(0);
	QVector<QString>_number(0);
	int tNum = table->rowCount();
	for (int i = 1; i <= tNum; i++)
	{
		Ticks.push_back(i);
		_number.push_back(table->item(i-1, 0)->text());
	}

	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(Ticks, _number);
	ui.qCustomPlot17->xAxis->setTicker(textTicker);
	ui.qCustomPlot18->xAxis->setTicker(textTicker);
	for (int i = 0; i < tNum; i++)
	{
		
		_verticalThirds.push_back(table->item(i, 5)->text().toDouble());
		_horizontalWear.push_back(table->item(i, 6)->text().toDouble());
		_maxOffset.push_back(table->item(i, 7)->text().toDouble());
		_minOffset.push_back(table->item(i, 8)->text().toDouble());
		_GQIValue.push_back(table->item(i, 9)->text().toDouble());
	}
	if (ui.qCustomPlot17->plottableCount())
		ui.qCustomPlot17->clearPlottables();
	if (ui.qCustomPlot18->plottableCount())
		ui.qCustomPlot18->clearPlottables();
	QCPCurve* fermatSpiralFA = new QCPCurve(ui.qCustomPlot17->xAxis, ui.qCustomPlot17->yAxis);
	QCPCurve* fermatSpiralFB = new QCPCurve(ui.qCustomPlot17->xAxis, ui.qCustomPlot17->yAxis);
	QCPCurve* fermatSpiralFC = new QCPCurve(ui.qCustomPlot17->xAxis, ui.qCustomPlot17->yAxis);
	QCPCurve* fermatSpiralFD = new QCPCurve(ui.qCustomPlot17->xAxis, ui.qCustomPlot17->yAxis);
	QCPCurve* fermatSpiralFE = new QCPCurve(ui.qCustomPlot18->xAxis, ui.qCustomPlot18->yAxis);

	fermatSpiralFA->setData(Ticks, _verticalThirds);
	fermatSpiralFB->setData(Ticks, _horizontalWear);
	fermatSpiralFC->setData(Ticks, _maxOffset);
	fermatSpiralFD->setData(Ticks, _minOffset);
	fermatSpiralFE->setData(Ticks, _GQIValue);
	fermatSpiralFA->setPen(QPen(Qt::green, 2));
	fermatSpiralFB->setPen(QPen(Qt::red, 2));
	fermatSpiralFC->setPen(QPen(Qt::yellow, 2));
	fermatSpiralFD->setPen(QPen(Qt::cyan, 2));
	fermatSpiralFE->setPen(QPen(Qt::green, 2));
	QVector<double> sum(0);
	sum += _verticalThirds + _horizontalWear + _maxOffset + _minOffset;
	ui.qCustomPlot17->xAxis->setRange(1,tNum);
	ui.qCustomPlot17->yAxis->setRange(*std::min_element(std::begin(sum), std::end(sum)) - 1, *std::max_element(std::begin(sum), std::end(sum)) + 1);
	ui.qCustomPlot18->xAxis->setRange(1, tNum);
	ui.qCustomPlot18->yAxis->setRange(*std::min_element(std::begin(_GQIValue), std::end(_GQIValue))-1, *std::max_element(std::begin(_GQIValue), std::end(_GQIValue)) + 1);
	fermatSpiralFA->setName(QString::fromLocal8Bit("垂磨"));
	fermatSpiralFB->setName(QString::fromLocal8Bit("侧磨"));
	fermatSpiralFC->setName(QString::fromLocal8Bit("最大偏差值"));
	fermatSpiralFD->setName(QString::fromLocal8Bit("最小偏差值"));
	ui.qCustomPlot17->replot();
	fermatSpiralFE->setName(QString::fromLocal8Bit("GQI值"));
	ui.qCustomPlot18->replot();
	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot17->yAxis2 << ui.qCustomPlot17->xAxis2 << ui.qCustomPlot17->yAxis << ui.qCustomPlot17->xAxis;
	ui.qCustomPlot17->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot17->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot17->setBackground(QBrush(Qt::black));
	ui.qCustomPlot17->xAxis->setLabel(QString::fromLocal8Bit("序号"));
	ui.qCustomPlot17->yAxis->setLabel(QString::fromLocal8Bit("数值(mm)"));
	ui.qCustomPlot17->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot17->yAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot17->xAxis->setLabelColor(Qt::white);
	ui.qCustomPlot17->yAxis->setLabelColor(Qt::white);


	ui.qCustomPlot17->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot17->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot17->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot17->yAxis->setTickPen(QPen(QColor(255, 255, 255)));

	QList<QCPAxis*> axes1;
	axes1 << ui.qCustomPlot18->yAxis2 << ui.qCustomPlot18->xAxis2 << ui.qCustomPlot18->yAxis << ui.qCustomPlot18->xAxis;
	ui.qCustomPlot18->axisRect()->setRangeZoomAxes(axes1);
	//设置图形可缩放
	ui.qCustomPlot18->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot18->setBackground(QBrush(Qt::black));
	ui.qCustomPlot18->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot18->yAxis->setTickLabelColor(Qt::white);

	ui.qCustomPlot18->xAxis->setLabelColor(Qt::white);
	ui.qCustomPlot18->yAxis->setLabelColor(Qt::white);
	ui.qCustomPlot18->xAxis->setLabel(QString::fromLocal8Bit("序号"));
	ui.qCustomPlot18->yAxis->setLabel(QString::fromLocal8Bit("GQI值"));

	ui.qCustomPlot18->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot18->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot18->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot18->yAxis->setTickPen(QPen(QColor(255, 255, 255)));


	fermatSpiralFA = NULL;
	fermatSpiralFB = NULL;
	fermatSpiralFC = NULL;
	fermatSpiralFD = NULL;
	fermatSpiralFE = NULL;
}
void SCRail::set_offsetPlot(int iNum,CheckStatus * check_status,QString time)
{
	QVector<double>localX(0), localY(0);
	QCPCurve* fermatSpiral1C = new QCPCurve(ui.qCustomPlot11->xAxis, ui.qCustomPlot11->yAxis);
	for (int i = 0; i < 66; i++)
	{
		localX.push_back(check_status->offsetX[i]);
		localY.push_back(check_status->offsetY[i]);
	}

	fermatSpiral1C->addData(localX, localY);
	ui.qCustomPlot11->xAxis->setRange(-28, 34);
	ui.qCustomPlot11->yAxis->setRange(-1, 1.5);
	fermatSpiral1C->setPen(QPen(QColor(penColor[iNum]),2));
	fermatSpiral1C->setName(time);	
	ui.qCustomPlot11->replot();
	fermatSpiral1C = NULL;
}
void SCRail::set_offsetPlot(int iNum, CheckStatus * check_status)
{
	QVector<double>localX(0), localY(0);
	QCPCurve* fermatSpiral1DP = new QCPCurve(ui.qCustomPlot15->xAxis, ui.qCustomPlot15->yAxis);
	for (int i = 0; i < 66; i++)
	{
		localX.push_back(check_status->offsetX[i]);
		localY.push_back(check_status->offsetY[i]);
	}

	fermatSpiral1DP->addData(localX, localY);
	ui.qCustomPlot15->xAxis->setRange(-28, 34);
	ui.qCustomPlot15->yAxis->setRange(-1, 1.5);
	fermatSpiral1DP->setPen(QPen(QColor(penColor[iNum]), 2));
	fermatSpiral1DP->setName(QString("K") + QString::number(int(check_status->current)) + "+" + QString::number((check_status->current - int(check_status->current)) * 1000));
	ui.qCustomPlot11 ->replot();
	fermatSpiral1DP = NULL;
}
void SCRail::set_offsetPlot(CheckStatus * check_status, int iNum)
{
	QVector<double>localX(0), localY(0);
	QCPCurve* fermatSpiral1DR = new QCPCurve(ui.qCustomPlot19->xAxis, ui.qCustomPlot19->yAxis);
	for (int i = 0; i < 66; i++)
	{
		localX.push_back(check_status->offsetX[i]);
		localY.push_back(check_status->offsetY[i]);
	}

	fermatSpiral1DR->addData(localX, localY);
	ui.qCustomPlot19->xAxis->setRange(-28, 34);
	ui.qCustomPlot19->yAxis->setRange(-1, 1.5);
	fermatSpiral1DR->setPen(QPen(QColor(penColor[iNum]), 2));
	fermatSpiral1DR->setName(QString::fromLocal8Bit("序号") + QString::number(iNum+1));
	ui.qCustomPlot19->replot();
	fermatSpiral1DR = NULL;
}
void SCRail::set_offsetPlotUpdate(QTableWidget * table)
{
	if (ui.qCustomPlot19->plottableCount())
		ui.qCustomPlot19->clearPlottables();
	CheckStatus checkstate;
	CheckData checkdata;
	QString fileName = "\0";
	for (int i = 0; i < table->rowCount(); i++)
	{
		for (int j = 0; j < ui.count_chooseBox->count(); j++)
		{
			if (ui.count_chooseBox->itemText(j).contains(table->item(i, 12)->text(), Qt::CaseSensitive))
			{
				fileName = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->itemText(j);
			}
		}
		
		FILE *fp;
		fp = fopen(fileName.toLocal8Bit().data(), "rb");
		if (NULL == fp)
		{
			QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
			return;
		}
		memset(&checkstate, 0, sizeof(checkstate));
		memset(&checkdata, 0, sizeof(checkdata));
		fseek(fp, 0, SEEK_SET);
		fread(&checkstate, sizeof(CheckStatus), 1, fp);
		fseek(fp, 2048, SEEK_SET);
		fread(&checkdata, sizeof(CheckData), 1, fp);
		set_offsetPlot(&checkstate, i);
		fclose(fp);
	}
	
	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot19->yAxis2 << ui.qCustomPlot19->xAxis2 << ui.qCustomPlot19->yAxis << ui.qCustomPlot19->xAxis;
	ui.qCustomPlot19->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot19->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot19->setBackground(QBrush(Qt::black));
	ui.qCustomPlot19->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot19->yAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot19->xAxis->setLabel(QString::fromLocal8Bit("钢轨横向(mm)"));
	ui.qCustomPlot19->yAxis->setLabel(QString::fromLocal8Bit("偏差值(mm)"));
	ui.qCustomPlot19->xAxis->setLabelColor(Qt::white);
	ui.qCustomPlot19->yAxis->setLabelColor(Qt::white);
	//ui.qCustomui.qCustomPlot11->graph(1)->setPen(QPen(Qt::red));
	ui.qCustomPlot19->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot19->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot19->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot19->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot19->replot();
}
void SCRail::set_tableView(QTableWidget * table,float *tableInfo)
{
	if (table->rowCount() != 0)
		table->clearContents();
	table->setRowCount(6);
	table->setColumnCount(8);
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (0 == i % 2)
			{
				table->setItem(i, j, new QTableWidgetItem(QString::fromLocal8Bit(table_info[i / 2][j])));
				table->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				table->item(i, j)->setTextColor(QColor(255, 255, 255));
				QFont font;
				font.setFamily(QString::fromLocal8Bit("黑体"));
				font.setPointSize(12);
				table->item(i, j)->setFont(font);
			}
			else
			{
				if (0 == j)
				{
					table->setItem(i, j, new QTableWidgetItem(QString::fromLocal8Bit("差值(mm)")));
					table->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					table->item(i, j)->setTextColor(QColor(255, 255, 255));
					QFont font;
					font.setFamily(QString::fromLocal8Bit("黑体"));
					font.setPointSize(12);
					table->item(i, j)->setFont(font);
				}

				else
				{
					char temp[50] = "\0";
					sprintf(temp, "%.2f", tableInfo[i / 2 * 7 + j - 1]);
					table->setItem(i, j, new QTableWidgetItem(QString("%1").arg(temp)));
					table->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					table->item(i, j)->setTextColor(QColor(255, 255, 255));
					QFont font;
					font.setFamily(QString::fromLocal8Bit("微软雅黑"));
					font.setPointSize(10);
					table->item(i, j)->setFont(font);
				}

			}
		}
	}
	table->setAlternatingRowColors(true);
	table->setStyleSheet("alternate-background-color:rgb(56,67,102);background: rgb(51, 51, 51);");
	table->verticalHeader()->setVisible(false);
	table->horizontalHeader()->setVisible(false);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//平均分配列宽
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);//平均分配行宽
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
void SCRail::set_reportTableView()
{
	QString m_sProjectPath = QFileDialog::getExistingDirectory(NULL, QString::fromLocal8Bit("选择打开的路径"), NULL, NULL);;/*文件夹全路径名*/
	ui.menuChoose->setText(m_sProjectPath);
	QDir dir(m_sProjectPath);
	int colCount = 0;
	QVector<QString> filePath(0);
	QVector<QString> fileName(0);
	if (dir.exists())
	{/*判断文件夹是否存在*/
		dir.setFilter(QDir::Files); /*设置dir的过滤模式,表示只遍历本文件夹内的文件*/
		QFileInfoList fileList = dir.entryInfoList();  /*获取本文件夹内所有文件的信息*/
		QFileInfo fileInfo;
		QString suffix;
		int fileCount = fileList.count();
		
		for (int i = 0; i < fileCount; i++)
		{
			QFileInfo fileInfo = fileList[i]; /*获取每个文件信息*/
			fileName.push_back(fileInfo.fileName());
			QString suffix = fileInfo.suffix(); /*获取文件后缀名*/
			/*筛选出所有ini文件(如果要筛选其他格式的文件则根据需要修改tr("ini")中的字符串即可)*/
			if (QString::compare(suffix, QString(tr("dat")), Qt::CaseInsensitive) == 0)
			{
				filePath.push_back(fileInfo.absoluteFilePath());/*获取文件绝对路径即全路径*/
				colCount++;
				
			}
		}
	} /*获取本文件夹内的文件个数*/
	ui.tableWidget_2->clearContents();
	ui.tableWidget_2->setRowCount(colCount);
	ui.tableWidget_2->setColumnCount(7);
	ui.tableWidget_2->setAlternatingRowColors(true);
	ui.tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget_2->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(56,67,102);color: white;};");
	ui.tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);//单击选中整行
	ui.tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
	QFont font;
	font.setFamily(QString::fromLocal8Bit("黑体"));
	font.setPointSize(20);
	ui.tableWidget_2->horizontalHeader()->setFont(font);
	ui.tableWidget_2->setStyleSheet("alternate-background-color:rgb(56,67,102);background: rgb(51, 51, 51);");
	QStringList header;
	header << "  " << QString::fromLocal8Bit("线路名") << QString::fromLocal8Bit("行别/车站") <<QString::fromLocal8Bit("左/右轨") <<  QString::fromLocal8Bit("检测里程") << QString::fromLocal8Bit("检测时间") << QString::fromLocal8Bit("文件名");
	ui.tableWidget_2->setHorizontalHeaderLabels(header);
	ui.tableWidget_2->verticalHeader()->setVisible(false);
	ui.tableWidget_2->verticalHeader()->setHidden(true);
	ui.tableWidget_2->verticalHeader()->setMinimumHeight(54);
	ui.tableWidget_2->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget_2->setColumnWidth(0,50);
	ui.tableWidget_2->setColumnWidth(1,140);
	ui.tableWidget_2->setColumnWidth(2,200);
	ui.tableWidget_2->setColumnWidth(3,140);
	ui.tableWidget_2->setColumnWidth(4,180);
	ui.tableWidget_2->setColumnWidth(5,260);
	for (int i = 0; i < 20; i++)
	{
		ui.tableWidget_2->setRowHeight(i, 54);
	}
	
	for (int i = 0; i < colCount; i++)
	{
		QStringList list = fileName[i].split("_");
		for (int j = 0; j < 7; j++)
		{
			
			if (0 == j)
			{
				QLabel *label = new QLabel(this);
				label->setPixmap(QPixmap("./../../SCRail/importData/ImageFolder/checkOut2.png"));
				ui.tableWidget_2->setCellWidget(i, j, label);
			}	
			else if(j == 5)
			{
				ui.tableWidget_2->setItem(i, j, new QTableWidgetItem(list[j - 1].section('.',0,0)));
				ui.tableWidget_2->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tableWidget_2->item(i, j)->setTextColor(QColor(255, 255, 255));
				QFont font;
				font.setFamily(QString::fromLocal8Bit("黑体"));
				font.setPointSize(17);
				ui.tableWidget_2->item(i, j)->setFont(font);
				
			}
			else if (j == 6) 
			{
				ui.tableWidget_2->setItem(i, j, new QTableWidgetItem(fileName[i]));
				ui.tableWidget_2->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tableWidget_2->item(i, j)->setTextColor(QColor(255, 255, 255));
				QFont font;
				font.setFamily(QString::fromLocal8Bit("黑体"));
				font.setPointSize(17);
				ui.tableWidget_2->item(i, j)->setFont(font);
				
			}
			else
			{
				ui.tableWidget_2->setItem(i, j, new QTableWidgetItem(list[j-1]));
				ui.tableWidget_2->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tableWidget_2->item(i, j)->setTextColor(QColor(255, 255, 255));
				QFont font;
				font.setFamily(QString::fromLocal8Bit("黑体"));
				font.setPointSize(17);
				ui.tableWidget_2->item(i, j)->setFont(font);
				
			}
		}
	}
}
void SCRail::set_reportTableView(QString filePath1)
{
	ui.menuChoose->setText(filePath1);
	QDir dir(filePath1);
	int colCount = 0;
	QVector<QString> filePath(0);
	QVector<QString> fileName(0);
	if (dir.exists())
	{/*判断文件夹是否存在*/
		dir.setFilter(QDir::Files); /*设置dir的过滤模式,表示只遍历本文件夹内的文件*/
		QFileInfoList fileList = dir.entryInfoList();  /*获取本文件夹内所有文件的信息*/
		QFileInfo fileInfo;
		QString suffix;
		int fileCount = fileList.count();

		for (int i = 0; i < fileCount; i++)
		{
			QFileInfo fileInfo = fileList[i]; /*获取每个文件信息*/
			fileName.push_back(fileInfo.fileName());
			QString suffix = fileInfo.suffix(); /*获取文件后缀名*/
			/*筛选出所有ini文件(如果要筛选其他格式的文件则根据需要修改tr("ini")中的字符串即可)*/
			if (QString::compare(suffix, QString(tr("dat")), Qt::CaseInsensitive) == 0)
			{
				filePath.push_back(fileInfo.absoluteFilePath());/*获取文件绝对路径即全路径*/
				colCount++;

			}
		}
	} /*获取本文件夹内的文件个数*/
	ui.tableWidget_2->clearContents();
	ui.tableWidget_2->setRowCount(colCount);
	ui.tableWidget_2->setColumnCount(7);
	ui.tableWidget_2->setAlternatingRowColors(true);
	ui.tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget_2->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(56,67,102);color: white;};");
	ui.tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);//单击选中整行
	ui.tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
	QFont font;
	font.setFamily(QString::fromLocal8Bit("黑体"));
	font.setPointSize(20);
	ui.tableWidget_2->horizontalHeader()->setFont(font);
	ui.tableWidget_2->setStyleSheet("alternate-background-color:rgb(56,67,102);background: rgb(51, 51, 51);");
	QStringList header;
	header << "  " << QString::fromLocal8Bit("线路名") << QString::fromLocal8Bit("行别/车站")  << QString::fromLocal8Bit("左/右轨")<< QString::fromLocal8Bit("检测里程") << QString::fromLocal8Bit("检测时间") << QString::fromLocal8Bit("文件名");
	ui.tableWidget_2->setHorizontalHeaderLabels(header);
	ui.tableWidget_2->verticalHeader()->setVisible(false);
	ui.tableWidget_2->verticalHeader()->setHidden(true);
	ui.tableWidget_2->verticalHeader()->setMinimumHeight(54);
	ui.tableWidget_2->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget_2->setColumnWidth(0, 50);
	ui.tableWidget_2->setColumnWidth(1, 140);
	ui.tableWidget_2->setColumnWidth(2, 200);
	ui.tableWidget_2->setColumnWidth(3, 140);
	ui.tableWidget_2->setColumnWidth(4, 180);
	ui.tableWidget_2->setColumnWidth(5, 260);
	for (int i = 0; i < 20; i++)
	{
		ui.tableWidget_2->setRowHeight(i, 54);
	}

	for (int i = 0; i < colCount; i++)
	{
		QStringList list = fileName[i].split("_");
		for (int j = 0; j < 7; j++)
		{

			if (0 == j)
			{
				QLabel *label = new QLabel(this);
				label->setPixmap(QPixmap("./../../SCRail/importData/ImageFolder/checkOut2.png"));
				ui.tableWidget_2->setCellWidget(i, j, label);
			}
			else if (j == 5)
			{
				ui.tableWidget_2->setItem(i, j, new QTableWidgetItem(list[j - 1].section('.', 0, 0)));
				ui.tableWidget_2->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tableWidget_2->item(i, j)->setTextColor(QColor(255, 255, 255));

			}
			else if (j == 6)
			{
				ui.tableWidget_2->setItem(i, j, new QTableWidgetItem(fileName[i]));
				ui.tableWidget_2->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tableWidget_2->item(i, j)->setTextColor(QColor(255, 255, 255));

			}
			else
			{
				ui.tableWidget_2->setItem(i, j, new QTableWidgetItem(list[j - 1]));
				ui.tableWidget_2->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				ui.tableWidget_2->item(i, j)->setTextColor(QColor(255, 255, 255));

			}
		}
	}
}
void SCRail::set_reportTableDiffTime(int fileCount, QVector<QString> _checktime, QVector<double> _verticalHalf, QVector<double> _verticalThirds,QVector<double> _horizontal,QVector<double> _max,QVector<double> _min,QVector<double> _GQI,QVector<QString>_cantValue)
{
	
	set_plotAnalysis(_checktime, _verticalThirds, _horizontal, _max, _min,_GQI);
	memset(&_avgValue, '0', sizeof(avgValue_j));
	ui.tableWidget_4->clearContents();
	ui.tableWidget_4->setRowCount(fileCount);
	ui.tableWidget_4->setColumnCount(8);
	ui.tableWidget_4->setAlternatingRowColors(true);
	ui.tableWidget_4->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget_4->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(56,67,102);color: white;};");
	ui.tableWidget_4->setSelectionMode(QAbstractItemView::SingleSelection);//单击选中整行
	ui.tableWidget_4->setSelectionBehavior(QAbstractItemView::SelectRows);
	QFont font;
	font.setFamily(QString::fromLocal8Bit("黑体"));
	font.setPointSize(20);
	ui.tableWidget_4->horizontalHeader()->setFont(font);
	ui.tableWidget_4->setStyleSheet("alternate-background-color:rgb(56,67,102);background: rgb(51, 51, 51);");
	QStringList header;
	header << QString::fromLocal8Bit("检测时间") << QString::fromLocal8Bit("垂磨(1/2)") << QString::fromLocal8Bit("垂磨(1/3)") << QString::fromLocal8Bit("侧磨") << QString::fromLocal8Bit("最大偏差值") << QString::fromLocal8Bit("最小偏差值") << QString::fromLocal8Bit("GQI") << QString::fromLocal8Bit("轨底坡");
	ui.tableWidget_4->setHorizontalHeaderLabels(header);
	ui.tableWidget_4->verticalHeader()->setVisible(false);
	ui.tableWidget_4->verticalHeader()->setHidden(true);
	ui.tableWidget_4->verticalHeader()->setMinimumHeight(54);
	ui.tableWidget_4->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget_4->setColumnWidth(0, 270);
	ui.tableWidget_4->setColumnWidth(1, 190);
	ui.tableWidget_4->setColumnWidth(2, 190);
	ui.tableWidget_4->setColumnWidth(3, 120);
	ui.tableWidget_4->setColumnWidth(4, 190);
	ui.tableWidget_4->setColumnWidth(5, 190);
	ui.tableWidget_4->setColumnWidth(6, 180);
	for (int i = 0; i < 20; i++)
	{
		ui.tableWidget_4->setRowHeight(i, 54);
	}
	char temp[50] = "\0";
	for (int i = 0; i < fileCount; i++)
	{
		
		ui.tableWidget_4->setItem(i, 0, new QTableWidgetItem(QString(_checktime.at(i))));
		ui.tableWidget_4->item(i, 0)->setTextAlignment(Qt::AlignCenter);
		sprintf(temp, "%.2f", _verticalHalf.at(i));
		ui.tableWidget_4->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_4->item(i,1)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_verticalHalf.at(i), ui.value_3->currentText().toDouble(), ui.tableWidget_4->item(i, 1));
		sprintf(temp, "%.2f", _verticalThirds.at(i));
		ui.tableWidget_4->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_4->item(i, 2)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_verticalThirds.at(i), ui.value_3->currentText().toDouble(), ui.tableWidget_4->item(i, 2));
		sprintf(temp, "%.2f", _horizontal.at(i));
		ui.tableWidget_4->setItem(i, 3, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_4->item(i, 3)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_horizontal.at(i), ui.value_4->currentText().toDouble(), ui.tableWidget_4->item(i, 3));
		sprintf(temp, "%.2f", _max.at(i));
		ui.tableWidget_4->setItem(i, 4, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_4->item(i, 4)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_max.at(i), ui.maxdiffBox_2->currentText().toDouble(), ui.tableWidget_4->item(i, 4));
		sprintf(temp, "%.2f", _min.at(i));
		ui.tableWidget_4->setItem(i, 5, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_4->item(i, 5)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_min.at(i), ui.mindiffBox_2->currentText().toDouble(), ui.tableWidget_4->item(i, 5));
		ui.tableWidget_4->setItem(i, 6, new QTableWidgetItem(QString::number(_GQI.at(i))));
		ui.tableWidget_4->item(i, 6)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(ui.GQIBox_2->currentText().toDouble(), _GQI.at(i), ui.tableWidget_4->item(i, 6));
		ui.tableWidget_4->setItem(i, 7, new QTableWidgetItem(_cantValue.at(i)));
		ui.tableWidget_4->item(i, 7)->setTextAlignment(Qt::AlignCenter);
		_avgValue.verticalHalf_wear += _verticalHalf.at(i);
		_avgValue.verticalThirds_wear += _verticalThirds.at(i);
		_avgValue.horizontal_wear += _horizontal.at(i);
		_avgValue.max_offset += _max.at(i);
		_avgValue.min_offset += _min.at(i);
		_avgValue.GQI_value += _GQI.at(i);
	}
	sprintf(temp, "%.2f", _avgValue.verticalHalf_wear / fileCount);
	_avgValue.verticalHalf_wear = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.verticalThirds_wear / fileCount);
	_avgValue.verticalThirds_wear = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.horizontal_wear / fileCount);
	_avgValue.horizontal_wear = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.max_offset / fileCount);
	_avgValue.max_offset = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.min_offset / fileCount);
	_avgValue.min_offset = QString("%1").arg(temp).toDouble();
	_avgValue.GQI_value = round(_avgValue.GQI_value / fileCount);
}
void SCRail::set_reportTableDiffMile(int fileCount, QVector<QString> _time, QVector<double> _verticalHalf, QVector<double> _verticalThirds, QVector<double> _horizontal, QVector<double> _max, QVector<double> _min, QVector<double> _GQI, QVector<double> _mile,QVector<QString>_cantValue)
{
	set_plotAnalysisDiffPoint(_mile, _verticalThirds, _horizontal, _max, _min, _GQI);
	memset(&_avgValue, '0', sizeof(avgValue_j));
	ui.tableWidget_6->clearContents();
	ui.tableWidget_6->setRowCount(fileCount);
	ui.tableWidget_6->setColumnCount(9);
	ui.tableWidget_6->setAlternatingRowColors(true);
	ui.tableWidget_6->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget_6->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(56,67,102);color: white;};");
	ui.tableWidget_6->setSelectionMode(QAbstractItemView::SingleSelection);//单击选中整行
	ui.tableWidget_6->setSelectionBehavior(QAbstractItemView::SelectRows);
	QFont font;
	font.setFamily(QString::fromLocal8Bit("黑体"));
	font.setPointSize(20);
	ui.tableWidget_6->horizontalHeader()->setFont(font);
	ui.tableWidget_6->setStyleSheet("alternate-background-color:rgb(56,67,102);background: rgb(51, 51, 51);");
	QStringList header;
	header << QString::fromLocal8Bit("检测里程") << QString::fromLocal8Bit("垂磨(1/2)") << QString::fromLocal8Bit("垂磨(1/3)") << QString::fromLocal8Bit("侧磨") << QString::fromLocal8Bit("最大偏差值") << QString::fromLocal8Bit("最小偏差值") << QString::fromLocal8Bit("GQI")<< QString::fromLocal8Bit("轨底坡") << QString::fromLocal8Bit("检测时间");
	ui.tableWidget_6->setHorizontalHeaderLabels(header);
	ui.tableWidget_6->verticalHeader()->setVisible(false);
	ui.tableWidget_6->verticalHeader()->setHidden(true);
	ui.tableWidget_6->verticalHeader()->setMinimumHeight(54);
	ui.tableWidget_6->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget_6->setColumnWidth(0, 155);
	ui.tableWidget_6->setColumnWidth(1, 165);
	ui.tableWidget_6->setColumnWidth(2, 165);
	ui.tableWidget_6->setColumnWidth(3, 125);
	ui.tableWidget_6->setColumnWidth(4, 175);
	ui.tableWidget_6->setColumnWidth(5, 175);
	ui.tableWidget_6->setColumnWidth(6, 125);
	ui.tableWidget_6->setColumnWidth(7, 110);
	for (int i = 0; i < 10; i++)
	{
		ui.tableWidget_6->setRowHeight(i, 54);
	}
	char temp[50] = "\0";
	for (int i = 0; i < fileCount; i++)
	{
		ui.tableWidget_6->setItem(i, 0, new QTableWidgetItem(QString("K") + QString::number(int(_mile.at(i))) + "+" + QString::number((_mile.at(i) - int(_mile.at(i))) * 1000)));
		ui.tableWidget_6->item(i, 0)->setTextAlignment(Qt::AlignCenter);
		sprintf(temp, "%.2f", _verticalHalf[i]);
		ui.tableWidget_6->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_6->item(i, 1)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_verticalHalf.at(i), ui.value_3->currentText().toDouble(), ui.tableWidget_6->item(i, 1));
		sprintf(temp, "%.2f", _verticalThirds[i]);
		ui.tableWidget_6->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_6->item(i, 2)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_verticalThirds.at(i), ui.value_3->currentText().toDouble(), ui.tableWidget_6->item(i, 2));
		sprintf(temp, "%.2f", _horizontal[i]);
		ui.tableWidget_6->setItem(i, 3, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_6->item(i, 3)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_horizontal.at(i), ui.value_4->currentText().toDouble(), ui.tableWidget_6->item(i, 3));
		sprintf(temp, "%.2f", _max[i]);
		ui.tableWidget_6->setItem(i, 4, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_6->item(i, 4)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_max.at(i), ui.maxdiffBox_2->currentText().toDouble(), ui.tableWidget_6->item(i, 4));
		sprintf(temp, "%.2f", _min[i]);
		ui.tableWidget_6->setItem(i, 5, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_6->item(i, 5)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_min.at(i), ui.mindiffBox_2->currentText().toDouble(), ui.tableWidget_6->item(i, 5));
		ui.tableWidget_6->setItem(i, 6, new QTableWidgetItem(QString::number(_GQI[i])));
		ui.tableWidget_6->item(i, 6)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(ui.GQIBox_2->currentText().toDouble(), _GQI.at(i), ui.tableWidget_6->item(i, 6));
		ui.tableWidget_6->setItem(i, 7, new QTableWidgetItem(_cantValue.at(i)));
		ui.tableWidget_6->item(i, 7)->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget_6->setItem(i, 8, new QTableWidgetItem(QString(_time.at(i))));
		ui.tableWidget_6->item(i, 8)->setTextAlignment(Qt::AlignCenter);
		
		_avgValue.verticalHalf_wear += _verticalHalf.at(i);
		_avgValue.verticalThirds_wear += _verticalThirds.at(i);
		_avgValue.horizontal_wear += _horizontal.at(i);
		_avgValue.max_offset += _max.at(i);
		_avgValue.min_offset += _min.at(i);
		_avgValue.GQI_value += _GQI.at(i);
	}
	sprintf(temp, "%.2f", _avgValue.verticalHalf_wear / fileCount);
	_avgValue.verticalHalf_wear = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.verticalThirds_wear / fileCount);
	_avgValue.verticalThirds_wear = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.horizontal_wear / fileCount);
	_avgValue.horizontal_wear = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.max_offset / fileCount);
	_avgValue.max_offset = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.min_offset / fileCount);
	_avgValue.min_offset = QString("%1").arg(temp).toDouble();
	_avgValue.GQI_value = round(_avgValue.GQI_value / fileCount);
}
void SCRail::set_reportTableDiffRail(int fileCount, QVector<QString> _time, QVector<double> _verticalHalf, QVector<double> _verticalThirds, QVector<double> _horizontal, QVector<double> _max, QVector<double> _min, QVector<double> _GQI, QVector<double> _mile, QVector<QString> _way, QVector<QString> _typeName,QVector<QString> _railDirec,QVector<QString>_cantValue)
{
	memset(&_avgValue, '0', sizeof(avgValue_j));
	ui.tableWidget_8->clearContents();
	ui.tableWidget_8->setRowCount(fileCount);
	ui.tableWidget_8->setColumnCount(13);
	ui.tableWidget_8->setAlternatingRowColors(true);
	ui.tableWidget_8->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget_8->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(56,67,102);color: white;};");
	ui.tableWidget_8->setSelectionMode(QAbstractItemView::SingleSelection);//单击选中整行
	ui.tableWidget_8->setSelectionBehavior(QAbstractItemView::SelectRows);
	QFont font;
	font.setFamily(QString::fromLocal8Bit("黑体"));
	font.setPointSize(18);
	ui.tableWidget_8->horizontalHeader()->setFont(font);
	ui.tableWidget_8->setStyleSheet("alternate-background-color:rgb(56,67,102);background: rgb(51, 51, 51);");
	QStringList header;
	header << QString::fromLocal8Bit("序号") << QString::fromLocal8Bit("线名") << QString::fromLocal8Bit("行别/车站") << QString::fromLocal8Bit("左/右轨") << QString::fromLocal8Bit("检测里程") << QString::fromLocal8Bit("垂磨(1/2)") << QString::fromLocal8Bit("垂磨(1/3)") << QString::fromLocal8Bit("侧磨") << QString::fromLocal8Bit("最大偏差值") << QString::fromLocal8Bit("最小偏差值") << QString::fromLocal8Bit("GQI") << QString::fromLocal8Bit("轨底坡") << QString::fromLocal8Bit("检测时间");
	ui.tableWidget_8->setHorizontalHeaderLabels(header);
	ui.tableWidget_8->verticalHeader()->setVisible(false);
	ui.tableWidget_8->verticalHeader()->setHidden(true);
	ui.tableWidget_8->verticalHeader()->setMinimumHeight(54);
	ui.tableWidget_8->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget_8->setColumnWidth(0, 70);
	ui.tableWidget_8->setColumnWidth(1, 90);
	ui.tableWidget_8->setColumnWidth(2, 130);
	ui.tableWidget_8->setColumnWidth(3, 120);
	ui.tableWidget_8->setColumnWidth(4, 150);
	ui.tableWidget_8->setColumnWidth(5, 120);
	ui.tableWidget_8->setColumnWidth(6, 120);
	ui.tableWidget_8->setColumnWidth(7, 100);
	ui.tableWidget_8->setColumnWidth(8, 135);
	ui.tableWidget_8->setColumnWidth(9, 135);
	ui.tableWidget_8->setColumnWidth(10, 70);
	ui.tableWidget_8->setColumnWidth(11, 100);

	for (int i = 0; i < 10; i++)
	{
		ui.tableWidget_8->setRowHeight(i, 54);
	}
	char temp[50] = "\0";
	for (int i = 0; i < fileCount; i++)
	{
		ui.tableWidget_8->setItem(i, 0, new QTableWidgetItem(QString::number(i+1)));
		ui.tableWidget_8->item(i, 0)->setTextAlignment(Qt::AlignCenter);
		
		ui.tableWidget_8->setItem(i, 1, new QTableWidgetItem(_way.at(i)));
		ui.tableWidget_8->item(i, 1)->setTextAlignment(Qt::AlignCenter);

		ui.tableWidget_8->setItem(i, 2, new QTableWidgetItem(_typeName.at(i)));
		ui.tableWidget_8->item(i, 2)->setTextAlignment(Qt::AlignCenter);

		ui.tableWidget_8->setItem(i, 3, new QTableWidgetItem(_railDirec.at(i)));
		ui.tableWidget_8->item(i, 3)->setTextAlignment(Qt::AlignCenter);

		ui.tableWidget_8->setItem(i, 4, new QTableWidgetItem(QString("K") + QString::number(int(_mile.at(i))) + "+" + QString::number((_mile.at(i) - int(_mile.at(i))) * 1000)));
		ui.tableWidget_8->item(i, 4)->setTextAlignment(Qt::AlignCenter);
		sprintf(temp, "%.2f", _verticalHalf[i]);
		ui.tableWidget_8->setItem(i, 5, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_8->item(i, 5)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_verticalHalf.at(i), ui.value_3->currentText().toDouble(), ui.tableWidget_8->item(i, 5));
		sprintf(temp, "%.2f", _verticalThirds[i]);
		ui.tableWidget_8->setItem(i, 6, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_8->item(i, 6)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_verticalThirds.at(i), ui.value_3->currentText().toDouble(), ui.tableWidget_8->item(i, 6));
		sprintf(temp, "%.2f", _horizontal[i]);
		ui.tableWidget_8->setItem(i, 7, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_8->item(i, 7)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_horizontal.at(i), ui.value_4->currentText().toDouble(), ui.tableWidget_8->item(i, 7));
		sprintf(temp, "%.2f", _max[i]);
		ui.tableWidget_8->setItem(i, 8, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_8->item(i, 8)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_max.at(i), ui.maxdiffBox_2->currentText().toDouble(), ui.tableWidget_8->item(i, 8));

		sprintf(temp, "%.2f", _min[i]);
		ui.tableWidget_8->setItem(i, 9, new QTableWidgetItem(QString("%1").arg(temp)));
		ui.tableWidget_8->item(i, 9)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(_min.at(i), ui.mindiffBox_2->currentText().toDouble(), ui.tableWidget_8->item(i, 9));
		ui.tableWidget_8->setItem(i, 10, new QTableWidgetItem(QString::number(_GQI[i])));
		ui.tableWidget_8->item(i, 10)->setTextAlignment(Qt::AlignCenter);
		on_judgeIsOver(ui.GQIBox_2->currentText().toDouble(), _GQI.at(i), ui.tableWidget_8->item(i, 10));
		ui.tableWidget_8->setItem(i, 11, new QTableWidgetItem(_cantValue.at(i)));
		ui.tableWidget_8->item(i, 11)->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget_8->setItem(i, 12, new QTableWidgetItem(QString(_time.at(i))));
		ui.tableWidget_8->item(i, 12)->setTextAlignment(Qt::AlignCenter);
		_avgValue.verticalHalf_wear += _verticalHalf.at(i);
		_avgValue.verticalThirds_wear += _verticalThirds.at(i);
		_avgValue.horizontal_wear += _horizontal.at(i);
		_avgValue.max_offset += _max.at(i);
		_avgValue.min_offset += _min.at(i);
		_avgValue.GQI_value += _GQI.at(i);
	}
	sprintf(temp, "%.2f", _avgValue.verticalHalf_wear / fileCount);
	_avgValue.verticalHalf_wear = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.verticalThirds_wear / fileCount);
	_avgValue.verticalThirds_wear = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.horizontal_wear / fileCount);
	_avgValue.horizontal_wear = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.max_offset / fileCount);
	_avgValue.max_offset = QString("%1").arg(temp).toDouble();
	sprintf(temp, "%.2f", _avgValue.min_offset / fileCount);
	_avgValue.min_offset = QString("%1").arg(temp).toDouble();
	_avgValue.GQI_value = round(_avgValue.GQI_value / fileCount);
}
void SCRail::on_sort(QComboBox * box ,int state)
{
	int boxSize = box->count();
	for (int i = 0; i < boxSize - 1; i++)
	{
		for (int j = 0; j < boxSize - i - 1; j++)
		{
			if (1 == state)
			{
				if (QDateTime::fromString(box->itemText(j).section('_', 4, 4).section(".", 0, 0), "yyyy-MM-dd-hh-mm-ss").toTime_t()\
					> QDateTime::fromString(box->itemText(j + 1).section('_', 4, 4).section(".", 0, 0), "yyyy-MM-dd-hh-mm-ss").toTime_t())
				{
					QString temp = box->itemText(j);
					box->setItemText(j, box->itemText(j + 1));
					box->setItemText(j + 1, temp);
				}
			
			}
			else if (2 == state)
			{
				if ((box->itemText(j).split('_').at(3).split('+').at(0).section('K', 1, 1).toInt() + box->itemText(j).split('_').at(3).section('+', 1, 1).toDouble() / 1000)\
					> (box->itemText(j+1).split('_').at(3).split('+').at(0).section('K', 1, 1).toInt() + box->itemText(j+1).split('_').at(3).section('+', 1, 1).toDouble() / 1000))
				{
					QString temp = box->itemText(j);
					box->setItemText(j, box->itemText(j + 1));
					box->setItemText(j + 1, temp);
				}
			}
		}

	}
}
void SCRail::on_cellValueChanged(int row, int column)
{
	
	if (0 == column && 3 == ui.funcInterface->currentIndex())
	{
		if (std::find(_row.begin(), _row.end(), row) != _row.end())
		{
			_row.erase(remove(_row.begin(), _row.end(), row),_row.end());
			QLabel *label = new QLabel(this);
			label->setPixmap(QPixmap("./../../SCRail/importData/ImageFolder/checkOut2.png"));
			ui.tableWidget_2->setCellWidget(row, column, label);
			ui.count_chooseBox->removeItem(ui.count_chooseBox->findText(ui.tableWidget_2->item(row, 6)->text()));
		}
		else
		{
			if (_row.size() >= 10)
			{
				QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("选中的文件已超过上限值!!!"), QMessageBox::Yes);
				return;
			}
			_row.append(row);
			QLabel *label = new QLabel(this);
			label->setPixmap(QPixmap("./../../SCRail/importData/ImageFolder/checkOn2.png"));
			ui.tableWidget_2->setCellWidget(row, column, label);
			QStringList strlist;
			strlist << ui.tableWidget_2->item(row, 6)->text();
			ui.count_chooseBox->addItems(strlist);			
		}
	}
}
void SCRail::on_cellValueDelete(int row, int coloum)
{
	QMessageBox msg(this);
	msg.setWindowTitle("Warnning");
	msg.setText(QString::fromLocal8Bit("选择\"移除\"可将文件从表中移除。\n选择\"删除\"可永久删除该文件。"));
	msg.setIcon(QMessageBox::Information);
	msg.setIcon(QMessageBox::NoIcon);
	msg.setStandardButtons(QMessageBox::Yes| QMessageBox::Ok | QMessageBox::No);
	msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("移  除"));
	msg.setButtonText(QMessageBox::Ok, QString::fromLocal8Bit("删 除"));
	msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
	int rel = msg.exec();
	if (rel == QMessageBox::No)
	{
		return;
	}
	else if(rel == QMessageBox::Ok)
	{
		QString info = "./../../SCRail/importData/saveDataFolder/" + ui.tableWidget_2->item(row, 6)->text();
		QFile file(info);
		file.remove();
		ui.tableWidget_2->removeRow(row);
	}
	else if (rel == QMessageBox::Yes)
	{
		ui.tableWidget_2->removeRow(row);
	}
}
void SCRail::on_choiceRowPlot(int row, int column)
{
	QString fileName = "\0";
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{
		if (ui.count_chooseBox->itemText(i).contains(ui.tableWidget_4->item(row, 0)->text(), Qt::CaseSensitive))
		{
			 fileName = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->itemText(i);
		}
	}
	FILE *fp;
	fp = fopen(fileName.toLocal8Bit().data(), "rb");
	if (NULL == fp)
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
		return;
	}
	CheckStatus checkstate;
	CheckData checkdata;
	fseek(fp, 0, SEEK_SET);
	fread(&checkstate, sizeof(CheckStatus), 1, fp);
	fseek(fp, 2048, SEEK_SET);
	fread(&checkdata, sizeof(CheckData), 1, fp);
	set_plot(checkstate.rail_type+4, &checkstate, &checkdata);
	fclose(fp);
}
void SCRail::on_choiceRowPlotDiffPoint(int row, int column)
{
	QString fileName = "\0";
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{
		if (ui.count_chooseBox->itemText(i).contains(ui.tableWidget_6->item(row, 8)->text(), Qt::CaseSensitive))
		{
			fileName = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->itemText(i);
		}
	}
	FILE *fp;
	fp = fopen(fileName.toLocal8Bit().data(), "rb");
	if (NULL == fp)
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
		return;
	}
	CheckStatus checkstate;
	CheckData checkdata;
	fseek(fp, 0, SEEK_SET);
	fread(&checkstate, sizeof(CheckStatus), 1, fp);
	fseek(fp, 2048, SEEK_SET);
	fread(&checkdata, sizeof(CheckData), 1, fp);
	set_plotDiffPoint(checkstate.rail_type+4, &checkstate, &checkdata);
	fclose(fp);
}
void SCRail::on_choiceRowPlotDiffRail(int row, int column)
{
	QString fileName = "\0";
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{
		if (ui.count_chooseBox->itemText(i).contains(ui.tableWidget_8->item(row, 12)->text(), Qt::CaseSensitive))
		{
			fileName = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->itemText(i);
		}
	}
	FILE *fp;
	fp = fopen(fileName.toLocal8Bit().data(), "rb");
	if (NULL == fp)
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
		return;
	}
	CheckStatus checkstate;
	CheckData checkdata;
	fseek(fp, 0, SEEK_SET);
	fread(&checkstate, sizeof(CheckStatus), 1, fp);
	fseek(fp, 2048, SEEK_SET);
	fread(&checkdata, sizeof(CheckData), 1, fp);
	set_plotDiffRail(checkstate.rail_type+4, &checkstate, &checkdata);
	fclose(fp);
}
void SCRail::set_save()
{
	QMessageBox msg(this);
	msg.setWindowTitle("Warnning");
	msg.setText(QString::fromLocal8Bit("确定保存这组数据吗？"));
	msg.setIcon(QMessageBox::Information);
	msg.setIcon(QMessageBox::NoIcon);
	msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确 定"));
	msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
	if (msg.exec() == QMessageBox::No)
	{
		return;
	}
	on_writeInfo();
}

void SCRail::interface_switch()
{
	_temp = 1;
	set_Title(_temp);
	set_Info(ui.lineName_2, ui.typeTitle, ui.typeName, ui.trackName, ui.mileageName, ui.target_oversizeName);
	//set_plotNormal(checkIn->_targetOversizeIndex);
	ui.funcInterface->setCurrentIndex(2);
	if (0 != ui.infoAnalysis->currentIndex())
	{
		ui.infoAnalysis->setCurrentIndex(0);

	}
	ui.btn_abraAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n1.png);border:none;color:white;");
	ui.btn_grindAdvice2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
	ui.btn_qualityAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
}
void SCRail::interface_switch2()
{
	_temp = 2;
	set_Title(_temp);
	//set_plot(ui.qCustomPlot3, checkIn->_targetOversizeIndex + 4);
	if (2 != ui.funcInterface->currentIndex())
		ui.funcInterface->setCurrentIndex(2);
	if (1 != ui.infoAnalysis->currentIndex())
	{
		ui.infoAnalysis->setCurrentIndex(1);
	}
	set_Info(ui.lineName_3, ui.typeTitle_2, ui.typeName_2, ui.trackName_2, ui.mileageName_2, ui.target_oversizeName_2);
	ui.btn_abraAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
	ui.btn_grindAdvice2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n1.png);border:none;color:white;");
	ui.btn_qualityAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");

}

void SCRail::interface_switch3()
{
	_temp = 3;
	set_Title(_temp);
	//set_plot(checkIn->_targetOversizeIndex + 4, ui.qCustomPlot4, ui.qCustomPlot5,checkdata);
	if (2 != ui.funcInterface->currentIndex())
		ui.funcInterface->setCurrentIndex(2);

	if (2 != ui.infoAnalysis->currentIndex())
	{

		ui.infoAnalysis->setCurrentIndex(2);

	}
	set_Info(ui.lineName_4, ui.typeTitle_3, ui.typeName_3, ui.trackName_3, ui.mileageName_3, ui.target_oversizeName_3);
	ui.btn_abraAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
	ui.btn_grindAdvice2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n2.png);border:none;color:white;");
	ui.btn_qualityAnalysis2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/button/n1.png);border:none;color:white;");
}

void SCRail::return_mainFace()
{
	this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg1.png)}");
	set_stateNormal();
	ui.stackedWidget->setCurrentIndex(0);
	ui.tableWidget->setRowCount(0);
	ui.Lab_titleCheck->clear();
	ui.Lab_titleOffset->clear();
	memset(checkdata,0,sizeof(CheckData));
	memset(_oversizeStateIndex, 0, sizeof(_oversizeStateIndex));
}

void SCRail::set_connectState()
{
	strcpy_s(contral.wayname, ui.lineCBox_2->currentText().toStdString().c_str());
	strcpy_s(contral.linename, ui.nameType->currentText().toStdString().c_str());
	contral.railtype = ui.oversizeBox->currentIndex();
	
	if (ui.btn_connectState->text() == QString::fromLocal8Bit("连    接"))
	{
		tcpClient->connectToHost(QHostAddress(IPAddress), Iport);	
		if (tcpClient->waitForConnected(5000))  // 连接成功则进入if{}
		{
			_state = 1;
			ui.btn_connectState->setText(QString::fromLocal8Bit("断    开"));
			//writeData();
		}
		else
		{
			QMessageBox::information(this, "Warning", QString::fromLocal8Bit("连接失败", QMessageBox::Yes));
		}
	}		
	else
	{
		writeData();
		tcpClient->disconnectFromHost();
		
		if (tcpClient->state() == QAbstractSocket::UnconnectedState|| tcpClient->waitForDisconnected(1000))  //已断开连接则进入if{}
		{
			ui.btn_connectState->setText(QString::fromLocal8Bit("连    接"));
			_state = 0;
		}
		
		
	}
	set_stateStyle(_state);
}

void SCRail::set_gatherState()
{
	if (QString::fromLocal8Bit("采    集") == ui.btn_gatherState->text())
		ui.btn_gatherState->setText(QString::fromLocal8Bit("重新采集"));
	
	strcpy_s(contral.wayname, ui.lineCBox_2->currentText().toStdString().c_str());
	strcpy_s(contral.linename, ui.nameType->currentText().toStdString().c_str());

	contral.railtype = ui.oversizeBox->currentIndex();
	contral.status = 1;
	writeData();
	Sleep(1);
	
}

void SCRail::on_grindIsVisble(QAbstractButton *)
{
	_grind[0]->setVisible(ui.CB_grindTarget->isChecked());
	_grind[1]->setVisible(ui.CB_grindReal->isChecked());
	_grind[2]->setVisible(ui.CB_degreeMinus10->isChecked());
	_grind[3]->setVisible(ui.CB_degreeMinus5->isChecked());
	_grind[4]->setVisible(ui.CB_degreeMinus2->isChecked());
	_grind[5]->setVisible(ui.CB_degreeMinus1->isChecked());
	_grind[6]->setVisible(ui.CB_degree0->isChecked());
	_grind[7]->setVisible(ui.CB_degree1->isChecked());
	_grind[8]->setVisible(ui.CB_degree2->isChecked());
	_grind[9]->setVisible(ui.CB_degree3->isChecked());
	_grind[10]->setVisible(ui.CB_degree4->isChecked());
	_grind[11]->setVisible(ui.CB_degree5->isChecked());
	_grind[12]->setVisible(ui.CB_degree10->isChecked());
	_grind[13]->setVisible(ui.CB_degree15->isChecked());
	_grind[14]->setVisible(ui.CB_degree20->isChecked());
	_grind[15]->setVisible(ui.CB_degree25->isChecked());
	_grind[16]->setVisible(ui.CB_degree30->isChecked());
	_grind[17]->setVisible(ui.CB_degree35->isChecked());
	_grind[18]->setVisible(ui.CB_degree40->isChecked());
	_grind[19]->setVisible(ui.CB_degree45->isChecked());
	_grind[20]->setVisible(ui.CB_degree50->isChecked());
	_grind[21]->setVisible(ui.CB_degree55->isChecked());
	_grind[22]->setVisible(ui.CB_degree60->isChecked());
	ui.qCustomPlot3->replot();
}

void SCRail::onButtonClicked(QAbstractButton *)
{
	if (1 == groupMilNum->checkedId())
		checkIn->_checkLocal = 1;
	else
		checkIn->_checkLocal = 0;
}

void SCRail::curRowSeparation(int index)
{
	if (index < 5)
	{
		ui.staCBox->setAttribute(Qt::WA_TransparentForMouseEvents);
		ui.staCBox->setStyleSheet("color:gray;");
		ui.trackBox->setAttribute(Qt::WA_TransparentForMouseEvents);
		ui.trackBox->setStyleSheet("color:gray;");
	}
	else {
		ui.staCBox->setAttribute(Qt::WA_TransparentForMouseEvents, false);
		ui.staCBox->setStyleSheet("color:white;");
		ui.trackBox->setAttribute(Qt::WA_TransparentForMouseEvents, false);
		ui.trackBox->setStyleSheet("color:white;");
	}
}

void SCRail::gatherStateChange(int)
{
	ui.btn_gatherState->setText(QString::fromLocal8Bit("采    集"));
}

void SCRail::updataTime()
{
	QDateTime time = QDateTime::currentDateTime();
	QString strTime = time.toString("yyyy-MM-dd hh:mm:ss dddd");
	ui.labTime->setText(strTime);
	
}
void SCRail::unclose()
{
	close();
	qApp->quit();
}
void SCRail::oskShow()
{
	QProcess::startDetached("osk");
}
void SCRail::slot_mouseRelease(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
		return;
	QPointF ChickedPoint = event->pos();
	if (!ui.qCustomPlot->viewport().contains(event->pos()))
		return;
	double currentX = ui.qCustomPlot->xAxis->pixelToCoord(ChickedPoint.x());
	double currentY = ui.qCustomPlot->yAxis->pixelToCoord(ChickedPoint.y());
	QString local = QString("%1").arg(currentX) + QString(",%1").arg(currentY);
	QToolTip::showText(event->globalPos() - this->pos(), QString::fromLocal8Bit("当前坐标:%1").arg(local),this);

}
void SCRail::slot_mouseRelease4(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
		return;
	QPointF ChickedPoint = event->pos();
	if (!ui.qCustomPlot4->viewport().contains(event->pos()))
		return;
	double currentX = ui.qCustomPlot4->xAxis->pixelToCoord(ChickedPoint.x());
	double currentY = ui.qCustomPlot4->yAxis->pixelToCoord(ChickedPoint.y());
	QString local = QString("%1").arg(currentX) + QString(",%1").arg(currentY);
	QToolTip::showText(event->globalPos() - this->pos(), QString::fromLocal8Bit("当前坐标:%1").arg(local), this);

}
void SCRail::slot_mouseRelease2(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
		return;
	QPointF ChickedPoint = event->pos();
	if (!ui.qCustomPlot2->viewport().contains(event->pos()))
		return;
	double currentX = ui.qCustomPlot2->xAxis->pixelToCoord(ChickedPoint.x());
	double currentY = ui.qCustomPlot2->yAxis->pixelToCoord(ChickedPoint.y());
	QString local = QString("%1").arg(currentX) + QString(",%1").arg(currentY);
	QToolTip::showText(event->globalPos() - this->pos(), QString::fromLocal8Bit("当前坐标:%1").arg(local), this);

}
void SCRail::slot_mouseRelease3(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
		return;
	QPointF ChickedPoint = event->pos();
	if (!ui.qCustomPlot3->viewport().contains(event->pos()))
		return;
	double currentX = ui.qCustomPlot3->xAxis->pixelToCoord(ChickedPoint.x());
	double currentY = ui.qCustomPlot3->yAxis->pixelToCoord(ChickedPoint.y());
	QString local = QString("%1").arg(currentX) + QString(",%1").arg(currentY);
	QToolTip::showText(event->globalPos() - this->pos(), QString::fromLocal8Bit("当前坐标:%1").arg(local), this);

}
void SCRail::on_generate()
{
	
	QDateTime currentTime = QDateTime::currentDateTime();
	QString curDate = currentTime.toString("yyyy-MM-dd-hh-mm-ss");
	QString path[3] = { "\0" };
	path[0] = QString("./../../SCRail/importData/qCustomImage/") + curDate +  QString("1.png");
	path[1] = QString("./../../SCRail/importData/qCustomImage/") + curDate + QString("2.png");
	path[2] = QString("./../../SCRail/importData/qCustomImage/") + curDate + QString("3.png");
	ui.qCustomPlot2->savePng(path[0], 414, 250);
	ui.qCustomPlot4->savePng(path[1], 414, 250);
	ui.qCustomPlot5->savePng(path[2], 441, 171);
	QString mileage_num = QString::number(checkIn->_mileage[0] * 1000 + checkIn->_mileage[1] * 100 + checkIn->_mileage[2] * 10\
		+ checkIn->_mileage[3]) + QString(" + ") + QString::number(checkIn->_mileage[4] * 100 + checkIn->_mileage[5] * 10 + checkIn->_mileage[6]);
	QString totalWear = QString::number(int(metricalInfo->verticalHalf_wear * 100 + 0.5) / 100.0 + int(metricalInfo->horizontal_wear * 100 + 0.5) / 200.0) + QString("mm");
	printinfo = new PrintInfo(NULL, ui.lineCBox_2->currentText(), ui.lab_typeName->text(), ui.nameType->currentText(), ui.trackBox_2->currentText(), \
		ui.oversizeBox->currentText(), mileage_num, checkIn->_checkLocal, ui.Lab_verticalHalfWear2->text(), ui.Lab_verticalThirdsWear->text(), ui.Lab_horizontalWear->text(), totalWear,ui.Lab_maxOffset2->text(),ui.Lab_minOffset2->text(),ui.Lab_GQIValue->text(), path,diffValue);
	printinfo->exec();
}
void SCRail::readData()
{
	//memset(_raildata, 0, sizeof(railData_j));
	//QByteArray array;
	//m_socket.read((char*)&array, sizeof(array));
	/*qDebug() << "readData" << endl;
	tcpClient->read((char*)_raildata, sizeof(railData_j));
	_objRaildata = _raildata;
	/*QDataStream data_stream(tcpClient->read((char*)));
	data_stream.setVersion(QDataStream::Qt_5_7);
	if (statu)
	{
		data_stream >> Message;
		_raildata = (railData_j*)Message.data();
		statu = false;
		
	}*/
	if (compuComple == 1)
	{
		QByteArray buffer1;
		buffer1 = tcpClient->readAll();
		buffer += buffer1;
		//qDebug() << buffer.size() << endl;
		int recv_len = tcpClient->bytesAvailable();
		if (buffer.size() < sizeof(railData_j)) // 不能小于头字节数
			return;	// 如果没有读完的话，还会再次进入槽函数
		else {
			char *src = buffer.data();
			memcpy(_raildata, src, sizeof(railData_j));
			if (_raildata->dotcount > 1280)
				return;
			_objRaildata = _raildata;
			//qDebug() << "already read!!!" << endl;

			//qDebug() << _objRaildata.dotcount << endl;
			compuComple = 0;
			set_plotNormal(checkIn->_targetOversizeIndex);//检测界面及磨耗分析
			set_plot(ui.qCustomPlot3, checkIn->_targetOversizeIndex + 4);//打磨建议
			set_plot(checkIn->_targetOversizeIndex + 4, ui.qCustomPlot4, ui.qCustomPlot5, checkdata);//质量分析
			set_data();
			buffer.clear();
			compuComple = 1;

		}
	}
	
}
void SCRail::ReadError(QAbstractSocket::SocketError)
{
	tcpClient->disconnectFromHost();
	//const QString state_color = state_style + QString("red");
	ui.stateLight->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lightOut.png);");
	ui.stateLight_2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lightOut.png);");
	ui.stateLight_3->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/lightOut.png);");
	ui.btn_connectState->setText(QString::fromLocal8Bit("连    接")); 
	QMessageBox msgBox(this);
	msgBox.setText(QString::fromLocal8Bit("与服务器连接中断，中断信息为")+QString(tcpClient->errorString()));
	msgBox.exec();
}
void SCRail::set_reportAnalysis()
{
	this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg2_1.png)}");
	ui.stackedWidget->setCurrentIndex(1);
	ui.funcInterface->setCurrentIndex(3);
	ui.infoAnalysis->setCurrentIndex(0);
	ui.Tbn_analysisList->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/huifang2.png);");
	ui.Tbn_checkRealTime->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/checkImage1.png);");
	ui.menuChoose->setEnabled(false);
	ui.count_chooseBox->clear();
	_row.clear();
	set_reportTableView("./../../SCRail/importData/saveDataFolder");
}

void SCRail::choose_menu()
{
	set_reportTableView();
}
void SCRail::on_startAnalysis()
{
	if (_row.size() == 0)
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("未选中任何文件，分析失败!!!"), QMessageBox::Yes);
		return;
	}
	if (groupAnalysisType->checkedId())
	{
		if (_row.size() < 2)
		{
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("缺少可对比文件，分析失败!!!"), QMessageBox::Yes);
			return;
		}
	}
	if (0 == groupAnalysisType->checkedId())
	{
		QString filename = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->currentText();
		on_readInfo(filename.toLocal8Bit().data());
		ui.funcInterface->setCurrentIndex(4);
		ui.AnalysisDiff->setCurrentIndex(0);
		this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg3.png)}");
		QString path = "./../../SCRail/importData/GeneratedImage/" + ui.count_chooseBox->currentText().split('.').at(0) + QString(".png");
		QString pathGQI = "./../../SCRail/importData/GeneratedImage/" + ui.count_chooseBox->currentText().split('.').at(0) + QString("GQI.png");

		//WinExec("cmd /C \"del D:\\SHCRail\\SCRail\\SCRail\\importData\\GeneratedImage\\*.png\"", SW_HIDE);
		ui.qCustomPlot6->savePng(path, 701, 401);
		ui.qCustomPlot7->savePng(pathGQI, 701, 401);
	
	}
	else if (1 == groupAnalysisType->checkedId())
	{
		if (!on_analysisCheck(1))
			return;
		on_sort(ui.count_chooseBox,1);
		on_readCountInfo();
		if(4 != ui.funcInterface->currentIndex())
			ui.funcInterface->setCurrentIndex(4);
		ui.AnalysisDiff->setCurrentIndex(1);
		this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg4.png)}");
		
	}
	else if (2 == groupAnalysisType->checkedId())
	{
		if (!on_analysisCheck(2))
			return;
		on_sort(ui.count_chooseBox, 2);
		on_readCountInfoDiffPoint();
		if (4 != ui.funcInterface->currentIndex())
			ui.funcInterface->setCurrentIndex(4);
		ui.AnalysisDiff->setCurrentIndex(2);
		this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg4.png)}");
	}
	else if (3 == groupAnalysisType->checkedId())
	{
		on_sort(ui.count_chooseBox, 1);
		on_readCountInfoDiffRail();
		if (4 != ui.funcInterface->currentIndex())
			ui.funcInterface->setCurrentIndex(4);
		ui.AnalysisDiff->setCurrentIndex(3);
		this->setStyleSheet("#SCRailClass{border-image: url(./../../SCRail/importData/ImageFolder/bg5.png)}");
	}
}
void SCRail::sortByColumn(int column)
{
	if (!_row.isEmpty())
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("排序前请取消所有选中!!!"), QMessageBox::Yes);
		return;
	}
	QStringList selectRowLine, targetRowLine;//创建两个QStringList用于储存表格数据
	int colCount = 7;
	int rowCount = ui.tableWidget_2->rowCount();
	if (4 == column)
	{	
		if (SortUporDown)
		{
			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_2->item(j, 4)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_2->item(j, 4)->text().split('+').at(1).toDouble() / 1000\
				> ui.tableWidget_2->item(j + 1, 4)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_2->item(j, 4)->text().split('+').at(1).toDouble() / 1000)
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 1; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_2->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_2->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount-1; k++)
						{
							ui.tableWidget_2->setItem(j, k+1, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_2->setItem(j + 1, k+1, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 1; j < colCount; j++)
				{
					ui.tableWidget_2->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = false;
		}
		else
		{
			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_2->item(j, 4)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_2->item(j, 4)->text().split('+').at(1).toDouble() / 1000\
				< ui.tableWidget_2->item(j + 1, 4)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_2->item(j, 4)->text().split('+').at(1).toDouble() / 1000)
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 1; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_2->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_2->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount-1; k++)
						{
							ui.tableWidget_2->setItem(j, k+1, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_2->setItem(j + 1, k+1, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 1; j < colCount; j++)
				{
					ui.tableWidget_2->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = true;
		}
	}
	else
	{
		if (SortUporDown)
		{
			ui.tableWidget_2->sortItems(column, Qt::AscendingOrder);
			SortUporDown = false;
		}
		else
		{
			ui.tableWidget_2->sortItems(column, Qt::DescendingOrder);
			SortUporDown = true;
		}
	}
	
}
void SCRail::sortByColumn2(int column)
{
	QStringList selectRowLine, targetRowLine;//创建两个QStringList用于储存表格数据
	int colCount = 8;
	int rowCount = ui.tableWidget_4->rowCount();
	if (0 == column || 7 == column)
	{
		if (SortUporDown)
		{
			ui.tableWidget_4->sortItems(column, Qt::AscendingOrder);
			SortUporDown = false;
		}
		else
		{
			ui.tableWidget_4->sortItems(column, Qt::DescendingOrder);
			SortUporDown = true;
		}
		on_sortFont(ui.tableWidget_4, 1);
	}
	else
	{
		if (SortUporDown)
		{
			
			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_4->item(j,column)->text().toDouble() > ui.tableWidget_4->item(j+1, column)->text().toDouble())
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 0; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_4->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_4->item(j+1, k)->text());
						}
						for (int k = 0; k < colCount; k++)
						{
							ui.tableWidget_4->setItem(j, k, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_4->setItem(j+1,k, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for(int i = 0;i < rowCount; i++)
			{
				for (int j = 0; j < colCount; j++)
				{
					ui.tableWidget_4->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = false;
		}
		else
		{
			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_4->item(j, column)->text().toDouble() < ui.tableWidget_4->item(j + 1, column)->text().toDouble())
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 0; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_4->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_4->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount; k++)
						{
							ui.tableWidget_4->setItem(j, k, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_4->setItem(j + 1, k, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < colCount; j++)
				{
					ui.tableWidget_4->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = true;
		}
		on_sortFont(ui.tableWidget_4, 1);
	}
	
}
void SCRail::sortByColumn3(int column)
{
	QStringList selectRowLine, targetRowLine;//创建两个QStringList用于储存表格数据
	int colCount = 8;
	int rowCount = ui.tableWidget_6->rowCount();
	if (7 == column || 8 == column)
	{
		if (SortUporDown)
		{
			ui.tableWidget_6->sortItems(column, Qt::AscendingOrder);
			SortUporDown = false;
		}
		else
		{
			ui.tableWidget_6->sortItems(column, Qt::DescendingOrder);
			SortUporDown = true;
		}
		on_sortFont(ui.tableWidget_6, 2);
	}
	else if (0 == column)
	{
		if (SortUporDown)
		{
			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_6->item(j, 0)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_6->item(j, 0)->text().split('+').at(1).toDouble() / 1000\
				> ui.tableWidget_6->item(j + 1, 0)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_6->item(j, 0)->text().split('+').at(1).toDouble() / 1000)
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 0; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_6->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_6->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount; k++)
						{
							ui.tableWidget_6->setItem(j, k, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_6->setItem(j + 1, k, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < colCount; j++)
				{
					ui.tableWidget_6->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = false;
		}
		else
		{
			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_6->item(j, 0)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_6->item(j, 0)->text().split('+').at(1).toDouble() / 1000\
				< ui.tableWidget_6->item(j + 1, 0)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_6->item(j, 0)->text().split('+').at(1).toDouble() / 1000)
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 0; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_6->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_6->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount; k++)
						{
							ui.tableWidget_6->setItem(j, k, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_6->setItem(j + 1, k, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < colCount; j++)
				{
					ui.tableWidget_6->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = true;
		}
		on_sortFont(ui.tableWidget_6, 2);
	}
	else
	{
		if (SortUporDown)
		{

			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_6->item(j, column)->text().toDouble() > ui.tableWidget_6->item(j + 1, column)->text().toDouble())
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 0; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_6->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_6->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount; k++)
						{
							ui.tableWidget_6->setItem(j, k, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_6->setItem(j + 1, k, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < colCount; j++)
				{
					ui.tableWidget_6->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = false;
		}
		else
		{
			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_6->item(j, column)->text().toDouble() < ui.tableWidget_6->item(j + 1, column)->text().toDouble())
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 0; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_6->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_6->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount; k++)
						{
							ui.tableWidget_6->setItem(j, k, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_6->setItem(j + 1, k, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < colCount; j++)
				{
					ui.tableWidget_6->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = true;
		}
		on_sortFont(ui.tableWidget_6, 2);
	}

}
void SCRail::sortByColumn4(int column)
{
	QStringList selectRowLine, targetRowLine;//创建两个QStringList用于储存表格数据
	int colCount = 11;
	int rowCount = ui.tableWidget_8->rowCount();
	if (1 == column || 2 == column || 3 == column|| 11 == column || 12 == column)
	{
		if (SortUporDown)
		{
			ui.tableWidget_8->sortItems(column, Qt::AscendingOrder);
			SortUporDown = false;
		}
		else
		{
			ui.tableWidget_8->sortItems(column, Qt::DescendingOrder);
			SortUporDown = true;
		}
		on_sortFont(ui.tableWidget_8, 3);
	}
	else if (4 == column)
	{
		if (SortUporDown)
		{
			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_8->item(j, 4)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_8->item(j, 4)->text().split('+').at(1).toDouble() / 1000\
				> ui.tableWidget_8->item(j + 1, 4)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_8->item(j, 4)->text().split('+').at(1).toDouble() / 1000)
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 0; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_8->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_8->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount; k++)
						{
							ui.tableWidget_8->setItem(j, k, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_8->setItem(j + 1, k, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < colCount; j++)
				{
					ui.tableWidget_8->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = false;
		}
		else
		{
			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_8->item(j, 4)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_8->item(j, 4)->text().split('+').at(1).toDouble() / 1000\
						< ui.tableWidget_8->item(j + 1, 4)->text().split('+').at(0).section('K', 1, 1).toInt() + ui.tableWidget_8->item(j, 4)->text().split('+').at(1).toDouble() / 1000)
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 0; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_8->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_8->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount; k++)
						{
							ui.tableWidget_8->setItem(j, k, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_8->setItem(j + 1, k, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < colCount; j++)
				{
					ui.tableWidget_8->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = true;
		}
		on_sortFont(ui.tableWidget_8, 3);
	}
	else
	{
		if (SortUporDown)
		{

			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_8->item(j, column)->text().toDouble() > ui.tableWidget_8->item(j + 1, column)->text().toDouble())
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 0; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_8->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_8->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount; k++)
						{
							ui.tableWidget_8->setItem(j, k, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_8->setItem(j + 1, k, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < colCount; j++)
				{
					ui.tableWidget_8->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = false;
		}
		else
		{
			for (int i = 0; i < rowCount - 1; i++)
			{
				for (int j = 0; j < rowCount - i - 1; j++)
				{
					if (ui.tableWidget_8->item(j, column)->text().toDouble() < ui.tableWidget_8->item(j + 1, column)->text().toDouble())
					{
						selectRowLine.clear();
						targetRowLine.clear();
						for (int k = 0; k < colCount; k++)
						{
							selectRowLine.append(ui.tableWidget_8->item(j, k)->text());//获取文本，并添加进QStringLine
							targetRowLine.append(ui.tableWidget_8->item(j + 1, k)->text());
						}
						for (int k = 0; k < colCount; k++)
						{
							ui.tableWidget_8->setItem(j, k, new QTableWidgetItem(targetRowLine.at(k)));//设置文本
							ui.tableWidget_8->setItem(j + 1, k, new QTableWidgetItem(selectRowLine.at(k)));
						}
					}
				}

			}
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < colCount; j++)
				{
					ui.tableWidget_8->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				}
			}
			SortUporDown = true;
		}
		on_sortFont(ui.tableWidget_8, 3);
	}

	for (int i = 0; i < rowCount; i++)
	{
		ui.tableWidget_8->item(i, 0)->setText(QString::number(i + 1));
	}

	set_plotAnalysisDiffRail(ui.tableWidget_8);
	set_offsetPlotUpdate(ui.tableWidget_8);
}
void SCRail::on_analysisGenerate()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString curDate = currentTime.toString("yyyy-MM-dd-hh-mm-ss");
	QString table;
	table += "<!DOCTYPE html> <html> \
			<meta content=""text/html""; charset = ""utf - 8"">\
			<style type = ""text/qss"">\
				+ ""#div3{""border:5px pink solid;width:605px;height:20px;inline - block;background - color:Cyan;""}\
				 #div1, #div2{""border:5px yellow solid;width:300px;height:80px;inline - block;background - color:chartreuse;\
				float:left; margin-right: 10px; margin - top: 10px; ""}""</style>\
			<body> \
			<img src=""./../../SCRail/importData/ImageFolder/xintie.png "">";
	//首页
	table += "<br/> <br/> <br/> <br/> <br/>  <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/>  <br/> <br/> <br/> <br/> <br/> <br/> <br/>";
	table += "<h1 align =\"center\">" + ui.count_chooseBox->currentText().section('_',0,0) + QStringLiteral("钢轨廓形检测报告") + "</h1>";
	table += "<h2 align =\"center\">" + ui.count_chooseBox->currentText().section('_', 1, 1)/* + QStringLiteral("(上/下) 行")*/ + "</h2>";
	table += "<h2 align =\"center\">" + ui.count_chooseBox->currentText().section('_', 3, 3)  + "</h2>";
	table += "<br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/><br/>";
	table += "<h2 align =\"center\">" + QStringLiteral("上海欣铁机电科技有限公司") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("Shanghai XINTIE Mechanical & Electrical Technology") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("Co., Ltd.") + "</h2>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<h2 align =\"center\">" + curDate.section('-',0,0) + QString::fromLocal8Bit("年")+ curDate.section('-',1,1) + QString::fromLocal8Bit("月") + "</h2>";


	//第二页 廓形检测报告
	table += "<div>";
	table += "<h1 align =\"center\">" + ui.count_chooseBox->currentText().section('_', 0, 0) + QStringLiteral("钢轨廓形检测报告") + "</h1>";
	table += "<h2>" +QStringLiteral("说明：本检测报告中所运用的限值、定义、标准及方法主要是依据《高速铁路无\
													砟轨道线路维修规则》(试行)(铁运[2012]83号)、《高速铁路钢轨打磨管理办法》\
														(铁总运[2014]357号)及《上海铁路局高速铁路钢轨打磨实施细则》\
														（SHG/GW204-2015）等技术规章中的相关内容。") + "</h2>";
	table += "<h2>"  + ui.count_chooseBox->currentText().section('_', 4, 4).section('-', 0, 0)\
		+ QString::fromLocal8Bit("年") + ui.count_chooseBox->currentText().section('_', 4, 4).section('-', 1, 1) + QString::fromLocal8Bit("月") \
		+ ui.count_chooseBox->currentText().section('_', 4, 4).section('-', 2, 2)+QString::fromLocal8Bit("日") + QStringLiteral("对") + ui.count_chooseBox->currentText().section('_', 0, 0) + \
		ui.count_chooseBox->currentText().section('_', 1, 1) + ui.count_chooseBox->currentText().section('_', 3, 3)  + ui.count_chooseBox->currentText().section('_', 2, 2) + QStringLiteral("钢轨进行了钢轨廓形检测\
											，本次统计共有钢轨廓形1个。廓形检测统计分析结论如下：") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("1、钢轨廓形检测，详见以下章节") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   1) 本次统计的钢轨廓形垂磨(1/2)值为") + ui.Lab_verticalHalfWear_2->text() +QStringLiteral("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   2) 本次统计的钢轨廓形垂磨(1/3)值为") + ui.Lab_verticalThirdsWear_2->text() + QStringLiteral("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   3) 本次统计的钢轨廓形侧磨值为") + ui.Lab_horizontalWear_2->text() + QStringLiteral("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   4) 本次统计的钢轨廓形质量指数GQI值为") + ui.Lab_GQIValue_2->text() + QStringLiteral("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   5) 本次统计的钢轨廓形与") + ui.target_oversizeName_4->text() + QStringLiteral("廓形对比，") + "</h2>";
	table += "<h2 >" + QStringLiteral("       最大偏差值为") + ui.Lab_maxOffset_2->text() + QStringLiteral("；") + "</h2>";
	table += "<h2 >" + QStringLiteral("       最小偏差值为") + ui.Lab_minOffset_2->text() + QStringLiteral("；") + "</h2>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";
	table += "<h1>" + QStringLiteral("1.钢轨廓形磨耗分析") + "</h1>";
	table += "<h2>" + QStringLiteral("根据《高速铁路无砟轨道线路维修规则》中有关钢轨头部磨耗轻伤及重伤标准的要求:") + "</h2>";
	table += "<h2>" + QStringLiteral("当区间钢轨、导轨的侧磨大于10mm，垂磨大于8mm，总磨耗大于9mm时达到轻伤；当区间钢轨、导轨的侧磨大于12mm，垂磨大于10mm时达到重伤；") + "</h2>";
	table += "<h2>" + QStringLiteral("其中，总磨耗=垂磨+1/2侧磨。") + "</h2>";
	QString State = "\0";
	if (ui.Lab_verticalThirdsWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() > 8 || ui.Lab_horizontalWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() > 10 || \
		(ui.Lab_verticalThirdsWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() + ui.Lab_horizontalWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() / 2) > 9)
		State = QString::fromLocal8Bit("存在");
	else
		State = QString::fromLocal8Bit("不存在");
	table += "<h2>" + QStringLiteral("实测钢轨廓形中，以") + ui.target_oversizeName_4->text() + QStringLiteral("为目标廓形时，钢轨轨头") + State + QStringLiteral("磨耗超限，具体磨耗值如下表所示：") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("钢轨轨头磨耗统计表") + "</h2>";
	whileTable(table);
	if (State == QString::fromLocal8Bit("存在"))
	{
		table += "<h2>" + QStringLiteral("下图为1号钢轨廓形的轨头磨耗对比图，实测1号钢轨廓形的垂磨(1/2处)值为") + ui.Lab_verticalHalfWear_2->text() + QStringLiteral("；垂磨(1/3处)值为") + ui.Lab_verticalThirdsWear_2->text() + \
			QStringLiteral("侧模值为") + ui.Lab_horizontalWear_2->text() + QStringLiteral("；总磨耗值为") + QString::number((ui.Lab_verticalThirdsWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble()) +\
			(ui.Lab_horizontalWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() / 2)) + QString("mm,此处钢轨轨头存在磨耗超限现象。(下图为实例)") + "</h2>";
		whileImage(table);
	}
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";
	table += "<h1>" + QStringLiteral("2. 钢轨廓形质量对比分析") + "</h1>";
	table += "<h2>" + QStringLiteral("当实测钢轨廓形越接近于目标廓形，则其 GQI（Grinding Quality Index）指标评分指数越高，与目标廓形偏差值的绝对值就越小，其轮轨接触状态也越理想。\
		经分析计算钢轨廓形质量 GQI 指数以及实测钢轨廓形与") + ui.target_oversizeName_4->text() + QStringLiteral("廓形的偏差值可得如下表所示的实测钢轨廓形GQI评分和偏差值。") + "</h2>";
	table += "<h2>" + QStringLiteral("对比可得，实测钢轨廓形：") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("钢轨廓形质量指数GQI为") + ui.Lab_GQIValue_2->text() + QStringLiteral("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("最大偏差值(欠打磨)为") + ui.Lab_maxOffset_2->text() + QStringLiteral("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("最小偏差值(过打磨)为") + ui.Lab_minOffset_2->text() + QStringLiteral("；") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("实测廓形GQI和偏差值对比表") + "</h2>";
	whileTableGQI(table);
	table += "<h2>" + QStringLiteral("下图为1号实测钢轨廓形和") + ui.target_oversizeName_4->text() + QStringLiteral("廓形的对比。由实测钢轨廓形与") + \
		ui.target_oversizeName_4->text() + QStringLiteral("廓形相比较可得出实测钢轨质量指数GQI评分为") + ui.Lab_GQIValue_2->text() + QStringLiteral("。（下图为示例）") + "</h2>";
	whileImageGQI(table);
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";
	table += "<h1>" + QStringLiteral("3.廓形质量小结") + "</h1>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形以") + ui.target_oversizeName_4->text() + QStringLiteral("廓形为目标廓形时，检测值如下表所示：") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_4->text() + QStringLiteral("廓形检测值汇总表（单位：mm）") + "</h2>";
	whileTableGather(table, State);
	table += "<br/><br/><br/><br/><br/>";
	table += "<h2>" + QStringLiteral("下表为《高速铁路钢轨打磨管理办法》中区间钢轨，导轨的钢轨头部磨耗轻伤及重伤标准：") + "</h2>";
	QStringList title;
	title.push_back(QStringLiteral(""));
	title.push_back(QStringLiteral("总磨耗(mm)"));
	title.push_back(QStringLiteral("垂直磨耗(mm)"));
	title.push_back(QStringLiteral("侧面磨耗(mm)"));
	table += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	table += "<tr style=\"background-color:gray\">";
	for (int i = 0; i < 4; i++)
	{
		table += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("轻伤"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("9"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("8"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("10"));
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("重伤"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("/"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("10"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("12"));
	table += "</tr>";

	table += "</table>";
	table += "<h3>" + QStringLiteral("注：总磨耗=垂直磨耗+1/2侧面磨耗。");
	table += "<h2 align =\" center\">" + QStringLiteral("下表为《高速铁路钢轨打磨管理办法》中钢轨打磨轨头廓形验收标准(手工检测)的要求：") + "</h2>";
	title.clear();
	title.push_back(QStringLiteral("设计行车速度(km/h)"));
	title.push_back(QStringLiteral("轨头横向-25mm～+25mm范围内\n(mm)"));
	title.push_back(QStringLiteral("轨头横向25mm～32mm范围内\n(mm)"));
	table += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	table += "<tr style=\"background-color:gray\">";
	for (int i = 0; i < 3; i++)
	{
		table += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("＜200"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.3 / -0.3"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.6"));
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("≥200"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.2"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.6"));
	table += "</tr>";
	table += "</table>";
	table += "<h3>" + QStringLiteral("注：廓形验收范围为钢轨轨头横向-25mm至32mm。表中+、-分别表示所测廓形高于和低于目标廓形的量值，下表同。") + "</h3>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";
	table += "<h1>" + QStringLiteral("4.钢轨廓形打磨量") + "</h1>";
	table += "<h2>" + QStringLiteral("将实测廓形与") + ui.target_oversizeName_4->text() + QStringLiteral("廓形对比，统计实测廓形与") + ui.target_oversizeName_4->text() + \
		QStringLiteral("廓形各个角度的法向差值(各角度处的廓形误差值)。正值代表实测廓形欠打磨(需要打磨的量值，而负值代表实测廓形过打磨(打磨过度的量值)。)") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/ImageFolder/") + QString("NVector.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2>" + QStringLiteral("以下图表是") + ui.lineName_5->text() + QStringLiteral(",") + ui.trackName_4->text() + QStringLiteral(",") + ui.mileageName_4->text() + QStringLiteral(",")\
		+ ui.count_chooseBox->currentText().section('_', 2, 2) + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_4->text() + QStringLiteral("廓型的对比图及差值表。") + "</h2>";
	table += "<h2 align =\"center\">"  + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_4->text() + QStringLiteral("廓形对比图") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + ui.count_chooseBox->currentText().split('.').at(0) + QString("GQI.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2 align =\"center\">"+ QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比表") + "</h2>";
	on_analysisTable(table, _diffValue);
	QMessageBox msg(this);
	msg.setWindowTitle("Warnning");
	msg.setText(QString::fromLocal8Bit("已成功生成报表!!!"));
	msg.setIcon(QMessageBox::Information);
	msg.setIcon(QMessageBox::NoIcon);
	msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Ok | QMessageBox::No);
	msg.setButtonText(QMessageBox::Ok, QString::fromLocal8Bit("保 存"));
	msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("另存为"));
	msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
	int rel = msg.exec();
	if (rel == QMessageBox::No)
	{
		return;
	}
	else if (rel == QMessageBox::Ok)
	{
		QPrinter *printer = new  QPrinter;
		printer->setPageSize(QPrinter::A4);
		printer->setOutputFormat(QPrinter::PdfFormat);
		printer->setOutputFileName(("./../../SCRail/importData/GeneratedPDF/" + ui.count_chooseBox->currentText().section('.', 0, 0) + ".pdf"));
		QTextDocument *text = new QTextDocument;
		text->setHtml(table);
		text->print(printer);
		text->end();
		delete printer;
		printer = NULL;
		delete text;
		text = NULL;
	}
	else if (rel == QMessageBox::Yes)
	{
		QString path = "./../../SCRail/importData/GeneratedPDF/" + ui.count_chooseBox->currentText().section('.', 0, 0) + ".pdf";
		QString fileName = QFileDialog::getSaveFileName(this,
			QString::fromLocal8Bit("文件另存为"),
			path,
			tr("Config Files (*.pdf)"));
		QPrinter *printer = new  QPrinter;
		printer->setPageSize(QPrinter::A4);
		printer->setOutputFormat(QPrinter::PdfFormat);
		printer->setOutputFileName(fileName);
		QTextDocument *text = new QTextDocument;
		text->setHtml(table);
		text->print(printer);
		text->end();
		delete printer;
		printer = NULL;
		delete text;
		text = NULL;
	}
	
	
}
void SCRail::on_analysisGenerateRailPoint()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString curDate = currentTime.toString("yyyy-MM-dd-hh-mm-ss");
	QString table;
	table += "<br/>";
	table += "<!DOCTYPE html> <html> \
			<meta content=""text/html""; charset = ""utf - 8"">\
			<body> \
			<img src=""./../../SCRail/importData/ImageFolder/xintie.png "">";
	//首页
	table += "<br/> <br/> <br/> <br/> <br/>  <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/>  <br/> <br/> <br/> <br/> <br/> <br/> <br/>";
	if (1 == groupAnalysisType->checkedId())
	{
		table += "<h1 align =\"center\">" + ui.lineName_6->text() + QStringLiteral("钢轨廓形检测报告") + "</h1>";
		table += "<h2 align =\"center\">" + ui.trackName_5->text() /*+ QStringLiteral("(上/下) 行")*/ + "</h2>";
		table += "<h2 align =\"center\">" + ui.mileageName_5->text() + "</h2>";
	}
	for (int i = 0; i < 12; i++)
	{
		table += "<br/>";
	}
	
	table += "<h2 align =\"center\">" + QStringLiteral("上海欣铁机电科技有限公司") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("Shanghai XINTIE Mechanical & Electrical Technology") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("Co., Ltd.") + "</h2>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<h2 align =\"center\">" + curDate.section('-', 0, 0) + QString::fromLocal8Bit("年") + curDate.section('-', 1, 1) + QString::fromLocal8Bit("月") + "</h2>";
	table += "<h1 align =\"center\">" + ui.lineName_6->text() + QStringLiteral("钢轨廓形检测报告") + "</h1>";
	table += "<h2>" + QStringLiteral("说明：本检测报告中所运用的限值、定义、标准及方法主要是依据《高速铁路无\
													砟轨道线路维修规则》(试行)(铁运[2012]83号)、《高速铁路钢轨打磨管理办法》\
														(铁总运[2014]357号)及《上海铁路局高速铁路钢轨打磨实施细则》\
														（SHG/GW204-2015）等技术规章中的相关内容。") + "</h2>";
	table += "<h2>";
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{
		if (0 == i)
		{
			table += ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 0, 0) + QString::fromLocal8Bit("年") + ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 1, 1) + QString::fromLocal8Bit("月") \
				+ ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 2, 2) + QString::fromLocal8Bit("日,");
			continue;
		}
		else {
			QString timeLast = ui.count_chooseBox->itemText(i - 1).section('_', 4, 4).section('-', 0, 0) + QString::fromLocal8Bit("年") + ui.count_chooseBox->itemText(i - 1).section('_', 4, 4).section('-', 1, 1) + QString::fromLocal8Bit("月") \
				+ ui.count_chooseBox->itemText(i - 1).section('_', 4, 4).section('-', 2, 2) + QString::fromLocal8Bit("日");
			QString time = ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 0, 0) + QString::fromLocal8Bit("年") + ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 1, 1) + QString::fromLocal8Bit("月") \
				+ ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 2, 2) + QString::fromLocal8Bit("日");
			if (time == timeLast)
				continue;
			table += time;
		}
	}
	table += "<h2 align =\"left\">" + QStringLiteral("对") + ui.lineName_6->text() + QStringLiteral(",")+ ui.trackName_5->text() + QStringLiteral(",") + ui.mileageName_5->text() + QStringLiteral(",")\
		+ ui.trackName_6->text() + QStringLiteral("钢轨进行了钢轨廓形检测,本次统计共有钢轨廓形") + QString::number(ui.count_chooseBox->count()) + QStringLiteral("个。廓形检测统计分析结论如下：") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("1、钢轨廓形检测，详见以下章节") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   1) 本次统计的钢轨廓形垂磨(1/2)均值为") + QString::number(_avgValue.verticalHalf_wear) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   2) 本次统计的钢轨廓形垂磨(1/3)均值为") + QString::number(_avgValue.verticalThirds_wear) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   3) 本次统计的钢轨廓形侧磨均值为") + QString::number(_avgValue.horizontal_wear) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   4) 本次统计的钢轨廓形质量指数GQI均值为") + QString::number(_avgValue.GQI_value) + QString::fromLocal8Bit("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   5) 本次统计的钢轨廓形与") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比，") + "</h2>";
	table += "<h2 >" + QStringLiteral("       最大偏差均值为") + QString::number(_avgValue.max_offset) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 >" + QStringLiteral("       最小偏差均值为") + QString::number(_avgValue.min_offset) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += " <br/>";

	table += "<h1>" + QStringLiteral("1.钢轨廓形磨耗分析") + "</h1>";
	table += "<h2>" + QStringLiteral("根据《高速铁路无砟轨道线路维修规则》中有关钢轨头部磨耗轻伤及重伤标准的要求:") + "</h2>";
	table += "<h2>" + QStringLiteral("当区间钢轨、导轨的侧磨大于10mm，垂磨大于8mm，总磨耗大于9mm时达到轻伤；当区间钢轨、导轨的侧磨大于12mm，垂磨大于10mm时达到重伤；") + "</h2>";
	table += "<h2>" + QStringLiteral("其中，总磨耗=垂磨+1/2侧磨。") + "</h2>";
	QString State = "\0";
	for (int i = 0; i < ui.tableWidget_4->rowCount(); i++)
	{
		if ((ui.tableWidget_4->item(i, 3)->text().toDouble() > 10) || (ui.tableWidget_4->item(i, 2)->text().toDouble() > 8) || ((ui.tableWidget_4->item(i, 3)->text().toDouble() / 2 + ui.tableWidget_4->item(i, 2)->text().toDouble()) > 9))
		{
			State = QString::fromLocal8Bit("存在");
			break;
		}	
		else
		{
			State = QString::fromLocal8Bit("不存在");
		}
	}
	table += "<h2>" + QStringLiteral("实测钢轨廓形中，以") + ui.target_oversizeName_5->text() + QStringLiteral("为目标廓形时，钢轨轨头") + State + QStringLiteral("磨耗超限，具体磨耗值如下表所示：") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("钢轨轨头磨耗统计表") + "</h2>";
	whileTable(table,ui.tableWidget_4);
	if (State == QString::fromLocal8Bit("存在"))
	{
		for (int i = 0; i < ui.tableWidget_4->rowCount(); i++)
		{
			if ((ui.tableWidget_4->item(i, 3)->text().toDouble() > 10) || (ui.tableWidget_4->item(i, 2)->text().toDouble() > 8) || ((ui.tableWidget_4->item(i, 3)->text().toDouble() / 2 + ui.tableWidget_4->item(i, 2)->text().toDouble()) > 9))
			{
				table += "<h2>" + QStringLiteral("下图为") + QString::number(i+1)+ QStringLiteral("号钢轨廓形的轨头磨耗对比图，实测1号钢轨廓形的垂磨(1/2处)值为") + ui.tableWidget_4->item(i, 1)->text() + QStringLiteral("；垂磨(1/3处)值为") + ui.tableWidget_4->item(i, 2)->text() + \
					QStringLiteral("侧模值为") + ui.tableWidget_4->item(i, 3)->text() + QStringLiteral("；总磨耗值为") + QString::number(ui.tableWidget_4->item(i, 3)->text().toDouble() / 2 + ui.tableWidget_4->item(i, 2)->text().toDouble()) \
					 + QStringLiteral("mm,此处钢轨轨头存在磨耗超限现象。(下图为实例)") + "</h2>";
				whileImage(table,ui.tableWidget_4,i);
			}
		}
	}
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";
	table += "<h1>" + QStringLiteral("2. 钢轨廓形质量对比分析") + "</h1>";
	table += "<h2>" + QStringLiteral("当实测钢轨廓形越接近于目标廓形，则其 GQI（Grinding Quality Index）指标评分指数越高，与目标廓形偏差值的绝对值就越小，其轮轨接触状态也越理想。\
		经分析计算钢轨廓形质量 GQI 指数以及实测钢轨廓形与") + ui.target_oversizeName_5->text() + QStringLiteral("廓形的偏差值可得如下表所示的实测钢轨廓形GQI评分和偏差值。") + "</h2>";
	table += "<h2>" + QStringLiteral("对比可得，实测钢轨廓形：") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("钢轨廓形质量指数GQI均值为") + QString::number(_avgValue.GQI_value) + QStringLiteral("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("最大偏差值(欠打磨)均值为") + QString::number(_avgValue.max_offset) + QStringLiteral("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("最小偏差值(过打磨)均值为") + QString::number(_avgValue.min_offset) + QStringLiteral("mm。") + "</h2>";
	table += "<br/>";
	table += "<h2 align =\"center\">" + QStringLiteral("实测廓形GQI和偏差值对比表") + "</h2>";
	whileTableGQI(table,ui.tableWidget_4);
	for (int i = 0; i < ui.tableWidget_4->rowCount(); i++)
	{
		table += "<h2>" + QStringLiteral("下图为") + QString::number(i + 1) + QStringLiteral("号实测钢轨廓形和") + ui.target_oversizeName_5->text() + QStringLiteral("廓形的对比。由实测钢轨廓形与") + \
			ui.target_oversizeName_5->text() + QStringLiteral("廓形相比较可得出实测钢轨质量指数GQI评分为") + ui.tableWidget_4->item(i,6)->text() + QStringLiteral("(下图为实例)") + "</h2>";
		whileImageGQI(table, ui.tableWidget_4, i);
		
	}
	table += "<br/><br/><br/>";
	table += "<h2>" + QStringLiteral("下图为实测钢轨廓形GQI随检测时间变化的分布图。") + "</h2>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形质量指数GQI均值为") + QString::number(_avgValue.GQI_value) + QStringLiteral("(下图为实例)") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") +  QString("GQIValue.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2 align = \"center\">" + QStringLiteral("实测钢轨廓形GQI随检测时间变化的分布图") + "</h2>";
	table += "<br/><br/><br/>";

	table += "<h2>" + QStringLiteral("下图为实测钢轨廓形与") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比最大偏差值(欠打磨)随检测时间变化的分布图。") + "</h2>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比最大偏差值(欠打磨)均值为") + QString::number(_avgValue.max_offset) + QStringLiteral("mm。") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + QString("maxOffset.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2 align = \"center\">" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比") + "</h2>";
	table += "<h2 align = \"center\">" + QStringLiteral("最大偏差值随检测时间变化的分布图") + "</h2>"; 

	table += "<br/><br/><br/>";
	table += "<h2>" + QStringLiteral("下图为实测钢轨廓形与") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比最小偏差值(过打磨)随检测时间变化的分布图。") + "</h2>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比最大偏差值(欠打磨)均值为") + QString::number(_avgValue.min_offset) + QStringLiteral("mm。") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + QString("minOffset.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2 align = \"center\">" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比") + "</h2>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<h2 align = \"center\">" + QStringLiteral("最小偏差值随检测时间变化的分布图") + "</h2>";
	
	table += "<h1>" + QStringLiteral("3.廓形质量小结") + "</h1>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形以") + ui.target_oversizeName_5->text() + QStringLiteral("廓形为目标廓形时，检测值如下表所示：") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_5->text() + QStringLiteral("廓形检测值汇总表（单位：mm）") + "</h2>";
	whileTableGather(table, ui.tableWidget_4, State);
	table += "<br/><br/><br/><br/><br/>";
	table += "<h2>" + QStringLiteral("下表为《高速铁路钢轨打磨管理办法》中区间钢轨，导轨的钢轨头部磨耗轻伤及重伤标准：") + "</h2>";
	QStringList title;
	title.push_back(QStringLiteral(""));
	title.push_back(QStringLiteral("总磨耗(mm)"));
	title.push_back(QStringLiteral("垂直磨耗(mm)"));
	title.push_back(QStringLiteral("侧面磨耗(mm)"));
	table += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	table += "<tr style=\"background-color:gray\">";
	for (int i = 0; i < 4; i++)
	{
		table += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("轻伤"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("9"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("8"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("10"));
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("重伤"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("/"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("10"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("12"));
	table += "</tr>";

	table += "</table>";
	table += "<h3>" + QStringLiteral("注：总磨耗=垂直磨耗+1/2侧面磨耗。");
	table += "<h2 align =\" center\">" + QStringLiteral("下表为《高速铁路钢轨打磨管理办法》中钢轨打磨轨头廓形验收标准(手工检测)的要求：") + "</h2>";
	title.clear();
	title.push_back(QStringLiteral("设计行车速度(km/h)"));
	title.push_back(QStringLiteral("轨头横向-25mm～+25mm范围内\n(mm)"));
	title.push_back(QStringLiteral("轨头横向25mm～32mm范围内\n(mm)"));
	table += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	table += "<tr style=\"background-color:gray\">";
	for (int i = 0; i < 3; i++)
	{
		table += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("＜200"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.3 / -0.3"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.6"));
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("≥200"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.2"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.6"));
	table += "</tr>";
	table += "</table>";
	table += "<h3>" + QStringLiteral("注：廓形验收范围为钢轨轨头横向-25mm至32mm。表中+、-分别表示所测廓形高于和低于目标廓形的量值，下表同。") + "</h3>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/><br/><br/><br/><br/>";
	
	table += "<h1>" + QStringLiteral("4.钢轨廓形打磨量") + "</h1>";
	table += "<h2>" + QStringLiteral("将实测廓形与") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比，统计实测廓形与") + ui.target_oversizeName_5->text() + \
		QStringLiteral("廓形各个角度的法向差值(各角度处的廓形误差值)。正值代表实测廓形欠打磨(需要打磨的量值，而负值代表实测廓形过打磨(打磨过度的量值)。)") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/ImageFolder/") + QString("NVector.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2>" + QStringLiteral("以下图表是不同时间在") + ui.lineName_6->text() + QStringLiteral(",") + ui.trackName_5->text() + QStringLiteral(",") + ui.mileageName_5->text() + QStringLiteral(",")\
		+ ui.trackName_6->text() + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_5->text() + QStringLiteral("廓型的对比图及差值表。") + "</h2>";
	for (int i = 0; i < ui.tableWidget_4->rowCount(); i++)
	{
		table += "<h2 align =\"center\">" + ui.tableWidget_4->item(i, 0)->text() + QStringLiteral("处实测钢轨廓形和") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比图") + "</h2>";
		table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + ui.tableWidget_4->item(i, 0)->text() + QString("GQI.png") + " width=\"526\" height=\"301\"></p>";
		table += "<h2 align =\"center\">" + ui.tableWidget_4->item(i, 0)->text() + QStringLiteral("处实测钢轨廓形和") + ui.target_oversizeName_5->text() + QStringLiteral("廓形对比表") + "</h2>";
		on_analysisTable(table,_diffValue,i);
	}
	table += "<h2 align =\"center\">" + ui.lineName_6->text() + QStringLiteral(",") + ui.trackName_5->text() + QStringLiteral(",") + ui.mileageName_5->text() + QStringLiteral(",")\
		+ ui.trackName_6->text() + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_5->text() + QStringLiteral("廓型差值汇总表(单位:mm)") + "</h2>";
	
	table += "<table  width=\"650\" border=\"1\" cellspacing=\"-1\" cellpadding=\"0\" align=\"left\" style=\"font-size:14px;\" style=\"border-collapse;\" bordercolor=\"gray\">";

	table += "<tr style=\"background-color:gray\">";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("序号"));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if(j!=0)
				table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[i][j]));
		}
			
	}
	table += "</tr>";
	char temp[50] = "\0";
	for (int i = 0; i < ui.tableWidget_4->rowCount(); i++)
	{
		if(1 == i%2)
			table += "<tr style=\"background-color:gray\">";
		else
			table += "<tr>";
		table += QString("<td align =\"center\">%1</td>").arg(QString::number(i+1));
		for (int j = 0; j < 21; j++)
		{
				sprintf(temp, "%.2f", _diffValue.at(i)[j]);
				table += QString("<td align =\"center\">%1</td>").arg(temp);
			
		}
		table += "</tr>";
	}
	table += "</table>";
	table += "<h3>" + QStringLiteral("注:");
	for (int i = 0; i < ui.tableWidget_4->rowCount(); i++)
	{
		table += QStringLiteral("序号") + QString::number(i + 1)+ QString(":") + ui.tableWidget_4->item(i, 0)->text() + QString(";");
	}
	table += "</h3>";
	
	table += "</div>";

	QMessageBox msg(this);
	msg.setWindowTitle("Warnning");
	msg.setText(QString::fromLocal8Bit("已成功生成报表!!!"));
	msg.setIcon(QMessageBox::Information);
	msg.setIcon(QMessageBox::NoIcon);
	msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Ok | QMessageBox::No);
	msg.setButtonText(QMessageBox::Ok, QString::fromLocal8Bit("保 存"));
	msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("另存为"));
	msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
	int rel = msg.exec();
	if (rel == QMessageBox::No)
	{
		return;
	}
	else if (rel == QMessageBox::Ok)
	{
		QString Name = ui.lineName_6->text() + QStringLiteral("_") + ui.trackName_5->text() + QStringLiteral("_") + ui.trackName_6->text() + QStringLiteral("_") + ui.mileageName_5->text() + QStringLiteral("_") + curDate;
		QString path = "./../../SCRail/importData/GeneratedPDF/" + Name + ".pdf";
		QPrinter *printer = new  QPrinter;
		printer->setPageSize(QPrinter::A4);
		printer->setOutputFormat(QPrinter::PdfFormat);
		printer->setOutputFileName(path);
		QTextDocument *text = new QTextDocument;
		QFont font(QString::fromLocal8Bit("宋体"), 10);
		text->setDefaultFont(font);
		text->setHtml(table);
		text->print(printer);
		text->end();
		delete printer;
		printer = NULL;
		delete text;
		text = NULL;
	}
	else if (rel == QMessageBox::Yes)
	{
		QString Name = ui.lineName_6->text() + QStringLiteral("_") + ui.trackName_5->text() + QStringLiteral("_") + ui.trackName_6->text() + QStringLiteral("_") + ui.mileageName_5->text() + QStringLiteral("_") + curDate;
		QString path = "./../../SCRail/importData/GeneratedPDF/" + Name + ".pdf";
		QString fileName = QFileDialog::getSaveFileName(this,
			QString::fromLocal8Bit("文件另存为"),
			path,
			tr("Config Files (*.pdf)"));
		QPrinter *printer = new  QPrinter;
		printer->setPageSize(QPrinter::A4);
		printer->setOutputFormat(QPrinter::PdfFormat);
		printer->setOutputFileName(fileName);
		QTextDocument *text = new QTextDocument;
		QFont font(QString::fromLocal8Bit("宋体"), 10);
		text->setDefaultFont(font);
		text->setHtml(table);
		text->print(printer);
		text->end();
		delete printer;
		printer = NULL;
		delete text;
		text = NULL;
	}
}
void SCRail::on_analysisGenerateRailDiffPoint()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString curDate = currentTime.toString("yyyy-MM-dd-hh-mm-ss");
	QString table;
	table += "<!DOCTYPE html> <html> \
			<meta content=""text/html""; charset = ""utf - 8"">\
			<body> \
			<img src=""./../../SCRail/importData/ImageFolder/xintie.png "">";
	//首页
	table += "<br/> <br/> <br/> <br/> <br/>  <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/>  <br/> <br/> <br/> <br/> <br/> <br/> <br/>";
	table += "<h1 align =\"center\">" + ui.lineName_7->text() + QStringLiteral("钢轨廓形检测报告") + "</h1>";
	table += "<h2 align =\"center\">" + ui.trackName_7->text() /*+ QStringLiteral("(上/下) 行")*/ + "</h2>";
	table += "<h2 align =\"center\">"+ ui.tableWidget_6->item(0,0)->text() + QStringLiteral("(");
	for (int i = 1; i < ui.tableWidget_6->rowCount(); i++)
	{
		if ( !(ui.tableWidget_6->item(i - 1, 0)->text() == ui.tableWidget_6->item(i, 0)->text()))
		{
			table += ui.tableWidget_6->item(i, 0)->text() + QStringLiteral("、");
		}
	}
	table.chop(1);
	table += QStringLiteral(")")  + "</h2>";
	
	for (int i = 0; i < 12; i++)
	{
		table += "<br/>";
	}

	table += "<h2 align =\"center\">" + QStringLiteral("上海欣铁机电科技有限公司") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("Shanghai XINTIE Mechanical & Electrical Technology") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("Co., Ltd.") + "</h2>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<h2 align =\"center\">" + curDate.section('-', 0, 0) + QString::fromLocal8Bit("年") + curDate.section('-', 1, 1) + QString::fromLocal8Bit("月") + "</h2>";
	
	table += "<h1 align =\"center\">" + ui.lineName_7->text() + QStringLiteral("钢轨廓形检测报告") + "</h1>";
	table += "<h2>" + QStringLiteral("说明：本检测报告中所运用的限值、定义、标准及方法主要是依据《高速铁路无\
													砟轨道线路维修规则》(试行)(铁运[2012]83号)、《高速铁路钢轨打磨管理办法》\
														(铁总运[2014]357号)及《上海铁路局高速铁路钢轨打磨实施细则》\
														（SHG/GW204-2015）等技术规章中的相关内容。") + "</h2>";
	table += "<h2>";
	on_sort(ui.count_chooseBox, 1);
	int temp = 0;
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{
		if (0 == i)
		{
			table += ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 0, 0) + QString::fromLocal8Bit("年") + ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 1, 1) + QString::fromLocal8Bit("月") \
				+ ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 2, 2) + QString::fromLocal8Bit("日,");
		}
		else {
			QString time = ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 0, 0) + QString::fromLocal8Bit("年") + ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 1, 1) + QString::fromLocal8Bit("月") \
				+ ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 2, 2) + QString::fromLocal8Bit("日");
			for (int j = 0; j < i; j++)
			{
				if (time == ui.count_chooseBox->itemText(j).section('_', 4, 4).section('-', 0, 0) + QString::fromLocal8Bit("年") + ui.count_chooseBox->itemText(j).section('_', 4, 4).section('-', 1, 1) + QString::fromLocal8Bit("月") \
					+ ui.count_chooseBox->itemText(j).section('_', 4, 4).section('-', 2, 2) + QString::fromLocal8Bit("日"))
				{
					temp = 1;
					break;
				}
			}
			if (temp == 0)
			{
				table += time;
			}
			temp = 0;
		}
	}
	table += QStringLiteral("对") + ui.lineName_7->text() + QStringLiteral(",") + ui.trackName_7->text() + QStringLiteral(",");
	on_sort(ui.count_chooseBox, 2);
	table += ui.count_chooseBox->itemText(0).section('_', 3, 3) + QStringLiteral("、");
	for (int i = 1; i < ui.count_chooseBox->count(); i++)
	{
		if (!(ui.count_chooseBox ->itemText(i-1).section('_',3,3) == ui.count_chooseBox->itemText(i).section('_',3,3)))
		{
			table += ui.count_chooseBox->itemText(i).section('_', 3, 3) + QStringLiteral("、");
		}
	}
	table.chop(1);
	table += ui.trackName_8->text() + QStringLiteral("钢轨进行了钢轨廓形检测,本次统计共有钢轨廓形") + QString::number(ui.count_chooseBox->count()) + QStringLiteral("个。廓形检测统计分析结论如下：") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("1、钢轨廓形检测，详见以下章节") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   1) 本次统计的钢轨廓形垂磨(1/2)均值为") + QString::number(_avgValue.verticalHalf_wear) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   2) 本次统计的钢轨廓形垂磨(1/3)均值为") + QString::number(_avgValue.verticalThirds_wear) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   3) 本次统计的钢轨廓形侧磨均值为") + QString::number(_avgValue.horizontal_wear) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   4) 本次统计的钢轨廓形质量指数GQI均值为") + QString::number(_avgValue.GQI_value) + QString::fromLocal8Bit("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   5) 本次统计的钢轨廓形与") + ui.target_oversizeName_6->text() + QStringLiteral("廓形对比，") + "</h2>";
	table += "<h2 >" + QStringLiteral(" 1        最大偏差均值为") + QString::number(_avgValue.max_offset) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 >" + QStringLiteral(" 1        最小偏差均值为") + QString::number(_avgValue.min_offset) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";

	table += "<h1>" + QStringLiteral("1.钢轨廓形磨耗分析") + "</h1>";
	table += "<h2>" + QStringLiteral("根据《高速铁路无砟轨道线路维修规则》中有关钢轨头部磨耗轻伤及重伤标准的要求:") + "</h2>";
	table += "<h2>" + QStringLiteral("当区间钢轨、导轨的侧磨大于10mm，垂磨大于8mm，总磨耗大于9mm时达到轻伤；当区间钢轨、导轨的侧磨大于12mm，垂磨大于10mm时达到重伤；") + "</h2>";
	table += "<h2>" + QStringLiteral("其中，总磨耗=垂磨+1/2侧磨。") + "</h2>";
	QString State = "\0";
	for (int i = 0; i < ui.tableWidget_6->rowCount(); i++)
	{
		if ((ui.tableWidget_6->item(i, 3)->text().toDouble() > 10) || (ui.tableWidget_6->item(i, 2)->text().toDouble() > 8) || ((ui.tableWidget_6->item(i, 3)->text().toDouble() / 2 + ui.tableWidget_6->item(i, 2)->text().toDouble()) > 9))
		{
			State = QString::fromLocal8Bit("存在");
			break;
		}
		else
		{
			State = QString::fromLocal8Bit("不存在");
		}
	}
	table += "<h2>" + QStringLiteral("实测钢轨廓形中，以") + ui.target_oversizeName_6->text() + QStringLiteral("为目标廓形时，钢轨轨头") + State + QStringLiteral("磨耗超限，具体磨耗值如下表所示：") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("钢轨轨头磨耗统计表") + "</h2>";
	whileTableRailDiffPoint(table, ui.tableWidget_6);
	if (State == QString::fromLocal8Bit("存在"))
	{
		for (int i = 0; i < ui.tableWidget_6->rowCount(); i++)
		{
			if ((ui.tableWidget_6->item(i, 3)->text().toDouble() > 10) || (ui.tableWidget_6->item(i, 2)->text().toDouble() > 8) || ((ui.tableWidget_6->item(i, 3)->text().toDouble() / 2 + ui.tableWidget_6->item(i, 2)->text().toDouble()) > 9))
			{
				table += "<h2>" + QStringLiteral("下图为") + QString::number(i + 1) + QStringLiteral("号钢轨廓形的轨头磨耗对比图，实测1号钢轨廓形的垂磨(1/2处)值为") + ui.tableWidget_6->item(i, 1)->text() + QStringLiteral("；垂磨(1/3处)值为") + ui.tableWidget_6->item(i, 2)->text() + \
					QStringLiteral("侧模值为") + ui.tableWidget_6->item(i, 3)->text() + QStringLiteral("；总磨耗值为") + QString::number(ui.tableWidget_6->item(i, 3)->text().toDouble() / 2 + ui.tableWidget_6->item(i, 2)->text().toDouble()) \
					+ QStringLiteral("mm,此处钢轨轨头存在磨耗超限现象。(下图为实例)") + "</h2>";
				table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + ui.tableWidget_6->item(i, 8)->text() + QString(".png") + " width=\"526\" height=\"301\"></p>";
				table += "<h2 align = \"center\">" + QString::number(i + 1) + QStringLiteral("号钢轨廓形的轨头磨耗对比图") + "</h2>";
			}
		}
	}
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";
	table += "<h1>" + QStringLiteral("2. 钢轨廓形质量对比分析") + "</h1>";
	table += "<h2>" + QStringLiteral("当实测钢轨廓形越接近于目标廓形，则其 GQI（Grinding Quality Index）指标评分指数越高，与目标廓形偏差值的绝对值就越小，其轮轨接触状态也越理想。\
		经分析计算钢轨廓形质量 GQI 指数以及实测钢轨廓形与") + ui.target_oversizeName_6->text() + QStringLiteral("廓形的偏差值可得如下表所示的实测钢轨廓形GQI评分和偏差值。") + "</h2>";
	table += "<h2>" + QStringLiteral("对比可得，实测钢轨廓形：") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("钢轨廓形质量指数GQI均值为") + QString::number(_avgValue.GQI_value) + QStringLiteral("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("最大偏差值(欠打磨)均值为") + QString::number(_avgValue.max_offset) + QStringLiteral("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("最小偏差值(过打磨)均值为") + QString::number(_avgValue.min_offset) + QStringLiteral("mm。") + "</h2>";
	table += "<br/>";
	table += "<h2 align =\"center\">" + QStringLiteral("实测廓形GQI和偏差值对比表") + "</h2>";
	whileTableGQIRailDiffPoint(table, ui.tableWidget_6);
	for (int i = 0; i < ui.tableWidget_6->rowCount(); i++)
	{
		table += "<h2>" + QStringLiteral("下图为") + QString::number(i + 1) + QStringLiteral("号实测钢轨廓形和") + ui.target_oversizeName_6->text() + QStringLiteral("廓形的对比。由实测钢轨廓形与") + \
			ui.target_oversizeName_6->text() + QStringLiteral("廓形相比较可得出实测钢轨质量指数GQI评分为") + ui.tableWidget_6->item(i, 6)->text() + QStringLiteral("(下图为实例)") + "</h2>";
		table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + ui.tableWidget_6->item(i, 8)->text() + QString("GQI.png") + " width=\"526\" height=\"301\"></p>";
		table += "<h2 align = \"center\">" + QString::number(i + 1) + QStringLiteral("号钢轨廓形GQI图") + "</h2>";

	}
	table += "<br/><br/><br/>";
	table += "<h2 align =\"center\">" + QStringLiteral("下图为实测钢轨廓形GQI随检测里程变化的分布图。") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("实测钢轨廓形质量指数GQI均值为") + QString::number(_avgValue.GQI_value) + QStringLiteral("(下图为实例)") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + QString("GQIValue.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2 align = \"center\">" + QStringLiteral("实测钢轨廓形GQI随检测里程变化的分布图") + "</h2>";
	table += "<br/><br/><br/>";

	table += "<h2>" + QStringLiteral("下图为实测钢轨廓形与") + ui.target_oversizeName_6->text() + QStringLiteral("廓形对比最大偏差值(欠打磨)随检测里程变化的分布图。") + "</h2>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_6->text() + QStringLiteral("廓形对比最大偏差值(欠打磨)均值为") + QString::number(_avgValue.max_offset) + QStringLiteral("mm。") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + QString("maxOffsetDiffPoint.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2 align = \"center\">" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_6->text() + QStringLiteral("廓形对比") + "</h2>";
	table += "<h2 align = \"center\">" + QStringLiteral("最大偏差值随检测里程变化的分布图") + "</h2>";

	table += "<br/><br/><br/>";
	table += "<h2>" + QStringLiteral("下图为实测钢轨廓形与") + ui.target_oversizeName_6->text() + QStringLiteral("廓形对比最小偏差值(过打磨)随检测里程变化的分布图。") + "</h2>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_6->text() + QStringLiteral("廓形对比最大偏差值(欠打磨)均值为") + QString::number(_avgValue.min_offset) + QStringLiteral("mm。") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + QString("minOffsetDiffPoint.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2 align = \"center\">" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_6->text() + QStringLiteral("廓形对比") + "</h2>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<h2 align = \"center\">" + QStringLiteral("最小偏差值随检测里程变化的分布图") + "</h2>";

	table += "<h1>" + QStringLiteral("3.廓形质量小结") + "</h1>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形以") + ui.target_oversizeName_6->text() + QStringLiteral("廓形为目标廓形时，检测值如下表所示：") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_6->text() + QStringLiteral("廓形检测值汇总表（单位：mm）") + "</h2>";
	whileTableGatherDiffPoint(table, ui.tableWidget_6, State);
	table += "<br/><br/><br/><br/><br/>";
	table += "<h2>" + QStringLiteral("下表为《高速铁路钢轨打磨管理办法》中区间钢轨，导轨的钢轨头部磨耗轻伤及重伤标准：") + "</h2>";
	QStringList title;
	title.push_back(QStringLiteral(""));
	title.push_back(QStringLiteral("总磨耗(mm)"));
	title.push_back(QStringLiteral("垂直磨耗(mm)"));
	title.push_back(QStringLiteral("侧面磨耗(mm)"));
	table += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	table += "<tr style=\"background-color:gray\">";
	for (int i = 0; i < 4; i++)
	{
		table += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("轻伤"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("9"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("8"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("10"));
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("重伤"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("/"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("10"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("12"));
	table += "</tr>";

	table += "</table>";
	table += "<h3>" + QStringLiteral("注：总磨耗=垂直磨耗+1/2侧面磨耗。");
	table += "<h2 align =\" center\">" + QStringLiteral("下表为《高速铁路钢轨打磨管理办法》中钢轨打磨轨头廓形验收标准(手工检测)的要求：") + "</h2>";
	title.clear();
	title.push_back(QStringLiteral("设计行车速度(km/h)"));
	title.push_back(QStringLiteral("轨头横向-25mm～+25mm范围内\n(mm)"));
	title.push_back(QStringLiteral("轨头横向25mm～32mm范围内\n(mm)"));
	table += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	table += "<tr style=\"background-color:gray\">";
	for (int i = 0; i < 3; i++)
	{
		table += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("＜200"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.3 / -0.3"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.6"));
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("≥200"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.2"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.6"));
	table += "</tr>";
	table += "</table>";
	table += "<h3>" + QStringLiteral("注：廓形验收范围为钢轨轨头横向-25mm至32mm。表中+、-分别表示所测廓形高于和低于目标廓形的量值，下表同。") + "</h3>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";
	table += "<h1>" + QStringLiteral("4.钢轨廓形打磨量") + "</h1>";
	table += "<h2>" + QStringLiteral("将实测廓形与") + ui.target_oversizeName_6->text() + QStringLiteral("廓形对比，统计实测廓形与") + ui.target_oversizeName_6->text() + \
		QStringLiteral("廓形各个角度的法向差值(各角度处的廓形误差值)。正值代表实测廓形欠打磨(需要打磨的量值，而负值代表实测廓形过打磨(打磨过度的量值)。)") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/ImageFolder/") + QString("NVector.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2>" + QStringLiteral("以下图表是在") + ui.lineName_7->text() + QStringLiteral(",") + ui.trackName_7->text() + QStringLiteral("在不同的点的,")\
		+ ui.trackName_7->text() + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_6->text() + QStringLiteral("廓型的对比图及差值表。") + "</h2>";
	for (int i = 0; i < ui.tableWidget_6->rowCount(); i++)
	{
		table += "<h2 align =\"center\">" + ui.tableWidget_6->item(i, 8)->text() + ui.tableWidget_6->item(i, 0)->text() + QStringLiteral("处实测钢轨廓形和") + ui.target_oversizeName_6->text() + QStringLiteral("廓形对比图") + "</h2>";
		table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + ui.tableWidget_6->item(i, 8)->text() + QString("GQI.png") + " width=\"526\" height=\"301\"></p>";
		table += "<h2 align =\"center\">" + ui.tableWidget_6->item(i, 0)->text() + QStringLiteral("处实测钢轨廓形和") + ui.target_oversizeName_6->text() + QStringLiteral("廓形对比表") + "</h2>";
		on_analysisTable(table, _diffValue, i);
	}
	table += "<h2 align =\"center\">" + ui.lineName_7->text() + QStringLiteral(",") + ui.trackName_7->text() + QStringLiteral(",") + QStringLiteral(",")\
		+ ui.trackName_8->text() + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_6->text() + QStringLiteral("廓型差值在不同里程的汇总表(单位:mm)") + "</h2>";

	table += "<table  width=\"650\" border=\"1\" cellspacing=\"-1\" cellpadding=\"0\" align=\"left\" style=\"font-size:14px;\" style=\"border-collapse;\" bordercolor=\"gray\">";

	table += "<tr style=\"background-color:gray\">";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("序号"));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			
			if (j != 0) {
				QString::fromLocal8Bit(table_info[i][j]).chop(1);
				table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[i][j]));
			}
				
		}

	}
	table += "</tr>";
	char temp1[50] = "\0";
	for (int i = 0; i < ui.tableWidget_6->rowCount(); i++)
	{
		if (1 == i % 2)
			table += "<tr style=\"background-color:gray\">";
		else
			table += "<tr>";
		table += QString("<td align =\"center\">%1</td>").arg(QString::number(i + 1));
		for (int j = 0; j < 21; j++)
		{
			sprintf(temp1, "%.2f", _diffValue.at(i)[j]);
			table += QString("<td align =\"center\">%1</td>").arg(temp1);

		}
		table += "</tr>";
	}
	table += "</table>";
	table += "<h3>" + QStringLiteral("注:");
	for (int i = 0; i < ui.tableWidget_6->rowCount(); i++)
	{
		table += QStringLiteral("序号") + QString::number(i + 1) + QString(":") + ui.tableWidget_6->item(i, 0)->text() + QString(";");
	}
	table += "</h3>";

	table += "</div>";

	QMessageBox msg(this);
	msg.setWindowTitle("Warnning");
	msg.setText(QString::fromLocal8Bit("已成功生成报表!!!"));
	msg.setIcon(QMessageBox::Information);
	msg.setIcon(QMessageBox::NoIcon);
	msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Ok | QMessageBox::No);
	msg.setButtonText(QMessageBox::Ok, QString::fromLocal8Bit("保 存"));
	msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("另存为"));
	msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
	int rel = msg.exec();
	if (rel == QMessageBox::No)
	{
		return;
	}
	else if (rel == QMessageBox::Ok)
	{
		
		QString Name = ui.lineName_7->text() + QStringLiteral("_") + ui.trackName_7->text() + QStringLiteral("_") + ui.trackName_8->text() + QStringLiteral("_") + ui.count_chooseBox->currentText().section('_', 3, 3) + QStringLiteral("_") + curDate;
		QString path = "./../../SCRail/importData/GeneratedPDF/" + Name + ".pdf";
		QPrinter *printer = new  QPrinter;
		printer->setPageSize(QPrinter::A4);
		printer->setOutputFormat(QPrinter::PdfFormat);
		printer->setOutputFileName(path);
		QTextDocument *text = new QTextDocument;
		QFont font(QString::fromLocal8Bit("宋体"), 10);
		text->setDefaultFont(font);
		text->setHtml(table);
		text->print(printer);
		text->end();
		delete printer;
		printer = NULL;
		delete text;
		text = NULL;
	}
	else if (rel == QMessageBox::Yes)
	{
		QString Name = ui.lineName_7->text() + QStringLiteral("_") + ui.trackName_7->text() + QStringLiteral("_") + ui.trackName_8->text() + QStringLiteral("_") + ui.count_chooseBox->currentText().section('_', 3, 3) + QStringLiteral("_") + curDate;
		QString path = "./../../SCRail/importData/GeneratedPDF/" + Name + ".pdf";
		QString fileName = QFileDialog::getSaveFileName(this,
			QString::fromLocal8Bit("文件另存为"),
			path,
			tr("Config Files (*.pdf)"));
		QPrinter *printer = new  QPrinter;
		printer->setPageSize(QPrinter::A4);
		printer->setOutputFormat(QPrinter::PdfFormat);
		printer->setOutputFileName(fileName);
		QTextDocument *text = new QTextDocument;
		QFont font(QString::fromLocal8Bit("宋体"), 10);
		text->setDefaultFont(font);
		text->setHtml(table);
		text->print(printer);
		text->end();
		delete printer;
		printer = NULL;
		delete text;
		text = NULL;
	}

}
void SCRail::on_analysisGenerateDiffRail()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	QString curDate = currentTime.toString("yyyy-MM-dd-hh-mm-ss");
	QString table;
	int temp = 0;
	table += "<!DOCTYPE html> <html> \
			<meta content=""text/html""; charset = ""utf - 8"">\
			<body> \
			<img src=""./../../SCRail/importData/ImageFolder/xintie.png "">";
	//首页
	table += "<br/> <br/> <br/> <br/> <br/>  <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/> <br/>  <br/> <br/> <br/> <br/> <br/> <br/> <br/>";
	table += "<h1 align =\"center\">";
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{

		if (0 == i)
		{
			table += ui.count_chooseBox->itemText(i).section('_', 0, 0) + QStringLiteral("、");
		}
		else {
			QString time = ui.count_chooseBox->itemText(i).section('_', 0, 0);
			for (int j = 0; j < i; j++)
			{
				if (time == ui.count_chooseBox->itemText(j).section('_', 0, 0))
				{
					temp = 1;
					break;
				}
			}
			if (temp == 0)
			{
				table += time + QStringLiteral("、");
			}
			temp = 0;
		}
	}
	table.chop(1);
	table += QStringLiteral("钢轨廓形检测报告") + "</h1>";
	//table += "<h2 align =\"center\">" + ui.tableWidget_8->item(2, 0)->text() /*+ QStringLiteral("(上/下) 行")*/ + "</h2>";
	on_sort(ui.count_chooseBox, 2);
	table += "<h2 align =\"center\">" + ui.count_chooseBox->itemText(0).section('_',3,3) + QStringLiteral("(");
	for (int i = 1; i < ui.count_chooseBox->count(); i++)
	{
		if (!(ui.count_chooseBox->itemText(i-1).section('_', 3, 3) == ui.count_chooseBox->itemText(i).section('_', 3, 3)))
		{
			table += ui.count_chooseBox->itemText(i).section('_', 3, 3) + QStringLiteral("、");
		}
	}
	table.chop(1);
	table += QStringLiteral(")") + "</h2>";

	for (int i = 0; i < 12; i++)
	{
		table += "<br/>";
	}

	table += "<h2 align =\"center\">" + QStringLiteral("上海欣铁机电科技有限公司") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("Shanghai XINTIE Mechanical & Electrical Technology") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("Co., Ltd.") + "</h2>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<h2 align =\"center\">" + curDate.section('-', 0, 0) + QString::fromLocal8Bit("年") + curDate.section('-', 1, 1) + QString::fromLocal8Bit("月") + "</h2>";

	table += "<h1 align =\"center\">";
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{

		if (0 == i)
		{
			table += ui.count_chooseBox->itemText(i).section('_', 0, 0) + QStringLiteral("、");
		}
		else {
			QString time = ui.count_chooseBox->itemText(i).section('_', 0, 0);
			for (int j = 0; j < i; j++)
			{
				if (time == ui.count_chooseBox->itemText(j).section('_', 0, 0))
				{
					temp = 1;
					break;
				}
			}
			if (temp == 0)
			{
				table += time + QStringLiteral("、");
			}
			temp = 0;
		}
	}
	table.chop(1);
	table += QStringLiteral("钢轨廓形检测报告") + "</h1>";
	table += "<h2>" + QStringLiteral("说明：本检测报告中所运用的限值、定义、标准及方法主要是依据《高速铁路无\
													砟轨道线路维修规则》(试行)(铁运[2012]83号)、《高速铁路钢轨打磨管理办法》\
														(铁总运[2014]357号)及《上海铁路局高速铁路钢轨打磨实施细则》\
														（SHG/GW204-2015）等技术规章中的相关内容。") + "</h2>";
	table += "<h2>";
	on_sort(ui.count_chooseBox, 1);
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{
		if (0 == i)
		{
			table += ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 0, 0) + QString::fromLocal8Bit("年") + ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 1, 1) + QString::fromLocal8Bit("月") \
				+ ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 2, 2) + QString::fromLocal8Bit("日,");
		}
		else {
			QString time = ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 0, 0) + QString::fromLocal8Bit("年") + ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 1, 1) + QString::fromLocal8Bit("月") \
				+ ui.count_chooseBox->itemText(i).section('_', 4, 4).section('-', 2, 2) + QString::fromLocal8Bit("日");
			for (int j = 0; j < i; j++)
			{
				if (time == ui.count_chooseBox->itemText(j).section('_', 4, 4).section('-', 0, 0) + QString::fromLocal8Bit("年") + ui.count_chooseBox->itemText(j).section('_', 4, 4).section('-', 1, 1) + QString::fromLocal8Bit("月") \
					+ ui.count_chooseBox->itemText(j).section('_', 4, 4).section('-', 2, 2) + QString::fromLocal8Bit("日"))
				{
					temp = 1;
					break;
				}		
			}
			if (temp == 0)
			{
				table += time;
			}
			temp = 0;
		}
	}
	table += QStringLiteral("对");
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{

		if (0 == i)
		{
			table += ui.count_chooseBox->itemText(i).section('_', 0, 0) + QStringLiteral("、");
		}
		else {
			QString time = ui.count_chooseBox->itemText(i).section('_', 0, 0);
			for (int j = 0; j < i; j++)
			{
				if (time == ui.count_chooseBox->itemText(j).section('_', 0, 0))
				{
					temp = 1;
					break;
				}
			}
			if (temp == 0)
			{
				table += time +QStringLiteral("、");
			}
			temp = 0;
		}
	}
	table.chop(1);
	table += QStringLiteral(",");
	on_sort(ui.count_chooseBox, 2);
	table += ui.count_chooseBox->itemText(0).section('_', 3, 3) + QStringLiteral("、");
	for (int i = 1; i < ui.count_chooseBox->count(); i++)
	{
		if (!(ui.count_chooseBox->itemText(i - 1).section('_', 3, 3) == ui.count_chooseBox->itemText(i).section('_', 3, 3)))
		{
			table += ui.count_chooseBox->itemText(i).section('_', 3, 3) + QStringLiteral("、");
		}
	}
	table.chop(1);
	table += QStringLiteral("钢轨进行了钢轨廓形检测,本次统计共有钢轨廓形") + QString::number(ui.count_chooseBox->count()) + QStringLiteral("个。廓形检测统计分析结论如下：") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("1、钢轨廓形检测，详见以下章节") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   1) 本次统计的钢轨廓形垂磨(1/2)均值为") + QString::number(_avgValue.verticalHalf_wear) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   2) 本次统计的钢轨廓形垂磨(1/3)均值为") + QString::number(_avgValue.verticalThirds_wear) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   3) 本次统计的钢轨廓形侧磨均值为") + QString::number(_avgValue.horizontal_wear) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   4) 本次统计的钢轨廓形质量指数GQI均值为") + QString::number(_avgValue.GQI_value) + QString::fromLocal8Bit("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("   5) 本次统计的钢轨廓形与") + ui.target_oversizeName_7->text() + QStringLiteral("廓形对比，") + "</h2>";
	table += "<h2 >" + QStringLiteral("        最大偏差均值为") + QString::number(_avgValue.max_offset) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "<h2 >" + QStringLiteral("        最小偏差均值为") + QString::number(_avgValue.min_offset) + QString::fromLocal8Bit("mm；") + "</h2>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";


	table += "<h1>" + QStringLiteral("1.钢轨廓形磨耗分析") + "</h1>";
	table += "<h2>" + QStringLiteral("根据《高速铁路无砟轨道线路维修规则》中有关钢轨头部磨耗轻伤及重伤标准的要求:") + "</h2>";
	table += "<h2>" + QStringLiteral("当区间钢轨、导轨的侧磨大于10mm，垂磨大于8mm，总磨耗大于9mm时达到轻伤；当区间钢轨、导轨的侧磨大于12mm，垂磨大于10mm时达到重伤；") + "</h2>";
	table += "<h2>" + QStringLiteral("其中，总磨耗=垂磨+1/2侧磨。") + "</h2>";
	QString State = "\0";
	for (int i = 0; i < ui.tableWidget_8->rowCount(); i++)
	{
		if ((ui.tableWidget_8->item(i, 6)->text().toDouble() > 10) || (ui.tableWidget_8->item(i, 5)->text().toDouble() > 8) || ((ui.tableWidget_8->item(i, 6)->text().toDouble() / 2 + ui.tableWidget_8->item(i, 5)->text().toDouble()) > 9))
		{
			State = QString::fromLocal8Bit("存在");
			break;
		}
		else
		{
			State = QString::fromLocal8Bit("不存在");
		}
	}
	table += "<h2>" + QStringLiteral("实测钢轨廓形中，以") + ui.target_oversizeName_7->text() + QStringLiteral("为目标廓形时，钢轨轨头") + State + QStringLiteral("磨耗超限，具体磨耗值如下表所示：") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("钢轨轨头磨耗统计表") + "</h2>";
	whileTableDiffRail(table, ui.tableWidget_8);
	if (State == QString::fromLocal8Bit("存在"))
	{
		for (int i = 0; i < ui.tableWidget_8->rowCount(); i++)
		{
			if ((ui.tableWidget_8->item(i, 7)->text().toDouble() > 10) || (ui.tableWidget_8->item(i, 6)->text().toDouble() > 8) || ((ui.tableWidget_8->item(i, 7)->text().toDouble() / 2 + ui.tableWidget_8->item(i, 6)->text().toDouble()) > 9))
			{
				table += "<h2>" + QStringLiteral("下图为") + QString::number(i + 1) + QStringLiteral("号钢轨廓形的轨头磨耗对比图，实测1号钢轨廓形的垂磨(1/2处)值为") + ui.tableWidget_8->item(i, 5)->text() + QStringLiteral("；垂磨(1/3处)值为") + ui.tableWidget_8->item(i, 6)->text() + \
					QStringLiteral("侧模值为") + ui.tableWidget_8->item(i, 7)->text() + QStringLiteral("；总磨耗值为") + QString::number(ui.tableWidget_8->item(i, 7)->text().toDouble() / 2 + ui.tableWidget_8->item(i, 6)->text().toDouble()) \
					+ QStringLiteral("mm,此处钢轨轨头存在磨耗超限现象。(下图为实例)") + "</h2>";
				table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + ui.tableWidget_8->item(i, 12)->text() + QString(".png") + " width=\"526\" height=\"301\"></p>";
				table += "<h2 align = \"center\">" + QString::number(i + 1) + QStringLiteral("号钢轨廓形的轨头磨耗对比图") + "</h2>";
			}
		}
	}
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";

	table += "<h1>" + QStringLiteral("2. 钢轨廓形质量对比分析") + "</h1>";
	table += "<h2>" + QStringLiteral("当实测钢轨廓形越接近于目标廓形，则其 GQI（Grinding Quality Index）指标评分指数越高，与目标廓形偏差值的绝对值就越小，其轮轨接触状态也越理想。\
		经分析计算钢轨廓形质量 GQI 指数以及实测钢轨廓形与") + ui.target_oversizeName_7->text() + QStringLiteral("廓形的偏差值可得如下表所示的实测钢轨廓形GQI评分和偏差值。") + "</h2>";
	table += "<h2>" + QStringLiteral("对比可得，实测钢轨廓形：") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("钢轨廓形质量指数GQI均值为") + QString::number(_avgValue.GQI_value) + QStringLiteral("；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("最大偏差值(欠打磨)均值为") + QString::number(_avgValue.max_offset) + QStringLiteral("mm；") + "</h2>";
	table += "<h2 align =\"left\">" + QStringLiteral("最小偏差值(过打磨)均值为") + QString::number(_avgValue.min_offset) + QStringLiteral("mm。") + "</h2>";
	table += "<br/>";
	table += "<h2 align =\"center\">" + QStringLiteral("实测廓形GQI和偏差值对比表") + "</h2>";
	whileTableGQIDiffRail(table, ui.tableWidget_8);
	for (int i = 0; i < ui.tableWidget_8->rowCount(); i++)
	{
		table += "<h2>" + QStringLiteral("下图为") + QString::number(i + 1) + QStringLiteral("号实测钢轨廓形和") + ui.target_oversizeName_7->text() + QStringLiteral("廓形的对比。由实测钢轨廓形与") + \
			ui.target_oversizeName_7->text() + QStringLiteral("廓形相比较可得出实测钢轨质量指数GQI评分为") + ui.tableWidget_8->item(i, 10)->text() + QStringLiteral("(下图为实例)") + "</h2>";
		table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + ui.tableWidget_8->item(i, 12)->text() + QString("GQIValue.png") + " width=\"526\" height=\"301\"></p>";
		table += "<h2 align = \"center\">" + QString::number(i + 1) + QStringLiteral("号钢轨廓形GQI图") + "</h2>";

	}
	table += "<br/><br/><br/>";
	table += "<h2 align =\"center\">" + QStringLiteral("下图为实测钢轨廓形GQI随检测序号变化的分布图。") + "</h2>";
	ui.qCustomPlot18->savePng(QString("./../../SCRail/importData/GeneratedImage/") + QString("GQIValue.png"), 621, 361);
	table += "<h2 align =\"center\">" + QStringLiteral("实测钢轨廓形质量指数GQI均值为") + QString::number(_avgValue.GQI_value) + QStringLiteral("(下图为实例)") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + QString("GQIValue.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2 align = \"center\">" + QStringLiteral("实测钢轨廓形GQI随检测序号变化的分布图") + "</h2>";
	table += "<br/><br/><br/>";

	table += "<h2>" + QStringLiteral("下图为实测钢轨廓形与") + ui.target_oversizeName_7->text() + QStringLiteral("廓形对比最大偏差值(欠打磨)随检测序号变化的分布图。") + "</h2>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_7->text() + QStringLiteral("廓形对比最大偏差值(欠打磨)均值为") + QString::number(_avgValue.max_offset) + QStringLiteral("mm。") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + QString("maxOffsetDiffRail.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2 align = \"center\">" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_7->text() + QStringLiteral("廓形对比") + "</h2>";
	table += "<h2 align = \"center\">" + QStringLiteral("最大偏差值随检测序号变化的分布图") + "</h2>";

	table += "<br/><br/><br/>";
	table += "<h2>" + QStringLiteral("下图为实测钢轨廓形与") + ui.target_oversizeName_7->text() + QStringLiteral("廓形对比最小偏差值(过打磨)随检测序号变化的分布图。") + "</h2>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_7->text() + QStringLiteral("廓形对比最大偏差值(欠打磨)均值为") + QString::number(_avgValue.min_offset) + QStringLiteral("mm。") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + QString("minOffsetDiffRail.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2 align = \"center\">" + QStringLiteral("实测钢轨廓形与") + ui.target_oversizeName_7->text() + QStringLiteral("廓形对比") + "</h2>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<h2 align = \"center\">" + QStringLiteral("最小偏差值随检测序号变化的分布图") + "</h2>";

	table += "<h1>" + QStringLiteral("3.廓形质量小结") + "</h1>";
	table += "<h2>" + QStringLiteral("实测钢轨廓形以") + ui.target_oversizeName_7->text() + QStringLiteral("廓形为目标廓形时，检测值如下表所示：") + "</h2>";
	table += "<h2 align =\"center\">" + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_7->text() + QStringLiteral("廓形检测值汇总表（单位：mm）") + "</h2>";
	whileTableGatherDiffRail(table, ui.tableWidget_8, State);
	table += "<br/><br/><br/><br/><br/>";
	table += "<h2>" + QStringLiteral("下表为《高速铁路钢轨打磨管理办法》中区间钢轨，导轨的钢轨头部磨耗轻伤及重伤标准：") + "</h2>";
	QStringList title;
	title.push_back(QStringLiteral(""));
	title.push_back(QStringLiteral("总磨耗(mm)"));
	title.push_back(QStringLiteral("垂直磨耗(mm)"));
	title.push_back(QStringLiteral("侧面磨耗(mm)"));
	table += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	table += "<tr style=\"background-color:gray\">";
	for (int i = 0; i < 4; i++)
	{
		table += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("轻伤"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("9"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("8"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("10"));
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("重伤"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("/"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("10"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("12"));
	table += "</tr>";

	table += "</table>";
	table += "<h3>" + QStringLiteral("注：总磨耗=垂直磨耗+1/2侧面磨耗。");
	table += "<h2 align =\" center\">" + QStringLiteral("下表为《高速铁路钢轨打磨管理办法》中钢轨打磨轨头廓形验收标准(手工检测)的要求：") + "</h2>";
	title.clear();
	title.push_back(QStringLiteral("设计行车速度(km/h)"));
	title.push_back(QStringLiteral("轨头横向-25mm～+25mm范围内\n(mm)"));
	title.push_back(QStringLiteral("轨头横向25mm～32mm范围内\n(mm)"));
	table += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	table += "<tr style=\"background-color:gray\">";
	for (int i = 0; i < 3; i++)
	{
		table += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("＜200"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.3 / -0.3"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.6"));
	table += "</tr>";
	table += "<tr>";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("≥200"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.2"));
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("+0.2 / -0.6"));
	table += "</tr>";
	table += "</table>";
	table += "<h3>" + QStringLiteral("注：廓形验收范围为钢轨轨头横向-25mm至32mm。表中+、-分别表示所测廓形高于和低于目标廓形的量值，下表同。") + "</h3>";
	table += "</div>";
	table += "<div style = \"page-break-after: always;\">";
	table += "<br/>";

	table += "<h1>" + QStringLiteral("4.钢轨廓形打磨量") + "</h1>";
	table += "<h2>" + QStringLiteral("将实测廓形与") + ui.target_oversizeName_7->text() + QStringLiteral("廓形对比，统计实测廓形与") + ui.target_oversizeName_7->text() + \
		QStringLiteral("廓形各个角度的法向差值(各角度处的廓形误差值)。正值代表实测廓形欠打磨(需要打磨的量值，而负值代表实测廓形过打磨(打磨过度的量值)。)") + "</h2>";
	table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/ImageFolder/") + QString("NVector.png") + " width=\"526\" height=\"301\"></p>";
	table += "<h2>" + QStringLiteral("以下图表是在")+ QStringLiteral("在不同轨道的,") + QStringLiteral("实测钢轨廓形和") + ui.target_oversizeName_7->text() + QStringLiteral("廓型的对比图及差值表。") + "</h2>";
	for (int i = 0; i < ui.tableWidget_8->rowCount(); i++)
	{
		table += "<h2 align =\"center\">" + ui.tableWidget_8->item(i, 12)->text() + ui.tableWidget_8->item(i, 1)->text()+ QStringLiteral(",")+ ui.tableWidget_8->item(i, 2)->text() + ui.tableWidget_8->item(i, 4)->text()+ ui.tableWidget_8->item(i, 3)->text() + QStringLiteral("处实测钢轨廓形和") + ui.target_oversizeName_7->text() + QStringLiteral("廓形对比图") + "</h2>";
		table += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + ui.tableWidget_8->item(i, 12)->text() + QString("GQIValue.png") + " width=\"526\" height=\"301\"></p>";
		table += "<h2 align =\"center\">" + ui.tableWidget_8->item(i, 1)->text() + QStringLiteral(",") + ui.tableWidget_8->item(i, 2)->text() + ui.tableWidget_8->item(i, 4)->text() + ui.tableWidget_8->item(i, 3)->text() + QStringLiteral("处实测钢轨廓形和") + ui.target_oversizeName_7->text() + QStringLiteral("廓形对比表") + "</h2>";
		on_analysisTable(table, _diffValue, i);
	}

	table += "<h2 align =\"center\">";
	for (int i = 1; i < ui.count_chooseBox->count(); i++)
	{

		if (0 == i)
		{
			table += ui.count_chooseBox->itemText(i).section('_', 0, 0) + QStringLiteral("、");
		}
		else {
			QString time = ui.count_chooseBox->itemText(i).section('_', 0, 0);
			for (int j = 0; j < i; j++)
			{
				if (time == ui.count_chooseBox->itemText(j).section('_', 0, 0))
				{
					temp = 1;
					break;
				}
			}
			if (temp == 0)
			{
				table += time + QStringLiteral("、");
			}
			temp = 0;
		}
	}
	table.chop(1);
	table +=  QStringLiteral("廓型差值的汇总表(单位:mm)") + "</h2>";

	table += "<table  width=\"650\" border=\"1\" cellspacing=\"-1\" cellpadding=\"0\" align=\"left\" style=\"font-size:14px;\" style=\"border-collapse;\" bordercolor=\"gray\">";

	table += "<tr style=\"background-color:gray\">";
	table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("序号"));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j++)
		{

			if (j != 0) {
				QString::fromLocal8Bit(table_info[i][j]).chop(1);
				table += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[i][j]));
			}

		}

	}
	table += "</tr>";
	char temp1[50] = "\0";
	for (int i = 0; i < ui.tableWidget_8->rowCount(); i++)
	{
		if (1 == i % 2)
			table += "<tr style=\"background-color:gray\">";
		else
			table += "<tr>";
		table += QString("<td align =\"center\">%1</td>").arg(QString::number(i + 1));
		for (int j = 0; j < 21; j++)
		{
			sprintf(temp1, "%.2f", _diffValue.at(i)[j]);
			table += QString("<td align =\"center\">%1</td>").arg(temp1);
		}
		table += "</tr>";
	}
	table += "</table>";
	table += "</div>";
	QMessageBox msg(this);
	msg.setWindowTitle("Warnning");
	msg.setText(QString::fromLocal8Bit("已成功生成报表!!!"));
	msg.setIcon(QMessageBox::Information);
	msg.setIcon(QMessageBox::NoIcon);
	msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Ok | QMessageBox::No);
	msg.setButtonText(QMessageBox::Ok, QString::fromLocal8Bit("保 存"));
	msg.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("另存为"));
	msg.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取 消"));
	int rel = msg.exec();
	if (rel == QMessageBox::No)
	{
		return;
	}
	else if (rel == QMessageBox::Ok)
	{
		QString Name = QStringLiteral("异轨异点") + QStringLiteral("_") + curDate;
		QString path = "./../../SCRail/importData/GeneratedPDF/" + Name + ".pdf";
		QPrinter *printer = new  QPrinter;
		printer->setPageSize(QPrinter::A4);
		printer->setOutputFormat(QPrinter::PdfFormat);
		printer->setOutputFileName(path);
		QTextDocument *text = new QTextDocument;
		QFont font(QString::fromLocal8Bit("宋体"), 10);
		text->setDefaultFont(font);
		text->setHtml(table);
		text->print(printer);
		text->end();
		delete printer;
		printer = NULL;
		delete text;
		text = NULL;
	}
	else if (rel == QMessageBox::Yes)
	{
		QString Name = QStringLiteral("异轨异点") + QStringLiteral("_") + curDate;
		QString path = "./../../SCRail/importData/GeneratedPDF/" + Name + ".pdf";
		QString fileName = QFileDialog::getSaveFileName(this,
			QString::fromLocal8Bit("文件另存为"),
			path,
			tr("Config Files (*.pdf)"));
		QPrinter *printer = new  QPrinter;
		printer->setPageSize(QPrinter::A4);
		printer->setOutputFormat(QPrinter::PdfFormat);
		printer->setOutputFileName(fileName);
		QTextDocument *text = new QTextDocument;
		QFont font(QString::fromLocal8Bit("宋体"), 10);
		text->setDefaultFont(font);
		text->setHtml(table);
		text->print(printer);
		text->end();
		delete printer;
		printer = NULL;
		delete text;
		text = NULL;
	}
}
void SCRail::importData(QVector<double> &x, QVector<double> &y,const char *conf_path, raildata_j *data)
{
	x.clear();
	y.clear();
	FILE *f;
	char buf[256] = "\0";
	int i = 0, j = 0, k = 0;
	char coordX[256] = "\0";
	char *BUF = new char[256];
	char *psrc = BUF;
	char coordY[256] = "\0";
	f = fopen(conf_path, "r");
	fseek(f, 0, SEEK_SET);
	double xz = 0.0;
	double yz = 0.0;
	while (fgets(buf, 256, f) != NULL)
	{
		if (strlen(buf) < 3)
			continue;
		i = 0;
		j = 0;
		while (buf[i] != '\t')
		{
			coordX[i] = buf[i];
			i++;
		}
		coordX[i] = '\0';
		BUF = strstr(buf, "\t");
		BUF = BUF + 1;
		while (BUF[j] != '\n')
		{
			coordY[j] = BUF[j];
			j++;
		}
		coordY[j] = '\0';
		xz = atof(coordX);
		yz = atof(coordY);
		data->dot[k].x = xz;
		data->dot[k].y = yz;
		x.push_back(xz);
		y.push_back(yz);
		memset(buf, 0, sizeof(buf));
		k++;
		fseek(f, -1, SEEK_CUR);
	}
	data->dotcount = k;
	delete psrc;
	psrc = NULL;
	fclose(f);
}

void SCRail::set_checkInterfaceTitle()
{
	if (checkIn->_checkLocal){
		ui.Lab_titleCheck->setText(QString::fromLocal8Bit("右轨钢轨轨头廓形图"));
		ui.Lab_titleOffset->setText(QString::fromLocal8Bit("右轨偏差图"));
		ui.Tab_angle->setText(QString::fromLocal8Bit("右轨实测廓形与目标廓形差值表"));
	}
	else
	{
		ui.Lab_titleCheck->setText(QString::fromLocal8Bit("左轨钢轨轨头廓形图"));
		ui.Lab_titleOffset->setText(QString::fromLocal8Bit("左轨偏差图"));
		ui.Tab_angle->setText(QString::fromLocal8Bit("左轨实测廓形与目标廓形差值表"));
	}
}

void SCRail::set_Title(int temp)
{
	if (1 == temp)
	{
		if (checkIn->_checkLocal)
			ui.Lab_title->setText(QString::fromLocal8Bit("右轨钢轨轨头磨耗分析比较图"));
		else
			ui.Lab_title->setText(QString::fromLocal8Bit("左轨钢轨轨头磨耗分析比较图"));
	}
	else if (2 == temp)
	{
		if (checkIn->_checkLocal)
			ui.Lab_title->setText(QString::fromLocal8Bit("右轨钢轨轨头打磨分析图"));
		else
			ui.Lab_title->setText(QString::fromLocal8Bit("左轨钢轨轨头打磨分析图"));
	}
	else if (3 == temp)
	{
		if (checkIn->_checkLocal)
			ui.Lab_title->setText(QString::fromLocal8Bit("右轨钢轨轨头质量分析图"));
		else
			ui.Lab_title->setText(QString::fromLocal8Bit("左轨钢轨轨头质量分析图"));
	}
}

void SCRail::set_stateNormal()
{
	while (checkdata->railState)
	{
		int _state = log10(checkdata->railState) / log10(2);
		checkdata->railState = checkdata->railState - pow(2, _state);
		switch (_state)
		{
		case 0:
			ui.Lab_state1->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d1.png);");
			break;
		case 1:
			ui.Lab_state2->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d2.png);");
			break;
		case 2:
			ui.Lab_state3->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d3.png);");
			break;
		case 3:
			ui.Lab_state4->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d4.png);");
			break;
		case 4:
			ui.Lab_state5->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d5.png);");
			break;
		case 5:
			ui.Lab_state6->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d6.png);");
			break;
		case 6:
			ui.Lab_state7->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d7.png);");
			break;
		case 7:
			ui.Lab_state8->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/d8.png);");
			break;
		case 8:
			ui.Lab_state9->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c1.png);");
			break;
		case 9:
			ui.Lab_state10->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c2.png);");
			break;
		case 10:
			ui.Lab_state11->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c3.png);");
			break;
		case 11:
			ui.Lab_state12->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c4.png);");
			break;
		case 12:
			ui.Lab_state13->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c5.png);");
			break;
		case 13:
			ui.Lab_state14->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c6.png);");
			break;
		case 14:
			ui.Lab_state15->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c7.png);");
			break;
		case 15:
			ui.Lab_state16->setStyleSheet("border-image: url(./../../SCRail/importData/ImageFolder/state/c8.png);");
			break;
		default:
			break;
		}
	}
}


double SCRail::compute_slopeStandard(raildata_j *rail, double tempX, double tempY)
{
	
	double localX = 0.0;
	double localY = 100.0;
	
	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x < rail->dot[x.size() / 2].x)
		{
			localY = fabs(rail->dot[i].y - 15 - tempY) < localY ? fabs(rail->dot[i].y - 15 - tempY) : localY;

		}
	}

	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x < rail->dot[x.size() / 2].x)
		{
			if (localY == fabs(rail->dot[i].y - 15 - tempY))
			{
				
				localX = rail->dot[i].x;
				localY = rail->dot[i].y;
			}
		}
	}
	//qDebug() << "xielv:" << (localY - tempY) / (localX - tempX) << endl;
	return  (localY - tempY) / (localX - tempX);
}

double SCRail::compute_slopeStandard(railData_j * rail, double tempX, double tempY)
{
	double localX = 0.0;
	double localY = 100.0;

	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x <rail->dot[x.size() / 2].x)
		{
			localY = fabs(rail->dot[i].y - 15 - tempY) < localY ? fabs(rail->dot[i].y - 15 - tempY) : localY;

		}
	}

	for (int i = 0; i < x.size(); i++)
	{
		if (rail->dot[i].x < rail->dot[x.size() / 2].x)
		{
			if (-0.001 < (localY-fabs(rail->dot[i].y - 15 - tempY)) && ((localY - fabs(rail->dot[i].y - 15 - tempY))<0.001))
			{
				qDebug() << i << endl;
				localX = rail->dot[i].x;
				localY = rail->dot[i].y;
			}
		}
	}
	//qDebug() << "xielv:" << (localY - tempY) / (localX - tempX) << endl;
	return  (localY - tempY) / (localX - tempX);

}

double SCRail::compute_angle(double slope_actual, double slope_standard)
{
	return atan(fabs(slope_actual-slope_standard)/fabs(1+slope_actual*slope_standard));
}
void SCRail::set_data()
{
	char temp[50] = "\0";
	sprintf(temp, "%.2f", metricalInfo->verticalThirds_wear);
	ui.Lab_verticalHalfWear->setText(QString("%1").arg(temp) + QString("mm"));
	on_judgeIsOver(metricalInfo->verticalThirds_wear, ui.value_1->currentText().toDouble(), ui.Lab_verticalHalfWear);
	sprintf(temp, "%.2f", metricalInfo->horizontal_wear);
	ui.Lab_horizontalWear->setText(QString("%1").arg(temp) + QString("mm"));
	on_judgeIsOver(metricalInfo->horizontal_wear, ui.value_2->currentText().toDouble(), ui.Lab_horizontalWear);

	sprintf(temp, "%.2f", metricalInfo->verticalHalf_wear);
	ui.Lab_verticalHalfWear2->setText(QString("%1").arg(temp) + QString("mm"));
	on_judgeIsOver(metricalInfo->verticalHalf_wear, ui.value_1->currentText().toDouble(), ui.Lab_verticalHalfWear2);
	sprintf(temp, "%.2f", metricalInfo->verticalThirds_wear);
	ui.Lab_verticalThirdsWear->setText(QString("%1").arg(temp) + QString("mm"));
	on_judgeIsOver(metricalInfo->verticalThirds_wear, ui.value_1->currentText().toDouble(), ui.Lab_verticalThirdsWear);

	sprintf(temp, "%.2f", metricalInfo->horizontal_wear);
	ui.Lab_horizontalWear2->setText(QString("%1").arg(temp) + QString("mm"));
	on_judgeIsOver(metricalInfo->horizontal_wear, ui.value_2->currentText().toDouble(), ui.Lab_horizontalWear2);

	sprintf(temp, "%.2f", metricalInfo->max_offset);
	ui.Lab_maxOffset->setText(QString("%1").arg(temp) + QString("mm"));
	on_judgeIsOver(metricalInfo->max_offset, ui.maxdiffBox->currentText().toDouble(), ui.Lab_maxOffset);
	ui.Lab_maxOffset2->setText(QString("%1").arg(temp) + QString("mm"));
	on_judgeIsOver(metricalInfo->max_offset, ui.maxdiffBox->currentText().toDouble(), ui.Lab_maxOffset2);

	sprintf(temp, "%.2f", metricalInfo->min_offset);
	ui.Lab_minOffset->setText(QString("%1").arg(temp) + QString("mm"));
	on_judgeIsOver(metricalInfo->min_offset, ui.mindiffBox->currentText().toDouble(), ui.Lab_minOffset);
	ui.Lab_minOffset2->setText(QString("%1").arg(temp) + QString("mm"));
	on_judgeIsOver(metricalInfo->min_offset, ui.mindiffBox->currentText().toDouble(), ui.Lab_minOffset2);

	ui.Lab_GQIValue->setText(QString::number(metricalInfo->GQI_value));
	on_judgeIsOver(ui.GQIBox->currentText().toDouble(),(double)metricalInfo->GQI_value, ui.Lab_GQIValue);
	ui.Lab_GQIValue2->setText(QString::number(metricalInfo->GQI_value));
	on_judgeIsOver(ui.GQIBox->currentText().toDouble(), (double)metricalInfo->GQI_value, ui.Lab_GQIValue2);

}
void SCRail::whileTable(QString &tabletitle)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	QStringList title;
	title.push_back(QStringLiteral("序号"));
	title.push_back(QStringLiteral("线 名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/\n右轨"));
	title.push_back(QStringLiteral("检测里程"));
	title.push_back(QStringLiteral("检测时间"));
	title.push_back(QStringLiteral("垂磨\n(1/2)\n(mm)"));
	title.push_back(QStringLiteral("垂磨\n(1/3)\n(mm)"));
	title.push_back(QStringLiteral("侧磨\n(mm)"));
	title.push_back(QStringLiteral("总磨耗\n(mm)"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg("1");
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_',0,0));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 1, 1));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 2, 2));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 3, 3));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 4, 4).split('.').at(0));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_verticalHalfWear_2->text().remove(QChar('mm', Qt::CaseInsensitive)));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_verticalThirdsWear_2->text().remove(QChar('mm', Qt::CaseInsensitive)));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_horizontalWear_2->text().remove(QChar('mm', Qt::CaseInsensitive)));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::number(ui.Lab_verticalThirdsWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() + ui.Lab_horizontalWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() / 2));
	tabletitle += "</tr>";
	tabletitle += "</table>";

}
void SCRail::whileTableGQI(QString & tabletitle)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	QStringList title;
	title.push_back(QStringLiteral("序号"));
	title.push_back(QStringLiteral("线名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/右\n轨"));
	title.push_back(QStringLiteral("检测里程"));
	title.push_back(QStringLiteral("检测时间"));
	title.push_back(QStringLiteral("GQI"));
	title.push_back(QStringLiteral("最大偏差值\n(mm)"));
	title.push_back(QStringLiteral("最小偏差值\n(mm)"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg("1");
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 0, 0));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 1, 1));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 2, 2));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 3, 3));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 4, 4).split('.').at(0));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_GQIValue_2->text());
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_maxOffset_2->text().remove(QChar('mm', Qt::CaseInsensitive)));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_minOffset_2->text().remove(QChar('mm', Qt::CaseInsensitive)));
	tabletitle += "</tr>";
	tabletitle += "</table>";
}
void SCRail::whileImage(QString & tabletitle)
{
	tabletitle += "<p align =\"center\"><img src= " +QString("./../../SCRail/importData/GeneratedImage/") + ui.count_chooseBox->currentText().split('.').at(0) + QString(".png") + " width=\"526\" height=\"301\"></p>";
	tabletitle += "<h2 align = \"center\">" + QStringLiteral("1号钢轨廓形的轨头磨耗对比图");
}
void SCRail::whileImageGQI(QString & tabletitle)
{
	tabletitle += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + ui.count_chooseBox->currentText().split('.').at(0) + QString("GQI.png") + " width=\"526\" height=\"301\"></p>";
	tabletitle += "<h2 align = \"center\">" + QStringLiteral("1号钢轨廓形的轨头磨耗对比图");
}
void SCRail::whileTableGather(QString & tabletitle, QString status)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	QStringList title;
	title.push_back(QStringLiteral("序\n号"));
	title.push_back(QStringLiteral("线\n名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/\n右轨"));
	title.push_back(QStringLiteral("检测\n里程"));
	title.push_back(QStringLiteral("检测\n时间"));
	title.push_back(QStringLiteral("垂磨\n(1/2)"));
	title.push_back(QStringLiteral("垂磨\n(1/3)"));
	title.push_back(QStringLiteral("侧\n磨"));
	title.push_back(QStringLiteral("总\n磨\n耗"));
	title.push_back(QStringLiteral("最\n大\n偏\n差\n值"));
	title.push_back(QStringLiteral("最\n小\n偏\n差\n值"));
	title.push_back(QStringLiteral("GQI"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::number(1));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.lineName_5->text());
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_4->text());
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_',2,2));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 3, 3));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.count_chooseBox->currentText().section('_', 4, 4).section('.',0,0));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_verticalHalfWear_2->text().remove(QChar('m'), Qt::CaseInsensitive));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_verticalThirdsWear_2->text().remove(QChar('m'), Qt::CaseInsensitive));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_horizontalWear_2->text().remove(QChar('m'), Qt::CaseInsensitive));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::number(ui.Lab_verticalThirdsWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() + ui.Lab_horizontalWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() / 2));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_maxOffset_2->text().remove(QChar('m'), Qt::CaseInsensitive));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_minOffset_2->text().remove(QChar('m'), Qt::CaseInsensitive));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.Lab_GQIValue_2->text());
	tabletitle += "</tr>";
	tabletitle += "</table>";
	if (status == QString::fromLocal8Bit("存在"))
	{
		tabletitle += "<h2>" + QStringLiteral("实测钢轨廓形中，以") + ui.target_oversizeName_5->text() + QStringLiteral("为目标廓形时，钢轨轨头存在检测值超限的情况，其中：") + "</h2>";
		if ( ui.Lab_horizontalWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() > 10)
		{
			tabletitle += "<h2>" +ui.count_chooseBox->currentText().section('_',4,4).section('.',0,0) + QStringLiteral("检测的") + ui.lineName_5->text() + QString(",") + \
				ui.trackName_4->text() + QString(",") + ui.mileageName_4->text() + QStringLiteral("处") + ui.count_chooseBox->currentText().section('_', 2, 2) + \
				QStringLiteral("存在侧磨(") + ui.Lab_horizontalWear_2->text().remove(QChar('m'), Qt::CaseInsensitive) + QStringLiteral(")超限") + "</h2>";
		}	
			else if (ui.Lab_verticalThirdsWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() > 8)
			{
				tabletitle += "<h2>" + ui.count_chooseBox->currentText().section('_', 4, 4).section('.', 0, 0) + QStringLiteral("检测的") + ui.lineName_5->text() + QString(",") + \
					ui.trackName_4->text() + QString(",") + ui.mileageName_4->text() + QStringLiteral("处") + ui.count_chooseBox->currentText().section('_', 2, 2) + \
					QStringLiteral("存在垂磨(") + ui.Lab_verticalThirdsWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() + QStringLiteral(")超限") + "</h2>";
			}
			else if ((ui.Lab_verticalThirdsWear_2->text().remove(QChar('m'),Qt::CaseInsensitive).toDouble() + ui.Lab_horizontalWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() / 2) > 9)
			{
				tabletitle += "<h2>" + ui.count_chooseBox->currentText().section('_', 4, 4).section('.', 0, 0) + QStringLiteral("检测的") + ui.lineName_5->text() + QString(",") + \
					ui.trackName_4->text() + QString(",") + ui.mileageName_4->text() + QStringLiteral("处") + ui.count_chooseBox->currentText().section('_', 2, 2) + \
					QStringLiteral("存在总磨耗(") + ui.Lab_verticalThirdsWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() + ui.Lab_horizontalWear_2->text().remove(QChar('m'), Qt::CaseInsensitive).toDouble() / 2 + QStringLiteral(")超限") + "</h2>";
			}
	}
}
void SCRail::on_analysisTable(QString & tabletitle, QVector<QVector<float>> diffvalue)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";

	tabletitle += "<tr style=\"background-color:gray\">";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][0]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][1]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][2]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][3]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][4]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][5]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][6]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][7]));
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("差值\n(mm)"));
	char temp[50] = "\0";
	for (int i = 0; i < 7; i++)
	{
		
		sprintf(temp, "%.2f", _diffValue.at(0)[i]);
		tabletitle += QString("<td align =\"center\">%1</td>").arg(temp);
	}
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][0]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][1]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][2]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][3]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][4]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][5]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][6]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][7]));
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("差值\n(mm)"));
	for (int i = 0; i < 7; i++)
	{

		sprintf(temp, "%.2f", _diffValue.at(0)[i+7]);
		tabletitle += QString("<td align =\"center\">%1</td>").arg(temp);
	}
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][0]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][1]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][2]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][3]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][4]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][5]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][6]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][7]));
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("差值\n(mm)"));
	for (int i = 0; i < 7; i++)
	{

		sprintf(temp, "%.2f", _diffValue.at(0)[i+14]);
		tabletitle += QString("<td align =\"center\">%1</td>").arg(temp);
	}
	tabletitle += "</tr>";
	tabletitle += "</table>";
}
void SCRail::whileTable(QString &tabletitle, QTableWidget * table)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	QStringList title;
	title.push_back(QStringLiteral("序号"));
	title.push_back(QStringLiteral("线名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/\n右轨"));
	title.push_back(QStringLiteral("检测里程"));
	title.push_back(QStringLiteral("检测时间"));
	title.push_back(QStringLiteral("垂磨\n(1/2)\n(mm)"));
	title.push_back(QStringLiteral("垂磨\n(1/3)\n(mm)"));
	title.push_back(QStringLiteral("侧磨\n(mm)"));
	title.push_back(QStringLiteral("总磨耗\n(mm)"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	for (int i = 0; i < table->rowCount(); i++)
	{
		tabletitle += "<tr>";
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::number(i+1));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.lineName_6->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_5->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_6->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.mileageName_5->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i,0)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i,1)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i,2)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i,3)->text());
		char temp[50] = "\0";
		sprintf(temp, "%.2f", (table->item(i, 2)->text().toDouble() + table->item(i, 3)->text().toDouble() / 2));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString("%1").arg(temp));
		tabletitle += "</tr>";
		
	}
	tabletitle += "</table>";
	
}
void SCRail::whileTableGQI(QString & tabletitle, QTableWidget * table)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	QStringList title;
	title.push_back(QStringLiteral("序号"));
	title.push_back(QStringLiteral("线名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/右\n轨"));
	title.push_back(QStringLiteral("检测里程"));
	title.push_back(QStringLiteral("检测时间"));
	title.push_back(QStringLiteral("最大偏差值\n(mm)"));
	title.push_back(QStringLiteral("最小偏差值\n(mm)"));
	title.push_back(QStringLiteral("GQI"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	for (int i = 0; i < table->rowCount(); i++)
	{
		tabletitle += "<tr>";
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::number(i + 1));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.lineName_6->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_5->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_6->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.mileageName_5->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 0)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 4)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 5)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 6)->text());
		tabletitle += "</tr>";
	}
	
	
	tabletitle += "</table>";
}
void SCRail::whileImage(QString & tabletitle, QTableWidget * table,int row)
{
	tabletitle += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + table->item(row,0)->text() + QString(".png") + " width=\"526\" height=\"301\"></p>";
	tabletitle += "<h2 align = \"center\">" +QString::number(row+1) +  QStringLiteral("号钢轨廓形的轨头磨耗对比图") + "</h2>";
}
void SCRail::whileImageGQI(QString & tabletitle, QTableWidget * table,int row)
{
	tabletitle += "<p align =\"center\"><img src= " + QString("./../../SCRail/importData/GeneratedImage/") + table->item(row,0)->text() + QString("GQI.png") + " width=\"526\" height=\"301\"></p>";
	tabletitle += "<h2 align = \"center\">" + QString::number(row + 1) + QStringLiteral("号钢轨廓形GQI图") + "</h2>";
}
void SCRail::whileTableGather(QString & tabletitle, QTableWidget * table,QString status)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	QStringList title;
	title.push_back(QStringLiteral("序\n号"));
	title.push_back(QStringLiteral("线\n名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/\n右轨"));
	title.push_back(QStringLiteral("检测\n里程"));
	title.push_back(QStringLiteral("检测\n时间"));
	title.push_back(QStringLiteral("垂磨\n(1/2)"));
	title.push_back(QStringLiteral("垂磨\n(1/3)"));
	title.push_back(QStringLiteral("侧\n磨"));
	title.push_back(QStringLiteral("总\n磨\n耗"));
	title.push_back(QStringLiteral("最\n大\n偏\n差\n值"));
	title.push_back(QStringLiteral("最\n小\n偏\n差\n值"));
	title.push_back(QStringLiteral("GQI"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	for (int i = 0; i < table->rowCount(); i++)
	{
		tabletitle += "<tr>";
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::number(i + 1));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.lineName_6->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_5->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_6->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.mileageName_5->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 0)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 1)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 2)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 3)->text());
		char temp[50] = "\0";
		sprintf(temp, "%.2f", (table->item(i, 2)->text().toDouble() + table->item(i, 3)->text().toDouble() / 2));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString("%1").arg(temp));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 4)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 5)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 6)->text());
		tabletitle += "</tr>";

	}
	tabletitle += "</table>";
	if (status == QString::fromLocal8Bit("存在"))
	{
		tabletitle += "<h2>" + QStringLiteral("实测钢轨廓形中，以") + ui.target_oversizeName_5->text() + QStringLiteral("为目标廓形时，钢轨轨头存在检测值超限的情况，其中：") + "</h2>";
		for (int i = 0; i < table->rowCount(); i++)
		{
			if (table->item(i, 3)->text().toDouble() > 10)
			{
				tabletitle += "<h2>" + table->item(i, 0)->text() + QStringLiteral("检测的") + ui.lineName_6->text() + QString(",") + \
					ui.trackName_5->text() + QString(",") + ui.mileageName_5->text() + QStringLiteral("处") + ui.trackName_6->text() + \
					QStringLiteral("存在侧磨(") + table->item(i, 3)->text() + QStringLiteral(")超限") + "</h2>";
			}
			if (table->item(i, 2)->text().toDouble() > 8)
			{
				tabletitle += "<h2>" + table->item(i, 0)->text() + QStringLiteral("检测的") + ui.lineName_6->text() + QString(",") + \
					ui.trackName_5->text() + QString(",") + ui.mileageName_5->text() + QStringLiteral("处") + ui.trackName_6->text() + \
					QStringLiteral("存在垂磨(") + table->item(i, 2)->text() + QStringLiteral(")超限") + "</h2>";
			}
			if ((table->item(i, 3)->text().toDouble() / 2 + table->item(i, 2)->text().toDouble()) > 9)
			{
				char temp[50] = "\0";
				sprintf(temp, "%.2f", (table->item(i, 2)->text().toDouble() + table->item(i, 3)->text().toDouble() / 2));
				tabletitle += "<h2>" + table->item(i, 0)->text() + QStringLiteral("检测的") + ui.lineName_6->text() + QString(",") + \
					ui.trackName_5->text() + QString(",") + ui.mileageName_5->text() + QStringLiteral("处") + ui.trackName_6->text() + \
					QStringLiteral("存在总磨耗(") + QString("%1").arg(temp) + QStringLiteral(")超限") + "</h2>";
			}
		}
	}
}
void SCRail::on_analysisTable(QString & tabletitle, QVector <QVector<float>> diffvalue, int count)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";

	tabletitle += "<tr style=\"background-color:gray\">";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][0]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][1]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][2]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][3]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][4]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][5]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][6]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[0][7]));
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("差值\n(mm)"));
	char temp[50] = "\0";
	for (int i = 0; i < 7; i++)
	{

		sprintf(temp, "%.2f", _diffValue.at(count)[i]);
		tabletitle += QString("<td align =\"center\">%1</td>").arg(temp);
	}
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][0]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][1]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][2]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][3]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][4]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][5]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][6]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[1][7]));
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("差值\n(mm)"));
	for (int i = 0; i < 7; i++)
	{

		sprintf(temp, "%.2f", _diffValue.at(count)[i+7]);
		tabletitle += QString("<td align =\"center\">%1</td>").arg(temp);
	}
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][0]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][1]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][2]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][3]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][4]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][5]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][6]));
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit(table_info[2][7]));
	tabletitle += "</tr>";
	tabletitle += "<tr>";
	tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::fromLocal8Bit("差值\n(mm)"));
	for (int i = 0; i < 7; i++)
	{

		sprintf(temp, "%.2f", _diffValue.at(count)[i+14]);
		tabletitle += QString("<td align =\"center\">%1</td>").arg(temp);
	}
	tabletitle += "</tr>";
	tabletitle += "</table>";
}
void SCRail::whileTableRailDiffPoint(QString & tabletitle, QTableWidget * table)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	char temp[50] = "\0";
	QStringList title;
	title.push_back(QStringLiteral("序号"));
	title.push_back(QStringLiteral("线\t名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/\n右轨"));
	title.push_back(QStringLiteral("检测里程"));
	title.push_back(QStringLiteral("检测时间"));
	title.push_back(QStringLiteral("垂磨\n(1/2)\n(mm)"));
	title.push_back(QStringLiteral("垂磨\n(1/3)\n(mm)"));
	title.push_back(QStringLiteral("侧磨\n(mm)"));
	title.push_back(QStringLiteral("总磨耗\n(mm)"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	for (int i = 0; i < table->rowCount(); i++)
	{
		tabletitle += "<tr>";
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::number(i + 1));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.lineName_7->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_7->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_8->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 0)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 8)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 1)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 2)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 3)->text());
		sprintf(temp, "%.2f", (table->item(i, 2)->text().toDouble() + table->item(i, 3)->text().toDouble() / 2));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString("%1").arg(temp));
		tabletitle += "</tr>";

	}
	tabletitle += "</table>";

}
void SCRail::whileTableGQIRailDiffPoint(QString & tabletitle, QTableWidget * table)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	QStringList title;
	title.push_back(QStringLiteral("序号"));
	title.push_back(QStringLiteral("线名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/右\n轨"));
	title.push_back(QStringLiteral("检测里程"));
	title.push_back(QStringLiteral("检测时间"));
	title.push_back(QStringLiteral("最大偏差值\n(mm)"));
	title.push_back(QStringLiteral("最小偏差值\n(mm)"));
	title.push_back(QStringLiteral("GQI"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	for (int i = 0; i < table->rowCount(); i++)
	{
		tabletitle += "<tr>";
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::number(i + 1));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.lineName_7->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_7->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_8->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 0)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 8)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 4)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 5)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 6)->text());
		tabletitle += "</tr>";
	}


	tabletitle += "</table>";
}
void SCRail::whileTableGatherDiffPoint(QString & tabletitle, QTableWidget * table, QString status)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	char temp[50] = "\0";
	QStringList title;
	title.push_back(QStringLiteral("序\n号"));
	title.push_back(QStringLiteral("线 名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/\n右轨"));
	title.push_back(QStringLiteral("检测\n里程"));
	title.push_back(QStringLiteral("检测\n时间"));
	title.push_back(QStringLiteral("垂磨\n(1/2)"));
	title.push_back(QStringLiteral("垂磨\n(1/3)"));
	title.push_back(QStringLiteral("侧\n磨"));
	title.push_back(QStringLiteral("总\n磨\n耗"));
	title.push_back(QStringLiteral("最\n大\n偏\n差\n值"));
	title.push_back(QStringLiteral("最\n小\n偏\n差\n值"));
	title.push_back(QStringLiteral("GQI"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	for (int i = 0; i < table->rowCount(); i++)
	{
		tabletitle += "<tr>";
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString::number(i + 1));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.lineName_7->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_7->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(ui.trackName_8->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 0)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 8)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 1)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 2)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 3)->text());
		sprintf(temp, "%.2f", (table->item(i, 2)->text().toDouble() + table->item(i, 3)->text().toDouble() / 2));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString("%1").arg(temp));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 4)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 5)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 6)->text());
		tabletitle += "</tr>";

	}
	tabletitle += "</table>";
	if (status == QString::fromLocal8Bit("存在"))
	{
		tabletitle += "<h2>" + QStringLiteral("实测钢轨廓形中，以") + ui.target_oversizeName_6->text() + QStringLiteral("为目标廓形时，钢轨轨头存在检测值超限的情况，其中：") + "</h2>";
		for (int i = 0; i < table->rowCount(); i++)
		{
			if (table->item(i, 3)->text().toDouble() > 10)
			{
				tabletitle += "<h2>" + table->item(i, 8)->text() + QStringLiteral("检测的") + ui.lineName_7->text() + QString(",") + \
					ui.trackName_7->text() + QString(",") + table->item(i, 0)->text() + QStringLiteral("处") + ui.trackName_8->text() + \
					QStringLiteral("存在侧磨(") + table->item(i, 3)->text() + QStringLiteral(")超限") + "</h2>";
			}
			if (table->item(i, 2)->text().toDouble() > 8)
			{
				tabletitle += "<h2>" + table->item(i, 8)->text() + QStringLiteral("检测的") + ui.lineName_7->text() + QString(",") + \
					ui.trackName_7->text() + QString(",") + table->item(i, 0)->text() + QStringLiteral("处") + ui.trackName_8->text() + \
					QStringLiteral("存在垂磨(") + table->item(i, 2)->text() + QStringLiteral(")超限") + "</h2>";
			}
			if ((table->item(i, 3)->text().toDouble() / 2 + table->item(i, 2)->text().toDouble()) > 9)
			{
				sprintf(temp, "%.2f", (table->item(i, 2)->text().toDouble() + table->item(i, 3)->text().toDouble() / 2));
				tabletitle += "<h2>" + table->item(i, 8)->text() + QStringLiteral("检测的") + ui.lineName_7->text() + QString(",") + \
					ui.trackName_7->text() + QString(",") + table->item(i, 0)->text() + QStringLiteral("处") + ui.trackName_8->text() + \
					QStringLiteral("存在总磨耗(") + QString(temp) + QStringLiteral(")超限") + "</h2>";
			}
		}
	}
}
void SCRail::whileTableDiffRail(QString & tabletitle, QTableWidget * table)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	char temp[50] = "\0";
	QStringList title;
	title.push_back(QStringLiteral("序号"));
	title.push_back(QStringLiteral("线\t名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/\n右轨"));
	title.push_back(QStringLiteral("检测里程"));
	title.push_back(QStringLiteral("检测时间"));
	title.push_back(QStringLiteral("垂磨\n(1/2)\n(mm)"));
	title.push_back(QStringLiteral("垂磨\n(1/3)\n(mm)"));
	title.push_back(QStringLiteral("侧磨\n(mm)"));
	title.push_back(QStringLiteral("总磨耗\n(mm)"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	for (int i = 0; i < table->rowCount(); i++)
	{
		tabletitle += "<tr>";
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 0)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 1)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 2)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 3)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 4)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 12)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 5)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 6)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 7)->text());
		sprintf(temp, "%.2f", (table->item(i, 6)->text().toDouble() + table->item(i, 7)->text().toDouble() / 2));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString("%1").arg(temp));
		tabletitle += "</tr>";

	}
	tabletitle += "</table>";
}
void SCRail::whileTableGQIDiffRail(QString & tabletitle, QTableWidget * table)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	QStringList title;
	title.push_back(QStringLiteral("序号"));
	title.push_back(QStringLiteral("线名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/右\n轨"));
	title.push_back(QStringLiteral("检测里程"));
	title.push_back(QStringLiteral("检测时间"));
	title.push_back(QStringLiteral("最大偏差值\n(mm)"));
	title.push_back(QStringLiteral("最小偏差值\n(mm)"));
	title.push_back(QStringLiteral("GQI"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	for (int i = 0; i < table->rowCount(); i++)
	{
		tabletitle += "<tr>";
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 0)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 1)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 2)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 3)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 4)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 12)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 8)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 9)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 10)->text());
		tabletitle += "</tr>";
	}


	tabletitle += "</table>";
}
void SCRail::whileTableGatherDiffRail(QString & tabletitle, QTableWidget * table, QString status)
{
	tabletitle += "<table width=\"650\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\" align=\"center\" style=\"font - size: 6px\" style=\"border-collapse;\" bordercolor=\"gray\">";
	tabletitle += "<tr style=\"background-color:gray\">";
	char temp[50] = "\0";
	QStringList title;
	title.push_back(QStringLiteral("序\n号"));
	title.push_back(QStringLiteral("线 名"));
	title.push_back(QStringLiteral("行别/\n车站"));
	title.push_back(QStringLiteral("左/\n右轨"));
	title.push_back(QStringLiteral("检测\n里程"));
	title.push_back(QStringLiteral("检测\n时间"));
	title.push_back(QStringLiteral("垂磨\n(1/2)"));
	title.push_back(QStringLiteral("垂磨\n(1/3)"));
	title.push_back(QStringLiteral("侧\n磨"));
	title.push_back(QStringLiteral("总\n磨\n耗"));
	title.push_back(QStringLiteral("最\n大\n偏\n差\n值"));
	title.push_back(QStringLiteral("最\n小\n偏\n差\n值"));
	title.push_back(QStringLiteral("GQI"));
	for (int i = 0; i < title.count(); i++)
	{
		tabletitle += QString("<th align =\"center\">%1</th>").arg(title.at(i));
	}
	tabletitle += "</tr>";
	for (int i = 0; i < table->rowCount(); i++)
	{
		tabletitle += "<tr>";
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 0)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 1)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 2)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 3)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 4)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 12)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 5)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 6)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 7)->text());
		sprintf(temp, "%.2f", (table->item(i, 6)->text().toDouble() + table->item(i, 7)->text().toDouble() / 2));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(QString("%1").arg(temp));
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 8)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 9)->text());
		tabletitle += QString("<td align =\"center\">%1</td>").arg(table->item(i, 10)->text());
		tabletitle += "</tr>";

	}
	tabletitle += "</table>";
	if (status == QString::fromLocal8Bit("存在"))
	{
		tabletitle += "<h2>" + QStringLiteral("实测钢轨廓形中，以") + ui.target_oversizeName_7->text() + QStringLiteral("为目标廓形时，钢轨轨头存在检测值超限的情况，其中：") + "</h2>";
		for (int i = 0; i < table->rowCount(); i++)
		{
			if (table->item(i, 7)->text().toDouble() > 10)
			{
				tabletitle += "<h2>" + table->item(i, 12)->text() + QStringLiteral("检测的") + table->item(i, 1)->text() + QString(",") + \
					table->item(i, 2)->text() + QString(",") + table->item(i, 4)->text() + QStringLiteral("处") + table->item(i, 3)->text() + \
					QStringLiteral("存在侧磨(") + table->item(i, 7)->text() + QStringLiteral(")超限") + "</h2>";
			}
			else if (table->item(i, 6)->text().toDouble() > 8)
			{
				tabletitle += "<h2>" + table->item(i, 12)->text() + QStringLiteral("检测的") + table->item(i, 1)->text() + QString(",") + \
					table->item(i, 2)->text() + QString(",") + table->item(i, 4)->text() + QStringLiteral("处") + table->item(i, 3)->text() + \
					QStringLiteral("存在垂磨(") + table->item(i, 6)->text() + QStringLiteral(")超限") + "</h2>";
			}
			else if ((table->item(i, 7)->text().toDouble() / 2 + table->item(i, 6)->text().toDouble()) > 9)
			{
				tabletitle += "<h2>" + table->item(i, 12)->text() + QStringLiteral("检测的") + table->item(i, 1)->text() + QString(",") + \
					table->item(i, 2)->text() + QString(",") + table->item(i, 4)->text() + QStringLiteral("处") + table->item(i, 3)->text() + \
					QStringLiteral("存在总磨耗(") + table->item(i, 7)->text().toDouble() / 2 + table->item(i, 6)->text().toDouble() + QStringLiteral(")超限") + "</h2>";
			}
		}
	}
}
void SCRail::on_choiceRowPlot(int row, QVector<QVector<float>> &diffvalue)
{
	QString fileName = "\0";
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{
		if (ui.count_chooseBox->itemText(i).contains(ui.tableWidget_4->item(row, 0)->text(), Qt::CaseSensitive))
		{
			fileName = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->itemText(i);
		}
	}
	FILE *fp;
	fp = fopen(fileName.toLocal8Bit().data(), "rb");
	if (NULL == fp)
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
		return;
	}
	CheckStatus checkstate;
	CheckData checkdata;
	fseek(fp, 0, SEEK_SET);
	fread(&checkstate, sizeof(CheckStatus), 1, fp);
	fseek(fp, 2048, SEEK_SET);
	fread(&checkdata, sizeof(CheckData), 1, fp);
	set_plot(checkstate.rail_type, &checkstate, &checkdata);
	QVector<float> temp(0);
	for (int i = 0; i < 21; i++)
	{
		temp.push_back(checkstate.diffData[i]);
	}
	diffvalue.push_back(temp);
	fclose(fp);
	
}
void SCRail::on_choiceRowPlotDiffPoint(int row, QVector<QVector<float>>& diffvalue)
{
	QString fileName = "\0";
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{
		if (ui.count_chooseBox->itemText(i).contains(ui.tableWidget_6->item(row, 8)->text(), Qt::CaseSensitive))
		{
			fileName = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->itemText(i);
		}
	}
	FILE *fp;
	fp = fopen(fileName.toLocal8Bit().data(), "rb");
	if (NULL == fp)
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
		return;
	}
	CheckStatus checkstate;
	CheckData checkdata;
	fseek(fp, 0, SEEK_SET);
	fread(&checkstate, sizeof(CheckStatus), 1, fp);
	fseek(fp, 2048, SEEK_SET);
	fread(&checkdata, sizeof(CheckData), 1, fp);
	set_plotDiffPoint(checkstate.rail_type , &checkstate, &checkdata);
	QVector<float> temp(0);
	for (int i = 0; i < 21; i++)
	{
		temp.push_back(checkstate.diffData[i]);
	}
	diffvalue.push_back(temp);
	fclose(fp);
}
void SCRail::on_choiceRowPlotDiffRail(int row, QVector<QVector<float>>& diffvalue)
{
	QString fileName = "\0";
	for (int i = 0; i < ui.count_chooseBox->count(); i++)
	{
		if (ui.count_chooseBox->itemText(i).contains(ui.tableWidget_8->item(row, 12)->text(), Qt::CaseSensitive))
		{
			fileName = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->itemText(i);
		}
	}
	FILE *fp;
	fp = fopen(fileName.toLocal8Bit().data(), "rb");
	if (NULL == fp)
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
		return;
	}
	CheckStatus checkstate;
	CheckData checkdata;
	fseek(fp, 0, SEEK_SET);
	fread(&checkstate, sizeof(CheckStatus), 1, fp);
	fseek(fp, 2048, SEEK_SET);
	fread(&checkdata, sizeof(CheckData), 1, fp);
	set_plotDiffRail(checkstate.rail_type, &checkstate, &checkdata);
	QVector<float> temp(0);
	for (int i = 0; i < 21; i++)
	{
		temp.push_back(checkstate.diffData[i]);
	}
	diffvalue.push_back(temp);
	fclose(fp);
}
void SCRail::on_writeInfo()
{
	_version++;
	QString curPath = "./../../SCRail/importData/saveDataFolder/";
	QString mileage =QString("K") + QString::number(ui.MileageBox_m_2->currentIndex() * 1000 + ui.MileageBox_sw_2->currentIndex() * 100 + ui.MileageBox_wan_2->currentIndex() * 10 + ui.MileageBox_s_2->currentIndex())\
		+ QString("+") + QString::number(ui.MileageBox_h_2->currentIndex() * 100 + ui.MileageBox_ten_2->currentIndex() * 10 + ui.MileageBox_one_2->currentIndex());
	QString railType = "\0";
	curPath += ui.lineCBox_2->currentText() + "_" + ui.nameType->currentText() + "_";
	if (0 == groupMilNum->checkedId())
		curPath += QString::fromLocal8Bit("左轨_");
	else
		curPath += QString::fromLocal8Bit("右轨_");
	curPath += mileage + "_";
	QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
	curPath+= curTime + ".dat";
	_path.push_back(curPath);
	QByteArray cpath = curPath.toLocal8Bit();
	FILE *p;
	p = fopen(cpath.data(), "wb");
	check.version = 1;
	check.right_left = checkIn->_checkLocal;
	check.rail_type = checkIn->_targetOversizeIndex;
	strcpy_s(check.railType, checkIn->_targetOversize.toStdString().c_str());
	//check.checkTime = curTime;
	check.current = floor((ui.MileageBox_m_2->currentIndex() * 1000 + ui.MileageBox_sw_2->currentIndex() * 100 + ui.MileageBox_wan_2->currentIndex() * 10 + ui.MileageBox_s_2->currentIndex()\
		+ ui.MileageBox_h_2->currentIndex()*0.1 + ui.MileageBox_ten_2->currentIndex()*0.01 + ui.MileageBox_one_2->currentIndex()*0.001)*10000.000f + 0.5)/10000.000f;
	if (NULL == ui.trackBox_2->currentText())
	{
		check.wayOrStation = 1;
		strcpy_s(check.line, ui.nameType->currentText().toStdString().c_str());
		strcpy_s(check.station, "");
		strcpy_s(check.track, "");
	}
		
	else
	{
		strcpy_s(check.line, "");
		strcpy_s(check.station, ui.nameType->currentText().toStdString().c_str());
		check.wayOrStation = 0;
		strcpy_s(check.track,ui.trackBox_2->currentText().toStdString().c_str());
	}
 	memset(check.diffData, 0, sizeof(check.diffData));
	memcpy(check.diffData,diffValue,sizeof(check.diffData));
	check.verticalHalf_wear = int(metricalInfo->verticalHalf_wear * 100 + 0.5) / 100.0;
	check.verticalThirds_wear = int(metricalInfo->verticalThirds_wear * 100 + 0.5) / 100.0;
	check.horizontal_wear = int(metricalInfo->horizontal_wear * 100 + 0.5) / 100.0;
	check.GQI_value = metricalInfo->GQI_value;
	check.max_offset = int(metricalInfo->max_offset * 100 + 0.5) / 100.0;
	check.min_offset = int(metricalInfo->min_offset * 100 + 0.5) / 100.0;
	strcpy_s(check.cantValue, ui.Lab_railCantValue->text().toStdString().c_str());
	for (int i = 0; i < metricalInfo->localX.size(); i++)
	{
		check.offsetX[i] = metricalInfo->localX[i];
		check.offsetY[i] = metricalInfo->localY[i];
	}
	
	strcpy(check.way, ui.lineCBox_2->currentText().toStdString().c_str());
	fseek(p, 0, SEEK_SET);
	fwrite(&check, sizeof(check), 1, p);
	fseek(p, 2048, SEEK_SET);
	fwrite(checkdata, sizeof(CheckData), 1, p);
	fclose(p);
}
void SCRail::on_readInfo(const char* fileName)
{
	FILE *fp;
	fp = fopen(fileName,"rb");
	if (NULL == fp)
	{
		QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
		return;
	}
	fseek(fp, 0, SEEK_SET);
	CheckStatus check_status;
	fread(&check_status, sizeof(check_status),1, fp);
	fseek(fp, 2048, SEEK_SET);
	CheckData check_data;
	fread(&check_data, sizeof(check_data), 1, fp);
	_diffValue.clear();
	QVector<float> temp(0);
	for (int i = 0; i < 21; i++)
	{
		temp.push_back(check_status.diffData[i]);
	}
	_diffValue.push_back(temp);
	set_plotNormal(check_status.rail_type, ui.qCustomPlot6,&check_data);
	set_plot(check_status.rail_type+4, ui.qCustomPlot7, ui.qCustomPlot8,&check_data); //质量分析
	set_tableView(ui.tableWidget_3,check_status.diffData);
	ui.lineName_5->setText(QString(check_status.way));
	if (check_status.wayOrStation)
	{
		ui.label_58->setText(QString::fromLocal8Bit("行       别:"));
		ui.trackName_4->setText(QString(check_status.line));
	}
	else
	{
		ui.label_58->setText(QString::fromLocal8Bit("车       站:"));
		ui.trackName_4->setText(QString(check_status.station));
	}
	ui.mileageName_4->setText(QString::number(int(check_status.current)) + "+" + QString::number((check_status.current - int(check_status.current)) * 1000));
	ui.target_oversizeName_4->setText(ui.oversizeBox_2->currentText());
	if (check_status.right_left)
	{
		ui.Lab_abrasionTitle->setText(QString::fromLocal8Bit("右轨钢轨轨头磨耗分析比较图"));
		ui.Lab_qualityTitle->setText(QString::fromLocal8Bit("右轨钢轨轨头与目标廓形对比图"));
		ui.Lab_offsetTitle->setText(QString::fromLocal8Bit("右轨偏差图"));
		ui.Lab_grindTableTitle->setText(QString::fromLocal8Bit("右轨钢轨轨头打磨量表"));

	}
	else
	{
		ui.Lab_abrasionTitle->setText(QString::fromLocal8Bit("左轨钢轨轨头磨耗分析比较图"));
		ui.Lab_qualityTitle->setText(QString::fromLocal8Bit("左轨钢轨轨头与目标廓形对比图"));
		ui.Lab_offsetTitle->setText(QString::fromLocal8Bit("左轨偏差图"));
		ui.Lab_grindTableTitle->setText(QString::fromLocal8Bit("左轨钢轨轨头打磨量表"));
	}
	char temp1[50] = "\0"; 
	sprintf(temp1, "%.2f", check_status.verticalHalf_wear);
	ui.Lab_verticalHalfWear_2->setText(QString("%1").arg(temp1) + "mm");
	on_judgeIsOver(check_status.verticalHalf_wear, ui.value_3->currentText().toDouble(), ui.Lab_verticalHalfWear_2);
	sprintf(temp1, "%.2f", check_status.verticalThirds_wear);
	ui.Lab_verticalThirdsWear_2->setText(QString("%1").arg(temp1) + "mm");
	on_judgeIsOver(check_status.verticalThirds_wear, ui.value_3->currentText().toDouble(), ui.Lab_verticalThirdsWear_2);
	sprintf(temp1, "%.2f", check_status.horizontal_wear);
	ui.Lab_horizontalWear_2->setText(QString("%1").arg(temp1) + "mm");
	on_judgeIsOver(check_status.horizontal_wear, ui.value_4->currentText().toDouble(), ui.Lab_horizontalWear_2);
	ui.Lab_GQIValue_2->setText(QString::number(check_status.GQI_value));
	on_judgeIsOver(ui.GQIBox_2->currentText().toDouble(), (double)check_status.GQI_value, ui.Lab_GQIValue_2);
	sprintf(temp1, "%.2f", check_status.min_offset);
	ui.Lab_minOffset_2->setText(QString("%1").arg(temp1) + "mm");
	on_judgeIsOver(check_status.min_offset, ui.mindiffBox_2->currentText().toDouble(), ui.Lab_minOffset_2);
	sprintf(temp1, "%.2f", check_status.max_offset);
	ui.Lab_maxOffset_2->setText(QString("%1").arg(temp1) + "mm");
	on_judgeIsOver(check_status.max_offset, ui.maxdiffBox_2->currentText().toDouble(), ui.Lab_maxOffset_2);
	ui.Lab_railCantValue4->setText(QString(check_status.cantValue));
	fclose(fp);

}
void SCRail::on_sortFont(QTableWidget * table,int index)
{
	for (int i = 0; i < table->rowCount(); i++)
	{
		if (3 != index)
		{
			if (abs(table->item(i, 2)->text().toDouble()) > abs(ui.value_3->currentText().toDouble()))
				table->item(i, 2)->setForeground(QBrush(QColor(255, 0, 0)));
			else
				table->item(i, 2)->setForeground(QBrush(QColor(255, 255, 255)));
			if (abs(table->item(i, 3)->text().toDouble()) > abs(ui.value_4->currentText().toDouble()))
				table->item(i, 3)->setForeground(QBrush(QColor(255, 0, 0)));
			else
				table->item(i, 3)->setForeground(QBrush(QColor(255, 255, 255)));
			if (abs(table->item(i, 4)->text().toDouble()) > abs(ui.maxdiffBox_2->currentText().toDouble()))
				table->item(i, 4)->setForeground(QBrush(QColor(255, 0, 0)));
			else
				table->item(i, 4)->setForeground(QBrush(QColor(255, 255, 255)));
			if (abs(table->item(i, 5)->text().toDouble()) > abs(ui.mindiffBox_2->currentText().toDouble()))
				table->item(i, 5)->setForeground(QBrush(QColor(255, 0, 0)));
			else
				table->item(i, 5)->setForeground(QBrush(QColor(255, 255, 255)));
			if (abs(table->item(i, 6)->text().toDouble()) < abs(ui.GQIBox_2->currentText().toDouble()))
				table->item(i, 6)->setForeground(QBrush(QColor(255, 0, 0)));
			else
				table->item(i, 6)->setForeground(QBrush(QColor(255, 255, 255)));
		}
		else
		{
			if (abs(table->item(i, 6)->text().toDouble()) > abs(ui.value_3->currentText().toDouble()))
				table->item(i, 6)->setForeground(QBrush(QColor(255, 0, 0)));
			else
				table->item(i, 6)->setForeground(QBrush(QColor(255, 255, 255)));
			if (abs(table->item(i, 7)->text().toDouble()) > abs(ui.value_4->currentText().toDouble()))
				table->item(i, 7)->setForeground(QBrush(QColor(255, 0, 0)));
			else
				table->item(i, 7)->setForeground(QBrush(QColor(255, 255, 255)));
			if (abs(table->item(i, 8)->text().toDouble()) > abs(ui.maxdiffBox_2->currentText().toDouble()))
				table->item(i, 8)->setForeground(QBrush(QColor(255, 0, 0)));
			else
				table->item(i, 8)->setForeground(QBrush(QColor(255, 255, 255)));
			if (abs(table->item(i, 9)->text().toDouble()) > abs(ui.mindiffBox_2->currentText().toDouble()))
				table->item(i, 9)->setForeground(QBrush(QColor(255, 0, 0)));
			else
				table->item(i, 9)->setForeground(QBrush(QColor(255, 255, 255)));
			if (abs(table->item(i, 10)->text().toDouble()) < abs(ui.GQIBox_2->currentText().toDouble()))
				table->item(i, 10)->setForeground(QBrush(QColor(255, 0, 0)));
			else
				table->item(i, 10)->setForeground(QBrush(QColor(255, 255, 255)));
		}
		
	}
}
void SCRail::on_readCountInfo()
{
	QVector<QString> _time(0);
	QVector<double> _verticalHalfWear(0);
	QVector<double> _verticalThirdsWear(0);
	QVector<double> _horizontalWear(0);
	QVector<double> _GQIValue(0);
	QVector<double> _maxOffset(0);
	QVector<double> _minOffset(0);
	QVector<QString> _cantValue(0);
	if (ui.qCustomPlot11->plottableCount())
		ui.qCustomPlot11->clearPlottables();
	int vNum = ui.count_chooseBox->count();
	FILE *pf;
	_diffValue.clear();
	for (int i = 0; i < vNum; i++)
	{
		ui.count_chooseBox->setCurrentIndex(i);
		QString filename = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->currentText();
		_time.push_back(filename.section('/', 6, 6).section('_', 4, 4).section(".", 0, 0));
		pf = fopen(filename.toLocal8Bit().data(), "rb");
		if (NULL == pf)
		{
			QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
			return;
		}
		fseek(pf, 0, SEEK_SET);
		CheckStatus check_status;
		fread(&check_status, sizeof(check_status), 1, pf);
		fseek(pf, 2048, SEEK_SET);
		CheckData check_data;
		fread(&check_data, sizeof(check_data), 1, pf);
		set_offsetPlot(i,&check_status,_time.at(i));
		ui.target_oversizeName_5->setText(ui.oversizeBox_2->currentText());
		 _verticalHalfWear.push_back(check_status.verticalHalf_wear);
		 _verticalThirdsWear.push_back(check_status.verticalThirds_wear);
		 _horizontalWear.push_back(check_status.horizontal_wear);
		 _GQIValue.push_back(check_status.GQI_value);
		 _maxOffset.push_back(check_status.max_offset);
		 _minOffset.push_back(check_status.min_offset);
		 _cantValue.push_back(QString(check_status.cantValue));
		 fclose(pf);

	}
	//重绘，这里可以不用，官方例子有，执行setData函数后自动重绘
	//我认为应该用于动态显示或者是改变坐标轴范围之后的动态显示，我们以后探索
	ui.qCustomPlot11->replot();
	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot11->yAxis2 << ui.qCustomPlot11->xAxis2 << ui.qCustomPlot11->yAxis << ui.qCustomPlot11->xAxis;
	ui.qCustomPlot11->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot11->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot11->setBackground(QBrush(Qt::black));
	ui.qCustomPlot11->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot11->yAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot11->xAxis->setLabel(QString::fromLocal8Bit("钢轨横向(mm)"));
	ui.qCustomPlot11->yAxis->setLabel(QString::fromLocal8Bit("偏差值(mm)"));
	ui.qCustomPlot11->xAxis->setLabelColor(Qt::white);
	ui.qCustomPlot11->yAxis->setLabelColor(Qt::white);
	//ui.qCustomui.qCustomPlot11->graph(1)->setPen(QPen(Qt::red));
	ui.qCustomPlot11->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot11->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot11->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot11->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	
	set_plotAnalysis(_time, _maxOffset, ui.qCustomPlot9);
	qDebug() << "stateMax:" << ui.qCustomPlot9->savePng("./../../SCRail/importData/GeneratedImage/" + QString("maxOffset.png"), 621, 361) << endl;
	set_plotAnalysis(_time, _minOffset, ui.qCustomPlot9);
	qDebug() << "statemin:" << ui.qCustomPlot9->savePng("./../../SCRail/importData/GeneratedImage/" + QString("minOffset.png"), 621, 361) << endl;;
	set_reportTableDiffTime(vNum,_time,_verticalHalfWear,_verticalThirdsWear,_horizontalWear,_maxOffset,_minOffset, _GQIValue,_cantValue);

	ui.qCustomPlot10->savePng("./../../SCRail/importData/GeneratedImage/" + QString("GQIValue.png"), 621, 361);
	
	for (int i = 0; i < vNum; i++)
	{
		on_choiceRowPlot(i, 0);
		ui.qCustomPlot12->savePng("./../../SCRail/importData/GeneratedImage/"+ ui.tableWidget_4->item(i, 0)->text() + QString("GQI.png"), 621, 361);
	}

	for (int i = 0; i < vNum; i++)
	{
		on_choiceRowPlot(i,_diffValue);
		ui.qCustomPlot12->savePng("./../../SCRail/importData/GeneratedImage/" + ui.tableWidget_4->item(i, 0)->text() + QString(".png"), 621, 361);
	}
	on_choiceRowPlot(0, 0);
}
void SCRail::on_readCountInfoDiffPoint()
{
	//WinExec("cmd /C \"del D:\\SHCRail\\SCRail\\SCRail\\importData\\GeneratedImage\\*.png\"", SW_HIDE);
	QVector<QString> _time(0);
	QVector<QString> _mileageValue(0);
	QVector<QString> _cantValue(0);
	QVector<double> _verticalHalfWear(0);
	QVector<double> _verticalThirdsWear(0);
	QVector<double> _horizontalWear(0);
	QVector<double> _GQIValue(0);
	QVector<double> _maxOffset(0);
	QVector<double> _minOffset(0);
	QVector<double> _mileage(0);
	_diffValue.clear();
	if (ui.qCustomPlot15->plottableCount())
		ui.qCustomPlot15->clearPlottables();
	int vNum = ui.count_chooseBox->count();
	FILE *pf;

	for (int i = 0; i < vNum; i++)
	{
		ui.count_chooseBox->setCurrentIndex(i);
		QString filename = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->currentText();
		_time.push_back(filename.section('/', 6, 6).section('_', 4, 4).section(".", 0, 0));
		_mileageValue.push_back(filename.section('/', 6, 6).section('_', 3, 3));
		pf = fopen(filename.toLocal8Bit().data(), "rb");
		if (NULL == pf)
		{
			QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
			return;
		}
		fseek(pf, 0, SEEK_SET);
		CheckStatus check_status;
		fread(&check_status, sizeof(check_status), 1, pf);
		fseek(pf, 2048, SEEK_SET);
		CheckData check_data;
		fread(&check_data, sizeof(check_data), 1, pf);
		set_offsetPlot(i, &check_status);
		ui.target_oversizeName_6->setText(ui.oversizeBox_2->currentText());
		_verticalHalfWear.push_back(check_status.verticalHalf_wear);
		_verticalThirdsWear.push_back(check_status.verticalThirds_wear);
		_horizontalWear.push_back(check_status.horizontal_wear);
		_GQIValue.push_back(check_status.GQI_value);
		_maxOffset.push_back(check_status.max_offset);
		_minOffset.push_back(check_status.min_offset);
		_mileage.push_back(check_status.current);
		_cantValue.push_back(QString(check_status.cantValue));
		fclose(pf);

	}
	//重绘，这里可以不用，官方例子有，执行setData函数后自动重绘
	//我认为应该用于动态显示或者是改变坐标轴范围之后的动态显示，我们以后探索
	ui.qCustomPlot15->replot();
	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot15->yAxis2 << ui.qCustomPlot15->xAxis2 << ui.qCustomPlot15->yAxis << ui.qCustomPlot15->xAxis;
	ui.qCustomPlot15->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot15->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot15->setBackground(QBrush(Qt::black));
	ui.qCustomPlot15->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot15->yAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot15->xAxis->setLabel(QString::fromLocal8Bit("钢轨横向(mm)"));
	ui.qCustomPlot15->yAxis->setLabel(QString::fromLocal8Bit("偏差值(mm)"));
	ui.qCustomPlot15->xAxis->setLabelColor(Qt::white);
	ui.qCustomPlot15->yAxis->setLabelColor(Qt::white);
	//ui.qCustomui.qCustomPlot11->graph(1)->setPen(QPen(Qt::red));
	ui.qCustomPlot15->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot15->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot15->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot15->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	
	set_plotAnalysisDiffPoint(_mileageValue, _maxOffset, ui.qCustomPlot13);
	ui.qCustomPlot13->savePng("./../../SCRail/importData/GeneratedImage/" + QString("maxOffsetDiffPoint.png"), 621, 361);
	set_plotAnalysisDiffPoint(_mileageValue, _minOffset, ui.qCustomPlot13);
	ui.qCustomPlot13->savePng("./../../SCRail/importData/GeneratedImage/" + QString("minOffsetDiffPoint.png"), 621, 361);

	set_reportTableDiffMile(vNum, _time, _verticalHalfWear, _verticalThirdsWear, _horizontalWear, _maxOffset, _minOffset, _GQIValue,_mileage,_cantValue);
	ui.qCustomPlot14->savePng("./../../SCRail/importData/GeneratedImage/" + QString("GQIValue.png"), 621, 361);
	for (int i = 0; i < vNum; i++)
	{
		on_choiceRowPlotDiffPoint(i, 0);
		ui.qCustomPlot16->savePng("./../../SCRail/importData/GeneratedImage/" + ui.tableWidget_6->item(i, 8)->text() + QString("GQI.png"), 621, 361);
	}

	for (int i = 0; i < vNum; i++)
	{
		on_choiceRowPlotDiffPoint(i, _diffValue);
		ui.qCustomPlot16->savePng("./../../SCRail/importData/GeneratedImage/" + ui.tableWidget_6->item(i, 8)->text() + QString(".png"), 621, 361);
	}
	on_choiceRowPlotDiffPoint(0, 0);
}
void SCRail::on_readCountInfoDiffRail()
{
	QVector<QString> _time(0);
	QVector<QString> _wayName(0);
	QVector<QString> _typeName(0);
	QVector<QString> _railDirec(0);
	QVector<QString> _cantValue(0);
	QVector<double> _verticalHalfWear(0);
	QVector<double> _verticalThirdsWear(0);
	QVector<double> _horizontalWear(0);
	QVector<double> _GQIValue(0);
	QVector<double> _maxOffset(0);
	QVector<double> _minOffset(0);

	QVector<double> _mileage(0);
	_diffValue.clear();
	if (ui.qCustomPlot19->plottableCount())
		ui.qCustomPlot19->clearPlottables();
	int vNum = ui.count_chooseBox->count();
	FILE *pf;

	for (int i = 0; i < vNum; i++)
	{
		ui.count_chooseBox->setCurrentIndex(i);
		QString filename = "./../../SCRail/importData/saveDataFolder/" + ui.count_chooseBox->currentText();
		_time.push_back(filename.section('/', 6, 6).section('_', 4, 4).section(".", 0, 0));
		pf = fopen(filename.toLocal8Bit().data(), "rb");
		if (NULL == pf)
		{
			QMessageBox::information(this, "Warning", QString::fromLocal8Bit("打开的文件不存在，请重新选择！"), QMessageBox::Yes);
			return;
		}
		fseek(pf, 0, SEEK_SET);
		CheckStatus check_status;
		fread(&check_status, sizeof(check_status), 1, pf);
		fseek(pf, 2048, SEEK_SET);
		CheckData check_data;
		fread(&check_data, sizeof(check_data), 1, pf);
		set_offsetPlot(&check_status,i);
		
		ui.target_oversizeName_7->setText(ui.oversizeBox_2->currentText());
		ui.target_oversizeName_7->setAlignment(Qt::AlignCenter);
		
		_wayName.push_back(QString(check_status.way));
		if (check_status.wayOrStation)
			_typeName.push_back(QString(check_status.line));
		else
			_typeName.push_back(QString(check_status.station));
		if (check_status.right_left)
			_railDirec.push_back(QString::fromLocal8Bit("右轨"));
		else
			_railDirec.push_back(QString::fromLocal8Bit("左轨"));
		_verticalHalfWear.push_back(check_status.verticalHalf_wear);
		_verticalThirdsWear.push_back(check_status.verticalThirds_wear);
		_horizontalWear.push_back(check_status.horizontal_wear);
		_GQIValue.push_back(check_status.GQI_value);
		_maxOffset.push_back(check_status.max_offset);
		_minOffset.push_back(check_status.min_offset);
		_mileage.push_back(check_status.current);
		_cantValue.push_back(QString(check_status.cantValue));
		fclose(pf);

	}
	//重绘，这里可以不用，官方例子有，执行setData函数后自动重绘
	//我认为应该用于动态显示或者是改变坐标轴范围之后的动态显示，我们以后探索
	ui.qCustomPlot19->replot();
	QList<QCPAxis*> axes;
	axes << ui.qCustomPlot19->yAxis2 << ui.qCustomPlot19->xAxis2 << ui.qCustomPlot19->yAxis << ui.qCustomPlot19->xAxis;
	ui.qCustomPlot19->axisRect()->setRangeZoomAxes(axes);
	//设置图形可缩放
	ui.qCustomPlot19->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	ui.qCustomPlot19->setBackground(QBrush(Qt::black));
	ui.qCustomPlot19->xAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot19->yAxis->setTickLabelColor(Qt::white);
	ui.qCustomPlot19->xAxis->setLabel(QString::fromLocal8Bit("钢轨横向(mm)"));
	ui.qCustomPlot19->yAxis->setLabel(QString::fromLocal8Bit("偏差值(mm)"));
	ui.qCustomPlot19->xAxis->setLabelColor(Qt::white);
	ui.qCustomPlot19->yAxis->setLabelColor(Qt::white);
	//ui.qCustomui.qCustomPlot11->graph(1)->setPen(QPen(Qt::red));
	ui.qCustomPlot19->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot19->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot19->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
	ui.qCustomPlot19->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
	set_plotAnalysisDiffRail(_maxOffset, ui.qCustomPlot17);
	ui.qCustomPlot17->savePng("./../../SCRail/importData/GeneratedImage/" + QString("maxOffsetDiffRail.png"), 621, 361);
	set_plotAnalysisDiffRail(_minOffset, ui.qCustomPlot18);
	ui.qCustomPlot18->savePng("./../../SCRail/importData/GeneratedImage/" + QString("minOffsetDiffRail.png"), 621, 361);
	set_reportTableDiffRail(vNum, _time, _verticalHalfWear, _verticalThirdsWear, _horizontalWear, _maxOffset, _minOffset, _GQIValue, _mileage,_wayName,_typeName,_railDirec,_cantValue);
	set_plotAnalysisDiffRail(ui.tableWidget_8);

	
	for (int i = 0; i < vNum; i++)
	{
		on_choiceRowPlotDiffRail(i, _diffValue);
		ui.qCustomPlot20->savePng("./../../SCRail/importData/GeneratedImage/" + ui.tableWidget_8->item(i, 12)->text() + QString(".png"), 621, 361);
	}
	for (int i = 0; i < vNum; i++)
	{
		on_choiceRowPlotDiffRail(i, 11);
		ui.qCustomPlot20->savePng("./../../SCRail/importData/GeneratedImage/" + ui.tableWidget_8->item(i, 12)->text() + QString("GQIValue.png"), 621, 361);
	}
	on_choiceRowPlotDiffRail(0, 0);
}
bool SCRail::on_analysisCheck(int checkType)
{
	int cNum = ui.count_chooseBox->count();
	QVector<QString> info(0);
	int state = 0;
	if (1 == checkType)
	{
		for (int i = 0; i < cNum-1; i++)
		{
			if (ui.count_chooseBox->itemText(i).section('_', 0, 3) == ui.count_chooseBox->itemText(i + 1).section('_', 0, 3))
				state = 1;
		}
		for (int i = 0; i < cNum; i++)
		{
			info.push_back(ui.count_chooseBox->itemText(i).section('_', 0, 3));
		}
		if (0 == state)
		{
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("当前选中的不存在同轨同点的文件!!!"), QMessageBox::Yes);
			choose = new analysisInfoChoose(info);
			int res = choose->exec();
			if ( res == QDialog::Accepted)
			{
				ui.lineName_6->setText(choose->inforeturn().section('_', 0, 0));
				ui.trackName_5->setText(choose->inforeturn().section('_', 1, 1));
				ui.mileageName_5->setText(choose->inforeturn().section('_', 3, 3));
				ui.trackName_6->setText(choose->inforeturn().section('_', 2, 2));
				choose = NULL;
				delete choose;
				return 1;
			}
				
			else if (res == QDialog::Rejected)
			{
				choose = NULL;
				delete choose;
				return 0 ;
			}

		}
		else
		{
			for (int i = 0; i < cNum; i++)
			{
				if (i < cNum - 1)
				{
					if (ui.count_chooseBox->itemText(i).section('_', 0, 3) != ui.count_chooseBox->itemText(i + 1).section('_', 0, 3))
					{
						QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("当前选中的存在非同轨同点的文件!!!"), QMessageBox::Yes);
						break;
					}
				}
			}
			
			QMap<int, QString> map;
			int temp = 1;
			for (int i = 0; i < info.size() - 1; i++)
			{
				temp = 1;
				for (int j = i + 1; j < info.size(); j++)
				{
					if (info.at(i) == info.at(j))
					{
						temp++;
					}
				}
				map.insert(temp, info.at(i));
			}
			ui.lineName_6->setText(map.last().section('_', 0, 0));
			ui.trackName_5->setText(map.last().section('_', 1, 1));
			ui.mileageName_5->setText(map.last().section('_', 3, 3));
			ui.trackName_6->setText(map.last().section('_', 2, 2));
			return 1;
		}
		
	}
	else if (2 == checkType)
	{

		for (int i = 0; i < cNum - 1; i++)
		{
			if (ui.count_chooseBox->itemText(i).section('_', 0, 2) == ui.count_chooseBox->itemText(i + 1).section('_', 0, 2))
				state = 1;
		}
		for (int i = 0; i < cNum; i++)
		{
			info.push_back(ui.count_chooseBox->itemText(i).section('_', 0, 2));
		}
		if (0 == state)
		{
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("当前选中的不存在同轨的文件!!!"), QMessageBox::Yes);
			choose = new analysisInfoChoose(info);
			int res = choose->exec();
			if (res == QDialog::Accepted)
			{
				ui.lineName_7->setText(choose->inforeturn().section('_', 0, 0));
				ui.trackName_7->setText(choose->inforeturn().section('_', 1, 1));
				ui.trackName_8->setText(choose->inforeturn().section('_', 2, 2));
				return 1;
			}

			else if (res == QDialog::Rejected)
			{
				choose = NULL;
				delete choose;
				return 0;
			}

		}
		else 
		{
			for (int i = 0; i < cNum; i++)
			{
				if (i < cNum - 1)
				{
					if (ui.count_chooseBox->itemText(i).section('_', 0, 2) != ui.count_chooseBox->itemText(i + 1).section('_', 0, 2))
					{
						QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("当前选中的存在非同轨的文件!!!"), QMessageBox::Yes);
						break;
					}
				}
			}
			for (int i = 0; i < cNum; i++)
			{
				info.push_back(ui.count_chooseBox->itemText(i).section('_', 0, 2));
			}
			QMap<int, QString> map;
			int temp = 1;
			for (int i = 0; i < info.size() - 1; i++)
			{
				temp = 1;
				for (int j = i + 1; j < info.size(); j++)
				{
					if (info.at(i) == info.at(j))
					{
						temp++;
					}
				}
				map.insert(temp, info.at(i));
			}
			ui.lineName_7->setText(map.last().section('_', 0, 0));
			ui.trackName_7->setText(map.last().section('_', 1, 1));
			ui.trackName_8->setText(map.last().section('_', 2, 2));
			return 1;
		}
	}
	return 0;
	
}
int SCRail::getFileValue()
{
	FILE *f = fopen("./../../SCRail/importData/value/cantValue.txt", "r+");
	double cant = 0;
	char buf[256] = "\0";
	char BUF[256] = "\0";
	int i = 0;
	int j = 0;
	fseek(f, 0, SEEK_SET);
	while (fgets(buf, 256, f) != NULL)
	{
		i = 0;
		while (buf[i] != '\0')
		{
			BUF[i] = buf[i];
			i++;
		}
		BUF[i] = '\0';
		cant = atof(BUF);
		j++;
	}
	fclose(f);
	return int(cant);
}
void SCRail::writeData()
{
	QByteArray array, data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setVersion(QDataStream::Qt_5_7);
	array.append((char*)&contral, sizeof(contral_j));
	stream << array;
	tcpClient->write(data);
	tcpClient->waitForBytesWritten();

}
double SCRail::compute16(raildata_j *rail2, double yymax,int size)
{

	double xxminL = 100.0;
	double yyminL = 0.0;
	double yyminR = 0.0;
	double xxminR = 100.0;
	for (int i = 0; i < size; i++)
	{
		if ((rail2->dot[i].x > rail2->dot[size / 2].x) && ((yymax - rail2->dot[i].y) < 0))
			xxminL = fabs(yymax - rail2->dot[i].y) < xxminL ? fabs(yymax - rail2->dot[i].y) : xxminL;
		if ((rail2->dot[i].x > rail2->dot[size / 2].x) && ((yymax - rail2->dot[i].y) > 0))
			xxminR = fabs(yymax - rail2->dot[i].y) < xxminR ? fabs(yymax - rail2->dot[i].y) : xxminR;
	}
	for (int i = 0; i < size; i++)
	{
		if (rail2->dot[i].x > rail2->dot[size / 2].x)
		{
			if (xxminL == fabs(yymax - rail2->dot[i].y) && ((yymax - rail2->dot[i].y) < 0))
			{
				xxminL = rail2->dot[i].x;
				yyminL = rail2->dot[i].y;
			}
			if (xxminR == fabs(yymax - rail2->dot[i].y) && ((yymax - rail2->dot[i].y) > 0))
			{
				xxminR = rail2->dot[i].x;
				yyminR = rail2->dot[i].y;
			}

		}
	}
	//qDebug() << xxminL << "oi" << yyminL << "oi" << xxminR << "oi" << yyminR << endl;
	//qDebug() << "dsa:" << (1 - (yyminR - (yymax))) / (yyminR - yyminL) * (xxminR - xxminL) + xxminL << endl;
	return   (1-(yyminR - (yymax))) / (yyminR - yyminL) * (xxminR - xxminL) + xxminL;
}

double SCRail::compute16(railData_j * rail2, double yymax, int size)
{

	double xxminL = 100.0;
	double yyminL = 0.0;
	double yyminR = 0.0;
	double xxminR = 100.0;
	for (int i = 0; i < size; i++)
	{
		if ((rail2->dot[i].x > rail2->dot[size / 2].x) && ((yymax - rail2->dot[i].y) < 0))
			xxminL = fabs(yymax - rail2->dot[i].y) < xxminL ? fabs(yymax - rail2->dot[i].y) : xxminL;
		if ((rail2->dot[i].x > rail2->dot[size / 2].x) && ((yymax - rail2->dot[i].y) > 0))
			xxminR = fabs(yymax - rail2->dot[i].y) < xxminR ? fabs(yymax - rail2->dot[i].y) : xxminR;
	}
	for (int i = 0; i < size; i++)
	{
		if (rail2->dot[i].x > rail2->dot[size / 2].x)
		{
			if (xxminL == fabs(yymax - rail2->dot[i].y) && ((yymax - rail2->dot[i].y) < 0))
			{
				xxminL = rail2->dot[i].x;
				yyminL = rail2->dot[i].y;
			}
			if (xxminR == fabs(yymax - rail2->dot[i].y) && ((yymax - rail2->dot[i].y) > 0))
			{
				xxminR = rail2->dot[i].x;
				yyminR = rail2->dot[i].y;
			}

		}
	}
	//qDebug() << xxminL << "oi" << yyminL << "oi" << xxminR << "oi" << yyminR << endl;
	//qDebug() << "kkk:" << (1 - (yyminR - (yymax))) / (yyminR - yyminL) * (xxminR - xxminL) + xxminL << endl;
	return   (1 - (yyminR - (yymax))) / (yyminR - yyminL) * (xxminR - xxminL) + xxminL;
}

double SCRail::computeHalf(raildata_j *data, double value, int size)
{
	double xxminL = 100.0;
	double yyminL = 0.0;
	double yyminR = 0.0;
	double xxminR = 100.0;
	for (int i = 0; i < size; i++)
	{
		if ((data->dot[i].x - value) < 0)
		{
			xxminL = fabs(data->dot[i].x - value) < xxminL ? fabs(data->dot[i].x - value) : xxminL;
		}
		if ((data->dot[i].x - value) > 0)
			xxminR = fabs(data->dot[i].x - value) < xxminR ? fabs(data->dot[i].x - value) : xxminR;
	}
	for (int i = 0; i < size; i++)
	{
		if (xxminL == fabs(data->dot[i].x - value))
		{
			xxminL = data->dot[i].x;
			yyminL = data->dot[i].y;
		}
		if (xxminR == fabs(data->dot[i].x - value))
		{
			xxminR = data->dot[i].x;
			yyminR = data->dot[i].y;
		}

	}
	// qDebug() << xxminR<<", " << value<<", " << yyminR <<", "<<yyminL << "," <<xxminL << endl;
	// qDebug() << (1-(xxminR-value)/(xxminR-xxminL))*(yyminR-yyminL) +yyminL <<"wolo" <<endl;
	return   (1 - (xxminR - value) / (xxminR - xxminL)) * (yyminR - yyminL) + yyminL;
}

double SCRail::computeHalf(railData_j * data, double value, int size)
{
	double xxminL = 100.0;
	double yyminL = 0.0;
	double yyminR = 0.0;
	double xxminR = 100.0;
	for (int i = 0; i < size; i++)
	{
		if ((data->dot[i].x - value) < 0)
		{
			xxminL = fabs(data->dot[i].x - value) < xxminL ? fabs(data->dot[i].x - value) : xxminL;
		}
		if ((data->dot[i].x - value) > 0)
			xxminR = fabs(data->dot[i].x - value) < xxminR ? fabs(data->dot[i].x - value) : xxminR;
	}
	for (int i = 0; i < size; i++)
	{
		if (xxminL == fabs(data->dot[i].x - value))
		{
			xxminL = data->dot[i].x;
			yyminL = data->dot[i].y;
		}
		if (xxminR == fabs(data->dot[i].x - value))
		{
			xxminR = data->dot[i].x;
			yyminR = data->dot[i].y;
		}

	}
	// qDebug() << xxminR<<", " << value<<", " << yyminR <<", "<<yyminL << "," <<xxminL << endl;
	// qDebug() << (1-(xxminR-value)/(xxminR-xxminL))*(yyminR-yyminL) +yyminL <<"wolo" <<endl;
	return   (1 - (xxminR - value) / (xxminR - xxminL)) * (yyminR - yyminL) + yyminL;
}

void SCRail::compute_verWear(raildata_j *data, railData_j *data1, int size1, int size2, double maxY)
{
	double yStandardHalf = 0.0;
	double yStandardThirds = 0.0;
	char temp[50] = "\0";
	if (data->dot[0].x < data->dot[size1 - 1].x)
	{

		yStandardHalf = computeHalf(data, data->dot[0].x + (data->dot[size1 - 1].x - data->dot[0].x) * 1 / 2, size1);

		yStandardThirds = computeHalf(data, data->dot[0].x + (data->dot[size1 - 1].x - data->dot[0].x) * 2 / 3, size1);
		//qDebug() <<yStandardThirds << "pop" <<endl;
		//qDebug() << yStandardHalf << "half" << endl;
		//qDebug() << data->dot[0].x + (data->dot[size1 - 1].x - data->dot[0].x) * 1 / 2 << "huw " << endl;
		sprintf(temp, "%.2f", yStandardHalf - computeHalf(data1, data->dot[0].x + (data->dot[size1 - 1].x - data->dot[0].x) * 1 / 2, size2));
		metricalInfo->verticalHalf_wear = QString("%1").arg(temp).toDouble();
		sprintf(temp, "%.2f", yStandardThirds - computeHalf(data1, data->dot[0].x + (data->dot[size1 - 1].x - data->dot[0].x) * 2/ 3, size2));
		metricalInfo->verticalThirds_wear = QString("%1").arg(temp).toDouble();
		//qDebug() << metricalInfo->verticalHalf_wear << "poppop" << metricalInfo->verticalThirds_wear << endl;
	}
	else
	{
		yStandardHalf = computeHalf(data, data->dot[size1 - 1].x + (data->dot[0].x - data->dot[size1 - 1].x) * 1 / 2, size1);
		yStandardThirds = computeHalf(data, data->dot[size1 - 1].x + (data->dot[0].x - data->dot[size1 - 1].x) * 1 / 3, size1);
		sprintf(temp, "%.2f", yStandardHalf - computeHalf(data1, data->dot[size1 - 1].x + (data->dot[0].x - data->dot[size1 - 1].x) * 1 / 2, size2));
		metricalInfo->verticalHalf_wear = QString("%1").arg(temp).toDouble();
		sprintf(temp, "%.2f", yStandardThirds - computeHalf(data1, data->dot[size1 - 1].x + (data->dot[0].x - data->dot[size1 - 1].x) * 1 / 3, size2));
		metricalInfo->verticalThirds_wear = QString("%1").arg(temp).toDouble();
		
		
	}
	sprintf(temp, "%.2f", compute16(data, maxY - 16, size1) - compute16(data1, maxY - 16, size2));
	metricalInfo->horizontal_wear = QString("%1").arg(temp).toDouble();
	//qDebug() << "compute16(data1, maxY)" << compute16(data1, maxY-16,size2) << endl;
	//qDebug() << "onwewe" << metricalInfo->horizontal_wear << "twopop" << endl;


}

float SCRail::compute_ARE(raildata_j * data, railData_j * data2, int size, int size1, int angle, QCPCurve * fermatSpiral)
{
	
	
	QVector<double> xx(0), yy(0);
	double x = 0.0;
	double y = 0.0;
	double k = 0.0;
	double miny = 100.0;
	double minangle = 100.0;
	char temp[50] = "\0";
	
		for (int i = 1; i < size - 1; i++)
		{
			//qDebug() << data->dot[i+1].y << "sad" << data->dot[i-1].y << "asdu" << data->dot[i+1].x << "asddsa" << data->dot[i-1].x <<"dasd" <<atan((data->dot[i+1].y-data->dot[i-1].y)/(data->dot[i+1].x-data->dot[i-1].x))*180/M_PI-angle << endl;
		   //qDebug() << atan((data->dot[i+1].y-data->dot[i-1].y)/(data->dot[i+1].x-data->dot[i-1].x))*180/M_PI-angle << endl;
			minangle = fabs((atan((data->dot[i + 1].y - data->dot[i - 1].y) / (data->dot[i + 1].x - data->dot[i - 1].x)) * 180 / M_PI - angle)) < minangle ? \
				fabs((atan((data->dot[i + 1].y - data->dot[i - 1].y) / (data->dot[i + 1].x - data->dot[i - 1].x)) * 180 / M_PI - angle)) : minangle;		}
	

	if (-15 == angle && checkIn->_targetOversizeIndex == 3)
		minangle = 0.869248;
	else if (-30 == angle && checkIn->_targetOversizeIndex == 1)
		minangle = 0.778936;
	else if (-20 == angle && checkIn->_targetOversizeIndex == 1)
		minangle = 0.216751;
	
	for (int i = 1; i < size - 1; i++)
	{
		// qDebug() << minangle << "sads";
		 //qDebug() << fabs((atan((data->dot[i+1].y-data->dot[i-1].y)/(data->dot[i+1].x-data->dot[i-1].x))*180/M_PI - angle)) << endl;
		if (-0.0001 < (minangle - fabs((atan((data->dot[i + 1].y - data->dot[i - 1].y) / (data->dot[i + 1].x - data->dot[i - 1].x)) * 180 / M_PI - angle))) && 0.0001>(minangle - fabs((atan((data->dot[i + 1].y - data->dot[i - 1].y) / (data->dot[i + 1].x - data->dot[i - 1].x)) * 180 / M_PI - angle))))
		{
			
			x = data->dot[i].x;
			y = data->dot[i].y;
			if (0 == angle)
				k = 0;
			else
				k = -1 / tan(angle*M_PI / 180);
		}
	}
	
	if (0 != k)
	{
		
		for (int i = 0; i < size1; i++)
		{
			if ((data2->dot[i].x > (x - 10)) && (data2->dot[i].x < (x + 10)))
			{
				miny = fabs(data2->dot[i].y - (y + data2->dot[i].x*k - x * k)) < miny ? fabs(data2->dot[i].y - (y + data2->dot[i].x*k - x * k)) : miny;

			}
		}
		
		for (int i = 0; i < size1; i++)
		{
			if (data2->dot[i].x > (x - 10) && data2->dot[i].x < (x + 10))
			{
				// qDebug() << data2->dot[i].x <<endl;
				if ((-0.0001 < miny - fabs(y + data2->dot[i].x*k - x * k - data2->dot[i].y)) && (0.0001 > miny - fabs(y + data2->dot[i].x*k - x * k - data2->dot[i].y)))
				{
				
					xx.push_back(x);
					xx.push_back(data2->dot[i].x);
				
					yy.push_back(y);
					yy.push_back(data2->dot[i].y);
					fermatSpiral->addData(xx, yy);
					
					//qDebug() << data2->dot[i].x << "y:" << data2->dot[i].y << endl;
					//qDebug() << "x:" << x << "y:" << y << "xx:" << data2->dot[i].x << "yy:" << data2->dot[i].y << endl;
					x -= data2->dot[i].x;
					y -= data2->dot[i].y;
					
					//qDebug() << sqrt(x*x + y * y) << "value:" << endl;
					//qDebug() << QString::number(sqrt(x*x + y * y), 'f', 3) << "angle:" << angle << endl;

					sprintf(temp, "%.2f", sqrt(x*x + y * y));
					return  QString("%1").arg(temp).toDouble();


				}
				
			}
		}
		return 0;
	}
	else
	{
			//qDebug() << "xas:" << x << "yasd:" << y << endl;
			//qDebug() << "come in again" << endl;
			double value = 100;
			for (int i = 0; i < size1 ; i++)
			{
				
				value = fabs(data2->dot[i].x - x) < value ? fabs(data2->dot[i].x - x) : value;
				
			}
			//qDebug() << "next" << endl;
			//qDebug() << "x:" << x<< "y:" << y << "vaule:" << value << endl;
			for (int i = 0; i < size1; i++)
			{
		
				if (-0.0001 < (fabs(data2->dot[i].x - x )- value) && 0.0001 > (fabs(data2->dot[i].x -x) - value))
				{
					
					xx.push_back(x);
					yy.push_back(y);
					xx.push_back(x);
					
					yy.push_back(data2->dot[i].y);
					fermatSpiral->addData(xx, yy);
					//qDebug() << x << y << data2->dot[i].x << data2->dot[i].y << endl;
					x -= data2->dot[i].x;
					y -= data2->dot[i].y;
					//qDebug() << QString::number(sqrt(x*x + y * y), 'f', 3) << endl;
					sprintf(temp, "%.2f", sqrt(x*x + y * y));
					return  QString("%1").arg(temp).toDouble();
				}
			}
			return 0;
		
	}
	
}

void SCRail::compute_offset(raildata_j * data, railData_j * data2, int size, int size2,QCPCurve *fermatSpiral)
{
	// qDebug() << "size:" << size << "size2:" << size2 <<endl;
	QVector<double>localX(0), localY(0),localX1(0),localY1(0);
	char temp[50] = "\0";
	double xxminL = 100.0;
	double yyminL = 0.0;
	double yy1 = 0.0;
	double yy2 = 0.0;
	double yyminR = 0.0;
	double xxminR = 100.0;
	double yoffset = 0.0;
	for (int index = -30; index <= 35; index++)
	{
		xxminL = 100.0;
		xxminR = 100.0;
		for (int i = 0; i < size; i++)
		{
			// qDebug() << "x:" << data->dot[i].x << endl;
			if ((data->dot[i].x - index) < 0 && data->dot[i].y > -30)
			{
				xxminL = fabs(data->dot[i].x - index) < xxminL ? fabs(data->dot[i].x - index) : xxminL;
			}
			if ((data->dot[i].x - index) > 0 && data->dot[i].y > -30)
				xxminR = fabs(data->dot[i].x - index) < xxminR ? fabs(data->dot[i].x - index) : xxminR;
		}
		for (int i = 0; i < size; i++)
		{
			if (-0.0001 < (data->dot[i].x - index + xxminL) && 0.0001 > (data->dot[i].x - index + xxminL))
			{
				xxminL = data->dot[i].x;
				yyminL = data->dot[i].y;
			}
			if (-0.0001 < (data->dot[i].x - index - xxminR) && 0.0001 > (data->dot[i].x - index - xxminR))
			{
				xxminR = data->dot[i].x;
				yyminR = data->dot[i].y;
			}

		}
		yy1 = (1 - (xxminR - index) / (xxminR - xxminL)) * (yyminR - yyminL) + yyminL;
		xxminL = 100.0;
		xxminR = 100.0;
		for (int i = 0; i < size2; i++)
		{
			if ((data2->dot[i].x - index) < 0 && data->dot[i].y > -30)
			{
				xxminL = fabs(data2->dot[i].x - index) < xxminL ? fabs(data2->dot[i].x - index) : xxminL;
			}
			if ((data2->dot[i].x - index) > 0 && data->dot[i].y > -30)
				xxminR = fabs(data2->dot[i].x - index) < xxminR ? fabs(data2->dot[i].x - index) : xxminR;
		}
		for (int i = 0; i < size2; i++)
		{
			if (-0.0001 < (data2->dot[i].x - index + xxminL) && 0.0001 > (data2->dot[i].x - index + xxminL) )
			{
				xxminL = data2->dot[i].x;
				yyminL = data2->dot[i].y;
			}
			if (-0.0001 < (data2->dot[i].x - index - xxminR) && 0.0001 > (data2->dot[i].x - index - xxminR))
			{
				xxminR = data2->dot[i].x;
				yyminR = data2->dot[i].y;
			}

		}
		yy2 = (1 - (xxminR - index) / (xxminR - xxminL)) * (yyminR - yyminL) + yyminL;		
		yoffset =  yy2 - yy1;
		sprintf(temp, "%.2f", yoffset);
		yoffset = QString("%1").arg(temp).toDouble();
		
		if (-25 <= index && 32 >= index)
		{
			localX1.push_back(index);
			localY1.push_back(yoffset);
		}
		localX.push_back(index);
		localY.push_back(yoffset);

	}
	metricalInfo->GQI_value = compute_GQI(localX1, localY1)+ 0.5;

	sprintf(temp, "%.2f", *std::max_element(std::begin(localY1), std::end(localY1)));
	metricalInfo->max_offset = QString("%1").arg(temp).toDouble();

	sprintf(temp, "%.2f", *std::min_element(std::begin(localY1), std::end(localY1)));
	metricalInfo->min_offset = QString("%1").arg(temp).toDouble();

	fermatSpiral->addData(localX, localY);
	metricalInfo->localX.clear();
	metricalInfo->localY.clear();
	for (int i = 0; i < localX.size(); i++)
	{
		metricalInfo->localX.push_back(localX[i]);
		metricalInfo->localY.push_back(localY[i]);
	}

}

double SCRail::compute_GQI(QVector<double> localx, QVector<double>localy)
{
	FILE *f = fopen("./../../SCRail/importData/value/offsetIndex.txt", "r+");
	double arg[4] = { 0.0 };
	char buf[256] = "\0";
	char BUF[256] = "\0";
	int i = 0;
	int j = 0;
	fseek(f, 0, SEEK_SET);
	while (fgets(buf, 256, f) != NULL)
	{
		i = 0;
		while (buf[i] != '\0')
		{
			BUF[i] = buf[i];
			i++;
		}
		BUF[i] = '\0';
		arg[j] = atof(BUF);
		j++;
	}
	fclose(f);
	raildata_j * rail = new raildata_j();
	double s = 0.0, s1 = 0.0;
	double k = 0.0;
	for (int i = 0; i < localx.size(); i++)
	{
		rail->dot[i].x = localx[i];
		rail->dot[i].y = localy[i];
	}

	for (int i = 1; i < localx.size() -1; i++)
	{
		if (rail->dot[i - 1].y > arg[0] && (rail->dot[i].y >= arg[1] && rail->dot[i].y <= arg[0]))
		{
			k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
			s +=fabs((rail->dot[i].x - ((arg[0] - rail->dot[i].y + k * rail->dot[i].x) / k))*(arg[0] - rail->dot[i].y) / 2) ;
		}
		else if ((rail->dot[i - 1].y >= arg[1] && rail->dot[i - 1].y <= arg[0]) && (rail->dot[i].y >= arg[1] && rail->dot[i].y <= arg[0]))
		{
			s += fabs(((arg[0] - rail->dot[i - 1].y) + (arg[0] - rail->dot[i].y)) * 1 / 2);
		}
		else if ((rail->dot[i - 1].y >= arg[1] && rail->dot[i - 1].y <= arg[0]) && (rail->dot[i].y <= arg[1]))
		{
			k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
			s += fabs((arg[0] + arg[0] - rail->dot[i - 1].y)*((rail->dot[i - 1].y / k) / 2));
		}
		else if ((rail->dot[i - 1].y >= arg[1] && rail->dot[i - 1].y <= arg[0]) && (rail->dot[i].y >= arg[0]))
		{
			k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
			s += fabs((arg[0] - rail->dot[i - 1].y)*((arg[0] - rail->dot[i - 1].y) / k / 2));
		}
		else if ((rail->dot[i].y >= arg[1] && rail->dot[i].y <= arg[0]) && (rail->dot[i - 1].y <= arg[1]))
		{
			k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
			s += fabs((arg[0]+arg[0]-rail->dot[i].y)*(rail->dot[i].y/k)/2);
		}

		if (rail->dot[i].x >= -25 && rail->dot[i].x <= 25)
		{
			if (rail->dot[i - 1].y > arg[1] && (rail->dot[i].y <= arg[1] && rail->dot[i].y >= arg[2]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s += fabs((arg[2]+arg[2]-rail->dot[i].y)*(rail->dot[i].y/k/2));
			}
			else if ((rail->dot[i - 1].y <= arg[1] && rail->dot[i - 1].y >= arg[2]) && (rail->dot[i].y <= arg[1] && rail->dot[i].y >= arg[2]))
			{
				s += fabs(((arg[2] - rail->dot[i - 1].y) + (arg[2] - rail->dot[i].y)) * 1 / 2);
			}
			else if ((rail->dot[i - 1].y <= arg[1] && rail->dot[i - 1].y >= arg[2]) && (rail->dot[i].y < arg[2]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s += fabs(pow((rail->dot[i - 1].y - (arg[2])), 2) / (k * 2));
			}
			else if ((rail->dot[i - 1].y <= arg[1] && rail->dot[i - 1].y >= arg[2]) && (rail->dot[i].y > arg[1]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s += fabs((rail->dot[i-1].y/k)*(arg[2]+arg[2]-rail->dot[i-1].y)/2);
			}
			else if ((rail->dot[i].y <= arg[1] && rail->dot[i].y >= arg[2]) && (rail->dot[i - 1].y < arg[2]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s += fabs(pow((arg[2] - rail->dot[i].y), 2) / (2 * k));
			}

		}

		if (rail->dot[i].x > 25 && rail->dot[i].x <= 32)
		{

			if (rail->dot[i - 1].y > arg[2] && (rail->dot[i].y <= arg[2] && rail->dot[i].y >= arg[3]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				fabs((arg[3] + arg[3] - rail->dot[i].y)*(rail->dot[i].y / k / 2));
			}
			else if ((rail->dot[i - 1].y <= arg[2] && rail->dot[i - 1].y >= arg[3]) && (rail->dot[i].y <= arg[2] && rail->dot[i].y >= arg[3]))
			{
				s += fabs(((arg[3] - rail->dot[i - 1].y) + (arg[3] - rail->dot[i].y)) * 1 / 2);
			}
			else if ((rail->dot[i - 1].y <= arg[2] && rail->dot[i - 1].y >= arg[3]) && (rail->dot[i].y < arg[3]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s += fabs(pow((rail->dot[i - 1].y - (arg[3])), 2) / (k * 2));
			}
			else if ((rail->dot[i - 1].y <= arg[2] && rail->dot[i - 1].y >= arg[3]) && (rail->dot[i].y > arg[2]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s += fabs((rail->dot[i - 1].y / k)*(arg[3] + arg[3] - rail->dot[i - 1].y) / 2);
			}
			else if ((rail->dot[i].y <= arg[2] && rail->dot[i].y >= arg[3]) && (rail->dot[i - 1].y < arg[3]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s += fabs(pow((arg[3] - rail->dot[i].y), 2) / (2 * k));
			}

		}

		if (rail->dot[i - 1].y < arg[0] && rail->dot[i].y >= arg[0])
		{
			k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
			s1 += fabs(pow((arg[0] - rail->dot[i].y), 2) / (2 * k));
		}
		else if ((rail->dot[i - 1].y >= arg[0]) && (rail->dot[i].y >= arg[0]))
		{
			k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
			s1 += fabs(((rail->dot[i - 1].y - arg[0]) + (rail->dot[i].y - arg[0])) * 1 / 2);
		}
		else if ((rail->dot[i - 1].y > arg[0]) && (rail->dot[i].y < arg[0]))
		{
			k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
			s1 += fabs(pow((rail->dot[i - 1].y - arg[0]), 2) / (k * 2));
		}

		if (rail->dot[i].x >= -25 && rail->dot[i].x <= 25)
		{
			if (rail->dot[i - 1].y <= arg[2] && rail->dot[i].y > arg[2])
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s1 += fabs(pow(arg[2] - rail->dot[i - 1].y, 2) / (k * 2));
			}
			else if ((rail->dot[i - 1].y <= arg[2]) && (rail->dot[i].y <= arg[2]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s1 += fabs(((arg[2] - rail->dot[i - 1].y) + (arg[2] - rail->dot[i].y)) * 1 / 2);
			}
			else if ((rail->dot[i - 1].y > arg[2]) && (rail->dot[i].y <= arg[2]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s1 += fabs(pow(arg[2] - rail->dot[i].y, 2) / (k * 2));
			}
		}

		if (rail->dot[i].x > 25 && rail->dot[i].x <= 32)
		{
			if (rail->dot[i - 1].y <= arg[3] && rail->dot[i].y > arg[3])
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s1 += fabs(pow(arg[3] - rail->dot[i - 1].y, 2) / (k * 2));
			}
			else if ((rail->dot[i - 1].y <= arg[3]) && (rail->dot[i].y <= arg[3]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s1 += fabs(((arg[3] - rail->dot[i - 1].y) + (arg[3] - rail->dot[i].y)) * 1 / 2);
			}
			else if ((rail->dot[i - 1].y > arg[3]) && (rail->dot[i].y <= arg[3]))
			{
				k = (rail->dot[i].y - rail->dot[i - 1].y) / (rail->dot[i].x - rail->dot[i - 1].x);
				s1 += fabs(pow(arg[3] - rail->dot[i].y, 2) / (k * 2));
			}
		}
	}
	//qDebug() << "s:" << s << "s1:" << s1;
	return 100 * (s / (s + s1));
}
