﻿#include "spectrogramctrl.h"
#include "ui_spectrogramctrl.h"
#include <QPainter>
#include <QPaintEvent>
spectroGramCtrl::spectroGramCtrl(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::spectroGramCtrl)
{
	ui->setupUi(this);
}

spectroGramCtrl::~spectroGramCtrl()
{
	delete ui;
}
void spectroGramCtrl::append(const short * p, const size_t sz)
{
	m_spfft.appendBuf(p,sz);
	m_totalTime += sz;
	int topline = m_totalTime / m_spfft.sampleRate()/m_lineSeconds;

	if (topline != m_currTopLine)
	{
		update();
		m_currTopLine = topline;
	}

}
void spectroGramCtrl::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QRect rect = painter.window();
	if (rect.width()<16 || rect.height()<16)
		return;
	int imwidht = m_spfft.transSize();
	if (m_spfft.rawType()==SPECGRAM_CORE::RT_REAL)
		imwidht/=2;
	if (m_image.isNull())
		m_image = QImage(imwidht,rect.height(),QImage::Format_RGB32);
	else if (m_image.height() != rect.height()||m_image.width()!=imwidht)
		m_image = QImage(imwidht,rect.height(),QImage::Format_RGB32);

	int lines = rect.height();
	for (int i=0;i<lines;++i)
	{
		double tmCenter = (m_currTopLine - i) * m_lineSeconds;
		std::vector<unsigned int> rgb = m_spfft.get_line(tmCenter,m_lineSeconds);
		if (rgb.size())
		{
			uchar * u = m_image.scanLine(i);
			if (u)
			{
				memcpy_s(u,m_image.width()*4,rgb.data(),rgb.size()*4);
			}
		}
	}
	QRectF target(0,0, rect.width(), rect.height());
	QRectF source(0.0, 0.0, m_image.width(),m_image.height());
	painter.drawImage(target, m_image, source);
}
