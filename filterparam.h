#ifndef SEARCHPARAM_H
#define SEARCHPARAM_H

#include <QString>
#include <QDate>

class CFilterParam
{
  QDate m_Date1;
  QDate m_Date2;
  QString m_sAmount1;
  QString m_sAmount2;
  QString m_sCheque1;
  QString m_sCheque2;
  QString m_sMemo;
  int m_iAccountId;
  int m_iPayeeId;
  int m_iCategoryId;
  int m_iClassId;
  int m_iAmountType;

public:
  CFilterParam();

  QDate date1() {return m_Date1;}
  QDate date2() {return m_Date2;}
  QString amount1() {return m_sAmount1;}
  QString amount2() {return m_sAmount2;}
  QString cheque1() {return m_sCheque1;}
  QString cheque2() {return m_sCheque2;}
  QString memo() {return m_sMemo;}
  int accountid() {return m_iAccountId;}
  int payeeid() {return m_iPayeeId;}
  int categoryid() {return m_iCategoryId;}
  int classid() {return m_iClassId;}
  int amountType() {return m_iAmountType;}

  void setDate1(const QDate& Date1) {m_Date1=Date1;}
  void setDate2(const QDate& Date2) {m_Date2=Date2;}
  void setAmount1(const QString& sAmount1) {m_sAmount1=sAmount1;}
  void setAmount2(const QString& sAmount2) {m_sAmount2=sAmount2;}
  void setCheque1(const QString& sCheque1) {m_sCheque1=sCheque1;}
  void setCheque2(const QString& sCheque2) {m_sCheque2=sCheque2;}
  void setMemo(const QString& sMemo) {m_sMemo=sMemo;}
  void setAccountid(int iAccountId) {m_iAccountId=iAccountId;}
  void setPayeeid(int iPayeeId) {m_iPayeeId=iPayeeId;}
  void setCategoryid(int iCategoryId) {m_iCategoryId=iCategoryId;}
  void setClassid(int iClassId) {m_iClassId=iClassId;}
  void setAmountType(int iAmountType) {m_iAmountType=iAmountType;}
  void setParams(const QDate& Date1, const QDate& Date2, const QString& sAmount1, const QString& sAmount2, const QString& sCheque1,
                 const QString& sCheque2, const QString& sMemo, int iAccountId, int iPayeeId, int iCategoryId, int iClassId, int iAmountType);
  void clear();
};

#endif // SEARCHPARAM_H
