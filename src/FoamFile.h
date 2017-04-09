#ifndef FOAMFILE_H
#define FOAMFILE_H
#include <QVector>
#include <QMap>
#include <fstream>
#include "Utils.h"


//FOAMファイル
class FoamFile
{

private:

    //括弧タイプ
    enum DIFINITON{
        DICTIONARY,
        LIST ,
    };

    QVector<Difinition> defs;
    std::ofstream ofs;

    //タブ出力
    void TabOut();
public:

    //定義開始
    void StartDictionaryDifinition(QString title); //辞書{}
    void StartListDifinition(QString title);       //リスト()
    //各出力
    template<class T>
    void VectorToString(QVector<T> vector);        //ベクトル出力
    void OutValue(QString name,QString value);     //値出力
    void OutString(QString name,QString value);    //文字列出力
    void EndScope();//閉じかっこ出力

    //ベクトルを文字列に変換
    template<class T>
    void VectorToString(QVector<T> vector);

    //ファイル操作
    void Open(QString filepath);
    void Close(QString filepath);

    FoamFile();
    ~FoamFile();
};

#endif // FOAMFILE_H
