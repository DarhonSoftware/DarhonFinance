#ifndef CCURRENCYEDIT_H
#define CCURRENCYEDIT_H

#include <QModelIndex>
#include "formbase.h"

class QSqlTableModel;
class QWidget;
class QShowEvent;
class QHideEvent;

namespace Ui
{
	class CCurrencyEdit;
}

class CCurrencyEdit : public CFormBase
{
	Q_OBJECT
	Q_DISABLE_COPY(CCurrencyEdit)

public:
	explicit CCurrencyEdit(QWidget *pWidget = 0);
	virtual ~CCurrencyEdit();

private:
	Ui::CCurrencyEdit *ui;
	QSqlTableModel *m_pModel;
	int m_iScrollValue;

private slots:
  void on_pPBDetail_clicked();
  void on_pPBCloseL_clicked();
  void on_pPBList_clicked();
  void on_pPBApply_clicked();
	void on_pPBClose_clicked();

  void addItem();
  void deleteItem();
  void fillForm(QModelIndex Index);
	void setDirty();

private:
	void cleanForm();
  void enableForm(bool bEnable);

protected:
	void showEvent( QShowEvent *pEvent );
	void hideEvent ( QHideEvent *pEvent );
};


#endif // CCURRENCYEDIT_H
