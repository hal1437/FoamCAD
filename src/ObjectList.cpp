#include "ObjectList.h"

QIcon ObjectList::getIcon(CObject *obj){
    QString filepath;
    if(obj->is<CBlock>())filepath = ":/ToolImages/Blocks";
    if(obj->is<CFace> ())filepath = ":/ToolImages/Face";

    if(obj->is<CEdge>()){
        if(obj->is<CLine>  ())filepath = ":/ToolImages/Line";
        if(obj->is<CArc>   ())filepath = ":/ToolImages/Arc";
        if(obj->is<CSpline>())filepath = ":/ToolImages/Spline";
    }
    if(obj->is<CPoint>())filepath = ":/ToolImages/Dot";
    if(obj->is<CStl  >())filepath = ":/ToolImages/Stl";

    //詳細表示
    if(!obj->is<CPoint>() && obj->isVisibleDetail())filepath += "Mesh";

    //不可視は薄くする
    if(obj->isVisible())filepath += ".png";
    else                filepath += "_disabled.png";

    return QIcon(filepath);
}

void ObjectList::mouseReleaseEvent(QMouseEvent* event){
    //メニュー表示
    if(event->button() == Qt::RightButton){
        menu.Show(event->globalPos());
    }
}

void ObjectList::AddStlToTree(CStl* stl,QTreeWidgetItem* parent,int){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    if(stl->name == ""){
        item->setText(0,QString("STL:") + QString::number(IndexOf(this->CadModelCoreInterface::model->GetStls(),stl)+1));
    }else{
        item->setText(0,stl->name);
    }
    item->setIcon(0,getIcon(stl));

    for(int i = 0;i<stl->edges.size();i++){
        AddEdgeToTree(stl->edges[i],item,i+1);
    }
    item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),stl));

    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::AddBlockToTree(CBlock* block,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    if(block->getName() == ""){
        item->setText(0,QString("Block:") + QString::number(IndexOf(this->CadModelCoreInterface::model->GetBlocks(),block)+1));
    }else{
        item->setText(0,block->getName());
    }
    item->setIcon(0,getIcon(block));

    for(int i = 0;i<block->faces.size();i++){
        AddFaceToTree(block->faces[i],item,i+1);
    }
    item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),block));

    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::AddFaceToTree(CFace*  face ,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();

    if(face->getName() == ""){
        item->setText(0,QString("Face:") + IndexOf(this->CadModelCoreInterface::model->GetFaces(),face)+1);
    }else{
        item->setText(0,face->getName());
    }
    item->setIcon(0,getIcon(face));

    //三平面の子は除外
    if(!exist(CFace::base,face)){
        for(int i = 0;i<face->edges.size();i++){
            AddEdgeToTree(face->edges[i],item,i+1);
        }
    }
    item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),face));
    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::AddEdgeToTree(CEdge* edge,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    QString s;
    if(edge->is<CLine>  ())s = "Line";
    if(edge->is<CArc>   ())s = "Arc";
    if(edge->is<CSpline>())s = "Spline";

    if(edge->getName() == ""){
        item->setText(0,s + ":" + QString::number(IndexOf(this->CadModelCoreInterface::model->GetEdges(),edge)+1));
    }else{
        item->setText(0,edge->getName());
    }
    item->setIcon(0,getIcon(edge));

    for(int i=0;i<edge->GetChildCount();i++){
        AddPointToTree(edge->GetPoint(i),item,i+1);
    }
    if(exist(this->CadModelCoreInterface::model->GetSelected(),edge)){
        item->setSelected(true);
    }else{
        item->setSelected(false);
    }
    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::AddPointToTree(CPoint* point,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    if(point == nullptr){
        item->setText(0,QString("NullPointer:") + QString::number(index));
    }else{
        if(point->getName() == ""){
            item->setText(0,QString("Point:") + QString::number(IndexOf(this->CadModelCoreInterface::model->GetPoints(),point)+1));
        }else{
            item->setText(0,point->getName());
        }
        item->setIcon(0,getIcon(point));
        item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),point));
    }
    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::pushSelectedBlock(CBlock* block,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(block));
    if(current->isSelected())this->CadModelCoreInterface::model->AddSelected(block);
    else                     this->CadModelCoreInterface::model->RemoveSelected(block);
    for(int i=0;i<current->childCount();i++){
        pushSelectedFace(block->faces[i],current->child(i));
    }
}
void ObjectList::pushSelectedFace (CFace*  face ,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(face));
    if(current->isSelected())this->CadModelCoreInterface::model->AddSelected(face);
    else                     this->CadModelCoreInterface::model->RemoveSelected(face);
    for(int i=0;i<current->childCount();i++){
        pushSelectedEdge(face->edges[i],current->child(i));
    }
}
void ObjectList::pushSelectedEdge (CEdge*  edge ,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(edge));
    if(current->isSelected()){
        this->CadModelCoreInterface::model->AddSelected(edge);
    }else{
        this->CadModelCoreInterface::model->RemoveSelected(edge);
    }
    for(int i=0;i<current->childCount();i++){
        pushSelectedPoint(edge->GetPoint(i),current->child(i));
    }
}
void ObjectList::pushSelectedPoint(CPoint* point,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(point));
    if(current->isSelected()){
        this->CadModelCoreInterface::model->AddSelected(point);
    }
}
void ObjectList::pullSelectedBlock(CBlock* block,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(block));
    current->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),block));
    for(int i=0;i<current->childCount();i++){
        pullSelectedFace(block->faces[i],current->child(i));
    }
}
void ObjectList::pullSelectedFace (CFace*  face ,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(face));
    current->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),face));
    for(int i=0;i<current->childCount();i++){
        pullSelectedEdge(face->edges[i],current->child(i));
    }
}
void ObjectList::pullSelectedEdge (CEdge*  edge ,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(edge));
    current->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),edge));
    for(int i=0;i<current->childCount();i++){
        pullSelectedPoint(edge->GetPoint(i),current->child(i));
    }
}
void ObjectList::pullSelectedPoint(CPoint* point,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(point));
    current->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),point));
}


