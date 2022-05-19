#ifndef PICTUREBOX_H
#define PICTUREBOX_H

#include <QLabel>
class QMenu;

class PictureBox : public QLabel
{
	Q_OBJECT
	Q_PROPERTY( bool	ShowRed		READ getShowRed		WRITE setShowRed )
	Q_PROPERTY( bool	ShowGreen	READ getShowGreen	WRITE setShowGreen )
	Q_PROPERTY( bool	ShowBlue	READ getShowBlue	WRITE setShowBlue )
	Q_PROPERTY( QString	filter		READ getFilter		WRITE setFilter )
	Q_PROPERTY( QString file		READ getFilePath	WRITE loadFromFile )

	static bool sm_initialized;
	bool		m_showRed;
	bool		m_showGreen;
	bool		m_showBlue;
	bool		m_showAlpha;
	unsigned int m_sizeLimit;
	QString     m_filePath;
	QString		m_filter;
	QMenu		*m_contextMenu;
public:
	explicit PictureBox( QWidget *parent = 0 );
	unsigned int getSizeLimit();
	void setSizeLimit(unsigned int i_sizeLimit);

	bool getShowRed();
	void setShowRed( bool i_show );
	bool getShowGreen();
	void setShowGreen( bool i_show );
	bool getShowBlue();
	void setShowBlue( bool i_show );

	void	setFilter( const QString &i_filter );
	QString getFilter();
	QString getFilePath();
	bool	hasImage();
	bool	loadFromFile( const QString &i_filePath );
    
signals:
	void signalImageChanged( QString );
	void signalClicked( QMouseEvent* );
	void signalDoubleClicked( QMouseEvent * );

public slots:
	void slotReload();
	void slotLoad( QString i_filePath );
	void slotClear();

private slots:
	void actionTriggered( QAction* );

protected:
	void mouseDoubleClickEvent( QMouseEvent *i_ev );
	void mouseReleaseEvent( QMouseEvent *i_ev );
	void mouseMoveEvent( QMouseEvent *i_ev );
	void dragEnterEvent( QDragEnterEvent *i_ev );
	void dropEvent( QDropEvent *i_ev );
	void contextMenuEvent( QContextMenuEvent *i_ev );
};

#endif // PICTUREBOX_H
