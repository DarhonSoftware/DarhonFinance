#ifndef SIGNSYMBOL_H
#define SIGNSYMBOL_H

#include <QLabel>

class CSignSymbol : public QLabel
{
  Q_OBJECT

  bool m_bPositive;
  bool m_bNegativeZero;

public:
  explicit CSignSymbol(QWidget *pWidget = 0);
  void setSign(double dAmount);
  int sign();
  void setNegativeZero(bool bNegativeZero) {m_bNegativeZero=bNegativeZero;}
  bool negativeZero() {return m_bNegativeZero;}

private:
  void drawSign();

protected:
  void mouseReleaseEvent (QMouseEvent *pEvent);
  void resizeEvent (QResizeEvent *pEvent );

signals:
  void signChanged();
  
public slots:
  
};

#endif // SIGNSYMBOL_H
