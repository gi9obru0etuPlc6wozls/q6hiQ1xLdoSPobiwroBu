#ifndef INVOICE_H
#define INVOICE_H

#include <TSqlObject>
#include <QSharedData>




class T_MODEL_EXPORT Invoice : public TSqlObject, public QSharedData
{
public:
    uuid col01;
    string col02;
    oldtype esra;
    type04 col04;
    oldtype col03;
    int lock_revision {0}; 


    enum PropertyIndex {
        Col01 = 0,
        Col02,
        Esra,
        Col04,
        Col03
    };

    int primaryKeyIndex() const override { return Col01; }
    int autoValueIndex() const override { return Col01; }
    QString tableName() const override { return QLatin1String("invoice"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(uuid col01  READ getcol01 WRITE setcol01)
    T_DEFINE_PROPERTY(uuid, col01 )
    Q_PROPERTY(string col02  READ getcol02 WRITE setcol02)
    T_DEFINE_PROPERTY(string, col02 )
    Q_PROPERTY(oldtype esra  READ getesra WRITE setesra)
    T_DEFINE_PROPERTY(oldtype, esra )
    Q_PROPERTY(type04 col04  READ getcol04 WRITE setcol04)
    T_DEFINE_PROPERTY(type04, col04 )
    Q_PROPERTY(oldtype col03  READ getcol03 WRITE setcol03)
    T_DEFINE_PROPERTY(oldtype, col03 )
};

#endif // INVOICE_H