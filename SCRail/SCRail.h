#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SCRail.h"
#include"CheckInfo.h"
#include"qcustomplot.h"
#include"save_info.h"
#include<QGridLayout>
#include<QTimer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include"printinfo.h"
#include"analysisinfochoose.h"
#pragma pack(push)
#pragma pack(4)
typedef struct dot {
	float x, y;

	dot()
	{
		x = 0.0;
		y = 0.0;
	}
}dot_j;

#pragma pack(pop)
#pragma pack(push)
#pragma pack(4)
typedef struct railData {
	int dotcount;
	dot_j dot[1280];
	int reserve[2];
	railData()
	{
		dotcount = 0;
		memset(dot, 0, sizeof(dot));
		memset(reserve, 0, sizeof(reserve));
	}
	railData operator=(railData* data)
	{
		this->dotcount = data->dotcount;
		memcpy(this->dot,data->dot, sizeof(this->dot));
		memcpy(this->reserve,data->reserve, sizeof(this->reserve));
		return *this;
	}
	railData(const railData &data)
	{
		this->dotcount = data.dotcount;
		memcpy(this->dot, data.dot, sizeof(this->dot));
		memcpy(this->reserve, data.reserve, sizeof(this->reserve));
	}

}railData_j;
#pragma pack(pop)
typedef struct raildata {
	int dotcount;
	dot_j dot[5120];
	raildata()
	{
		dotcount = 0;
		memset(dot, 0, sizeof(dot));
	}

}raildata_j;
typedef struct metricalInfo
{
	double verticalHalf_wear;
	double verticalThirds_wear;
	double horizontal_wear;
	int GQI_value;
	double max_offset;
	double min_offset;
	QVector<double>localX, localY;
	metricalInfo()
	{
		 verticalHalf_wear = 0.0;
		 verticalThirds_wear = 0.0;
		 horizontal_wear = 0.0;
		 GQI_value = 0;
		 max_offset = 0.0;
		 min_offset = 0.0;
		 
	}
}metricalInfo_j;
typedef struct {
	char wayname[20]; //��·����, 
	char linename[20]; //�б�
	int railtype; //���� 0/60kg,1/75kg,2/50kg,3/43kg��4Ϊ60Dkg��5Ϊ60Nkg
	bool status; //false/��ͣ,true/����
	bool shutdown; //�ػ�����
	int reserve[10]; //Ԥ��
}contral_j;
typedef struct avgValue
{
	double verticalHalf_wear;
	double verticalThirds_wear;
	double horizontal_wear;
	double GQI_value;
	double max_offset;
	double min_offset;
	avgValue()
	{
		verticalHalf_wear = 0.0;
		verticalThirds_wear = 0.0;
		horizontal_wear = 0.0;
		GQI_value = 0;
		max_offset = 0.0;
		min_offset = 0.0;

	}
}avgValue_j;
class SCRail : public QMainWindow
{
	Q_OBJECT

public:
	SCRail(QWidget *parent = Q_NULLPTR);
	~SCRail();

private slots:
	void setReal_check();
	void start_check();
	void abrasion_analysis();
	void grind_advice();
	void quality_analysis();
	void return_checkFace();
	void interface_switch();
	void interface_switch2();
	void interface_switch3();
	void return_mainFace();
	void set_connectState();
	void set_gatherState();
	void on_grindIsVisble(QAbstractButton*);
	void onButtonClicked(QAbstractButton*);
	void curRowSeparation(int);
	void gatherStateChange(int);
	void set_save();
	void updataTime();
	void unclose();
	void oskShow();
	void slot_mouseRelease(QMouseEvent*);
	void slot_mouseRelease2(QMouseEvent*);
	void slot_mouseRelease3(QMouseEvent*);
	void slot_mouseRelease4(QMouseEvent*);
	void on_generate();
	void readData();
	void ReadError(QAbstractSocket::SocketError);

