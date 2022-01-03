#include <QtGui>
#include <QtWidgets>
#include "statuswidget.h"
#include "global.h"

#define ICON_UNCHECKED   ":/status/icons/status-unchecked.png"
#define ICON_PARTIALLY   ":/status/icons/status-partially.png"
#define ICON_CHECKED     ":/status/icons/status-checked.png"

CStatusWidget::CStatusWidget(QWidget *pWidget) : QLabel(pWidget)
{
  setAlignment(Qt::AlignCenter);
  setFrameStyle(FrameYes);
}

CStatusWidget::~CStatusWidget()
{
}

void CStatusWidget::setFrameStyle(FrameStyle iFrame)
{
  if (iFrame==FrameYes)
  {
    setFrameShadow(QFrame::Raised);
    setFrameShape(QFrame::StyledPanel);
  }
  if (iFrame==FrameNo)
  {
    setFrameShadow(QFrame::Plain);
    setFrameShape(QFrame::NoFrame);
  }
}


void CStatusWidget::setState(int iState)
{
  m_iState=iState;
  if (iState==0) setPixmap(QPixmap(ICON_UNCHECKED));
  if (iState==1) setPixmap(QPixmap(ICON_PARTIALLY));
  if (iState==2) setPixmap(QPixmap(ICON_CHECKED));
}

int CStatusWidget::state()
{
  return m_iState;
}

void CStatusWidget::mousePressEvent (QMouseEvent *pEvent)
{
  if (pEvent->button()==Qt::LeftButton)
  {
    int iVal=m_iState;
    if (++iVal==3) iVal=0;
    m_iState=iVal;
    setState(m_iState);
    pEvent->accept();
  }
  else
    pEvent->ignore();
}

QSize	CStatusWidget::sizeHint () const
{
  return QSize(ICON_WIDTH,ICON_WIDTH);
}
