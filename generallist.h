#ifndef CGENERALLIST_H
#define CGENERALLIST_H

#include <QModelIndex>
#include <QString>
#include "formbase.h"

class QSqlTableModel;
class QListView;
class QLineEdit;
class QWidget;
class QShowEvent;
class QHideEvent;

namespace Ui
{
	class CGeneralList;
}

class CGeneralList : public CFormBase
{
	Q_OBJECT
	Q_DISABLE_COPY(CGeneralList)

public:
	explicit CGeneralList(QWidget *pWidget = 0);
	virtual ~CGeneralList();

private:
	Ui::CGeneralList *ui;
	QSqlTableModel *m_pModelBank;
	QSqlTableModel *m_pModelPayee;
	QSqlTableModel *m_pModelCategory;
	QSqlTableModel *m_pModelClass;
	QSqlTableModel *m_pModelType;
	int m_iScrollValue;

private:
	void cleanForm();
  void enableForm(bool bEnable);
  void deleteItem1(const QString &sQuerySearch1, const QString &sQuerySearch2, const QString &sQuerySearch3,
                   const QString &sQueryDel,QListView *pListView,	QSqlTableModel *pModel);
  void addItem1(const QString &sTable, QListView *pListView, QSqlTableModel *pModel, QLineEdit *pLineEdit);

private slots:
  void on_pPBMemo_toggled(bool bChecked);
  void on_pTabWidget_currentChanged(int iIndex);
	void on_pPBApply_clicked();
	void on_pTBIconCategory_clicked();
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


#endif // CGENERALLIST_H
