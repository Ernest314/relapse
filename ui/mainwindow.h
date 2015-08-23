#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cmath>

#include <QString>
#include <QTimer>

#include <QFileDialog>

#include <QMainWindow>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

signals:
	void updated_interval();

private slots:
	void update_interval_count();
	void update_interval_interval();
	void update_interval();

	void on_radioButton_captureFreq_count_toggled(bool checked);
	void on_radioButton_captureFreq_interval_toggled(bool checked);

	void on_pushButton_folder_clicked();

private:
	Ui::MainWindow *ui;

	QString dir_save;
	QTimer timer_capture;
	double interval_capture; // seconds

	void setEnabled_captureFreq_count(bool isEnabled=true);
	void setDisabled_captureFreq_count() { setEnabled_captureFreq_count(false); }
	void setEnabled_captureFreq_interval(bool isEnabled=true);
	void setDisabled_captureFreq_interval() { setEnabled_captureFreq_interval(false); }
};



#endif // MAINWINDOW_H
