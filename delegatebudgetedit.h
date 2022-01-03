#ifndef DELEGATEBUDGETEDIT_H
#define DELEGATEBUDGETEDIT_H

#include <QStyleOptionViewItem>
#include <QModelIndex>
#include "basedelegate.h"

class QWidget;

class CDelegateBudgetEdit : public CBaseDelegate
{
  Q_OBJECT

public:
  CDelegateBudgetEdit(QObject *pObject);
  QWidget *createEditor(QWidget *pWidget, const QStyleOptionViewItem &Option,const QModelIndex &Index) const;
  void setEditorData(QWidget *pWidgetEditor, const QModelIndex &Index) const;
  void setModelData(QWidget *pWidgetEditor, QAbstractItemModel *pModel,const QModelIndex &Index) const;
  void updateEditorGeometry(QWidget *pWidgetEditor,const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
};

#endif // DELEGATEBUDGETEDIT_H
