#include "PxMaterialView.h"
#include "ui_PxMaterialView.h"
#include <QMenu>
#include <QToolBar>

PxMaterialView::PxMaterialView( QWidget *i_parent )
: QWidget( i_parent ),
m_ui( new Ui::PxMaterialView )
{
	m_ui->setupUi( this );
	QToolBar *toolBar = new QToolBar( this );
	toolBar->setIconSize( QSize( 16, 16 ) );
	toolBar->addAction( m_ui->Add );
	toolBar->addAction( m_ui->Remove );
	toolBar->addSeparator();
	toolBar->addAction( m_ui->Edit );
	toolBar->addAction( m_ui->Rename );
	toolBar->addAction( m_ui->Duplicate );
	toolBar->addAction( m_ui->Copy );
	toolBar->addAction( m_ui->Paste );
	toolBar->addSeparator();
	toolBar->addAction( m_ui->Open );
	m_ui->ToolBarLayout->addWidget( toolBar );
}

PxMaterialView::~PxMaterialView()
{
	delete m_ui;
}

void PxMaterialView::on_Materials_customContextMenuRequested( QPoint )
{
	QMenu menu( this );
	menu.addAction( m_ui->Edit );
	menu.addAction( m_ui->Rename );
	menu.addSeparator();
	menu.addAction( m_ui->Add );
	menu.addAction( m_ui->Remove );
	menu.addAction( m_ui->ClearAll );
	menu.addSeparator();
	menu.addAction( m_ui->SaveThis );
	menu.addAction( m_ui->SaveAll );
	menu.addAction( m_ui->Open );
	menu.addSeparator();
	menu.addAction( m_ui->Duplicate );
	menu.addAction( m_ui->Copy );
	menu.addAction( m_ui->Paste );
	menu.exec( QCursor::pos() );
}
