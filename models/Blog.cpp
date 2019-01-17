#include <TreeFrogModel>
#include "blog.h"
#include "blogobject.h"

Blog::Blog()
    : TAbstractModel(), d(new BlogObject())
{
    d->col_integer = 0;
    d->col_float = 0;
    d->col_double = 0;
    d->col_numeric = 0;
    d->lock_revision = 0;
}

Blog::Blog(const Blog &other)
    : TAbstractModel(), d(new BlogObject(*other.d))
{ }

Blog::Blog(const BlogObject &object)
    : TAbstractModel(), d(new BlogObject(object))
{ }

Blog::~Blog()
{
    // If the reference count becomes 0,
    // the shared data object 'BlogObject' is deleted.
}

qulonglong Blog::id() const
{
    return d->id;
}

QString Blog::title() const
{
    return d->title;
}

void Blog::setTitle(const QString &title)
{
    d->title = title;
}

QString Blog::body() const
{
    return d->body;
}

void Blog::setBody(const QString &body)
{
    d->body = body;
}

QString Blog::colString() const
{
    return d->col_string;
}

void Blog::setColString(const QString &colString)
{
    d->col_string = colString;
}

int Blog::colInteger() const
{
    return d->col_integer;
}

void Blog::setColInteger(int colInteger)
{
    d->col_integer = colInteger;
}

double Blog::colFloat() const
{
    return d->col_float;
}

void Blog::setColFloat(double colFloat)
{
    d->col_float = colFloat;
}

double Blog::colDouble() const
{
    return d->col_double;
}

void Blog::setColDouble(double colDouble)
{
    d->col_double = colDouble;
}

double Blog::colNumeric() const
{
    return d->col_numeric;
}

void Blog::setColNumeric(double colNumeric)
{
    d->col_numeric = colNumeric;
}

char Blog::colBoolean() const
{
    return d->col_boolean;
}

void Blog::setColBoolean(const char &colBoolean)
{
    d->col_boolean = colBoolean;
}

QString Blog::colUuid() const
{
    return d->col_uuid;
}

void Blog::setColUuid(const QString &colUuid)
{
    d->col_uuid = colUuid;
}

QDateTime Blog::colDatetime() const
{
    return d->col_datetime;
}

void Blog::setColDatetime(const QDateTime &colDatetime)
{
    d->col_datetime = colDatetime;
}

QDateTime Blog::createdAt() const
{
    return d->created_at;
}

QDateTime Blog::updatedAt() const
{
    return d->updated_at;
}

int Blog::lockRevision() const
{
    return d->lock_revision;
}

Blog &Blog::operator=(const Blog &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

Blog Blog::create(const QString &title, const QString &body, const QString &colString, int colInteger, double colFloat, double colDouble, double colNumeric, const char &colBoolean, const QString &colUuid, const QDateTime &colDatetime)
{
    BlogObject obj;
    obj.title = title;
    obj.body = body;
    obj.col_string = colString;
    obj.col_integer = colInteger;
    obj.col_float = colFloat;
    obj.col_double = colDouble;
    obj.col_numeric = colNumeric;
    obj.col_boolean = colBoolean;
    obj.col_uuid = colUuid;
    obj.col_datetime = colDatetime;
    if (!obj.create()) {
        return Blog();
    }
    return Blog(obj);
}

Blog Blog::create(const QVariantMap &values)
{
    Blog model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

Blog Blog::get(const qulonglong &id)
{
    TSqlORMapper<BlogObject> mapper;
    return Blog(mapper.findByPrimaryKey(id));
}

Blog Blog::get(const qulonglong &id, int lockRevision)
{
    TSqlORMapper<BlogObject> mapper;
    TCriteria cri;
    cri.add(BlogObject::Id, id);
    cri.add(BlogObject::LockRevision, lockRevision);
    return Blog(mapper.findFirst(cri));
}

int Blog::count()
{
    TSqlORMapper<BlogObject> mapper;
    return mapper.findCount();
}

QList<Blog> Blog::getAll()
{
    return tfGetModelListByCriteria<Blog, BlogObject>(TCriteria());
}

QJsonArray Blog::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<BlogObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<BlogObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(Blog(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *Blog::modelData()
{
    return d.data();
}

const TModelObject *Blog::modelData() const
{
    return d.data();
}

QDataStream &operator<<(QDataStream &ds, const Blog &model)
{
    auto varmap = model.toVariantMap();
    ds << varmap;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Blog &model)
{
    QVariantMap varmap;
    ds >> varmap;
    model.setProperties(varmap);
    return ds;
}

// Don't remove below this line
T_REGISTER_STREAM_OPERATORS(Blog)
