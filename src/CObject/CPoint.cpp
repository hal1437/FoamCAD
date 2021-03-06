#include "CPoint.h"

CPoint* CPoint::hanged;

CREATE_RESULT CPoint::Create(CPoint *pos){
    this->x() = pos->x();
    this->y() = pos->y();
    return CREATE_RESULT::COMPLETE;//生成終了
}

void CPoint::DrawGL(Pos camera,Pos center)const{
    if(!this->isVisible())return;
    glBegin(GL_LINE_LOOP);
    Pos cc = camera - center;
    double theta1 = std::atan2(cc.y(),std::sqrt(cc.x()*cc.x()+cc.z()*cc.z()));
    double theta2 = std::atan2(-cc.x(),cc.z());
    Quat quat = Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2));
    const double length = cc.Length()*10;
    const int POLY_COUNT = 8;
    if(!this->isControlPoint()){
        //円の描画
        for(double k=0;k < 2*M_PI;k += 2*M_PI/POLY_COUNT){
            Pos p = Pos(length*std::sin(k),length*std::cos(k),0).Dot(quat);
            glVertex3f((p + *this).x(),(p + *this).y(),(p + *this).z());
        }
    }else{
        //四角の描画
        for(double k=0;k < 2*M_PI;k += M_PI/2){
            Pos p = Pos(length * std::sqrt(2) * std::sin(k+M_PI/4),
                        length * std::sqrt(2) * std::cos(k+M_PI/4),0)
                        .Dot(quat);
            glVertex3f((p + *this).x(),(p + *this).y(),(p + *this).z());
        }
    }
    glEnd();

    //固定時
    if(this->isLock()){
        //円の描画
        glBegin(GL_LINES);
        double angle[4] = {M_PI/4.0, M_PI*5.0/4.0, M_PI*3.0/4.0, M_PI*7.0/4.0};
        for(int i=0;i<4;i++){
            Pos p = Pos(length * std::sin(angle[i]),
                        length * std::cos(angle[i]),0).Dot(quat);
            glVertex3f((p + *this).x(),(p + *this).y(),(p + *this).z());
        }
        glEnd();
    }
}

void CPoint::MoveAbsolute(const Pos& diff){
    this->MoveRelative(diff - *this);
}
void CPoint::MoveRelative(const Pos& diff){
    if(diff.Length() > SAME_POINT_EPS && isLock() == false && !isControlPoint() /*&& !this->isMoving()*/){
        //this->SetMoving(true);  //再帰呼び出し制限
        *this += diff;//移動
        emit Changed(this);
        //this->SetMoving(false); //再帰呼び出し制限解除
    }
}

CObject* CPoint::GetChild(int){
    throw "CPointに子は存在しません";
}
void CPoint::SetChild(int,CObject*){
    throw "CPointに子は存在しません";
}
int CPoint::GetChildCount()const{
    return 0;
}
QList<CPoint*> CPoint::GetAllChildren() const{
    QList<CPoint*>ans;
    ans.push_back(const_cast<CPoint*>(this));
    return ans;
}

Pos CPoint::GetNearPos(const Pos&)const{
    return *this;
}
Pos CPoint::GetNearLine(const Pos&,const Pos&)const{
    return *this;
}

CObject* CPoint::Clone()const{
    CPoint* new_obj = new CPoint(static_cast<Pos>(*this));
    new_obj->ControlPoint  = this->ControlPoint;
    new_obj->Visible       = this->Visible;
    new_obj->VisibleDetail = this->VisibleDetail;
    new_obj->Lock          = this->Lock;
    return new_obj;
}


CPoint::CPoint(QObject* parent):
    CObject(parent){
}
CPoint::CPoint(const Pos& origin){
    this->x() = origin.x();
    this->y() = origin.y();
    this->z() = origin.z();
}
CPoint::CPoint(const Pos &pos,QObject* parent):
    CObject(parent),
    Pos(pos){
}
CPoint::~CPoint()
{

}
CPoint& CPoint::operator=(const Pos& rhs){
    this->x() = rhs.x();
    this->y() = rhs.y();
    this->z() = rhs.z();
    return (*this);
}


