CREATE UNLOGGED TEMPORARY TABLE invoice (
    col01 UUID DEFAULT uuid_generate_v4() NOT NULL PRIMARY KEY,
    col02 string(255),
    esra oldtype(512),
    col04 type04(1235),
    col03 oldtype(512),
    col05 delete_me(512)
);

