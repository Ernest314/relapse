#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>

#include <cmath>

#include <QString>
#include <QTimer>

#include <QPixmap>

#include <QShowEvent>
#include <QHideEvent>
#include <QCloseEvent>

#include <QGuiApplication>

#include <QFile>

#include <QScreen>

#include <QDesktopServices>
#include <QSystemTrayIcon>

#include <QFileDialog>

#include <QMainWindow>



// TODO: Compile Qt without webkit module, to exclude ICU libraries.
// TODO: Move all "preferences" to its own class.

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
	void screenshot();

	void update_interval_count();
	void update_interval_interval();
	void update_interval();
	void update_progress();

	void on_radioButton_captureFreq_count_toggled(bool checked);
	void on_radioButton_captureFreq_interval_toggled(bool checked);

    void on_lineEdit_filename_prefix_textChanged(const QString &text_changed);
    void on_lineEdit_filename_suffix_textChanged(const QString &text_changed);
    void on_lineEdit_folder_textChanged(const QString &text_changed);
    void on_button_folder_browse_clicked();

    void on_button_minimize_clicked();
	void on_button_start_clicked();
    void on_button_play_clicked();
    void on_button_capture_clicked();

    void on_button_folder_open_clicked();

private:
	Ui::MainWindow *ui;

    // TODO: Refactor `isRecording` and `isPlaying` into a "record_state" enum.
	bool isRecording;
	bool isPlaying;
    double interval_capture; // seconds
	QTimer* timer_capture;
	QTimer* timer_updateProgress;
	QSystemTrayIcon* trayIcon;

    // Variables concerned with filename info.
    int file_num; // TODO: Make this configurable via a spinbox?
    QString file_prefix;
    QString file_suffix;
    QString file_name;
    QString dir_save;

    bool event(QEvent* event);
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);
    void closeEvent(QCloseEvent* event);

	void setEnabled_captureFreq_count(bool isEnabled=true);
	void setDisabled_captureFreq_count() { setEnabled_captureFreq_count(false); }
	void setEnabled_captureFreq_interval(bool isEnabled=true);
	void setDisabled_captureFreq_interval() { setEnabled_captureFreq_interval(false); }
};



#endif // MAINWINDOW_H
