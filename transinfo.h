#ifndef TRANSINFO_H
#define TRANSINFO_H

#include <QVariant>
#include "formbase.h"

class QSqlTableModel;
class QWidget;
class QShowEvent;
class QStandardItemModel;

namespace Ui
{
	class CTransInfo;
}

class CTransInfo : public CFormBase
{
	Q_OBJECT

public:
	CTransInfo(const QVariant& Id,QWidget *pWidget = 0);
	~CTransInfo();

private:
	Ui::CTransInfo *ui;
	QSqlTableModel *m_pModel;
  QSqlTableModel *m_pModelAccount;
	QSqlTableModel *m_pModelCategory;
	QSqlTableModel *m_pModelPayee;
	QSqlTableModel *m_pModelClass;
  QStandardItemModel *m_pModelSplit;

protected:
	 void showEvent( QShowEvent *pEvent );


private slots:
   void on_pPBMemo_toggled(bool pChecked);
   void on_pPBClose_clicked();
   void on_pPBSplit_clicked();
};

#endif // TRANSINFO_H
