#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <CObject/CObject.h>
#include <CObject/CPoint.h>
#include <CObject/CLine.h>
#include <CObject/CArc.h>
#include <CObject/CSpline.h>
#include <CObject/CBlock.h>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>
#include <QSettings>
#include "FoamFile.h"

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT
private:
    QVector<CObject*> objects;
    QVector<CBlock*> blocks;
    void Export(QString filename)const;

    //座標から番号へ変換
    int GetPosIndex(Pos p)const;

    //全頂点リストを取得
    QVector<Pos> GetVerticesPos()const;

    //境界面の座標を取得
    QVector<Pos> GetBoundaryPos(CBlock *block, BoundaryDir dir)const;

public:
    void SetBlocks(QVector<CBlock *> blocks);

    explicit ExportDialog(QWidget *parent = 0);
    ~ExportDialog();

public slots:
    void ChangeDirctory();
    void AcceptDialog();

private:
    Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H