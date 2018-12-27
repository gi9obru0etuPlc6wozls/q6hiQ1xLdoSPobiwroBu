#ifndef INVOICEITEMS_H
#define INVOICEITEMS_H

#include <TSqlObject>
#include <QSharedData>




class T_MODEL_EXPORT InvoiceItems : public TSqlObject, public QSharedData
{
public:
    string itemNo;
    int lock_revision {0}; 


    enum PropertyIndex {
        ItemNo = 0
    };

    QString tableName() const override { return QLatin1String("invoice_items"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(string item_no  READ getitem_no WRITE setitem_no)
    T_DEFINE_PROPERTY(string, item_no )
};

#endif // INVOICEITEMS_H