#ifndef SETTINGS_H
#define SETTINGS_H

#include "formbase.h"

class QSqlTableModel;
class QWidget;

namespace Ui
{
	class CSettings;
}

class CSettings : public CFormBase
{
	Q_OBJECT
public:
	CSettings(QWidget *pWidget = 0);
	~CSettings();

private:
	Ui::CSettings *ui;
	QSqlTableModel *m_pModel;

private slots:
		void on_pPBCancel_clicked();
    void on_pPBApply_clicked();
    void on_pPBReset_clicked();

protected:
  void showEvent( QShowEvent *pEvent );
};

#endif // SETTINGS_H
