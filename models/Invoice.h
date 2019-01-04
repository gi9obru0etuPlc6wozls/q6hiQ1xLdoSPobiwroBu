#ifndef INVOICE_H
#define INVOICE_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>


class TModelObject;
class InvoiceObject;
class QJsonArray;


class T_MODEL_EXPORT Invoice : public TAbstractModel
{
public:
    Invoice();
    Invoice(const Invoice &other);
    Invoice(const InvoiceObject &object);
    ~Invoice();

    *** map: Map key not found *** col01() const;
    void setCol01(const *** map: Map key not found *** &col01);
    *** map: Map key not found *** col02() const;
    void setCol02(const *** map: Map key not found *** &col02);
    *** map: Map key not found *** col03() const;
    void setCol03(const *** map: Map key not found *** &col03);
    *** map: Map key not found *** col05() const;
    void setCol05(const *** map: Map key not found *** &col05);
    Invoice &operator=(const Invoice &other);

    bool create() override { return TAbstractModel::create(); }
    bool update() override { return TAbstractModel::update(); }
    bool save()   override { return TAbstractModel::save(); }
    bool remove() override { return TAbstractModel::remove(); }

    static Invoice create(
        const *** map: Map key not found *** &col01,
        const *** map: Map key not found *** &col02,
        const *** map: Map key not found *** &col03,
        const *** map: Map key not found *** &col05
    );
    static Invoice create(const QVariantMap &values);
    static Invoice get(const *** map: Map key not found *** &col01);
    static int count();
    static QList<Invoice> getAll();
    static QList<Invoice> getAll(const int limit,const int offset);
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<InvoiceObject> d;

    TModelObject *modelData() override;
    const TModelObject *modelData() const override;
    friend QDataStream &operator<<(QDataStream &ds, const Invoice &model);
    friend QDataStream &operator>>(QDataStream &ds, Invoice &model);
};

Q_DECLARE_METATYPE(Invoice)
Q_DECLARE_METATYPE(QList<Invoice>)

#endif // INVOICE_H
