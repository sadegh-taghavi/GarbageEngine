#pragma once
#include <QDialog>

namespace Ui { class SelectItemDialog; }

class GEW_SelectItemDialog : public QDialog
{
	Q_OBJECT
public:
	Ui::SelectItemDialog *m_ui;
	explicit GEW_SelectItemDialog( QWidget *i_parent = 0 );
	~GEW_SelectItemDialog();
	void addItem( QIcon &i_icon, QString &i_text, unsigned int i_userValue );
	class QListWidgetItem *getCurrentItem();
};