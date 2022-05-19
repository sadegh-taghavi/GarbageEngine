#include "GEW_SelectItemDialog.h"
#include "ui_GEW_SelectItemDialog.h"
#include <QListWidgetItem>

GEW_SelectItemDialog::GEW_SelectItemDialog( QWidget *i_parent /*= 0 */ ) : QDialog( i_parent ), m_ui( new Ui::SelectItemDialog )
{
	m_ui->setupUi( this );
}

GEW_SelectItemDialog::~GEW_SelectItemDialog()
{
	delete m_ui;
}

void GEW_SelectItemDialog::addItem( QIcon &i_icon, QString &i_text, unsigned int i_userValue )
{
	QListWidgetItem *item = new QListWidgetItem( i_icon, i_text, m_ui->ItemsList );
	item->setData( Qt::UserRole, i_userValue );
}

QListWidgetItem * GEW_SelectItemDialog::getCurrentItem()
{
	return m_ui->ItemsList->currentItem();
}
