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

    *** Map key not found *** col01() const;
    void setCol01(const *** Map key not found *** &col01);
    *** Map key not found *** col02() const;
    void setCol02(const *** Map key not found *** &col02);
    *** Map key not found *** esra() const;
    void setEsra(const *** Map key not found *** &esra);
    *** Map key not found *** col04() const;
    void setCol04(const *** Map key not found *** &col04);
    *** Map key not found *** col03() const;
    void setCol03(const *** Map key not found *** &col03);
    *** Map key not found *** col05() const;
    void setCol05(const *** Map key not found *** &col05);
    Invoice &operator=(const Invoice &other);

    bool create() override { return TAbstractModel::create(); }
    bool update() override { return TAbstractModel::update(); }
    bool save()   override { return TAbstractModel::save(); }
    bool remove() override { return TAbstractModel::remove(); }

    static Invoice create(
        const *** Map key not found *** &col01,
        const *** Map key not found *** &col02,
        const *** Map key not found *** &esra,
        const *** Map key not found *** &col04,
        const *** Map key not found *** &col03,
        const *** Map key not found *** &col05
    );
    static Invoice create(const QVariantMap &values);
    static Invoice get(const *** Map key not found *** &col01);
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
