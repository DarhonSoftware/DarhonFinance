#ifndef DR_SQL_H
#define DR_SQL_H

class QSqlTableModel;
class QVariant;
class QString;

/***** Global constats *****/


/***** Global enums *****/


/***** Global functions *****/

int g_rowCount(const QSqlTableModel& Model);
bool g_findRecordFromTable(const QSqlTableModel& Model, const QString& sSearchField, const QString& sSearchValue, const QString& sResultField=QString(), QVariant *pResultValue=0);
bool g_findRecordFromTable(const QString& sModel, const QString& sSearchField, const QString& sSearchValue, const QString& sResultField=QString(), QVariant *pResultValue=0, const QString &sFilter=QString());
void g_fetchAllData(QSqlTableModel *pModel);

#endif // DR_SQL_H
