#include <QtGui>
#include <QtWidgets>
#include "formlist.h"

CFormList::CFormList(QObject *pObject) :
    QObject(pObject)
{
}

bool CFormList::containsWidget(const QString& sObjectName)
{
  for (int i=0;i<count();i++)
    if (at(i)->objectName()==sObjectName) return true;
  return false;
}

void CFormList::addWidget(QWidget* pWidget)
{
  if (!isEmpty()) last()->hide();

  //Append Widget in the list
  append(pWidget);
}

void CFormList::removeWidget(QWidget* pWidget)
{
  //Remove Widget from the list
  if (!removeOne(pWidget)) return;

  if (!isEmpty()) last()->show();
}
