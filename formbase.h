#ifndef CFORMBASE_H
#define CFORMBASE_H

#include <QtWidgets>

class CFormBase : public QWidget
{
  Q_OBJECT

public:
  CFormBase(QWidget *pWidget=0);
  virtual ~CFormBase();
};

#endif // CFORMBASE_H
