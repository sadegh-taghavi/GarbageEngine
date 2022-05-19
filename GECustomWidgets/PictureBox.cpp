#include "PictureBox.h"
#include <atlstr.h>
#include <DirectXTex.h>
#include <QtGui/QImage>
#include <QtGui/QMouseEvent>
#include <QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QPixmap>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QFileDialog>
#include <QMenu>
#include <QtGui/QContextMenuEvent>
#include <QAction>
#include <QString>
#include <QDialog>
#include <QGridLayout>
#include <ScrollArea.h>

bool PictureBox::sm_initialized = false;

PictureBox::PictureBox( QWidget *parent ) :
QLabel( parent ),
m_showRed( true ),
m_showGreen( true ),
m_showBlue( true ),
m_filter( "Texture files(*.png *.jpg *.bmp *.dds *.tga)" )
{
	m_sizeLimit = 16384;
	if ( !sm_initialized )
	{
		CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
		sm_initialized = true;
	}
	QAction *action;
	m_contextMenu = new QMenu( this );
	action = m_contextMenu->addAction( tr( "Show Red" ) );
	action->setCheckable( true );
	action->setChecked( true );
	action = m_contextMenu->addAction( tr( "Show Green" ) );
	action->setCheckable( true );
	action->setChecked( true );
	action = m_contextMenu->addAction( tr( "Show Blue" ) );
	action->setCheckable( true );
	action->setChecked( true );
	m_contextMenu->addSeparator();
	m_contextMenu->addAction( tr( "Load" ) );
	m_contextMenu->addAction( tr( "Reload" ) );
	m_contextMenu->addAction( tr( "Clear" ) );
	m_contextMenu->addSeparator();
	m_contextMenu->addAction( tr( "View On Dialog" ) );
	connect( m_contextMenu, SIGNAL( triggered( QAction* ) ), this, SLOT( actionTriggered( QAction* ) ) );
}

bool PictureBox::getShowRed()
{
	return m_showRed;
}

void PictureBox::setShowRed( bool i_show )
{
	m_showRed = i_show;
	m_contextMenu->blockSignals( true );
	m_contextMenu->actions()[ 0 ]->setChecked( i_show );
	m_contextMenu->blockSignals( false );
	bool block = signalsBlocked();
	blockSignals( true );
	slotReload();
	blockSignals( block );
}

bool PictureBox::getShowGreen()
{
	return m_showGreen;
}

void PictureBox::setShowGreen( bool i_show )
{
	m_showGreen = i_show;
	m_contextMenu->blockSignals( true );
	m_contextMenu->actions()[ 1 ]->setChecked( i_show );
	m_contextMenu->blockSignals( false );
	bool block = signalsBlocked();
	blockSignals( true );
	slotReload();
	blockSignals( block );
}

bool PictureBox::getShowBlue()
{
	return m_showBlue;
}

void PictureBox::setShowBlue( bool i_show )
{
	m_showBlue = i_show;
	m_contextMenu->blockSignals( true );
	m_contextMenu->actions()[ 2 ]->setChecked( i_show );
	m_contextMenu->blockSignals( false );
	bool block = signalsBlocked();
	blockSignals( true );
	slotReload();
	blockSignals( block );
}

bool PictureBox::hasImage()
{
	return !m_filePath.isEmpty();
}

QString PictureBox::getFilePath()
{
	return m_filePath;
}

QString PictureBox::getFilter()
{
	return m_filter;
}

void PictureBox::setFilter( const QString &i_filter )
{
	m_filter = i_filter;
}

