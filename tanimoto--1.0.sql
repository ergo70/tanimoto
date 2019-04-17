/* contrib/tanimoto/tanimoto--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION tanimoto" to load this file. \quit

-- Register the functions.
CREATE or replace FUNCTION tanimoto_c(bit varying, bit varying)
RETURNS real
AS 'MODULE_PATHNAME', 'tanimoto'
LANGUAGE C PARALLEL SAFE STRICT IMMUTABLE LEAKPROOF security invoker;

CREATE OR REPLACE FUNCTION cdk.tanimoto(bit varying, bit varying)
 RETURNS real
 LANGUAGE sql
 IMMUTABLE STRICT leakproof security invoker parallel safe
AS $function$select length(replace(($1 & $2)::text, '0', ''))::real / length(replace(($1 | $2)::text, '0', ''))::real;$function$;
