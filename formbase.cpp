#include <QtGui>
#include <QtWidgets>
#include "formbase.h"
#include "mainwindow0.h"
#include "global.h"

CFormBase::CFormBase(QWidget *pWidget):QWidget(pWidget)
{
  setAttribute(Qt::WA_DeleteOnClose,true);

  connect(this,SIGNAL(destroyed(QObject*)),g_pMainWindow,SLOT(closeForm(QObject*)));
}


CFormBase::~CFormBase()
{
}
