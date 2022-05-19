#pragma once
#include <QString>
#include <iostream>

namespace GEW_Utility
{
	std::string absolutePath( char* i_name, QString i_path );
	std::string copyFile( QString i_file, QString i_targetPath );
};

