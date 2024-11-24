CREATE OR REPLACE FUNCTION count_unique_words(text)
RETURNS integer
AS 'unique_words', 'count_unique_words'
LANGUAGE C IMMUTABLE STRICT;