bool PictureBox::loadFromFile( const QString &i_filePath )
{
	m_filePath.clear();
	clear();
	setToolTip( "" );

	QString fileName;
	QUrl url( i_filePath );
	if ( url.isLocalFile() )
		fileName = url.toLocalFile();
	else
		fileName = i_filePath;

	if ( fileName.isEmpty() )
	{
		emit signalImageChanged( tr( "" ) );
		return false;
	}

	if ( fileName.toLower() == m_filePath.toLower() )
		return true;

	QString format = fileName.split( '.' ).last().toLower();
	if( format.isEmpty() || !m_filter.toLower().contains( format ) )
	{
		emit signalImageChanged( tr( "" ) );
		return false;
	}

	DirectX::TexMetadata    metaData;
	DirectX::ScratchImage   scrachImage;
	DirectX::ScratchImage   outImage;
	HRESULT					hr;
	bool sizeLimit = false;
	CString					str( fileName.toStdWString().c_str() );
	if ( format == "dds" )
	{
		hr = DirectX::GetMetadataFromDDSFile(str, 0, metaData);
		if (SUCCEEDED(hr) && metaData.width <= m_sizeLimit && metaData.height <= m_sizeLimit)
			hr = DirectX::LoadFromDDSFile(str, 0, &metaData, scrachImage);
		else
			sizeLimit = true;
	}
	else if ( format == "tga" )
	{
		hr = DirectX::GetMetadataFromTGAFile(str, metaData);
		if (SUCCEEDED(hr) && metaData.width <= m_sizeLimit && metaData.height <= m_sizeLimit)
			hr = DirectX::LoadFromTGAFile( str, &metaData, scrachImage );
		else
			sizeLimit = true;
	}
	else
	{
		hr = DirectX::GetMetadataFromWICFile(str, 0, metaData);
		if (SUCCEEDED(hr) && metaData.width <= m_sizeLimit && metaData.height <= m_sizeLimit)
			hr = DirectX::LoadFromWICFile( str, 0, &metaData, scrachImage );
		else
			sizeLimit = true;
	}

	if ( FAILED( hr ) )
	{
		emit signalImageChanged( tr( "" ) );
		return false;
	}

	m_filePath = fileName;
	
	QString tooltipStr = m_filePath +
		tr( "\nSize: %1 x %2" ).arg( metaData.width ).arg( metaData.height ) +
		( metaData.mipLevels > 1 ? tr( "\nMips : %3" ).arg( metaData.mipLevels ) : tr( "" ) ) +
		( metaData.IsCubemap() ? tr( "\nCube Map" ) : tr( "" ) ) +
		( metaData.IsVolumemap() ? tr( "\nVolume Map" ) : tr( "" ) ) +
		( metaData.arraySize > 1 ? tr( "\nArray Size : %4" ).arg( metaData.arraySize ) : tr( "" ) );
	setToolTip( tooltipStr );

	if (!sizeLimit)
	{
		const DirectX::Image *image;
		hr = DirectX::Decompress(scrachImage.GetImages()[0], DXGI_FORMAT_R8G8B8A8_UNORM, outImage);
		if (SUCCEEDED(hr))
			image = outImage.GetImages();
		else
		{
			hr = DirectX::Convert(scrachImage.GetImages()[0], DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, outImage);
			if (SUCCEEDED(hr))
				image = outImage.GetImages();
			else
				image = scrachImage.GetImages();
		}
			
		QImage img((uchar*)image->pixels, image->width, image->height, image->rowPitch, QImage::Format_RGBA8888);
		if (!m_showRed || !m_showGreen || !m_showBlue)
		{
			for (int h = 0; h < img.height(); h++)
			{
				for (int w = 0; w < img.width(); w++)
				{
					QRgb rgba = img.pixel(w, h);
					if (!m_showRed)
						rgba &= 0xff00ffff;
					if (!m_showGreen)
						rgba &= 0xffff00ff;
					if (!m_showBlue)
						rgba &= 0xffffff00;
					img.setPixel(w, h, rgba);
				}
			}
		}
		setPixmap(QPixmap::fromImage(img));
	}
	else
	{
		setPixmap(QPixmap());
	}
	
	emit signalImageChanged( m_filePath );
	return true;
}

void PictureBox::slotReload()
{
	QString filePath = m_filePath;
	m_filePath = "";
	loadFromFile( filePath );
}

void PictureBox::slotLoad( QString i_filePath )
{
	loadFromFile( i_filePath );
}

void PictureBox::slotClear()
{
	loadFromFile( "" );
}

