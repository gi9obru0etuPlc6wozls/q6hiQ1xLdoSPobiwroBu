CREATE UNLOGGED TEMPORARY TABLE blog (
    id SERIAL PRIMARY KEY,
    title *** map: db_type key not found:string ***(20),
    body *** map: db_type key not found:string ***(200),
    col_string *** map: db_type key not found:string ***(12),
    col_integer *** map: db_type key not found:integer ***,
    col_float *** map: db_type key not found:float ***(12,2),
    col_double *** map: db_type key not found:double ***(14,2),
    col_numeric *** map: db_type key not found:numeric ***(16,2),
    col_boolean *** map: db_type key not found:boolean ***,
    col_uuid *** map: db_type key not found:string ***(36),
    col_datetime *** map: db_type key not found:datetime ***,
    created_at *** map: db_type key not found:datetime ***,
    updated_at *** map: db_type key not found:datetime ***,
    lock_revision *** map: db_type key not found:int ***
);

