#pragma once
#include <QDialog>

namespace Ui { class EntityBindDialog; }

class GEW_EntityBindDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GEW_EntityBindDialog( QWidget *i_parent = 0 );
	~GEW_EntityBindDialog();
	void init( const QStringList &i_parentList, const QStringList &i_binsPosList, const QStringList &i_childList );
private:
	Ui::EntityBindDialog *m_ui;
};