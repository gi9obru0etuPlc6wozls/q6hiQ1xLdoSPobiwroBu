#include <TreeFrogModel>
#include "invoiceitems.h"
#include "invoiceitemsobject.h"

InvoiceItems::InvoiceItems()
    : TAbstractModel(), d(new InvoiceItemsObject())
{
    d->lock_revision = 0;
}

InvoiceItems::InvoiceItems(const InvoiceItems &other)
    : TAbstractModel(), d(new InvoiceItemsObject(*other.d))
{ }

InvoiceItems::InvoiceItems(const InvoiceItemsObject &object)
    : TAbstractModel(), d(new InvoiceItemsObject(object))
{ }

InvoiceItems::~InvoiceItems()
{
    // If the reference count becomes 0,
    // the shared data object 'InvoiceItemsObject' is deleted.
}

//
// Columns Start
//
QString InvoiceItems::itemNo() const
{
    return d->item_no;
}

void InvoiceItems::setItemNo(const QString &itemNo)
{
    d->item_no = itemNo;
}
//
// Columns End
//

InvoiceItems &InvoiceItems::operator=(const InvoiceItems &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

InvoiceItems InvoiceItems::create(
        const string &itemNo)
{
    InvoiceItemsObject obj;
    obj.item_no = itemNo;
    if (!obj.create()) {
        return InvoiceItems();
    }
    return InvoiceItems(obj);
}

InvoiceItems InvoiceItems::create(const QVariantMap &values)
{
    InvoiceItems model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

InvoiceItems InvoiceItems::get(const QString &id)
{
    TSqlORMapper<InvoiceItemsObject> mapper;
    return InvoiceItems(mapper.findByPrimaryKey(id));
}

int InvoiceItems::count()
{
    TSqlORMapper<InvoiceItemsObject> mapper;
    return mapper.findCount();
}

QList<InvoiceItems> InvoiceItems::getAll()
{
    return tfGetModelListByCriteria<InvoiceItems, InvoiceItemsObject>(TCriteria());
}

QList<InvoiceItems> InvoiceItems::getAll(const int limit,const int offset)
{
    return tfGetModelListByCriteria<InvoiceItems, InvoiceItemsObject>(TCriteria(), limit, offset);
}

QJsonArray InvoiceItems::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<InvoiceItemsObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<InvoiceItemsObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(InvoiceItems(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *InvoiceItems::modelData()
{
    return d.data();
}

const TModelObject *InvoiceItems::modelData() const
{
    return d.data();
}

QDataStream &operator<<(QDataStream &ds, const InvoiceItems &model)
{
    auto varmap = model.toVariantMap();
    ds << varmap;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, InvoiceItems &model)
{
    QVariantMap varmap;
    ds >> varmap;
    model.setProperties(varmap);
    return ds;
}

// Don't remove below this line
T_REGISTER_STREAM_OPERATORS(InvoiceItems)