	void set_reportAnalysis();
	void on_cellValueChanged(int , int);
	void on_cellValueDelete(int, int);
	void on_choiceRowPlot(int, int); //ͬ��ͬ�㵥������Ϣ
	void on_choiceRowPlotDiffPoint(int, int);//ͬ����㵥������Ϣ
	void on_choiceRowPlotDiffRail(int, int); //�����㵥������Ϣ
	void choose_menu();
	void on_startAnalysis(); //��ʼ��������
	void sortByColumn(int);//��������������
	void sortByColumn2(int);//ͬ��ͬ��������
	void sortByColumn3(int);//ͬ�����������
	void sortByColumn4(int);//������������
	void on_analysisGenerate();  //��������
	void on_analysisGenerateRailPoint(); //ͬ��ͬ���������
	void on_analysisGenerateRailDiffPoint(); //ͬ������������
	void on_analysisGenerateDiffRail();//�������������
private:
	void set_stateStyle(int state);
	void set_comBoxItem(const QString &path, QComboBox *combobox);
	void import_info();
	void init();
	void on_judgeIsOver(double real,double stand,QLabel *label); //�жϼ���ֵ�Ƿ��� ����������ɫ
	void on_judgeIsOver(double real, double stand, QTableWidgetItem *item);
	void set_radioGroup();
	void set_checkGroup();
	void save_checkInfo();
	void set_checkInfo();
	void set_Info(QLabel *label1, QLabel *label2, QLabel *label3, QLabel *label4, QLabel *label5,QLabel *label6);
	void set_plotNormal(int index);//���Ʊ�׼����
	void set_plotNormal(int index,QCustomPlot* plot, CheckData *check);//���ݷ������ƻ��Ʊ�׼����
	void set_plot(QCustomPlot *plot, int index);//��ĥ�����������(������µ�)
	void set_plot(int index, CheckStatus *state, CheckData *data); //ͬ��ͬ��ʵ�����׼�Ƚ�ͼ
	void set_plotDiffPoint(int index, CheckStatus *state, CheckData *data); //ͬ�����ʵ�����׼�Ƚ�ͼ
	void set_plotDiffRail(int index, CheckStatus *state, CheckData *data);  //������ʵ�����׼�Ƚ�ͼ
	void set_plot(int index, QCustomPlot* plot, QCustomPlot* plot1, CheckData *check); //��������
	void set_plotAnalysis(QVector<QString>time,QVector<double> vertical, QVector<double> hori, QVector<double> max, QVector<double> min,QVector<double> GQI);//ͬ��ͬ����Ϣͼ
	void set_plotAnalysis(QVector<QString>time, QVector<double> value , QCustomPlot *plot); //���Ƶ���ƫ��ͼ
	void set_plotAnalysisDiffPoint(QVector<QString>mileage, QVector<double> value, QCustomPlot *plot); //���Ƶ���ƫ��ͼ
	void set_plotAnalysisDiffRail(QVector<double> value, QCustomPlot *plot);
	void set_plotAnalysisDiffPoint(QVector<double>mile, QVector<double> vertical, QVector<double> hori, QVector<double> max, QVector<double> min, QVector<double> GQI);//ͬ�������Ϣͼ
	void set_plotAnalysisDiffRail(QTableWidget *table);
	void set_offsetPlot(int iNum,CheckStatus *check_status,QString time); //ͬ��ͬ��ƫ��ͼ
	void set_offsetPlot(int iNum, CheckStatus *check_status);           //ͬ�����ƫ��ͼ
	void set_offsetPlot(CheckStatus *check_status, int iNum);           //������ƫ��ͼ
	void set_offsetPlotUpdate(QTableWidget *table);
	void set_tableView(QTableWidget* table, float *tableInfo);
	void set_reportTableView();
	void set_reportTableView(QString filePath);
	void set_reportTableDiffTime(int fileCount, QVector<QString> _time, QVector<double> _verticalHalfWear, QVector<double> _verticalThirdsWear, \
	QVector<double> _horizontalWear,
	QVector<double> _GQIValue,
	QVector<double> _maxOffset,
	QVector<double> _minOffset,
	QVector<QString> _cantValue);
	void set_reportTableDiffMile(int fileCount, QVector<QString> _time, QVector<double> _verticalHalfWear, QVector<double> _verticalThirdsWear, \
		QVector<double> _horizontalWear,
		QVector<double> _GQIValue,
		QVector<double> _maxOffset,
		QVector<double> _minOffset,
		QVector<double> _mile,
		QVector<QString> _cantValue);
	void set_reportTableDiffRail(int fileCount, QVector<QString> _time, QVector<double> _verticalHalfWear, QVector<double> _verticalThirdsWear, \
		QVector<double> _horizontalWear,
		QVector<double> _GQIValue,
		QVector<double> _maxOffset,
		QVector<double> _minOffset,
		QVector<double> _mile,
		QVector<QString> _way,
		QVector<QString> _typeName,
		QVector<QString> _railDirec,
		QVector<QString> _cantValue);
	void on_sort(QComboBox *box,int state);
	double compute_slopeStandard(raildata_j *rail, double tempX, double tempY);
	double compute_slopeStandard(railData_j *rail, double tempX, double tempY);
	double compute_angle(double slope_actual, double slope_standard);
	void set_data();
	void whileTable(QString &tabletitle);
	void whileTableGQI(QString &tabletitle);
	void whileImage(QString &tabletitle);
	void whileImageGQI(QString &tabletitle);
	void whileTableGather(QString &tabletitle,QString status);
	void on_analysisTable(QString & tabletitle, QVector<QVector<float>> diffvalue);

	void whileTable(QString &tabletitle, QTableWidget *table);
	void whileTableGQI(QString &tabletitle,QTableWidget *table);
	void whileImage(QString &tabletitle, QTableWidget *table,int row);
	void whileImageGQI(QString &tabletitle, QTableWidget *table,int row);
	void whileTableGather(QString &tabletitle, QTableWidget *table,QString status);
	void on_analysisTable(QString & tabletitle, QVector<QVector<float>> diffvalue,int count);

	void whileTableRailDiffPoint(QString &tabletitle, QTableWidget *table);
	void whileTableGQIRailDiffPoint(QString &tabletitle, QTableWidget *table);
	void whileTableGatherDiffPoint(QString &tabletitle, QTableWidget *table, QString status);
	//void whileTableGQI(QString &tabletitle, QTableWidget *table);

