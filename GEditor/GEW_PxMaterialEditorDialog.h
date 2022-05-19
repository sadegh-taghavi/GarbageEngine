#pragma once

#include <QDialog>

namespace Ui { class GEW_PxMaterialEditorDialog; }

class GEW_PxMaterialEditorDialog : public QDialog
{
	Q_OBJECT
public:
	GEW_PxMaterialEditorDialog( QWidget *i_parent = 0 );
	~GEW_PxMaterialEditorDialog();
	
public slots:

private:
	Ui::GEW_PxMaterialEditorDialog *m_ui;
};
