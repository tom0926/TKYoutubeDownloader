#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QProcess>
#include "ui_mainwindow.h"
#include "cxurllist.h"
#include <QTimeLine>

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0 );
	~MainWindow();
protected:
	void closeEvent(QCloseEvent *event) ;
public slots:
	void onSelectUrlFolder() ;
	void onSelectTargetFolder() ;
	void onOpenUrlFolder() ;
	void onOpenTargetFolder() ;
	void onStart() ;
	void onProcessDone(int exitCode, QProcess::ExitStatus status ) ;
	void OnProcessOutput() ;
	void finishDownloading() ;
	void onSingleClicked() ;
	void onEditFinished(QString str) ;
	void getDuration() ;
	void onTimeOut() ;
private:
	void initConnection() ;
	void initUI() ;
	void initVar() ;
	void download( int id ) ;
	void removeBorder( QToolButton* btn ) ;
	QString parseLink(QString str) ;
	QList<QString> m_urlList ;
	Ui::Form_MainWindow ui ;

	bool m_isDownloading ;

	int m_curStep ;
	QProcess* m_process ;
	QString m_execPath ;
	QString m_outputPath ;
	bool m_isDurationMode ;
	int m_downloadMode ;
	CxUrlList* m_lw ;
	QTimeLine* m_timeLine ;
};

#endif // MAINWINDOW_H
