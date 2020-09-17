#include "mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <QFileInfoList>
#include <QFileInfo>
#include <QTextStream>
#include <QCloseEvent>
#include <QApplication>
#include <QUrl>
#include <QDesktopServices>
#include <QStandardPaths>
/*
#include <QWebSettings>
#include <QNetworkProxyFactory>
#include <QVideoWidget>
#include <QMediaContent>
#include <QMediaPlayer>
#include <QWebView>
*/

#define MULTIMODE 0
#define SINGLEMODE 1

MainWindow::MainWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this) ;
//	m_lw = ui.listWidget ;
	initUI() ;
	initVar() ;
	initConnection() ;


/*	
	QFile styleSheet(":/res/qss/1.qss") ;
	if (!styleSheet.open(QIODevice::ReadOnly)) {
	}
	else qApp->setStyleSheet(styleSheet.readAll());
	*/
//	ui.le_link->setText("https://www.youtube.com/watch?v=YV72GiX6lfQ") ;
	ui.tb_singlemode->hide() ;
	ui.horizontalSpacer_4->changeSize(100,20) ;
//	ui.tb_singlemode->setEnabled(false) ;
	ui.lbl_check->setPixmap(QPixmap("")) ;
} 

MainWindow::~MainWindow()
{

}


void MainWindow::initUI()
{
//	ui.tb_open_url->setEnabled(false) ;
	removeBorder(ui.tb_sel_url) ;
	removeBorder(ui.tb_st_up) ;
	removeBorder(ui.tb_st_down) ;
	removeBorder(ui.tb_en_up) ;
	removeBorder(ui.tb_en_down) ;
	removeBorder(ui.tb_singlemode) ;
}

void MainWindow::removeBorder( QToolButton* btn )
{
	btn->setPalette(Qt::transparent);
	btn->setAttribute( Qt::WA_TranslucentBackground, true );
	btn->setAttribute( Qt::WA_OpaquePaintEvent, true );
	btn->setAutoFillBackground(false);
	btn->setStyleSheet("QWidget{background-color: transparent;}");
	btn->setAttribute(Qt::WA_NoSystemBackground);
}

void MainWindow::initConnection()
{
	connect( ui.le_link, SIGNAL(textChanged(QString)), this, SLOT(onEditFinished(QString))) ;
	connect( ui.tb_sel_url, SIGNAL(clicked()), this, SLOT(onSelectUrlFolder()) ) ;
//	connect( ui.tb_sel_tar, SIGNAL(clicked()), this, SLOT(onSelectTargetFolder()) ) ;
//	connect( ui.tb_open_url, SIGNAL(clicked()), this, SLOT(onOpenUrlFolder()) ) ;
//	connect( ui.tb_open_tar, SIGNAL(clicked()), this, SLOT(onOpenTargetFolder()) ) ;
	connect( ui.tb_start, SIGNAL(clicked()), this, SLOT(onStart())) ;
	connect( m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onProcessDone(int,QProcess::ExitStatus))) ;
	connect( ui.tb_singlemode, SIGNAL(clicked()), this, SLOT(onSingleClicked())) ;
	connect (m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(OnProcessOutput()));

	connect( ui.tb_st_up, SIGNAL(clicked()), ui.te_start, SLOT(stepUp()) ) ;
	connect( ui.tb_st_down, SIGNAL(clicked()), ui.te_start, SLOT(stepDown()) ) ;
	connect( ui.tb_en_up, SIGNAL(clicked()), ui.te_end, SLOT(stepUp()) ) ;
	connect( ui.tb_en_down, SIGNAL(clicked()), ui.te_end, SLOT(stepDown()) ) ;
	connect( m_timeLine, SIGNAL(finished()), this, SLOT(onTimeOut())) ;
}