void PictureBox::actionTriggered( QAction* i_action )
{
	if ( i_action->text() == tr( "Show Red" ) )
	{
		m_showRed = i_action->isChecked();
		bool block = signalsBlocked();
		blockSignals( true );
		slotReload();
		blockSignals( block );
	}
	else if ( i_action->text() == tr( "Show Green" ) )
	{
		m_showGreen = i_action->isChecked();
		bool block = signalsBlocked();
		blockSignals( true );
		slotReload();
		blockSignals( block );
	}
	else if ( i_action->text() == tr( "Show Blue" ) )
	{
		m_showBlue = i_action->isChecked();
		bool block = signalsBlocked();
		blockSignals( true );
		slotReload();
		blockSignals( block );
	}
	else if ( i_action->text() == tr( "Load" ) )
	{
		QString fileName = QFileDialog::getOpenFileName( this, QString(), m_filePath.left( m_filePath.length() - m_filePath.split( "/" ).last().length() ), m_filter );
		if ( fileName.isEmpty() )
			return;
		loadFromFile( fileName );
	}
	else if ( i_action->text() == tr( "Reload" ) )
	{
		slotReload();
	}
	else if ( i_action->text() == tr( "Clear" ) )
	{
		slotClear();
	}
	else if ( i_action->text() == tr( "View On Dialog" ) )
	{
		if ( !m_filePath.isEmpty() )
		{
			QDialog dialog( this );
			QGridLayout *gridLayout;
			ScrollArea *scrollArea;
			QWidget *scrollAreaWidgetContents;
			QGridLayout *gridLayout_2;
			PictureBox *pictureBox;

			dialog.setWindowIcon( QIcon() );
			dialog.setWindowTitle( m_filePath );
			dialog.resize( 404, 384 );
			gridLayout = new QGridLayout( &dialog );
			gridLayout->setObjectName( QStringLiteral( "gridLayout" ) );
			gridLayout->setContentsMargins( 0, 0, 0, 0 );
			scrollArea = new ScrollArea( &dialog );
			scrollArea->setObjectName( QStringLiteral( "scrollArea" ) );
			scrollArea->setWidgetResizable( true );
			scrollAreaWidgetContents = new QWidget();
			scrollAreaWidgetContents->setObjectName( QStringLiteral( "scrollAreaWidgetContents" ) );
			scrollAreaWidgetContents->setGeometry( QRect( 0, 0, 402, 382 ) );
			gridLayout_2 = new QGridLayout( scrollAreaWidgetContents );
			gridLayout_2->setObjectName( QStringLiteral( "gridLayout_2" ) );
			pictureBox = new PictureBox( scrollAreaWidgetContents );
			pictureBox->setObjectName( QStringLiteral( "pictureBox" ) );
			QSizePolicy sizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
			sizePolicy.setHorizontalStretch( 0 );
			sizePolicy.setVerticalStretch( 0 );
			sizePolicy.setHeightForWidth( pictureBox->sizePolicy().hasHeightForWidth() );
			pictureBox->setSizePolicy( sizePolicy );
			pictureBox->setAcceptDrops( true );
			pictureBox->setFrameShape( QFrame::Box );
			pictureBox->setScaledContents( false );

			gridLayout_2->addWidget( pictureBox, 0, 0, 1, 1 );
			scrollArea->setWidget( scrollAreaWidgetContents );
			gridLayout->addWidget( scrollArea, 0, 0, 1, 1 );

			pictureBox->loadFromFile( m_filePath );

			dialog.exec();
		}

	}
}

void PictureBox::mouseDoubleClickEvent( QMouseEvent *i_ev )
{
	i_ev->accept();
	QString fileName = QFileDialog::getOpenFileName( this, QString(), m_filePath.left( m_filePath.length() - m_filePath.split( "/" ).last().length() ), m_filter );
	if ( fileName.isEmpty() )
		return;
	loadFromFile( fileName );
	//emit signalDoubleClicked( i_ev );
}

void PictureBox::mouseReleaseEvent( QMouseEvent *i_ev )
{
	if ( i_ev->buttons() & Qt::LeftButton && QRect( QPoint( 0, 0 ), size() ).contains( i_ev->pos() ) )
	{
		i_ev->accept();
		emit signalClicked( i_ev );
	}
	else
		QLabel::mouseReleaseEvent( i_ev );
}

void PictureBox::mouseMoveEvent( QMouseEvent *i_ev )
{
	if ( i_ev->buttons() & Qt::LeftButton && !QRect( QPoint( 0, 0 ), size() ).contains( i_ev->pos() ) )
	{
		i_ev->accept();
		QMimeData *data = new QMimeData;
		data->setText( m_filePath );
		QDrag* drag = new QDrag( this );
		drag->setMimeData( data );
		if ( pixmap() )
			drag->setPixmap( pixmap()->scaled( 64, 64 ) );
		drag->exec();
	}
	else
		QLabel::mouseMoveEvent( i_ev );
}

void PictureBox::dragEnterEvent( QDragEnterEvent *i_ev )
{
	if ( i_ev->mimeData()->hasText() )
	{
		i_ev->acceptProposedAction();
		i_ev->accept();
	}
	else
		QLabel::dragEnterEvent( i_ev );
}

void PictureBox::dropEvent( QDropEvent *i_ev )
{
	if ( i_ev->mimeData()->hasText() )
	{
		if ( loadFromFile( i_ev->mimeData()->text() ) )
		{
			i_ev->accept();
			return;
		}
	}
	QLabel::dropEvent( i_ev );
}

void PictureBox::contextMenuEvent( QContextMenuEvent *i_ev )
{
	i_ev->accept();
	m_contextMenu->exec( mapToGlobal( i_ev->pos() ) );
}

unsigned int PictureBox::getSizeLimit()
{
	return m_sizeLimit;
}

void PictureBox::setSizeLimit(unsigned int i_sizeLimit)
{
	m_sizeLimit = i_sizeLimit;
}
