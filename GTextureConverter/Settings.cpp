#include "Settings.h"
#include <GE_Core.h>
#include <QSettings>
#include <QFileDialog>
#include <QApplication>
#include <QString>
#include "MemoryLeakDetect.h"
#include <DirectXTex.h>
#include <GE_Texture.h>
#include <GE_StreamMap.h>
#include <GE_PostProcess.h>
#include <GE_RenderTarget.h>
#include <QWindow>

Settings::Settings(QWidget *parent)
: QMainWindow(parent)
{
	ui.setupUi(this);
	m_filter = ".DDS";
	QSettings set("GE", "GTextureConverter");
	ui.Type->setCurrentIndex(set.value("type", 0).toInt());
	ui.Format->setCurrentIndex(set.value("format", 5).toInt());
	ui.Filter->setCurrentIndex(set.value("filter", 2).toInt());
	ui.Wrap->setCurrentIndex(set.value("wrap", 0).toInt());
	ui.GenerateMips->setChecked(set.value("mips", true).toBool());
	on_Type_currentIndexChanged(ui.Type->currentIndex());
	GE_Core::getSingleton().createRenderStates(15);
	m_swapChain.ViewPort.Width = m_swapChain.ViewPort.Height = 64;
	m_swapChain.Hwnd = (HWND)ui.centralwidget->winId();
	GE_Core::getSingleton().createSwapChain(m_swapChain);
	GE_PostProcess::getSingleton().init();
}

Settings::~Settings()
{

}

bool Settings::setValues(int i_argc, char *i_argv[])
{
	m_argc = i_argc;
	m_argv = i_argv;

	m_keyDown = (GetAsyncKeyState(VK_LSHIFT) & 0x8001);
	m_argc--;
	if (!m_argc)
		return false;

	makePath(m_argv[1]);

	return true;
}

