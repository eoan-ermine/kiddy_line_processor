DROP SCHEMA IF EXISTS processor_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS processor_schema;

CREATE TABLE IF NOT EXISTS processor_schema.baseball (
    id SERIAL UNIQUE,
    coefficient DOUBLE PRECISION
);

CREATE TABLE IF NOT EXISTS processor_schema.american_football (
    id SERIAL UNIQUE,
    coefficient DOUBLE PRECISION
);

CREATE TABLE IF NOT EXISTS processor_schema.football (
    id SERIAL UNIQUE,
    coefficient DOUBLE PRECISION
);

DROP SCHEMA IF EXISTS hello_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS hello_schema;

CREATE TABLE IF NOT EXISTS hello_schema.users (
    name TEXT PRIMARY KEY,
    count INTEGER DEFAULT(1)
);