	void whileTableDiffRail(QString &tabletitle, QTableWidget *table);
	void whileTableGQIDiffRail(QString &tabletitle, QTableWidget *table);
	void whileTableGatherDiffRail(QString &tabletitle, QTableWidget *table, QString status);

	void on_choiceRowPlot(int row,QVector<QVector<float>> &diffvalue); //ͬ��ͬ��ĥ��ͼ
	void on_choiceRowPlotDiffPoint(int row, QVector<QVector<float>> &diffvalue); //ͬ�����ĥ��ͼ
	void on_choiceRowPlotDiffRail(int row, QVector<QVector<float>> &diffvalue); //������ĥ��ͼ
	void on_writeInfo();
	void on_readInfo(const char* fileName);
	void on_sortFont(QTableWidget* table,int index); //����������ó�������ɫ
	void on_readCountInfo(); //ͬ��ͬ��
	void on_readCountInfoDiffPoint();  //ͬ�����
	void on_readCountInfoDiffRail(); //������
	bool on_analysisCheck(int checkType);
	int getFileValue();
	void writeData();
protected:
	bool eventFilter(QObject *watched, QEvent *event);
	bool event(QEvent *event);
	bool gestureEvent(QGestureEvent* event);
	void pinchTriggered(QPinchGesture* pinch);
	void customPlotZoom(short local, qreal zoomOffset);
	void zoomReplot(qreal zoomOffset, QCustomPlot* plot);
	void importData(QVector<double> &x, QVector<double> &y,const char* conf_path, raildata_j *data);
public:
	void set_checkInterfaceTitle();
	void set_Title(int temp);
	void set_stateNormal();
	double compute16(raildata_j* rail2, double yymax,int size);
	double compute16(railData_j* rail2, double yymax, int size);
	double computeHalf(raildata_j* data, double yymax, int size);
	double computeHalf(railData_j* data, double yymax, int size);
	void compute_verWear(raildata_j *data, railData_j *data1, int size1, int size2, double maxY);
	float compute_ARE(raildata_j *data, railData_j *data2, int size, int size2, int angle, QCPCurve *fermatSpiral);
	void compute_offset(raildata_j *data, railData_j *data2, int size, int size2, QCPCurve *fermatSpiral);
	double compute_GQI(QVector<double>localx, QVector<double>localy);
	
private:
	Ui::SCRailClass ui;
	QGridLayout *grid;
	int _temp;//���״̬
	int _version;//�汾��
	bool _state;
	short mouseLocal;
	bool SortUporDown = true;
	QString state_style = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px ;background:";
	QButtonGroup *groupMilNum,*groupDiffValue,*groupOffset, *groupAnalysisType;
	bool rail_chooseState;
	CheckInfo *checkIn;
	QVector<double> x,y;
	QVector<QCPCurve*> _grind, _quality,_offset;
	QVector<QString>_path;
	QVector<int> _row;
	bool compuComple;
	const char* analogData = "./../../SCRail/standardValue/left.txt";
	raildata_j *railData;	//Ŀ����������
	railData_j *_raildata;	//ʵ����������
	railData_j _objRaildata; //��ĥ���������ݶ���
	QTcpSocket *tcpClient;  //SOCKET�ӿ�
	QString IPAddress;
	QByteArray buffer = NULL;
	quint16 Iport;
	metricalInfo_j *metricalInfo;
	avgValue_j _avgValue;  //����ֵ��ֵ
	qreal scaleFactor;//���Ʒ�������ֵ
	CheckStatus check;
	CheckData *checkdata;
	QByteArray Message;
	analysisInfoChoose *choose;
	PrintInfo *printinfo;
	contral_j contral = { "\0","\0",0,0,0,{0}};
	bool _oversizeStateIndex[16];
	float diffValue[21] = { '\0' };
	QVector<QVector<float>> _diffValue;
	QCPCurve *fermatSpiralF = NULL;
	QCPCurve *fermatSpiral0 = NULL;
	QCPCurve *fermatSpiralF1 = NULL;
	QCPCurve *fermatSpiral01 = NULL;
	QCPCurve *fermatSpiral1 = NULL;//��ĥ����ʵ��
	QCPCurve *fermatSpiral2 = NULL;//��ĥ����Ŀ��
	QCPCurve *fermatSpiral3 = NULL;//��������ʵ��
	QCPCurve *fermatSpiral4 = NULL;//��������Ŀ��
	QCPCurve *fermatSpiral5 = NULL;
	QCPCurve *fermatSpiral6 = NULL;
	QCPCurve *fermatSpiral7 = NULL;
	//QCPCurve *fermatSpiral3 = new QCPCurve(plot->xAxis, plot->yAxis);
	QCPCurve* fermatSpiral[21] = { NULL };
	QString penColor[10] =
	{
		"#DC143C",
		"#008000",
		"#FFFF00",
		"#FFA500",
		"#FF0000",
		"#FF6347",
		"#00FF00",
		"#00FFFF",
		"#FFB6C1",
		"#DAA520"
	};
};
