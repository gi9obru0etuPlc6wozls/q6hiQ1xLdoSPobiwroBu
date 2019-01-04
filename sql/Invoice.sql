CREATE UNLOGGED TEMPORARY TABLE invoice (
    col01 UUID DEFAULT uuid_generate_v4() NOT NULL PRIMARY KEY,
    col02 *** map: Map key not found ***(255),
    col03 *** map: Map key not found ***(512),
    col05 *** map: Map key not found ***(512)
);

