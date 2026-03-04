// zzarray_wrapper.h

#ifndef ZZARRAY_WRAPPER_H
#define ZZARRAY_WRAPPER_H

#include "result.h"

// ============================================================
// WRAPPERS DE ARRAY
// ============================================================

EvaluatorResult builtin_push(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

EvaluatorResult builtin_pop(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

EvaluatorResult builtin_len(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

EvaluatorResult builtin_is_empty(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

EvaluatorResult builtin_get(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

EvaluatorResult builtin_set(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

EvaluatorResult builtin_insert(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

EvaluatorResult builtin_remove(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

EvaluatorResult builtin_swap(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

EvaluatorResult builtin_sort(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

EvaluatorResult builtin_rsort(
	EvaluatorResult* args,
	int arg_count,
	int line,
	int column,
    ScopeStack* scope_stack);

#endif
