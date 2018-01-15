#ifndef CFILEEDGE_H
#define CFILEEDGE_H

#include <QFile>
#include <QException>
#include <QMessageBox>
#include <CObject/CSpline.h>

class CFileEdge : public CSpline
{
    Q_OBJECT
private:
    Pos start_base; //始点初期値
public:
    static CFileEdge* CreateFromFile(QString filepath);
public:
    virtual QString DefaultClassName(){return "CFileEdge";}


    virtual void DrawGL(Pos camera,Pos center)const;

    virtual CREATE_RESULT Create(CPoint *pos);

    CFileEdge();

public slots:
    //点移動コールバックオーバーライド
    virtual void ChangeChildCallback(QList<CObject*>);

};

#endif // CFILEEDGE_H