void MainWindow::initVar()
{
	m_downloadMode = -1 ;
	m_isDurationMode = false ;
	m_isDownloading = false ;
	m_process = new QProcess ;
//	m_outputPath = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first() ;
	m_outputPath = QString("%1/Downloads").arg(QDir::homePath()) ;
	QDir dir(m_outputPath) ;
	dir.mkdir("YOUTUBE LINKS-MP4FILE") ;
	m_outputPath.append("/YOUTUBE LINKS-MP4FILE") ;
//	ui.lbl_tar->setText(m_outputPath) ;
	m_timeLine = new QTimeLine(1000) ;
}

QString MainWindow::parseLink(QString str)
{
	QString parse ;
	return parse ; 
}

void MainWindow::onOpenUrlFolder()
{
//	QDesktopServices::openUrl( QUrl::fromLocalFile(ui.lbl_url->text()) );
}

void MainWindow::onOpenTargetFolder()
{
//	QDesktopServices::openUrl( QUrl::fromLocalFile(ui.lbl_tar->text()) );
}

void MainWindow::onSelectUrlFolder()
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
	if( path.length() == 0 ) return ;
//	ui.lbl_url->setText(path) ;
	m_urlList.clear() ;
	QDir dir(path) ;
	QFileInfoList fileList = dir.entryInfoList() ;
	int cnt = fileList.count() ;
//	m_lw->clear() ;
	for( int i = 0; i < cnt; i++ )
	{
		QFileInfo info = fileList.at(i) ;
		QString suf = info.suffix().toLower() ;
		if( suf != "url" ) continue ;
		QFile file(info.absoluteFilePath()) ;
		if( file.open(QFile::ReadOnly))
		{
			QTextStream in(&file) ;
			QString line;
			do {
				line = in.readLine();
				if( line.startsWith("URL=https://www.youtube.com/",Qt::CaseInsensitive))
				{
					line = line.right(line.length()-4) ;
//					m_lw->addMyItem(line) ;
					m_urlList << line ;
				}
			} while (!line.isNull());	
			file.close() ;
		}
	}
	m_downloadMode = MULTIMODE ;
//	ui.tb_open_url->setEnabled(true) ;
}

void MainWindow::onSelectTargetFolder() 
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
	if( path.length() == 0 ) return ;
//	ui.lbl_tar->setText(path) ;
	m_outputPath = path ;
}

void MainWindow::finishDownloading()
{
	ui.tb_start->setText("Start") ;
	m_isDownloading = false ;
	m_process->kill() ;
	ui.lbl_curfile->setText("Status:") ;
	m_process->terminate() ;
	QFile::remove(m_execPath) ;
	return ;
}
void MainWindow::onStart()
{
	if( m_urlList.count() == 0 )
	{
		QMessageBox::information(this, "", "No videos to download") ;
		return ;
	}
	if( m_isDownloading )
	{
		finishDownloading() ;
		return ;
	}
	
	m_execPath = QString("%1/1.exe").arg(m_outputPath) ;
	QFile::copy("1.exe", m_execPath ) ;
	ui.tb_start->setText("Stop") ;
	m_isDownloading = true ;
	m_curStep = 0 ;
	ui.progressBar_cur->setMaximum(100) ;
	ui.progressBar_tot->setMaximum(100*m_urlList.count()) ;
	ui.progressBar_tot->setValue(0) ;
	ui.progressBar_cur->setValue(0) ;
	download(0) ;
}

void MainWindow::download( int id )
{
	if( id >= m_urlList.count() ) return ;
	m_curStep = id ;
	QStringList args ;
	if( m_downloadMode == SINGLEMODE )
	{
		QFile::copy("res/ffmpeg.exe","ffmpeg.exe") ;
//		QMessageBox::information(NULL,"",QString("-ss %1 -to %2").arg(ui.te_start->time().toString("h:mm:ss")).arg(ui.te_end->time().toString("h:mm:ss"))) ;
	//	return ;
		args << "--postprocessor-args"<<QString("-ss %1 -to %2").arg(ui.te_start->time().toString("h:mm:ss")).arg(ui.te_end->time().toString("h:mm:ss"));
//		args << "--postprocessor-args"<<QString("-ss %1 -to %2").arg("0:00:00").arg("0:00:05");
		args <<"--output"<< QString("%1/%(title)s.%(ext)s").arg(m_outputPath) ;
		args << m_urlList.at(id) ;
		m_process->start("1.exe",args) ;
		return ;
	}
	else
	{
		QFile::remove("ffmpeg.exe") ;
		args << m_urlList.at(id) ;
		args << "-o" ;
		args << QString("%1/%(title)s.%(ext)s").arg(m_outputPath) ;
	}
	ui.progressBar_cur->setValue(0) ;
	m_process->start(m_execPath, args ) ;
}

