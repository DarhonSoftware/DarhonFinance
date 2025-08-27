#ifndef GLOBAL_H
#define GLOBAL_H

#include <QDate>
#include <QPointer>
#include <QString>
#include <QVariant>
#include "darhon_tools/dr_sql.h"
#include "filterparam.h"

class QComboBox;
class QStandardItemModel;
class QAbstractItemView;
class CMainWindow;

/***** Global constats *****/

#define ICON_WIDTH      32
#define RATE_PREC       10
#define MAX_AMOUNT      1000000000
#define ID_MAX          (qint32)0x7FFFFFF0	//+2147483632
#define ID_ANY          (qint32)0x8000000F	//-2147483633
#define ID_SPLIT        (qint32)0x8000000E	//-2147483634 To be used in category.idx
#define ID_TRANSFER     (qint32)0x8000000D	//-2147483635 To be used in payee.idx
#define SPLIT           (qint32)0x40000000	//+1073741824 To be used in trans.split
#define DB_VER          200
#define TRANSFER_STR    " ====>"
#define TRANSFER_QTD    "' ====>'"
#define SPLIT_STR       " --///--"
#define SPLIT_QTD       "' --///--'"
#define CRYPT_MARK      "fa117c68b78bf056"

/***** Global enums *****/

enum Status {StatusAll=0,StatusReconciled,StatusUnreconciled,StatusCleared,StatusUncleared};
enum Action {ActionAdd=0,ActionEdit};
enum Recurrent {RecurrentWeekly=1,RecurrentFortnightly,RecurrentMonthly};
enum Resolution {ResSmall=0,ResNormal,ResLarge,ResXLarge};

/***** Global types *****/


/***** Global functions *****/

int g_generateId(const QString &sTable);
void g_transfer(const QVariant& Id1);
void g_refreshComboCategory(QComboBox *pCB, const QVariant& Id1, const QVariant& IdAcc1, bool bAny);
void g_filterText(QComboBox *pCB,const QString &sField,QString &sFilter,bool bModel);
void g_refreshCombobox(QComboBox *pCB, const QVariant& Id1, bool bAny);
bool g_isActiveDatabase();
QString g_dateFormat();
void g_createSplitTrans(const QVariant& Id1,QStandardItemModel *pModelSplit);
void g_initComboboxStatus(QComboBox *pCB,Status iStatus);
void g_initComboboxAccStatus(QComboBox *pCB,int iAccStatus);
void g_initComboboxAmounts(QComboBox *pCB,int iAmountType);
void g_filterStatus(QComboBox *pCB,QString &sFilter);
double g_transferRate(int iAcc1, int iAcc2, bool *pbDiff=0);
void g_refreshView(QAbstractItemView *pView, bool bFilter, const QString& sFilter=QString());
void g_messageWarning(QWidget * pWidget, const QString& sText1, const QString& sText2);
int g_messageYesNo( QWidget * pWidget, const QString& sText1, const QString& sText2);
void g_updateFontSize(QWidget * pWidget);
void g_waitingStatus(bool bWait);
Resolution g_resolution();
QDate g_endRecurrentDate(const QDate& Date1,Recurrent iRecurrent);

/***** Global exported variables *****/

extern QPointer<CMainWindow> g_pMainWindow;
extern CFilterParam g_FilterParam;

#endif // GLOBAL_H
