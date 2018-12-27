#ifndef INVOICEITEMS_H
#define INVOICEITEMS_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>


class TModelObject;
class InvoiceItemsObject;
class QJsonArray;


class T_MODEL_EXPORT InvoiceItems : public TAbstractModel
{
public:
    InvoiceItems();
    InvoiceItems(const InvoiceItems &other);
    InvoiceItems(const InvoiceItemsObject &object);
    ~InvoiceItems();

    string itemNo() const;
    void setItemNo(const string &itemNo);
    InvoiceItems &operator=(const InvoiceItems &other);

    bool create() override { return TAbstractModel::create(); }
    bool update() override { return TAbstractModel::update(); }
    bool save()   override { return TAbstractModel::save(); }
    bool remove() override { return TAbstractModel::remove(); }

    static InvoiceItems create(
        const string &itemNo
    );
    static InvoiceItems create(const QVariantMap &values);
    static int count();
    static QList<InvoiceItems> getAll();
    static QList<InvoiceItems> getAll(const int limit,const int offset);
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<InvoiceItemsObject> d;

    TModelObject *modelData() override;
    const TModelObject *modelData() const override;
    friend QDataStream &operator<<(QDataStream &ds, const InvoiceItems &model);
    friend QDataStream &operator>>(QDataStream &ds, InvoiceItems &model);
};

Q_DECLARE_METATYPE(InvoiceItems)
Q_DECLARE_METATYPE(QList<InvoiceItems>)

#endif // INVOICEITEMS_H