void ObjectList::SetModel(CadModelCore* m){
    this->CadModelCoreInterface::model = m;
    this->menu.SetModel(m);
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateAnyObject()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected ()),this,SLOT(PullSelected()));
}

void ObjectList::UpdateObject  (){
    this->points = this->CadModelCoreInterface::model->GetPoints();
    this->edges  = this->CadModelCoreInterface::model->GetEdges();
    this->faces  = this->CadModelCoreInterface::model->GetFaces();
    this->blocks = this->CadModelCoreInterface::model->GetBlocks();
    this->stls   = this->CadModelCoreInterface::model->GetStls();

    //排他処理
    for(CEdge* edge: this->edges){
        for(int i =0;i<edge->GetChildCount();i++)this->points.removeAll(edge->GetPoint(i));
    }
    for(CFace* face: this->faces){
        for(CEdge* e : face->edges)this->edges.removeAll(e);
    }
    for(CBlock* block: this->blocks){
        for(CFace* f : block->faces)this->faces.removeAll(f);
    }

    //ツリーにセット
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    this->clear();
    for(int i=0;i<this->blocks.size();i++)AddBlockToTree(this->blocks[i],nullptr,i+1);
    for(int i=0;i<this->faces .size();i++)AddFaceToTree (this->faces [i],nullptr,i+1);
    for(int i=0;i<this->edges .size();i++)AddEdgeToTree (this->edges [i],nullptr,i+1);
    for(int i=0;i<this->points.size();i++)AddPointToTree(this->points[i],nullptr,i+1);
    for(int i=0;i<this->stls  .size();i++)AddStlToTree  (this->stls  [i],nullptr,i+1);
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//再コネクト
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

void ObjectList::PullSelected(){
    //this <- model

    int count = 0;
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    for(int i=0;i<this->blocks.size();i++,count++)pullSelectedBlock(this->blocks[i],this->topLevelItem(count));
    for(int i=0;i<this->faces .size();i++,count++)pullSelectedFace (this->faces [i],this->topLevelItem(count));
    for(int i=0;i<this->edges .size();i++,count++)pullSelectedEdge (this->edges [i],this->topLevelItem(count));
    for(int i=0;i<this->points.size();i++,count++)pullSelectedPoint(this->points[i],this->topLevelItem(count));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//再コネクト
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

void ObjectList::PushSelected(){
    //this -> model

    int count = 0;
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    this->CadModelCoreInterface::model->SelectedClear();
    for(int i=0;i<this->blocks.size();i++,count++)pushSelectedBlock(this->blocks[i],this->topLevelItem(count));
    for(int i=0;i<this->faces .size();i++,count++)pushSelectedFace (this->faces [i],this->topLevelItem(count));
    for(int i=0;i<this->edges .size();i++,count++)pushSelectedEdge (this->edges [i],this->topLevelItem(count));
    for(int i=0;i<this->points.size();i++,count++)pushSelectedPoint(this->points[i],this->topLevelItem(count));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//再コネクト
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

ObjectList::ObjectList(QWidget *parent) :
    QTreeWidget(parent)
{
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

ObjectList::~ObjectList()
{
}
