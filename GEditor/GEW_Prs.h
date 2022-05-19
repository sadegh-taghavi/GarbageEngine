#pragma once

#include <QWidget>
#include <QList>
#include <stdint.h>
#include <GE_View.h>
#include "GEW_Input.h"
#include <PxPhysics.h>

namespace Ui { class GEW_Prs; }
class GEW_Item;
class GE_PerformanceTimer;

class GEW_Prs : public QWidget
{
public:
	Q_OBJECT
	Ui::GEW_Prs *m_ui;
	static GEW_Prs *m_singleton;
	GEW_Item *m_item;
	bool m_pickEnabled;
	physx::PxRigidStatic *m_pickRigidStatic;
	QList< physx::PxTriangleMesh *> m_pickTriangleMeshes;
public:
	static QIcon m_iconQuestion;

	struct Action
	{
		enum Type
		{
			Move,
			Rotate,
			Scale
		} MyType;
		float X;
		float Y;
		float Z;
		uint32_t ItemId;
		bool operator ==( Action &i_other );
	};
	QList< Action > m_history;
	int m_historyPointer;
	bool m_disableHistory;
	GE_PerformanceTimer *m_historyPt;

	explicit GEW_Prs( QWidget *i_parent = 0 );
	~GEW_Prs();
	static GEW_Prs &getSingleton() { return *m_singleton; }
	void updateUi();
	void updatePick( GEW_Input::Data &i_inputData, const GE_View &i_view, const GE_Vec2 &i_cursorPos, const GE_Vec2 &i_widthHeight );

	void push( GEW_Prs::Action &i_action );
	void clearHistory();
	void clearExtraHistory();
	bool hasHistory( uint32_t i_itemId );

public slots:
	void slotSetItem( GEW_Item *i_item );
	void slotUpdateItem();
	void slotUpdateItemOnEditFinished();
	void showBar();

private slots:
	void on_PRSx_valueChanged( double i_value );
	void on_PRSy_valueChanged( double i_value );
	void on_PRSz_valueChanged( double i_value );

	void on_PRSx_editingFinished();
	void on_PRSy_editingFinished();
	void on_PRSz_editingFinished();

	void on_PRSPosition_clicked( bool i_checked );
	void on_PRSRotation_clicked( bool i_checked );
	void on_PRSScale_clicked( bool i_checked );

	void on_PRSUndo_clicked();
	void on_PRSRedo_clicked();

	void on_FocusToTarget_clicked();
	void on_TurnToTarget_clicked( );
	void on_PRSVisible_toggled( bool i_checked );
	

	void on_Pick_toggled( bool i_checked );
};