#ifndef CLINE_H
#define CLINE_H
#include "CEdge.h"

//CAD上の点
class CLine : public CEdge
{
public:
    //作成関数(完了時:true , 継続時:false)
    virtual bool Create(CPoint* pos,int index);
    virtual bool Draw(QPainter& painter)const;//描画関数
    virtual bool Move(const Pos& move);//移動関数
    virtual void Lock(bool lock);//ロック

    //中間点操作
    virtual int GetMiddleCount()const;
    virtual CPoint* GetMiddle(int index);

    //近接点
    virtual Pos GetNear(const Pos& hand)const;

    CLine(QObject* parent=nullptr);
    ~CLine();
};


#endif // CLINE_H
