#include "MainWindow.h"
#include "ui_MainWindow.h"


void MainWindow::SetModel(CadModelCore* model){
    //モデルと結合
    this->model = model;
    connect(this->model,SIGNAL(UpdateSelected()),this,SLOT(RefreshUI()));
    this->ui->ObjectTree->SetModel(this->model);
    this->ui->SolidEdit ->SetModel(this->model);
    this->move_diag->     SetModel(this->model);
    this->prop_diag->     SetModel(this->model);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ObjectDock->resize(ui->ObjectDock->minimumSize());

    //ボタン関係
    connect(this ,SIGNAL(ToggleChanged(MAKE_OBJECT)),ui->SolidEdit ,SLOT(SetState(MAKE_OBJECT)));
    connect(ui->actionDelete     ,SIGNAL(triggered()) ,this,SLOT(Delete()));
    connect(ui->actionProperty   ,SIGNAL(triggered()) ,this,SLOT(ShowProperty()));
    connect(ui->actionMove       ,SIGNAL(triggered()) ,this,SLOT(ShowMoveTransform()));
    connect(ui->actionGridFilter ,SIGNAL(triggered()) ,this,SLOT(ShowGridFilter()));
    connect(ui->ToolBlocks       ,SIGNAL(triggered()) ,this,SLOT(MakeBlock()));
    connect(ui->ToolFace         ,SIGNAL(triggered()) ,this,SLOT(MakeFace()));
    connect(ui->actionSave       ,SIGNAL(triggered()) ,this,SLOT(Save()));
    connect(ui->actionOpen       ,SIGNAL(triggered()) ,this,SLOT(Load()));

    //リスト変更系
    connect(ui->RestraintList ,SIGNAL(itemClicked(QListWidgetItem*)) ,this ,SLOT(MakeRestraint(QListWidgetItem*)));

    //SolidEditFoamにイベントフィルター導入
    this->installEventFilter(ui->SolidEdit);
    connect(this->ui->SolidEdit,SIGNAL(MousePosChanged(Pos)),this,SLOT(RefreshStatusBar(Pos)));

    //移動ダイアログ関係
    move_diag = new MoveTransformDialog(this);
    this->installEventFilter(move_diag);
    connect(move_diag,SIGNAL(RepaintRequest()),this,SLOT(repaint()));
    connect(move_diag,SIGNAL(RepaintRequest()),ui->SolidEdit,SLOT(repaint()));

    //プロパティダイアログ関係
    prop_diag = new PropertyDefinitionDialog(this);
    connect(prop_diag,SIGNAL(RepaintRequest()),this,SLOT(repaint()));
    connect(prop_diag,SIGNAL(RepaintRequest()),ui->SolidEdit,SLOT(repaint()));

    //ドック関係
    connect(this->ui->actionShowObjectList,SIGNAL(triggered()),this,SLOT(ShowObjectList()));
    connect(this->ui->actionExport        ,SIGNAL(triggered()),this,SLOT(Export()));

    ConnectSignals();
    ui->ToolBlocks->setEnabled(false);
    CObject::drawing_scale = 1.0;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent  (QKeyEvent* event){
    this->ui->SolidEdit->keyPressEvent(event);
    if(event->key() == Qt::Key_Shift)this->shift_press = true;

    //ESC押下時
    if(event->key() == Qt::Key_Escape)ClearButton();
}

void MainWindow::keyReleaseEvent(QKeyEvent* event){
    if(event->key() == Qt::Key_Shift)this->shift_press = false;
    this->ui->SolidEdit->keyReleaseEvent(event);
}


void MainWindow::CtrlZ(){
}
void MainWindow::Delete(){
    if(this->shift_press){
        QVector<CPoint*> pp;
        for(CObject* obj : this->model->GetSelected()){
            for(CPoint* p:obj->GetAllChildren()){
                pp.push_back(p);
            }
        }
        for(CPoint* obj : pp){
            this->model->Delete(obj);
        }
    }else{
        for(CObject* obj : this->model->GetSelected()){
            this->model->Delete(obj);
        }
    }

    this->ui->ObjectTree->UpdateObject();
    this->ui->SolidEdit->repaint();
    this->model->SelectedClear();
    repaint();
    RefreshUI();
}

void MainWindow::ConnectSignals(){
    connect(ui->ToolPoint   ,SIGNAL(toggled(bool)),this,SLOT(ToggledPoint(bool)));
    connect(ui->ToolArc     ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    connect(ui->ToolLine    ,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
    connect(ui->ToolSpline  ,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
    connect(ui->ToolFileEdge,SIGNAL(triggered(bool)),this,SLOT(ToggledFileEdge(bool)));
    connect(ui->ToolSTL     ,SIGNAL(triggered(bool)),this,SLOT(ToggledSTL(bool)));
}

void MainWindow::DisconnectSignals(){
    disconnect(ui->ToolPoint   ,SIGNAL(toggled(bool)),this,SLOT(ToggledPoint(bool)));
    disconnect(ui->ToolArc     ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    disconnect(ui->ToolLine    ,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
    disconnect(ui->ToolSpline  ,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
    disconnect(ui->ToolFileEdge,SIGNAL(triggered(bool)),this,SLOT(ToggledFileEdge(bool)));
    disconnect(ui->ToolSTL     ,SIGNAL(triggered(bool)),this,SLOT(ToggledSTL(bool)));
}

void MainWindow::ClearButton(){
    if(ui->ToolPoint ->isChecked())ui->ToolPoint ->setChecked(false);
    if(ui->ToolLine  ->isChecked())ui->ToolLine  ->setChecked(false);
    if(ui->ToolArc   ->isChecked())ui->ToolArc   ->setChecked(false);
    if(ui->ToolSpline->isChecked())ui->ToolSpline->setChecked(false);
}
void MainWindow::RefreshUI(){

    ui->RestraintList->clear();

    this->creatable = Restraint::Restraintable(this->model->GetSelected());
    for(Restraint* r:this->creatable){
        ui->RestraintList->addItem(new QListWidgetItem(r->GetRestraintName()));
        ui->RestraintList->item(ui->RestraintList->count()-1)->setIcon(QIcon(r->GetIconPath()));
    }

    //ブロック生成可否判定
    ui->ToolBlocks->setEnabled(CBlock::Creatable(this->model->GetSelected()));
    //ブロック生成可否判定
    ui->ToolFace->setEnabled(CFace::Creatable(this->model->GetSelected()));
    //スマート寸法は1つから
    ui->ToolDimension->setEnabled(this->model->GetSelected().size() >= 1);
    //リスト要素数で出力ボタンの無効化を決定
    //ui->ExportButton->setEnabled(this->ui->BlockList->count() > 0);

    this->repaint();
}

#define ToggledToolDefinition(TYPE)             \
void MainWindow::Toggled##TYPE (bool checked){  \
    if(MAKE_OBJECT::TYPE != MAKE_OBJECT::Edit)DisconnectSignals();        \
    if(checked){                                \
        ClearButton();                          \
        emit ToggleChanged(MAKE_OBJECT::TYPE);  \
        ui->Tool##TYPE->setChecked(true);       \
    }else{                                      \
        emit ToggleChanged(MAKE_OBJECT::Edit);  \
    }                                           \
    ConnectSignals();                           \
}                                               \

ToggledToolDefinition(Point)
ToggledToolDefinition(Line)
ToggledToolDefinition(Arc)
//ToggledToolDefinition(Rect)
ToggledToolDefinition(Spline)

void MainWindow::ToggledFileEdge(bool){
    //ファイルパス変更ダイアログ
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    "外部ファイルを選択",
                                                    "",
                                                    "CSV File(*.csv);;All Files (*)");
    if(filepath != ""){
        CFileEdge* edge = CFileEdge::CreateFromFile(filepath);
        if(edge != nullptr){
            this->model->AddEdges(edge);
            this->model->AddPoints(edge->start);
            for(int i =0;i<edge->GetChildCount();i++){
                this->model->AddObject(edge->GetChild(i));
            }
            this->model->AddPoints(edge->end);
        }
    }
}
void MainWindow::ToggledSTL(bool){
    //ファイルパス変更ダイアログ
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    "STLファイルを選択",
                                                    "",
                                                    "STL File(*.stl);;All Files (*)");
    if(filepath != ""){
        CStl* stl = CStl::CreateFromFile(filepath);
        if(stl != nullptr){
            this->model->AddStls(stl);
            this->model->SetPause(true);
            for(int i =0;i<stl->points.size();i++){
                this->model->AddPoints(stl->points[i]);
            }/*
            for(int i =0;i<stl->edges.size();i++){
                this->model->AddEdges (stl->edges[i]);
            }*/
            this->model->SetPause(false);
        }

    }
}

void MainWindow::ToggleConflict(bool conflict){
    if(conflict)this->ui->actionCheckConflict->setIcon(QIcon(":/Others/Conflict.png"));
    else        this->ui->actionCheckConflict->setIcon(QIcon(":/Others/NotConflict.png"));
}
void MainWindow::ShowProperty(){
    prop_diag->UpdateLayout();
    prop_diag->show();
}

void MainWindow::ShowMoveTransform(){
    move_diag->show();
}
void MainWindow::ShowGridFilter(){
    static GridFilterDialog* diag = new GridFilterDialog(this);
    //connect(diag,SIGNAL(ChangeGri dStatus(double,double)),ui->CadEdit,SLOT(SetGridFilterStatus(double,double)));
    diag->setWindowTitle("GridFilter");
    diag->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    diag->show();
}

void MainWindow::MakeRestraint(QListWidgetItem*){
    //qDebug() << text;
    Restraint* rest = nullptr;
    int index = ui->RestraintList->currentRow();
    rest = this->creatable[index];
    rest->Create(this->model->GetSelected());

    /*
    if(ui->RestraintList->currentItem()->text() == "等値"){
        rest = new EqualLengthRestraint();
        CEdge* ee = dynamic_cast<CEdge*>(this->model->GetSelected().first());
        rest->Create(this->model->GetSelected());
    }*/

    if(rest != nullptr){
        this->model->AddRestraints(rest);
        rest->Calc();
        this->model->SelectedClear();
    }
    ui->RestraintList->clear();
    this->RefreshUI();

}

void MainWindow::MakeBlock(){
    CBlock* block = new CBlock(this);
    QVector<CFace*> faces;
    for(CObject* obj:this->model->GetSelected()){
        faces.push_back(dynamic_cast<CFace*>(obj));
    }
    block->Create(faces);
    this->model->AddBlocks(block);
    this->model->GetSelected().clear();//選択解除
}
void MainWindow::MakeFace(){
    CFace* face = new CFace(this);
    QVector<CEdge*> ee;
    for(QObject* obj: this->model->GetSelected()){
        ee.push_back(dynamic_cast<CEdge*>(obj));
    }
    face->Create(ee);
    this->model->AddFaces(face);
    this->model->GetSelected().clear();//選択解除
}

void MainWindow::RefreshStatusBar(Pos pos){
    this->ui->statusBar->showMessage(QString("( %1,%2,%3)").arg(QString::number(pos.x() ,'f',3))
                                                           .arg(QString::number(pos.y() ,'f',3))
                                                           .arg(QString::number(pos.z() ,'f',3)));
}

void MainWindow::ShowObjectList(){
    this->ui->ObjectDock->show();
}

void MainWindow::Save(){
    QString filepath = QFileDialog::getSaveFileName(this,
                                                    "Save file",
                                                    "",
                                                    "FoamCAD File(*.foamcad);;All Files (*)");
    this->model->ExportFoamFile(filepath);
}

void MainWindow::Load(){
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    "Load file",
                                                    "",
                                                    "FoamCAD File(*.foamcad);;All Files (*)");
    this->model->ImportFoamFile(filepath);

}

void MainWindow::Export(){
    ExportDialog* diag = new ExportDialog();
    diag->SetModel(this->model);
    diag->exec();

}


