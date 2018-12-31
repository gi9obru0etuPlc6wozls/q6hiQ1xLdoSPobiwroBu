#ifndef INVOICE_H
#define INVOICE_H

#include <TSqlObject>
#include <QSharedData>




class T_MODEL_EXPORT Invoice : public TSqlObject, public QSharedData
{
public:
    *** Map key not found *** col01;
    *** Map key not found *** col02;
    *** Map key not found *** esra;
    *** Map key not found *** col04;
    *** Map key not found *** col03;
    *** Map key not found *** col05;
    int lock_revision {0}; 


    enum PropertyIndex {
        Col01 = 0,
        Col02,
        Esra,
        Col04,
        Col03,
        Col05
    };

    int primaryKeyIndex() const override { return Col01; }
    int autoValueIndex() const override { return Col01; }
    QString tableName() const override { return QLatin1String("invoice"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(*** Map key not found *** col01  READ getcol01 WRITE setcol01)
    T_DEFINE_PROPERTY(*** Map key not found ***, col01 )
    Q_PROPERTY(*** Map key not found *** col02  READ getcol02 WRITE setcol02)
    T_DEFINE_PROPERTY(*** Map key not found ***, col02 )
    Q_PROPERTY(*** Map key not found *** esra  READ getesra WRITE setesra)
    T_DEFINE_PROPERTY(*** Map key not found ***, esra )
    Q_PROPERTY(*** Map key not found *** col04  READ getcol04 WRITE setcol04)
    T_DEFINE_PROPERTY(*** Map key not found ***, col04 )
    Q_PROPERTY(*** Map key not found *** col03  READ getcol03 WRITE setcol03)
    T_DEFINE_PROPERTY(*** Map key not found ***, col03 )
    Q_PROPERTY(*** Map key not found *** col05  READ getcol05 WRITE setcol05)
    T_DEFINE_PROPERTY(*** Map key not found ***, col05 )
};

#endif // INVOICE_H