#include "cxurllist.h"
#include <QResizeEvent>

#define ITEMH 40

CxPieceWidget::CxPieceWidget()
{
	ui.setupUi(this) ;
}

//////////////////////////////////////////////////////////////////////////

CxUrlItem::CxUrlItem(QString url)
{
	m_url = url ;
	m_w = new CxPieceWidget ;
	m_w->form().lbl_url->setText(url) ;
}

//////////////////////////////////////////////////////////////////////////

CxUrlList::CxUrlList(QWidget *parent)
	: QListWidget(parent)
{
}

CxUrlList::~CxUrlList()
{

}

void CxUrlList::addMyItem( QString str )
{
	CxUrlItem* item = new CxUrlItem(str) ;
	item->setSizeHint(QSize(width()-30,ITEMH )) ;
	addItem(item) ;
	item->setFont(QFont("arial",12)) ;
	setItemWidget(item,item->myWidget( )) ;
}

void CxUrlList::resizeEvent(QResizeEvent* event)
{
	for( int i = count()-1; i >= 0; i-- )
	{
		item(i)->setSizeHint(QSize(width()-30,ITEMH)) ;
	}
}