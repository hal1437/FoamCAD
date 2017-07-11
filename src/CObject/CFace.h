#ifndef CFACE_H
#define CFACE_H

#include "CObject/CEdge.h"
#include "CObject/CLine.h"

//平面オブジェクト
class CFace : public CObject
{
    Q_OBJECT
public:
    QVector<CEdge*> edges;//構成線
    bool is_polygon = true;

public:
    //面が作成可能か
    static bool Creatable(QVector<CObject*> lines);

    bool isPolygon()const{return this->is_polygon;}
    void SetPolygon(bool poly){this->is_polygon = poly;}

    bool isParadox()const;              //平面に矛盾がないか確認する。
    bool isComprehension(Pos pos)const; //平面上かチェックする。
    Pos  GetNorm()const ;               //法線ベクトル取得
    CPoint* GetBasePoint()const;        //基準点取得
    CPoint* GetPoint(int index)const;   //構成点取得
    CEdge*  GetEdgeSeqence(int index) const;   //ループエッジ取得
    virtual QVector<CPoint*> GetAllNodes();//全ての構成点を取得

    virtual bool Draw(QPainter& painter)const;//描画関数
    virtual bool DrawGL(Pos camera,Pos center)const;//三次元描画関数
    virtual bool DrawNormArrowGL()const;//三次元法線ベクトル描画関数
    virtual bool Move(const Pos& diff);//移動関数



    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    CFace(QObject* parent=nullptr);
    ~CFace();
};

#endif // CFACE_H
