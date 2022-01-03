#ifndef REPORT_H
#define REPORT_H

#include <QModelIndex>
#include <QString>
#include <QList>
#include "formbase.h"
#include "global.h"

class QStandardItemModel;
class QSqlTableModel;
class CBaseDelegate;
class QWidget;
class QShowEvent;

namespace Ui
{
	class CReport;
}

class CReport : public CFormBase
{
	Q_OBJECT
public:
	CReport(Status iStatus, QWidget *pWidget = 0);
	~CReport();

private:
  enum Draw {DrawNone, DrawHistogram,DrawPie};

  Ui::CReport *ui;
	QStandardItemModel *m_pModel;
	QSqlTableModel *m_pModelClass;
  QSqlTableModel *m_pModelCurrency;
  CBaseDelegate* m_pDelegate;
  Draw m_iDraw;
  QStringList m_lstVisibleIDs;
  QString m_sDateFormat;

protected:
	void showEvent( QShowEvent *pEvent );

private:
	void writeDates(int iPeriod);
	void initColumn(int iCol, int iNumRows);
  void fillColumn(const QString& sField,int iCol, int iDateB, int iDateE,const QString& sFilter,const QString& sTitle, double dEndingRate);
	void fillTotalColumn(int iCol, int iNumRows);
	void fillTotalRow(int iRow, int iNumColumns);

private slots:
  void on_pPBReturn_clicked();
  void on_pTBDrawPie_clicked();
  void on_pTBDrawHistogram_clicked();
  void on_pTBReport_clicked();
  void on_pPBClose_clicked();
  void on_pTableView_activated(QModelIndex Index);
	void on_pPBEnd_clicked();
	void on_pPBBegin_clicked();
	void on_pCBPeriod_currentIndexChanged(int iIndex);
  void updateView();
  void viewControls();
};


#endif // REPORT_H
