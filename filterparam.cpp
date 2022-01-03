#include "filterparam.h"
#include "global.h"

CFilterParam::CFilterParam()
{
  clear();
}

void CFilterParam::setParams(const QDate& Date1, const QDate& Date2, const QString& sAmount1, const QString& sAmount2, const QString& sCheque1,
                             const QString& sCheque2, const QString& sMemo, int iAccountId, int iPayeeId, int iCategoryId, int iClassId, int iAmountType)
{
  m_Date1=Date1;
  m_Date2=Date2;
  m_sAmount1=sAmount1;
  m_sAmount2=sAmount2;
  m_sCheque1=sCheque1;
  m_sCheque2=sCheque2;
  m_sMemo=sMemo;
  m_iAccountId=iAccountId;
  m_iPayeeId=iPayeeId;
  m_iCategoryId=iCategoryId;
  m_iClassId=iClassId;
  m_iAmountType=iAmountType;
}

void CFilterParam::clear()
{
  m_Date1=QDate();
  m_Date2=QDate();
  m_sAmount1.clear();
  m_sAmount2.clear();
  m_sCheque1.clear();
  m_sCheque2.clear();
  m_sMemo.clear();
  m_iAccountId=ID_ANY;
  m_iPayeeId=ID_ANY;
  m_iCategoryId=ID_ANY;
  m_iClassId=ID_ANY;
  m_iAmountType=0;
}
