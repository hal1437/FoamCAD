#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //connect(ui->actionCtrlZ          ,SIGNAL(triggered())                        ,this       ,SLOT(CtrlZ()));
    connect(ui->actionDelete         ,SIGNAL(triggered())                        ,this       ,SLOT(Delete()));
    connect(ui->actionMove           ,SIGNAL(triggered())                        ,this       ,SLOT(MoveTransform()));
    connect(ui->actionResetExpantion ,SIGNAL(triggered())                        ,this       ,SLOT(ResetAllExpantion()));
    connect(ui->ToolDimension        ,SIGNAL(triggered())                        ,ui->CadEdit,SLOT(MakeSmartDimension()));
    connect(ui->ToolBlocks           ,SIGNAL(triggered())                        ,this       ,SLOT(MakeBlock()));
    connect(ui->BlockList            ,SIGNAL(itemDoubleClicked(QListWidgetItem*)),ui->CadEdit,SLOT(ConfigureBlock(QListWidgetItem*)));
    connect(ui->CadEdit              ,SIGNAL(ToggleConflict(bool))               ,this       ,SLOT(ToggleConflict(bool)));
    connect(ui->ExportButton         ,SIGNAL(pressed())                          ,ui->CadEdit,SLOT(Export()));

    //CadEditFoam関連
    connect(this          ,SIGNAL(ToggleChanged(CEnum)),ui->CadEdit   ,SLOT(SetState(CEnum)));
    connect(ui->CadEdit   ,SIGNAL(ScaleChanged(double)),ui->ScaleSpin ,SLOT(setValue(double)));
    connect(ui->CadEdit   ,SIGNAL(RequireRefreshUI())  ,this          ,SLOT(RefreshUI()));
    connect(ui->CadEdit   ,SIGNAL(MouseMoved(Pos))     ,this          ,SLOT(RefreshStatusBar(Pos)));
    connect(ui->actionSave,SIGNAL(triggered())         ,ui->CadEdit   ,SLOT(Save()));
    connect(ui->actionLoad,SIGNAL(triggered())         ,ui->CadEdit   ,SLOT(Load()));

    //リスト変更系
    connect(ui->RestraintList ,SIGNAL(clicked(QModelIndex)) ,this ,SLOT(MakeRestraint(QModelIndex)));
    connect(ui->ObjectList    ,SIGNAL(clicked(QModelIndex)) ,this ,SLOT(ReciveObjectListChanged(QModelIndex)));
    connect(ui->BlockList     ,SIGNAL(clicked(QModelIndex)) ,this ,SLOT(ReciveBlockListChanged (QModelIndex)));

    //CadEditFoamにイベントフィルター導入
    this->installEventFilter(ui->CadEdit);

    RefreshUI();
    ReciveObjectListChanged(QModelIndex());
    ReciveBlockListChanged(QModelIndex());
    ConnectSignals();
    ui->ToolBlocks->setEnabled(false);
    ui->ObjectList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    CObject::drawing_scale = 1.0;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent  (QKeyEvent* event){
    this->ui->CadEdit->keyPressEvent(event);

    //ESC押下時
    if(event->key() == Qt::Key_Escape){
        this->ui->CadEdit->Escape();
        ClearButton();
        RefreshUI();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event){
    this->ui->CadEdit->keyReleaseEvent(event);
}


void MainWindow::CtrlZ(){
    /*
    if(!log.empty()){
        ui->CadEdit->RemoveObject(log.back());
        log.erase(log.end()-1);
        creating_count=0;
    }*/
}
void MainWindow::Delete(){
    for(int i =0;i<CObject::selected.size();i++){
        //ui->CadEdit->RemoveObject(CObject::selected[i]);
    }
    CObject::selected.clear();
    repaint();
    RefreshUI();
}

void MainWindow::ConnectSignals(){
    connect(ui->ToolDot   ,SIGNAL(toggled(bool)),this,SLOT(ToggledDot(bool)));
    connect(ui->ToolArc   ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    connect(ui->ToolLine  ,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
    connect(ui->ToolRect  ,SIGNAL(toggled(bool)),this,SLOT(ToggledRect(bool)));
    connect(ui->ToolSpline,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
}

void MainWindow::DisconnectSignals(){
    disconnect(ui->ToolDot   ,SIGNAL(toggled(bool)),this,SLOT(ToggledDot(bool)));
    disconnect(ui->ToolArc   ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    disconnect(ui->ToolLine  ,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
    disconnect(ui->ToolRect  ,SIGNAL(toggled(bool)),this,SLOT(ToggledRect(bool)));
    disconnect(ui->ToolSpline,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
}

void MainWindow::ClearButton(){
    //if(make_obj != nullptr && make_obj->isCreating())make_obj->Create(nullptr,-1);
    if(ui->ToolDot   ->isChecked())ui->ToolDot   ->setChecked(false);
    if(ui->ToolLine  ->isChecked())ui->ToolLine  ->setChecked(false);
    if(ui->ToolArc   ->isChecked())ui->ToolArc   ->setChecked(false);
    if(ui->ToolRect  ->isChecked())ui->ToolRect  ->setChecked(false);
    if(ui->ToolSpline->isChecked())ui->ToolSpline->setChecked(false);
}
void MainWindow::RefreshUI(){

    ui->RestraintList->clear();
    //ui->BlockList->clear();

    QVector<RestraintType> able = Restraint::Restraintable(CObject::selected);
    for(RestraintType r:able){
        std::pair<std::string,std::string> p;
        if(r == MATCH     )p = std::make_pair("一致"   ,":/Restraint/MatchRestraint.png");
        if(r == EQUAL     )p = std::make_pair("等値"   ,":/Restraint/EqualRestraint.png");
        if(r == CONCURRENT)p = std::make_pair("並行"   ,":/Restraint/ConcurrentRestraint.png");
        if(r == VERTICAL  )p = std::make_pair("垂直"   ,":/Restraint/VerticalRestraint.png");
        if(r == HORIZONTAL)p = std::make_pair("水平"   ,":/Restraint/HorizontalRestraint.png");
        if(r == TANGENT   )p = std::make_pair("正接"   ,":/Restraint/TangentRestraint.png");
        if(r == LOCK      )p = std::make_pair("固定"   ,":/Restraint/LockRestraint.png");
        if(r == UNLOCK    )p = std::make_pair("固定解除",":/Restraint/UnlockRestraint.png");
        if(r == MARGE     )p = std::make_pair("マージ"  ,":/Restraint/Marge.png");
        ui->RestraintList->addItem(new QListWidgetItem(p.first.c_str()));
        ui->RestraintList->item(ui->RestraintList->count()-1)->setIcon(QIcon(p.second.c_str()));
    }
    this->ui->CadEdit->ExportObjectList(this->ui->ObjectList);
    this->ui->CadEdit->ExportCBoxList  (this->ui->BlockList);

    //拘束更新
    ui->CadEdit->RefreshRestraints();
    //ブロック生成可否判定
    ui->ToolBlocks->setEnabled(CBlock::Creatable(CObject::selected));
    //スマート寸法は1つから
    ui->ToolDimension->setEnabled(CObject::selected.size() >= 1);
    //リスト要素数で出力ボタンの無効化を決定
    ui->ExportButton->setEnabled(this->ui->BlockList->count() > 0);

    this->repaint();
}

#define ToggledToolDefinition(TYPE)             \
void MainWindow::Toggled##TYPE (bool checked){  \
    if(TYPE != Edit)DisconnectSignals();        \
    if(checked){                                \
        ClearButton();                          \
        emit ToggleChanged(TYPE);               \
        ui->Tool##TYPE->setChecked(true);       \
    }else{                                      \
        emit ToggleChanged(Edit);               \
    }                                           \
    ConnectSignals();                           \
}                                               \

ToggledToolDefinition(Dot)
ToggledToolDefinition(Line)
ToggledToolDefinition(Arc)
ToggledToolDefinition(Rect)
ToggledToolDefinition(Spline)

void MainWindow::ToggleConflict(bool conflict){
    if(conflict){
        this->ui->actionCheckConflict->setIcon(QIcon(":/Others/Conflict.png"));
    }else{
        this->ui->actionCheckConflict->setIcon(QIcon(":/Others/NotConflict.png"));
    }
}
void MainWindow::ResetAllExpantion(){
    CObject::drawing_scale = 1.0;
    this->ui->ScaleSpin->setValue(1.0);
    this->ui->CadEdit->ResetAllExpantion();
}

void MainWindow::MoveTransform(){
    static MoveTransformDialog* diag = new MoveTransformDialog(this);
    connect(diag,SIGNAL(RepaintRequest()),this,SLOT(repaint()));
    diag->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    diag->show();
}


void MainWindow::MakeRestraint(QModelIndex){
    //qDebug() << text;
    RestraintType type = Paradox;
    if(ui->RestraintList->currentItem()->text() == "一致")type = MATCH;
    if(ui->RestraintList->currentItem()->text() == "並行")type = CONCURRENT;
    if(ui->RestraintList->currentItem()->text() == "垂直")type = VERTICAL;
    if(ui->RestraintList->currentItem()->text() == "水平")type = HORIZONTAL;
    if(ui->RestraintList->currentItem()->text() == "正接")type = TANGENT;
    if(ui->RestraintList->currentItem()->text() == "固定")type = LOCK;
    if(ui->RestraintList->currentItem()->text() == "固定解除")type = UNLOCK;
    if(type != Paradox){
        ui->CadEdit->MakeRestraint(type);
    }

    if(ui->RestraintList->currentItem()->text() == "マージ"){
        ui->CadEdit->MergePoints();
    }
    ui->RestraintList->clear();
    this->RefreshUI();
}

void MainWindow::MakeBlock(){
    this->ui->CadEdit->MakeBlock();
    RefreshUI();
}


void MainWindow::ReciveObjectListChanged(QModelIndex){
    this->ui->CadEdit->ImportObjectList(this->ui->ObjectList);
    this->ui->CadEdit->repaint();
}
void MainWindow::ReciveBlockListChanged(QModelIndex ){
    this->ui->CadEdit->ImportCBoxList  (this->ui->BlockList);
    RefreshUI();
}
void MainWindow::RefreshStatusBar(Pos){
    Pos out;
    if(CObject::hanged == nullptr){
        //マウス位置のローカル座標
        out = CObject::mouse_pos;
    }else{
        //選択オブジェクトの最近点
        out = CObject::hanged->GetNear(CObject::mouse_pos);
    }
    this->ui->statusBar->showMessage(QString("(") + QString::number(out.x) + "," + QString::number(out.y) + ")");

}
