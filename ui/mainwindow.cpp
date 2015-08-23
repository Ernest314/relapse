#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->radioButton_captureFreq_count->setChecked(false);
	setDisabled_captureFreq_count();
	ui->radioButton_captureFreq_interval->setChecked(true);
	setEnabled_captureFreq_interval();
}

MainWindow::~MainWindow()
{
	delete ui;
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
