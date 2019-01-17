-- MySQL
CREATE TABLE blog (
    id bigint(20) unsigned NOT NULL PRIMARY KEY,
    title varchar(20),
    body varchar(200),
    col_string varchar(12),
    col_integer int,
    col_float float(12,2),
    col_double double(14,2),
    col_numeric decimal(16,2),
    col_boolean boolean,
    col_uuid varchar(36),
    col_datetime datetime,
    created_at datetime,
    updated_at datetime,
    lock_revision int
);

