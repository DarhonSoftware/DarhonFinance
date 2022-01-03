#ifndef FORMLIST_H
#define FORMLIST_H

#include <QList>
#include <QString>
#include <QObject>

class QWidget;

class CFormList : public QObject, public QList<QWidget*>
{
  Q_OBJECT
public:
  explicit CFormList(QObject *pObject = 0);
  void addWidget(QWidget* pWidget);
  void removeWidget(QWidget* pWidget);
  bool containsWidget(const QString& sObjectName);
};

#endif // FORMLIST_H
