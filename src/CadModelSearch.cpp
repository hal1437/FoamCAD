#include "CadModelSearch.h"

//2つの線の共有点を取得
CPoint* CadModelSearch::GetUnionPoint(CEdge* e1,CEdge* e2){
    if(e1->start == e2->start)return e1->start;
    if(e1->start == e2->end  )return e1->start;
    if(e1->end   == e2->start)return e1->end;
    if(e1->end   == e2->end  )return e1->end;
    return nullptr;
}
//2つの線が連続であるか判定
bool CadModelSearch::isContinuity(CEdge* e1,CEdge* e2){
    return GetUnionPoint(e1,e2) != nullptr;
}
//線の端点のどちらかが指定した点であるか判定
bool CadModelSearch::HavePoint(CEdge* e1,CPoint* pos){
    return (e1->start == pos || e1->end == pos);
}


//面生成補完関数
QVector<CEdge*> CadModelSearch::SearchEdgeMakeFace (QVector<CEdge*> select){
    QVector<QVector<CEdge*>> ans;

    //作成対象外
    if(select.size() <= 1)return QVector<CEdge*>();
    if(select.size() >= 4){
        // CFace::Creatable関数で作成可能であれば作成
        QVector<CObject*> objects;
        //CObjectに変換
        for(CEdge* edge:select)objects.push_back(edge);
        if(CFace::Creatable(objects))return select;
        else return QVector<CEdge*>();
    }

    //選択されたオブジェクトを除外したedgesを作成
    QVector<CEdge*> edges = this->model->GetEdges();
    for(CEdge* edge:select){
        edges.removeAll(edge);
    }

    //2つのエッジからなる補間
    if(select.size() == 2){
        CPoint* union_pos = GetUnionPoint(select.first(),select.last());//共有点

        //連続補間
        if(union_pos == nullptr){
            //2線は非連続
            //組み合わせ探索
            for(int i=0;i<edges.size();i++){
                //両方の辺に連続な線の探索
                if(!isContinuity(edges[i],select.first()) ||
                   !isContinuity(edges[i],select.last ()))continue;
                for(int j=i+1;j<edges.size();j++){
                    if(isContinuity(edges[j],select.first()) &&
                       isContinuity(edges[j],select.last() ) &&
                       !isContinuity(edges[i],edges[j])){
                        //適合パターン検出
                        ans.push_back({select.first(),select.last(),edges[i],edges[j]});
                    }
                }
            }
        }
        //非連続補間
        else{
            //2線は連続
            //三角形
            for(int i=0;i<edges.size();i++){
                if(isContinuity(edges[i],select.first()) &&
                   isContinuity(edges[i],select.last())){
                    //適合パターン検出
                    ans.push_back({select.first(),select.last(),edges[i]});
                }
            }
            //四角形
            for(int i=0;i<edges.size();i++){
                //両方の辺に連続な線の探索
                if((!isContinuity(edges[i],select.first()) && !isContinuity(edges[i],select.last ())) ||
                   ( isContinuity(edges[i],select.first()) &&  isContinuity(edges[i],select.last ())))continue;
                for(int j=i+1;j<edges.size();j++){
                    if(isContinuity(edges[j],edges[i]) &&
                      ((isContinuity(edges[j],select.first ()) && isContinuity(edges[i],select.last ())) ||
                       (isContinuity(edges[i],select.first ()) && isContinuity(edges[j],select.last ())))){
                        //適合パターン検出
                        ans.push_back({select.first(),select.last(),edges[i],edges[j]});
                    }
                }
            }
        }
    }else{
        //3線
        CEdge *e1,*e3;//e1とe3は非連続
        e1 = e3 = nullptr;
        for(int i =0;i<3;i++){
            for(int j =i+1;j<3;j++){
                if(!this->isContinuity(select[i],select[j])){
                    if(e1 == nullptr){
                        e1 = select[i];
                        e3 = select[j];
                    }else{
                        //二回目以降は失敗
                        return QVector<CEdge*>();
                    }
                }
            }
        }
        if(e1 == nullptr)return QVector<CEdge*>();

        //のこり1辺を探す
        for(int i=0;i<edges.size();i++){
            if(isContinuity(edges[i],e1) &&
               isContinuity(edges[i],e3)){
                //適合パターン検出
                ans.push_back({select[0],select[1],select[2],edges[i]});
            }
        }
    }
    if(ans.size() == 1)return ans.first();
    else return QVector<CEdge*>();
}
QVector<CEdge*> CadModelSearch::SearchEdgeMakeBlock(QVector<CEdge*> select){
/*
    QVector<QVector<CEdge*>> ans;

    //無理
    if(select.size() != 3)return QVector<CEdge*>();
    if(!isContinuity(select[0],select[1]) ||
       !isContinuity(select[1],select[2]) ||
       !isContinuity(select[2],select[0]))return QVector<CEdge*>();

    //端点を3つ割り出す
    CPoint* center = GetUnionPoint(select[0],select[1]); //共通点
    CPoint* out[3]; //端点
    for(int i =0;i<3;i++){
        out[i] = ((select[i]->start == center) ? select[i]->end : select[i]->start);
    }

    //端点は全て違う点である
    if(out[0] == out[1] || out[1] == out[2] || out[2] == out[0])return QVector<CEdge*>();

    //選択されたオブジェクトを除外したedgesを作成
    QVector<CEdge*> edges = this->model->GetEdges();
    for(CEdge* edge:select){
        edges.removeAll(edge);
    }

    //残りのEdgesで候補を探す
    for(int i =0;i<edges.size();i++){
        if(std::all_of(out,out+3,[&](CPoint *p){return edges[i]->start != p && edges[i]->end != p;}))continue;
        for(int j = i+1;j<edges.size();j++){
            if(std::all_of(out,out+3,[&](CPoint *p){return edges[j]->start != p && edges[j]->end != p;}))continue;
            for(int k =j+1;k<edges.size();k++){
                if(std::all_of(out,out+3,[&](CPoint *p){return edges[k]->start != p && edges[k]->end != p;}))continue;
                //判定
                if(!isContinuity(edges[i],edges[j]) ||
                   !isContinuity(edges[j],edges[k]) ||
                   !isContinuity(edges[k],edges[i]))continue;

                //端点を3つ割り出す
                CPoint* center_p = GetUnionPoint(edges[i],edges[j]); //共通点
                CPoint* out_p[3]; //端点
                for(int p =0;p<3;p++){
                    out_p[p] = ((edges[i]->start == center_p) ? edges[0]->end : edges[0]->start);
                }

                //端点は全て違う点である
                if(out_p[0] == out_p[1] || out_p[1] == out_p[2] || out_p[2] == out_p[0])return QVector<CEdge*>();

                //比較
                if(!exist(out,out_p[0]) && !exist(out,out_p[2]) && !exist(out,out_p[2])){
                    ans.push_back({
                        select[0],select[1],select[2],
                         edges[i], edges[j], edges[k]
                    });
                }
            }
        }
    }


*/
    return QVector<CEdge*>();
    /*
    if(ans.size() > 1)return QVector<CEdge*>();
    else
    return ans.first();*/
}

