#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private slots:
	void on_radioButton_captureFreq_count_toggled(bool checked);
	void on_radioButton_captureFreq_interval_toggled(bool checked);

private:
	Ui::MainWindow *ui;

	void setEnabled_captureFreq_count(bool isEnabled=true);
	void setDisabled_captureFreq_count() { setEnabled_captureFreq_count(false); }
	void setEnabled_captureFreq_interval(bool isEnabled=true);
	void setDisabled_captureFreq_interval() { setEnabled_captureFreq_interval(false); }
};



#endif // MAINWINDOW_H
