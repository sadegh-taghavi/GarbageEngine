#include "GEW_EntityBindDialog.h"
#include "ui_GEW_EntityBindDialog.h"

GEW_EntityBindDialog::GEW_EntityBindDialog( QWidget *i_parent /*= 0 */ ) : QDialog( i_parent ), m_ui( new Ui::EntityBindDialog )
{
	m_ui->setupUi( this );
}

GEW_EntityBindDialog::~GEW_EntityBindDialog()
{
	delete m_ui;
}

void GEW_EntityBindDialog::init( const QStringList &i_parentList, const QStringList &i_binsPosList, const QStringList &i_childList )
{
	m_ui->Parent->addItems( i_parentList );
	m_ui->Child->addItems( i_childList );
	int index = 0;
	foreach( QString str, i_binsPosList )
		m_ui->BoundPos->addItem( tr( "%1 - " ).arg( index++ ) + str );
}