QVector<CFace*> CadModelSearch::SearchFaceMakeBlock(QVector<CFace*> select){
    QVector<QVector<CFace*>> ans;

    //作成対象外
    if(select.size() <= 1)return QVector<CFace*>();
    if(select.size() == 6){
        // CBlock::Creatable関数で作成可能であれば作成
        QVector<CObject*> objects;
        //CObjectに変換
        for(CFace* face:select)objects.push_back(face);
        if(CBlock::Creatable(objects))return select;
        else return QVector<CFace*>();
    }

    //対面のみ
    if(select.size() == 2){
        if(std::any_of(select[0]->edges.begin(),select[0]->edges.end(),[&](CEdge* edge){
            return exist(select[1]->edges,edge);
        })){
            return QVector<CFace*>();
        }
        for(int j=0;j<4;j++){//シフト
            QVector<CEdge*> connection;//接続数
            for(int k=0;k<4;k++){//ループ
                CPoint* p1 = select[0]->GetPointSequence(k);
                CPoint* p2 = select[1]->GetPointSequence((j + k)%4);

                //2つの点を持つ点を探す
                QVector<CEdge*>::iterator it = std::find_if(this->model->GetEdges().begin(),this->model->GetEdges().end(),[&](CEdge* edge){
                    return HavePoint(edge,p1) && HavePoint(edge,p2);
                });
                if(it != this->model->GetEdges().end()){
                    connection.push_back(*it);
                }
            }
            if(connection.size() == 4){
                //構築
                QVector<CFace*>faces;
                faces.push_back(select[0]);
                faces.push_back(select[1]);
                //面を作成する
                for(int k = 0;k<4;k++){
                    CFace* face = new CFace();
                    QVector<CEdge*> edges;
                    QVector<CEdge*>::iterator it1 = std::find_if(select[0]->edges.begin(),
                                                                 select[0]->edges.end(),
                                                                 [&](CEdge* edge){
                        return HavePoint(edge,select[0]->GetPointSequence(k)) &&
                               HavePoint(edge,select[0]->GetPointSequence((k+1)%4));
                    });
                    QVector<CEdge*>::iterator it2 = std::find_if(select[1]->edges.begin(),
                                                                 select[1]->edges.end(),
                                                                 [&](CEdge* edge){
                        return HavePoint(edge,select[1]->GetPointSequence((j+k)%4)) &&
                               HavePoint(edge,select[1]->GetPointSequence((j+k+1)%4));
                    });
                    if(it1 != select[0]->edges.end())edges.push_back(*it1);
                    if(it2 != select[1]->edges.end())edges.push_back(*it2);
                    edges.push_back(connection[k]);
                    edges.push_back(connection[(k+1)%4]);
                    face->Create(edges);
                    faces.push_back(face);
                }
                ans.push_back(faces);
            }
        }
    }
    if(ans.size() == 1){
        //モデルに等価のものが存在していればすり替える
        for(CFace*& face:ans.first()){
            //モデルに存在していなければ
            if(!exist(this->model->GetFaces(),face)){
                //モデルにすり替えられるものがないか確認
                for(CFace* face_m:this->model->GetFaces()){
                    //face_m.edgeがface.edgeと等しければ
                    if(std::all_of(face_m->edges.begin(),face_m->edges.end(),[&](CEdge* edge){
                        return exist(face->edges,edge);
                    })){
                        //すり替え
                        face = face_m;
                    }
                }
            }
        }
        return ans.first();
    }
    else return QVector<CFace*>();
}

CadModelSearch::CadModelSearch()
{

}
