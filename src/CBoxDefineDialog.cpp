#include "CBoxDefineDialog.h"
#include "ui_CBoxDefineDialog.h"
#include <QMessageBox>


void CBoxDefineDialog::paintEvent    (QPaintEvent* ){
}
QComboBox* CBoxDefineDialog::ConvertDirToCombo   (BoundaryDir dir)const{
    if     (dir == Top   )return ui->TopTypecombo;
    else if(dir == Right )return ui->RightTypecombo;
    else if(dir == Left  )return ui->LeftTypecombo;
    else if(dir == Bottom)return ui->BottomTypecombo;
    else if(dir == Front )return ui->FrontTypecombo;
    else if(dir == Back  )return ui->BackTypecombo;
    return nullptr;
}
QLineEdit* CBoxDefineDialog::ConvertDirToNameEdit(BoundaryDir dir)const{
    if     (dir == Top   )return ui->TopNameEdit;
    else if(dir == Right )return ui->RightNameEdit;
    else if(dir == Left  )return ui->LeftNameEdit;
    else if(dir == Bottom)return ui->BottomNameEdit;
    else if(dir == Front )return ui->FrontNameEdit;
    else if(dir == Back  )return ui->BackNameEdit;
    return nullptr;
}
QString      CBoxDefineDialog::ConvertBoundaryToString(BoundaryType dir)const{
    if     (dir == BoundaryType::Patch        )return QString("patch (パッチ)");
    else if(dir == BoundaryType::Wall         )return QString("wall (壁)");
    else if(dir == BoundaryType::SymmetryPlane)return QString("symmetryPlane (対称面)");
    else if(dir == BoundaryType::Cyclic       )return QString("cyclic (周期境界)");
    else if(dir == BoundaryType::CyclicAMI    )return QString("cyclicAMI (不整合周期境界)");
    else if(dir == BoundaryType::Wedge        )return QString("wedge (2次元軸対称)");
    else if(dir == BoundaryType::Empty        )return QString("empty (2 次元)");
    else return QString("Unknown");
}
BoundaryType CBoxDefineDialog::ConvertStringToBoundary(QString str)const{
    for(int i=0;i<7;i++){
        if(str == ConvertBoundaryToString(static_cast<BoundaryType>(i))){
            return static_cast<BoundaryType>(i);
        }
    }
    return static_cast<BoundaryType>(0);
}
QString      CBoxDefineDialog::ConvertGradingToString (GradingType  dir)const{
    if     (dir == GradingType::SimpleGrading)return QString("SimpleGrading");
    else if(dir == GradingType::EdgeGrading  )return QString("EdgeGrading");
    else return QString("");
}
GradingType  CBoxDefineDialog::ConvertStringToGrading (QString str)const{
    if     (str == "SimpleGrading")return GradingType::SimpleGrading;
    else if(str == "EdgeGrading"  )return GradingType::EdgeGrading;
    else return GradingType::EmptyGrading;
}

//面の情報を取得
BoundaryType CBoxDefineDialog::GetBoundaryType(BoundaryDir dir)const{
    QString index;
    if     (dir == Top   )index = ui->TopTypecombo->currentText();
    else if(dir == Right )index = ui->RightTypecombo->currentText();
    else if(dir == Left  )index = ui->LeftTypecombo->currentText();
    else if(dir == Bottom)index = ui->BottomTypecombo->currentText();
    else if(dir == Front )index = ui->FrontTypecombo->currentText();
    else if(dir == Back  )index = ui->BackTypecombo->currentText();
    return this->ConvertStringToBoundary(index);
}
QString CBoxDefineDialog::GetBoundaryName(BoundaryDir dir)const{
    if     (dir == Top   ){
        if(ui->TopNameEdit->text() == "")return "Top";
        else return ui->TopNameEdit   ->text();
    }else if(dir == Right ){
        if(ui->RightNameEdit->text() == "")return "Right";
        else return ui->RightNameEdit ->text();
    }else if(dir == Left  ){
        if(ui->LeftNameEdit->text() == "")return "Left";
        else return ui->LeftNameEdit  ->text();
    }else if(dir == Bottom){
        if(ui->BottomNameEdit->text() == "")return "Bottom";
        else return ui->BottomNameEdit->text();
    }else if(dir == Front ){
        if(ui->FrontNameEdit->text() == "")return "Front";
        else return ui->FrontNameEdit ->text();
    }else if(dir == Back  ){
        if(ui->BackNameEdit->text() == "")return "Back";
        else return ui->BackNameEdit  ->text();
    }else return "";
}

GradingType CBoxDefineDialog::GetGradigngType()const{
    GradingType type;
    if(ui->GradingCombo->currentText() == "simpleGrading")type = SimpleGrading;
    if(ui->GradingCombo->currentText() == "EdgeGrading"  )type = EdgeGrading;
    return type;
}
QString CBoxDefineDialog::GetGradigngArgs()const{
    return ui->GradingEdit->text();
}

//エラー判定
bool CBoxDefineDialog::isFormatError()const{
    //引数の数があっているかな判定
    bool failed=false;

    int args_num = (GetGradigngType() == SimpleGrading) ? 2 : 11;
    if(GetGradigngArgs().split(' ').size() == args_num)failed = true;

    return failed;
}

//出力
CBlock CBoxDefineDialog::ExportCBlock()const{
    CBlock blocks;
    for(int i=0;i<6;i++){
        BoundaryDir dir = static_cast<BoundaryDir>(i);
        blocks.boundery[i] = this->GetBoundaryType(dir);
        blocks.name[i]     = this->GetBoundaryName(dir);
    }
    blocks.div[0] = this->ui->XspinBox->value();
    blocks.div[1] = this->ui->YspinBox->value();
    blocks.div[2] = this->ui->ZspinBox->value();
    blocks.depth  = this->ui->DepthSpinBox->value();
    blocks.grading   = this->GetGradigngType();
    QStringList list = ui->GradingEdit->text().split(' ');
    for(int i=0;i<list.size();i++){
        blocks.grading_args.push_back(list[i].toDouble());
    }
    return blocks;
}
void CBoxDefineDialog::ImportCBlock(const CBlock &block){
    //麺の設定
    for(int i = 0;i<6;i++){
        this->ConvertDirToNameEdit(static_cast<BoundaryDir>(i))->setText(block.name[i]);
        this->ConvertDirToCombo   (static_cast<BoundaryDir>(i))->setCurrentText(this->ConvertBoundaryToString(block.boundery[i]));
    }
    //全体設定
    QStringList list;
    for(double d :block.grading_args)list.push_back(QString::number(d));
    this->ui->GradingCombo->setCurrentText(this->ConvertGradingToString(block.grading));
    this->ui->GradingEdit ->setText(list.join(" "));
}



CBoxDefineDialog::CBoxDefineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CBoxDefineDialog)
{
    ui->setupUi(this);
    ui->GradingEdit       ->setText("1 1 1");
}

CBoxDefineDialog::~CBoxDefineDialog()
{
    delete ui;
}

void CBoxDefineDialog::AcceptProxy(){
    if(this->isFormatError()){
        QMessageBox::information(this,"フォーマットエラー","Gradigngの引数が合っていません。",QMessageBox::Ok);
    }else{
        accept();
    }
}
