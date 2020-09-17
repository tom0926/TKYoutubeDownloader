#ifndef CXURLLIST_H
#define CXURLLIST_H

#include <QListWidget>
#include "ui_piecewidget.h"

class CxPieceWidget : public QWidget
{
	Q_OBJECT
public:
	CxPieceWidget() ;
	Ui::Form_PieceWidget form(){ return ui ; }
private:
	Ui::Form_PieceWidget ui ;
};

class CxUrlItem : public QListWidgetItem
{
public:
	CxUrlItem(QString url) ;
	CxPieceWidget* myWidget(){ return m_w; }
private:
	CxPieceWidget* m_w ;
	QString m_url ;
};

class CxUrlList : public QListWidget
{
	Q_OBJECT

public:
	CxUrlList(QWidget *parent);
	~CxUrlList();
	void addMyItem( QString str ) ;
protected:
	void resizeEvent(QResizeEvent* event) ;
private:
	
};

#endif // CXURLLIST_H
