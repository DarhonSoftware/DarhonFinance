#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "delegatetransview.h"
#include "transview.h"
#include "global.h"
#include "statuswidget.h"

CDelegateTransView::CDelegateTransView(QObject *pObject):CBaseDelegate(pObject)
{
  m_pTransView=qobject_cast<CTransView*>(pObject);
}

QWidget *CDelegateTransView::createEditor(QWidget * /*pWidgetParent*/, const QStyleOptionViewItem & /*Option*/,const QModelIndex &Index) const
{
  if (Index.column()==2)
  {
    if (Index.model()->index(Index.row(),14).data(Qt::DisplayRole).toInt()>SPLIT) return 0;
    CStatusWidget *pStatusWidget=new CStatusWidget(m_pTransView);
    return pStatusWidget;
  }
  return 0;
}

void CDelegateTransView::setEditorData(QWidget *pWidgetEditor, const QModelIndex &Index) const
{
  if (Index.column()==2)
  {
    int iState = Index.data(Qt::EditRole).toInt();
    CStatusWidget *pStatusWidget = qobject_cast<CStatusWidget*>(pWidgetEditor);
    pStatusWidget->setState(iState);
  }
}

void CDelegateTransView::setModelData(QWidget *pWidgetEditor, QAbstractItemModel *pModel,const QModelIndex &Index) const
{
  if (Index.column()==2)
  {
    QSqlTableModel *pModelTable=qobject_cast<QSqlTableModel*>(pModel);
    CStatusWidget *pStatusWidget = qobject_cast<CStatusWidget*>(pWidgetEditor);
    int iValue = pStatusWidget->state();

    QSqlQuery Query;
    Query.setForwardOnly(true);
    QString sIdx=pModelTable->record(Index.row()).value(0).toString();
    Query.exec("UPDATE trans SET status="+QString::number(iValue)+" WHERE idx="+sIdx);

    if (pModelTable->record(Index.row()).value(7).toInt()==ID_SPLIT)
    {
      int iSplit=pModelTable->record(Index.row()).value(14).toInt();
      Query.exec("UPDATE trans SET status="+QString::number(iValue)+" WHERE split="+QString::number(SPLIT+iSplit));
    }
  }
}

void CDelegateTransView::updateEditorGeometry(QWidget *pWidgetEditor,const QStyleOptionViewItem &Option, const QModelIndex &/*Index*/) const
{
  QPoint Point1=Option.rect.topLeft();
  QPoint Point2=Option.rect.bottomRight();
  Point1=m_pTransView->tableView()->viewport()->mapTo(m_pTransView,Point1);
  Point2=m_pTransView->tableView()->viewport()->mapTo(m_pTransView,Point2);
  QRect Rect(Point1,Point2);
  pWidgetEditor->setGeometry(Rect);
}

