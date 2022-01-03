#ifndef DELEGATETRANSVIEW_H
#define DELEGATETRANSVIEW_H

#include <QStyleOptionViewItem>
#include <QModelIndex>
#include "basedelegate.h"

class QWidget;
class QAbstractItemModel;
class CTransView;

class CDelegateTransView : public CBaseDelegate
{
  Q_OBJECT

  CTransView *m_pTransView;

public:
  CDelegateTransView(QObject *pObject);
  QWidget *createEditor(QWidget *pWidgetParent, const QStyleOptionViewItem &Option,const QModelIndex &Index) const;
  void setEditorData(QWidget *pWidgetEditor, const QModelIndex &Index) const;
  void setModelData(QWidget *pWidgetEditor, QAbstractItemModel *pModel,const QModelIndex &Index) const;
  void updateEditorGeometry(QWidget *pWidgetEditor,const QStyleOptionViewItem &Option, const QModelIndex &Index) const;
};

#endif // DELEGATETRANSVIEW_H
