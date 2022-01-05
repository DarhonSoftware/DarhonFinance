#include <QtGui>
#include <QtWidgets>
#include "signsymbol.h"

CSignSymbol::CSignSymbol(QWidget *pWidget) :  QLabel(pWidget)
{
  m_bNegativeZero=true;
}

void CSignSymbol::setSign(double dAmount)
{
  if (dAmount>0)
  {
    m_bPositive=true;
  }
  else if(dAmount==0.0)
  {
    m_bPositive=!m_bNegativeZero;
  }
  else
  {
    m_bPositive=false;
  }

  drawSign();
}

int CSignSymbol::sign()
{
  if (m_bPositive) return 1;
  return -1;
}

void CSignSymbol::drawSign()
{
  //Read measurements of drawing area
  const int iHeightPix = this->height();

  //Initializes Painter
  QPixmap Pixmap(iHeightPix,iHeightPix);
  Pixmap.fill(QColor("transparent"));
  QPainter Painter(&Pixmap);

  //Set Painter
  Painter.setWindow(0,0,100,100);
  Painter.setViewport(0,0,iHeightPix,iHeightPix);
  Painter.setRenderHint(QPainter::Antialiasing,true);

  //Set colors
  QColor ColorPositive=QColor("blue");
  QColor ColorNegative=QColor("red");

  //Set Pen
  QPen Pen(Qt::NoBrush,20,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
  int iRound=20;

  //Draw POSITIVE
  if (m_bPositive)
  {
    Pen.setColor(ColorPositive);
    Painter.setPen(Pen);
    Painter.drawLine(iRound,50,100-iRound,50);
    Painter.drawLine(50,iRound,50,100-iRound);
  }
  //Draw NEGATIVE
  else
  {
    Pen.setColor(ColorNegative);
    Painter.setPen(Pen);
    Painter.drawLine(iRound,50,100-iRound,50);
  }

  //Show border
  QPen PenBorder(palette().text(),0,Qt::SolidLine);
  Painter.setBrush(Qt::NoBrush);
  Painter.setPen(PenBorder);
  Painter.drawRoundedRect(0,0,100,100,50,50,Qt::RelativeSize);

  setPixmap(Pixmap);
}

void CSignSymbol::mouseReleaseEvent (QMouseEvent *pEvent)
{
  m_bPositive=!m_bPositive;
  drawSign();
  emit signChanged();

  QLabel::mouseReleaseEvent(pEvent);
}

void CSignSymbol::resizeEvent (QResizeEvent *pEvent)
{
  drawSign();
  QLabel::resizeEvent(pEvent);
}
