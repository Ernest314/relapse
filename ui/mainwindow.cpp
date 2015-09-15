#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	isRecording(false),
	isPlaying(false),
	interval_capture(15.00),
	timer_capture(new QTimer(this)),
	timer_updateProgress(new QTimer(this)),
	trayIcon(new QSystemTrayIcon(QIcon(":/icons/monitor.png"), this)),
    file_num(1),
    file_prefix(),
    file_suffix(),
    file_name(),
    dir_save()
{
	ui->setupUi(this);

	QObject::connect(timer_capture,	&QTimer::timeout,
                     this,			&MainWindow::screenshot);

	QObject::connect(trayIcon,	&QSystemTrayIcon::activated,
					 this,		&MainWindow::show);

	QObject::connect(this, &MainWindow::updated_interval,
					 this, &MainWindow::update_interval);
	QObject::connect(timer_updateProgress,	&QTimer::timeout,
					 this,					&MainWindow::update_progress);

	// TODO: Delete this connection in Qt 5.5.1 where the remainingTime bug is fixed
	// (Currently reminingTime stays at 0 after the first timeout(), even when
	// isSingleShot == false.)
    // TODO: Eventually move to using Qt Animation Framework for smoother (faked) animation.
	QObject::connect(timer_capture,		&QTimer::timeout,
					 timer_capture,		static_cast<void (QTimer::*)()>(&QTimer::start));

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

	trayIcon->show();

	timer_capture->setInterval(static_cast<int>(round(interval_capture * 1000)));
	timer_capture->setSingleShot(false);
	timer_updateProgress->setInterval(500);
	timer_updateProgress->setSingleShot(false);

	ui->spinBox_captureFreq_interval_sec->setValue(15.00);
	update_interval();

	ui->radioButton_captureFreq_count->setChecked(false);
	setDisabled_captureFreq_count();
	ui->radioButton_captureFreq_interval->setChecked(true);
	setEnabled_captureFreq_interval();
}

MainWindow::~MainWindow()
{
	delete timer_capture;
	delete timer_updateProgress;
	delete trayIcon;
	delete ui;
}

void MainWindow::screenshot()
{
	bool window_wasHidden = this->isHidden();
    bool hide_on_capture = ui->checkBox_settings_hideOnCapture->isChecked();
    if (hide_on_capture && !window_wasHidden) {
		this->setWindowOpacity(0);
	}

    // NOTE: .png is the only allowed format for design simplicity.
	QScreen* screen = QGuiApplication::primaryScreen();
    QPixmap screenshot = screen->grabWindow(0);
    QString file_num_str = QString::number(file_num).rightJustified(4, '0');
        // ^ 4 is what blender uses for its rendering.
    file_name = dir_save + "/" + file_prefix + file_num_str + file_suffix + ".png";
    QFile file_out(file_name); // TODO: Check if file already exists.
    file_out.open(QIODevice::WriteOnly);
    screenshot.save(&file_out, "PNG");
    int thumbnail_width = ui->label_thumbnail->width();
    int thumbnail_height = ui->label_thumbnail->height();
    QPixmap screenshot_scaled = screenshot.scaled(thumbnail_width,
                                                  thumbnail_height,
                                                  Qt::KeepAspectRatio);
    ui->label_thumbnail->setPixmap(screenshot_scaled);
    file_num++;

	if (!window_wasHidden) {
		this->setWindowOpacity(1);
	}
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
    // TODO: Make this interval update proportional to current elapsed time.
    // Also make sure it goes and changes that. (What is the desired behavior?)
	timer_capture->setInterval(static_cast<int>(round(interval_capture)) * 1000);
	update_progress();
}
void MainWindow::update_progress()
{
	double total_sec_accurate =
			static_cast<double>(timer_capture->remainingTime()) / 1000.0;
	int total_sec = static_cast<int>(round(total_sec_accurate));
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

	double time_passed = interval_capture - total_sec_accurate;
	double fraction_passed = time_passed / interval_capture;
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

void MainWindow::on_lineEdit_filename_prefix_textChanged(const QString &text_changed)
{
    file_prefix = text_changed;
}
void MainWindow::on_lineEdit_filename_suffix_textChanged(const QString &text_changed)
{
    file_suffix = text_changed;
}

void MainWindow::on_button_folder_browse_clicked()
{
	dir_save = QFileDialog::getExistingDirectory(this,
												 "Save in folder...",
												 dir_save);
	ui->lineEdit_folder->setText(dir_save);
}

void MainWindow::on_button_minimize_clicked()
{
    this->hide();
}
void MainWindow::on_button_start_clicked()
{
	isRecording = !isRecording;
    switch (isRecording) {
		case true :
			ui->button_start->setIcon(QIcon(":/icons/stop.png"));
            ui->button_play->setEnabled(true);
            ui->button_play->setIcon(QIcon(":/icons/pause.png"));
            isPlaying = true;
			timer_capture->start();
			timer_updateProgress->start();
            file_num = 1;
			break;
		case false :
			ui->button_start->setIcon(QIcon(":/icons/record.png"));
            ui->button_play->setEnabled(false);
            ui->button_play->setIcon(QIcon(":/icons/play.png"));
            isPlaying = false;
			timer_capture->stop();
			timer_updateProgress->stop();
			break;
	}
}
void MainWindow::on_button_play_clicked()
{
    // TODO: Extend QTimer to provide a play/pause interface.
    // Maybe use QElapsedTimer instead?
    isPlaying = !isPlaying;
    switch (isPlaying) {
        case true :
            ui->button_play->setIcon(QIcon(":/icons/pause.png"));
            timer_capture->start();
            timer_updateProgress->start();
            break;
        case false :
            ui->button_play->setIcon(QIcon(":/icons/play.png"));
            timer_capture->stop();
            timer_updateProgress->stop();
            break;
    }
}
void MainWindow::on_button_capture_clicked()
{
    screenshot();
}

void MainWindow::on_button_folder_open_clicked()
{
    QString native_path = "file:///" + QDir::toNativeSeparators(dir_save);
    QDesktopServices::openUrl(QUrl(native_path));
}
