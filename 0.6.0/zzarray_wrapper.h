// zzarray_wrapper.h

#ifndef ZZARRAY_WRAPPER_H
#define ZZARRAY_WRAPPER_H

#include "evaluator.h"

// ============================================================
// WRAPPERS DE ARRAY
// ============================================================

EvaluatorResult builtin_push(EvaluatorResult* args, int arg_count, int line, int column);
EvaluatorResult builtin_pop(EvaluatorResult* args, int arg_count, int line, int column);
EvaluatorResult builtin_len(EvaluatorResult* args, int arg_count, int line, int column);
EvaluatorResult builtin_is_empty(EvaluatorResult* args, int arg_count, int line, int column);
EvaluatorResult builtin_get(EvaluatorResult* args, int arg_count, int line, int column);
EvaluatorResult builtin_set(EvaluatorResult* args, int arg_count, int line, int column);
EvaluatorResult builtin_insert(EvaluatorResult* args, int arg_count, int line, int column);
EvaluatorResult builtin_remove(EvaluatorResult* args, int arg_count, int line, int column);

// ============================================================
// WRAPPERS DE STRING (futuro)
// ============================================================

// EvaluatorResult builtin_upper(EvaluatorResult* args, int arg_count);
// EvaluatorResult builtin_lower(EvaluatorResult* args, int arg_count);
// EvaluatorResult builtin_trim(EvaluatorResult* args, int arg_count);

// ============================================================
// WRAPPERS DE MATH (futuro)
// ============================================================

// EvaluatorResult builtin_sqrt(EvaluatorResult* args, int arg_count);
// EvaluatorResult builtin_sin(EvaluatorResult* args, int arg_count);

#endif
