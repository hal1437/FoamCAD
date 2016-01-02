#ifndef CARC_H
#define CARC_H

#include "CObject.h"
#include "CPoint.h"

class CArc : public CObject
{
private:
    const static int DRAWING_LINE_SIZE = 3;
    const static int COLLISION_SIZE = 10;

protected:
    Relative<Pos> pos[2];
    Relative<Pos> center;
    double round=0;//半径

    virtual bool Create(Relative<Pos> pos,int index);
public:

    double GetRound()const;
    Pos    GetCenter()const;
    virtual bool Refresh();
    virtual Pos GetNear(const Pos& hand)const;
    virtual bool Draw(QPainter& painter)const;
    virtual bool Selecting();
    virtual bool isLocked();
    virtual bool Move(const Pos& diff);
    virtual int GetJointNum()const;
    virtual Pos GetJointPos(int index)const;


    CArc();
    ~CArc();
};


#endif // CARC_H