void MainWindow::onProcessDone(int exitCode, QProcess::ExitStatus status )
{
	if( m_isDurationMode )
	{
		QApplication::restoreOverrideCursor() ;
		m_isDurationMode = false ;
		return ;
	}
	if( m_curStep == m_urlList.count()-1 )
	{
		finishDownloading() ;
		if (QMessageBox::question(this, "Finished", "Downloading has finished. Do you want to open the output folder?") == QMessageBox::Yes )
		{
			QDesktopServices::openUrl( QUrl::fromLocalFile(m_outputPath) );
		}
		return ;
	}
	download(m_curStep+1) ;
}


void MainWindow::closeEvent(QCloseEvent* event)
{
	if( m_isDownloading ) 
	{
		finishDownloading() ;
	}
	event->accept() ;
}

void MainWindow::OnProcessOutput()
{
	QString str = m_process->readAllStandardOutput();
	if( m_isDurationMode )
	{
		str.chop(1) ;
		ui.te_end->setTime(QTime::fromString(str,"m:ss")) ;
		ui.te_end->setMaximumTime(QTime::fromString(str,"m:ss")) ;
		ui.te_start->setMaximumTime(QTime::fromString(str,"m:ss")) ;
		ui.tb_start->setEnabled(true) ;

		return ;
	}
	if( str.contains("[download]") && str.contains("MiB at ") )
	{
		QString per ;
		for( int i = 11; i < str.length(); i++ )
		{
			if( str.at(i) == QChar('%') ) break ;
			per.append(str.at(i)) ;
		}
		int val = per.toDouble() ;
		ui.progressBar_cur->setValue(val) ;
		ui.lbl_curfile->setText("Downloading...") ;
		ui.progressBar_tot->setValue(m_curStep*100+val) ;
	}
	else ui.lbl_curfile->setText(str) ;
	repaint() ;
		
//	QMessageBox::information(NULL,"",str) ;
}

void MainWindow::onSingleClicked()
{
	if( !ui.le_link->text().contains("https://www.youtube.com/watch?v=") )
	{
		QMessageBox::information(this,"","Invalid input") ;
		return ;
	}

	m_urlList.clear(); 
//	m_lw->clear() ;
	m_urlList << ui.le_link->text() ;
	m_downloadMode = SINGLEMODE ;
	getDuration() ;
}

void MainWindow::getDuration()
{
	if( !ui.le_link->text().contains("https://www.youtube.com/watch?v=") )
	{
		return ;
	}
	if( m_isDurationMode || m_process->state() == QProcess::Running ) return ;
	QStringList args; 
	args << ui.le_link->text()<<"--get-duration" ;
	m_isDurationMode = true ;
	QApplication::setOverrideCursor(Qt::WaitCursor) ;
	m_process->start("1.exe",args) ;
	ui.te_start->setTime(QTime(0,0,0,0)) ;
}

void MainWindow::onEditFinished(QString str)
{
	ui.lbl_check->setPixmap(QPixmap("")) ;
	if( ui.le_link->text().contains("https://www.youtube.com/watch?v=") && str.length() == 43 )
	{
		m_timeLine->start() ;
		ui.tb_start->setEnabled(false) ;
		onSingleClicked() ;
//		ui.tb_singlemode->show() ;
//		ui.horizontalSpacer_4->changeSize(40,20) ;
	}
	else {
//		ui.tb_singlemode->hide() ;
//		ui.horizontalSpacer_4->changeSize(100,20) ;
	}
}

void MainWindow::onTimeOut()
{
	ui.lbl_check->setPixmap(QPixmap(":/res/icon/check.png")) ;
}