#ifndef CACCOUNTEDIT_H
#define CACCOUNTEDIT_H

#include <QVariant>
#include <QModelIndex>
#include <QString>
#include "formbase.h"

class QSqlTableModel;
class QWidget;
class QShowEvent;
class QHideEvent;

namespace Ui
{
	class CAccountEdit;
}

class CAccountEdit : public CFormBase
{
	Q_OBJECT
public:
  CAccountEdit(const QVariant& Id, QWidget *pWidget = 0);
  virtual ~CAccountEdit();

private:
	Ui::CAccountEdit *ui;
	QSqlTableModel *m_pModel;
	QSqlTableModel *m_pModelBank;
	QSqlTableModel *m_pModelType;
	QSqlTableModel *m_pModelCurrency;
	int m_iScrollValue;

private:
	void cleanForm();
  void enableForm(bool bEnable);

private slots:
  void on_pPBDetail_clicked();
  void on_pPBCloseL_clicked();
  void on_pPBList_clicked();
  void on_pPBMemo_toggled(bool bChecked);
  void on_pPBApply_clicked();
	void on_pTBIcon_clicked();
	void on_pPBClose_clicked();

  void addItem();
  void deleteItem();
  void updateIcon();
  void fillForm(QModelIndex Index);
	void setDirty();

protected:
	void showEvent( QShowEvent *pEvent );
	void hideEvent ( QHideEvent *pEvent );
};

#endif // CACCOUNTEDIT_H
