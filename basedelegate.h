#ifndef BASEDELEGATE_H
#define BASEDELEGATE_H

#include <QStyledItemDelegate>
#include <QHash>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QString>

class QObject;
class QPainter;

class CBaseDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
  enum Format {FormatIcon,FormatNumber,FormatDate,FormatCheck,FormatCategory,FormatUnit,FormatClass,
               FormatFrequency,FormatProgress};
	CBaseDelegate(QObject *pObject=0);
	void paint(QPainter *pPainter, const QStyleOptionViewItem &Option,const QModelIndex &Index) const;
  QSize sizeHint ( const QStyleOptionViewItem &Option, const QModelIndex &Index ) const;
  void setNumberPrecision(int iNumberPrecision) {m_iNumberPrecision=iNumberPrecision;}
	void setDateFormat(const QString &sDateFormat) {m_sDateFormat=sDateFormat;}
	void updateCategoryData();
  void updateClassData();
  void addColumnFormat(int iColumn,Format iFormat) {m_hashColumnFormat.insert(iColumn,iFormat);}
	void clear();

private:
	QHash<int,Format> m_hashColumnFormat;
	QHash<int,QString> m_hashCategoryData;
  QHash<int,QString> m_hashClassData;
  int m_iNumberPrecision;
	QString m_sDateFormat;
};

#endif // BASEDELEGATE_H
