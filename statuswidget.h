#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QLabel>

class CStatusWidget : public QLabel
{
  Q_OBJECT

  int m_iState;

public:
  enum FrameStyle {FrameYes,FrameNo};
  CStatusWidget(QWidget *pWidget);
  ~CStatusWidget();
  void setState(int iState);
  int state();
  void setFrameStyle(FrameStyle iFrame);
  virtual QSize	sizeHint () const;

protected:
  virtual void mousePressEvent (QMouseEvent *pEvent);

};

#endif // STATUSWIDGET_H