void Settings::on_OK_clicked()
{
	hide();

	if (ui.Type->currentIndex() < 5)
	{
		string shellAssemble;
		shellAssemble = "..\\..\\texassemble -" + ui.Type->currentText().toLower().toStdString() + " -f R8G8B8A8_UNORM -o " + m_filePath + m_fileName + " ";

		string shellConv = "..\\..\\texconv ";
		if (!ui.GenerateMips->isChecked())
			shellConv += "-m 1 ";
		shellConv += "-f " + ui.Format->currentText().toStdString() + " -srgb -o " + m_filePath + " ";
		if (ui.Wrap->currentIndex())
			shellConv += "-" + ui.Wrap->currentText().toLower().toStdString() + " ";

		if (ui.Type->currentIndex())
		{
			for (int32_t i = 0; i < m_argc; i++)
			{
				shellAssemble += m_argv[i + 1];
				shellAssemble += " ";
			}
			system(shellAssemble.c_str());

			shellConv += m_filePath + m_fileName;

		}
		else
			shellConv += m_argv[1];

		system(shellConv.c_str());
	}
	else
	{
		cout << "Mega texture bulding started.\nLoading texture " << m_argv[1] << "...";
		GE_Texture *map = GE_TextureManager::getSingleton().createTexture(m_argv[1]);
		cout << "done.\nGenerating levels...";
		GE_RenderTarget::Parameters par;
		par.Width[0] = map->getWidth();
		par.Height[0] = map->getHeight();
		par.MipLevel[0] = (uint32_t)(log(par.Width[0]) / log(2));
		par.HasDepth = false;
		par.Count = 1;
		GE_RenderTarget rt(par);

		DirectX::ScratchImage img;
		GE_StreamMap streamMap;

		GE_PostProcess::getSingleton().begin();

		rt.setToDevice();
		rt.clearRenderTarget();
		GE_PostProcess::getSingleton().renderConvert(map->getTextureSRV());
		GE_Core::getSingleton().getImmediateContext()->GenerateMips(rt.getSRV(0));
		cout << "done.\nWriting to file(s)...";
		DirectX::CaptureTexture(GE_Core::getSingleton().getDevice(), GE_Core::getSingleton().getImmediateContext(), rt.getRTTexture(0), img);
		GE_TextureManager::getSingleton().remove(&map);

		GE_StreamMap::Parameters streamParams;
		streamParams.FixBytes = 128;
		streamParams.Level = (uint32_t)img.GetImageCount();
		streamParams.MapDepth = 4;
		streamParams.NumberOfChunk = m_argc;
		streamParams.Size = (uint32_t)par.Width[0];
		streamParams.UserData = 0;
		streamParams.Mips = true;
		vector<uint32_t > descriptor;
		descriptor.resize( streamParams.Level );
		for (uint32_t i = 0; i < streamParams.Level; ++i)
			descriptor[i] = 1;
		streamMap.create((m_filePath + m_fileName).c_str(), streamParams, descriptor);

		for (uint32_t level = 0; level < streamMap.m_params.Level; ++level)
		{
			char * mem = streamMap.m_levels[level].Memory[0];
			DirectX::Image *mip = (DirectX::Image *)img.GetImage(level, 0, 0);
			DirectX::TexMetadata md;
			DirectX::Blob blob;
			md.arraySize = 0;
			md.depth = 1;
			md.dimension = TEX_DIMENSION_TEXTURE2D;
			md.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			md.height = md.width = mip->width;
			md.mipLevels = streamMap.m_params.Level - level;
			DirectX::SaveToDDSMemory(&mip[level], md.mipLevels - level, md, 0, blob);
			memcpy_s(mem, blob.GetBufferSize(), blob.GetBufferPointer(), blob.GetBufferSize());
		}
		streamMap.write(-1, 0);
		cout << "done.\n";

		for (int32_t i = 1; i < m_argc; i++)
		{
			cout << "Loading texture " << m_argv[i + 1] << "...";
			map = GE_TextureManager::getSingleton().createTexture(m_argv[i + 1]);
			cout << "done.\nGenerating levels...";
			rt.clearRenderTarget();
			GE_PostProcess::getSingleton().renderConvert(map->getTextureSRV());
			GE_Core::getSingleton().getImmediateContext()->GenerateMips(rt.getSRV(0));
			cout << "done.\nWriting to file(s)...";
			DirectX::CaptureTexture(GE_Core::getSingleton().getDevice(), GE_Core::getSingleton().getImmediateContext(), rt.getRTTexture(0), img);
			GE_TextureManager::getSingleton().remove(&map);

			for (uint32_t level = 0; level < streamMap.m_params.Level; ++level)
			{
				char * mem = streamMap.m_levels[level].Memory[0];
				DirectX::Image *mip = (DirectX::Image *)img.GetImage(level, 0, 0);
				DirectX::TexMetadata md;
				DirectX::Blob blob;
				md.arraySize = 0;
				md.depth = 1;
				md.dimension = TEX_DIMENSION_TEXTURE2D;
				md.format = DXGI_FORMAT_R8G8B8A8_UNORM;
				md.height = md.width = mip->width;
				md.mipLevels = streamMap.m_params.Level - level;
				DirectX::SaveToDDSMemory(&mip[level], md.mipLevels - level, md, 0, blob);
				memcpy_s(mem, blob.GetBufferSize(), blob.GetBufferPointer(), blob.GetBufferSize());
			}
			streamMap.write(-1, i);
			cout << "done.\n";
		}

		GE_PostProcess::getSingleton().end();
		cout << "successfully done.\n";

	}

	QSettings set("GE", "GTextureConverter");
	set.setValue("type", ui.Type->currentIndex());
	set.setValue("format", ui.Format->currentIndex());
	set.setValue("filter", ui.Filter->currentIndex());
	set.setValue("wrap", ui.Wrap->currentIndex());
	set.setValue("mips", ui.GenerateMips->isChecked());

	if ((GetAsyncKeyState(VK_LSHIFT) & 0x8001) || m_keyDown)
		system("pause");
	PostQuitMessage(0);
}

void Settings::on_FileSelect_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Output", m_filePath.c_str(), "File( *" + tr("%1").arg(m_filter.c_str()) + " )");
	if (fileName.isEmpty())
		return;
	if (fileName.right(4).toUpper() != m_filter.c_str())
		fileName += m_filter.c_str();
	makePath(fileName.toStdString().c_str());
}

void Settings::makePath(const char *i_file)
{
	QString str = i_file;
	str.replace("/", "\\");
	m_filePath = str.toStdString();
	m_filePath = m_filePath.substr(0, m_filePath.rfind('\\')) + "\\";
	m_fileName = str.toStdString();
	m_fileName = m_fileName.substr(m_filePath.length(), m_fileName.length());
	m_fileName = m_fileName.substr(0, m_fileName.rfind('.'));
	m_fileName += m_filter.c_str();

	ui.FileName->setText((m_filePath + m_fileName).c_str());
}

void Settings::on_Type_currentIndexChanged(int i_index)
{
	if (i_index >= 5)
		m_filter = ".GES";
	else
		m_filter = ".DDS";
	ui.groupBox->setEnabled(ui.Type->currentIndex());
	QString str = ui.FileName->text();
	ui.FileName->setText(str.left(str.length() - 4) + m_filter.c_str());

}
