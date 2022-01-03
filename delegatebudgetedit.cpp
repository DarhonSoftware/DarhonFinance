#include <QtGui>
#include <QtSql>
#include "delegatebudgetedit.h"
#include "basedelegate.h"
#include "global.h"
#include "amounteditor.h"

CDelegateBudgetEdit::CDelegateBudgetEdit(QObject *pObject):CBaseDelegate(pObject)
{
}

QWidget *CDelegateBudgetEdit::createEditor(QWidget *pWidget, const QStyleOptionViewItem & /*Option*/,const QModelIndex &Index) const
{
  if (Index.column()==2)
  {
    CAmountEditor *pAmountEditor=new CAmountEditor(pWidget);
    return pAmountEditor;
  }

  return 0;
}

void CDelegateBudgetEdit::setEditorData(QWidget *pWidgetEditor, const QModelIndex &Index) const
{
  if (Index.column()==2)
  {
    double dAmount = Index.data(Qt::EditRole).toDouble();
    CAmountEditor *pAmountEditor = qobject_cast<CAmountEditor*>(pWidgetEditor);
    pAmountEditor->setAmount(dAmount);
  }
}

void CDelegateBudgetEdit::setModelData(QWidget *pWidgetEditor, QAbstractItemModel *pModel,const QModelIndex &Index) const
{
  if (Index.column()==2)
  {
    CAmountEditor *pAmountEditor = qobject_cast<CAmountEditor*>(pWidgetEditor);
    double dAmount=pAmountEditor->amount();
    pModel->setData(Index,dAmount);
  }
}

void CDelegateBudgetEdit::updateEditorGeometry(QWidget *pWidgetEditor,const QStyleOptionViewItem &Option, const QModelIndex &/*Index*/) const
{
  pWidgetEditor->setGeometry(Option.rect);
}

