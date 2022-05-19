#include "GEW_Utility.h"
#include <QFile>
#include <QStringList>

std::string GEW_Utility::absolutePath( char* i_name, QString i_path )
{
	if( i_name[ 0 ] == 0 )
		return std::string();
	i_path += i_name;
	return i_path.toStdString();
}

std::string GEW_Utility::copyFile( QString i_file, QString i_targetPath )
{
	QString fileName = i_file.split( "/" ).last();
	if( i_file.isEmpty() )
		return fileName.toStdString();
	QString temp = i_targetPath + fileName + "T";
	QFile::copy( i_file, temp );
	QFile::remove( i_targetPath + fileName );
	QFile::copy( temp, i_targetPath + fileName );
	QFile::remove( temp );
	return fileName.toStdString();
}
