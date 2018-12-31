#include <TreeFrogModel>
#include "invoice.h"
#include "invoiceobject.h"

Invoice::Invoice()
    : TAbstractModel(), d(new InvoiceObject())
{
    d->lock_revision = 0;
}

Invoice::Invoice(const Invoice &other)
    : TAbstractModel(), d(new InvoiceObject(*other.d))
{ }

Invoice::Invoice(const InvoiceObject &object)
    : TAbstractModel(), d(new InvoiceObject(object))
{ }

Invoice::~Invoice()
{
    // If the reference count becomes 0,
    // the shared data object 'InvoiceObject' is deleted.
}

//
// Columns Start
//
QString Invoice::col01() const
{
    return d->col01;
}

void Invoice::setCol01(const QString &col01)
{
    d->col01 = col01;
}

QString Invoice::col02() const
{
    return d->col02;
}

void Invoice::setCol02(const QString &col02)
{
    d->col02 = col02;
}

QString Invoice::esra() const
{
    return d->esra;
}

void Invoice::setEsra(const QString &esra)
{
    d->esra = esra;
}

QString Invoice::col04() const
{
    return d->col04;
}

void Invoice::setCol04(const QString &col04)
{
    d->col04 = col04;
}

QString Invoice::col03() const
{
    return d->col03;
}

void Invoice::setCol03(const QString &col03)
{
    d->col03 = col03;
}

QString Invoice::col05() const
{
    return d->col05;
}

void Invoice::setCol05(const QString &col05)
{
    d->col05 = col05;
}
//
// Columns End
//

Invoice &Invoice::operator=(const Invoice &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

Invoice Invoice::create(
        const *** Map key not found *** &col01,
        const *** Map key not found *** &col02,
        const *** Map key not found *** &esra,
        const *** Map key not found *** &col04,
        const *** Map key not found *** &col03,
        const *** Map key not found *** &col05)
{
    InvoiceObject obj;
    obj.col01 = col01;
    obj.col02 = col02;
    obj.esra = esra;
    obj.col04 = col04;
    obj.col03 = col03;
    obj.col05 = col05;
    if (!obj.create()) {
        return Invoice();
    }
    return Invoice(obj);
}

Invoice Invoice::create(const QVariantMap &values)
{
    Invoice model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

Invoice Invoice::get(const QString &id)
{
    TSqlORMapper<InvoiceObject> mapper;
    return Invoice(mapper.findByPrimaryKey(id));
}

int Invoice::count()
{
    TSqlORMapper<InvoiceObject> mapper;
    return mapper.findCount();
}

QList<Invoice> Invoice::getAll()
{
    return tfGetModelListByCriteria<Invoice, InvoiceObject>(TCriteria());
}

QList<Invoice> Invoice::getAll(const int limit,const int offset)
{
    return tfGetModelListByCriteria<Invoice, InvoiceObject>(TCriteria(), limit, offset);
}

QJsonArray Invoice::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<InvoiceObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<InvoiceObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(Invoice(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *Invoice::modelData()
{
    return d.data();
}

const TModelObject *Invoice::modelData() const
{
    return d.data();
}

QDataStream &operator<<(QDataStream &ds, const Invoice &model)
{
    auto varmap = model.toVariantMap();
    ds << varmap;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Invoice &model)
{
    QVariantMap varmap;
    ds >> varmap;
    model.setProperties(varmap);
    return ds;
}

// Don't remove below this line
T_REGISTER_STREAM_OPERATORS(Invoice)
