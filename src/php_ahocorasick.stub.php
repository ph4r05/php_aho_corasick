<?php

/**
 * @generate-function-entries
 * @generate-legacy-arginfo
 */


/**
 * @param resource $id
 */
function ahocorasick_match(string $needle, $id, bool $findAll=true): false|array {}

/**
 * @return resource
 */
function ahocorasick_init(array $data) {}

/**
 * @param resource $id
 */
function ahocorasick_deinit($id):bool {}

/**
 * @param resource $id
 */
function ahocorasick_isValid($id): bool {}

/**
 * @param resource $id
 */
function ahocorasick_finalize($id): bool {}

/**
 * @param resource $id
 */
function ahocorasick_add_patterns($id, array $patterns): bool {}

