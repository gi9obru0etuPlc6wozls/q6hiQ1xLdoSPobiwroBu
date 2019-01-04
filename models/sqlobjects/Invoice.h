#ifndef INVOICE_H
#define INVOICE_H

#include <TSqlObject>
#include <QSharedData>




class T_MODEL_EXPORT Invoice : public TSqlObject, public QSharedData
{
public:
    *** map: Map key not found *** col01;
    *** map: Map key not found *** col02;
    *** map: Map key not found *** col03;
    *** map: Map key not found *** col05;
    int lock_revision {0}; 


    enum PropertyIndex {
        Col01 = 0,
        Col02,
        Col03,
        Col05
    };

    int primaryKeyIndex() const override { return Col01; }
    int autoValueIndex() const override { return Col01; }
    QString tableName() const override { return QLatin1String("invoice"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(*** map: Map key not found *** col01  READ getcol01 WRITE setcol01)
    T_DEFINE_PROPERTY(*** map: Map key not found ***, col01 )
    Q_PROPERTY(*** map: Map key not found *** col02  READ getcol02 WRITE setcol02)
    T_DEFINE_PROPERTY(*** map: Map key not found ***, col02 )
    Q_PROPERTY(*** map: Map key not found *** col03  READ getcol03 WRITE setcol03)
    T_DEFINE_PROPERTY(*** map: Map key not found ***, col03 )
    Q_PROPERTY(*** map: Map key not found *** col05  READ getcol05 WRITE setcol05)
    T_DEFINE_PROPERTY(*** map: Map key not found ***, col05 )
};

#endif // INVOICE_H