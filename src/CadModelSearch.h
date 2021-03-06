#ifndef CADMODELSEARCH_H
#define CADMODELSEARCH_H

#include "CadModelCore.h"

//CadModelCoreに対して探索を行うクラス
class CadModelSearch : public QObject,public CadModelCoreInterface
{
    Q_OBJECT
private:
    //2つの線の共有点を取得
    static CPoint* GetUnionPoint(CEdge* e1,CEdge* e2);
    //2つの線が連続であるか判定
    static bool isContinuity(CEdge* e1,CEdge* e2);
    //線の端点のどちらかが指定した点であるか判定
    static bool HavePoint(CEdge* e1,CPoint* pos);

public:
    //オブジェクト生成補完関数
    QList<CEdge*> SearchEdgeMakeFace (QList<CEdge*> select)const;
    QList<CEdge*> SearchEdgeMakeBlock(QList<CEdge*> select)const;
    QList<CFace*> SearchFaceMakeBlock(QList<CFace*> select)const;

    //投影チェック、投影面生成
    static bool   Projectable(QList<CObject*>objects);
    static CFace* CreateProjectionFace(QList<CObject*>objects);

public:
    //モデルを必要とするためインタンスが必要
    CadModelSearch();
};

#endif // CADMODELSEARCH_H
