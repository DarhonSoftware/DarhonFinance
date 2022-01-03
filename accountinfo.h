#ifndef CACCOUNTINFO_H
#define CACCOUNTINFO_H

#include <QVariant>
#include "formbase.h"

class QSqlTableModel;
class QWidget;
class QShowEvent;

namespace Ui
{
	class CAccountInfo;
}

class CAccountInfo : public CFormBase
{
	Q_OBJECT
	Q_DISABLE_COPY(CAccountInfo)
public:
	explicit CAccountInfo(const QVariant& Id,QWidget *pWidget = 0);
	virtual ~CAccountInfo();

private:
	Ui::CAccountInfo *ui;
	QSqlTableModel *m_pModel;
	QSqlTableModel *m_pModelBank;
	QSqlTableModel *m_pModelType;
	QSqlTableModel *m_pModelCurrency;

protected:
	 void showEvent( QShowEvent *pEvent );

private slots:
  void on_pPBMemo_toggled(bool pChecked);
  void on_pPBClose_clicked();
};

#endif // CACCOUNTINFO_H
