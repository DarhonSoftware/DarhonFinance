#ifndef CACCOUNTSVIEW_H
#define CACCOUNTSVIEW_H

#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QPoint>
#include "formbase.h"

class QSqlRelationalTableModel;
class QSqlTableModel;
class QWidget;
class QShowEvent;
class QHideEvent;

namespace Ui
{
	class CAccountsView;
}

class CAccountsView : public CFormBase
{
	Q_OBJECT
	Q_DISABLE_COPY(CAccountsView)
public:
	explicit CAccountsView(QWidget *pWidget = 0);
	virtual ~CAccountsView();

public slots:
	void updateViewState();
	void saveViewState();

private:
	Ui::CAccountsView *ui;
	QSqlRelationalTableModel *m_pModel;
	QSqlTableModel *m_pModelBank;
	QSqlTableModel *m_pModelType;
	QSqlTableModel *m_pModelCurrency;
	int m_iScrollValue;

private:
	void updateGeneralTotal();

protected:
	void showEvent( QShowEvent *pEvent );
	void hideEvent ( QHideEvent *pEvent );

private slots:
  void on_pTableView_activated(QModelIndex index);
  void on_pPBFilters_clicked();
  void on_pPBAccounts_clicked();

  void infoItem();
  void reportForm();
  void filterList();
  void updateColumnTotal();
  void menuColumnsChooser(QPoint Point);
	void updateView();
};


#endif // CACCOUNTSVIEW_H
