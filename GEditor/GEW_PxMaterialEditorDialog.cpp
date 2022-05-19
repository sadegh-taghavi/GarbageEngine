#include "GEW_PxMaterialEditorDialog.h"
#include "ui_GEW_PxMaterialEditorDialog.h"
#include <GE_Material.h>
#include <GE_Texture.h>
#include <GE_Vec3.h>

GEW_PxMaterialEditorDialog::GEW_PxMaterialEditorDialog( QWidget *i_parent /*= 0 */ )
: QDialog( i_parent ),
m_ui( new Ui::GEW_PxMaterialEditorDialog )
{
	m_ui->setupUi( this );
}

GEW_PxMaterialEditorDialog::~GEW_PxMaterialEditorDialog()
{
	delete m_ui;
}
