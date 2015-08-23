#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	isRecording(false),
	isPlaying(false),
	interval_capture(15.00 * 1000),
	timer_capture(this),
	timer_updateProgress(this),
	dir_save()
{
	ui->setupUi(this);

	QObject::connect(this, &MainWindow::updated_interval,
					 this, &MainWindow::update_interval);
	QObject::connect(&timer_updateProgress,	&QTimer::timeout,
					 this,					&MainWindow::update_progress);

	QObject::connect(ui->radioButton_captureFreq_count,
					 &QRadioButton::toggled,
					 this,
					 &MainWindow::update_interval_count);
	QObject::connect(ui->comboBox_captureFreq_count_unit,
					 static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
					 this,
					 &MainWindow::update_interval_count);
	QObject::connect(ui->spinBox_captureFreq_count,
					 static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					 this,
					 &MainWindow::update_interval_count);

	QObject::connect(ui->radioButton_captureFreq_interval,
					 &QRadioButton::toggled,
					 this,
					 &MainWindow::update_interval_interval);
	QObject::connect(ui->spinBox_captureFreq_interval_sec,
					 static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
					 this,
					 &MainWindow::update_interval_interval);
	QObject::connect(ui->spinBox_captureFreq_interval_min,
					 static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					 this,
					 &MainWindow::update_interval_interval);
	QObject::connect(ui->spinBox_captureFreq_interval_hr,
					 static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
					 this,
					 &MainWindow::update_interval_interval);

	timer_capture.setInterval(interval_capture);
	timer_updateProgress.setInterval(40);

	ui->spinBox_captureFreq_interval_sec->setValue(15.00);
	update_interval();

	ui->radioButton_captureFreq_count->setChecked(false);
	setDisabled_captureFreq_count();
	ui->radioButton_captureFreq_interval->setChecked(true);
	setEnabled_captureFreq_interval();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::update_interval_count()
{
	int unit = 60; // seconds
	if (ui->comboBox_captureFreq_count_unit->currentText() == "minute") {
		unit = 60;
	} else if (ui->comboBox_captureFreq_count_unit->currentText() == "hour") {
		unit = 60*60;
	}
	int count = ui->spinBox_captureFreq_count->value();
	interval_capture = static_cast<double>(unit) / static_cast<double>(count);
	emit updated_interval();
}
void MainWindow::update_interval_interval()
{
	double sec = ui->spinBox_captureFreq_interval_sec->value();
	int min = ui->spinBox_captureFreq_interval_min->value();
	int hrs = ui->spinBox_captureFreq_interval_hr->value();
	interval_capture = sec + 60*min + 60*60*hrs;
	emit updated_interval();
}
void MainWindow::update_interval()
{
	timer_capture.setInterval(interval_capture * 1000);

	int total_sec = static_cast<int>(round(interval_capture));
	int sec = total_sec % 60;
	int total_min = (total_sec - sec) / 60;
	int min = total_min % 60;
	int total_hrs = (total_min - min) / 60;
	int hrs = total_hrs % 60;

	QString time;
	if (hrs > 0) {
		time += QString::number(hrs) + ":";
	}
	time += QString::number(min).rightJustified(2, '0') + ":" +
			QString::number(sec).rightJustified(2, '0');
	ui->progressBar_timer->setFormat(time);
}
void MainWindow::update_progress()
{
	double time_passed =
			static_cast<double>(interval_capture) -
			static_cast<double>(timer_capture.remainingTime())/1000.0;
	double fraction_passed =
			time_passed / static_cast<double>(interval_capture);
	int percentage = static_cast<int>(round(100 * fraction_passed));
	ui->progressBar_timer->setValue(percentage);
}

void MainWindow::setEnabled_captureFreq_count(bool isEnabled)
{
	ui->comboBox_captureFreq_count_unit->setEnabled(isEnabled);
	ui->spinBox_captureFreq_count->setEnabled(isEnabled);
}
void MainWindow::setEnabled_captureFreq_interval(bool isEnabled)
{
	ui->spinBox_captureFreq_interval_hr->setEnabled(isEnabled);
	ui->label_captureFreq_interval_colon_A->setEnabled(isEnabled);
	ui->spinBox_captureFreq_interval_min->setEnabled(isEnabled);
	ui->label_captureFreq_interval_colon_B->setEnabled(isEnabled);
	ui->spinBox_captureFreq_interval_sec->setEnabled(isEnabled);
}

void MainWindow::on_radioButton_captureFreq_count_toggled(bool checked)
{
	setEnabled_captureFreq_count(checked);
	setEnabled_captureFreq_interval(!checked);
}
void MainWindow::on_radioButton_captureFreq_interval_toggled(bool checked)
{
	setEnabled_captureFreq_interval(checked);
	setEnabled_captureFreq_count(!checked);
}

void MainWindow::on_pushButton_folder_clicked()
{
	dir_save = QFileDialog::getExistingDirectory(this,
												 "Save in folder...",
												 dir_save);
	ui->lineEdit_folder->setText(dir_save);
}

void MainWindow::on_button_start_clicked()
{
	isRecording = !isRecording;
	switch (isRecording) {
		case true :
			ui->button_start->setIcon(QIcon(":/icons/stop.png"));
			timer_capture.start();
			timer_updateProgress.start();
			break;
		case false :
			ui->button_start->setIcon(QIcon(":/icons/record.png"));
			timer_capture.stop();
			timer_updateProgress.stop();
			break;
	}
}
