#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Settings.h"
#include <string>
#include <GE_Core.h>

class QListWidgetItem;

class Settings : public QMainWindow
{
	Q_OBJECT
	int m_argc;
	char **m_argv;
	std::string m_filePath;
	std::string m_fileName;
	std::string m_filter;
	bool	    m_keyDown;
	GE_SwapChain m_swapChain;
	void makePath( const char *i_file );
public:
	Settings( QWidget *parent = 0 );
	~Settings( );
	bool setValues( int i_argc, char *i_argv[] );
private slots:
	void on_OK_clicked( );
	void on_FileSelect_clicked( );
	void on_Type_currentIndexChanged( int i_index );
private:
	Ui::MainWindow ui;
};
