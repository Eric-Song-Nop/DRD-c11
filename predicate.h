#ifndef __PREDICATE_H__
#define __PREDICATE_H__

#include "funcinst.h"
#include "hashset.h"
#include "predicatetypes.h"
#include "classlist.h"

unsigned int pred_expr_hash (struct pred_expr *);
bool pred_expr_equal(struct pred_expr *, struct pred_expr *);
typedef HashSet<struct pred_expr *, uintptr_t, 0, model_malloc, model_calloc, model_free, pred_expr_hash, pred_expr_equal> PredExprSet;
typedef HSIterator<struct pred_expr *, uintptr_t, 0, model_malloc, model_calloc, model_free, pred_expr_hash, pred_expr_equal> PredExprSetIter;

class Predicate {
public:
	Predicate(FuncInst * func_inst, bool is_entry = false);
	~Predicate();

	FuncInst * get_func_inst() { return func_inst; }
	PredExprSet * get_pred_expressions() { return &pred_expressions; }

	void add_predicate_expr(token_t token, FuncInst * func_inst, bool value);
	void add_child(Predicate * child);
	void set_parent(Predicate * parent_pred) { parent = parent_pred; }
	void add_backedge(Predicate * back_pred) { backedges.add(back_pred); }
	void copy_predicate_expr(Predicate * other);

	ModelVector<Predicate *> * get_children() { return &children; }
	Predicate * get_parent() { return parent; }
	PredSet * get_backedges() { return &backedges; }

	bool is_entry_predicate() { return entry_predicate; }
	void set_entry_predicate() { entry_predicate = true; }

	/* Whether func_inst does write or not */
	bool is_write() { return does_write; }
	void set_write(bool is_write) { does_write = is_write; }

	ConcretePredicate * evaluate(inst_act_map_t * inst_act_map, thread_id_t tid);

	void print_predicate();
	void print_pred_subtree();

	MEMALLOC
private:
	FuncInst * func_inst;
	bool entry_predicate;
	bool does_write;

	/* May have multiple predicate expressions */
	PredExprSet pred_expressions;
	ModelVector<Predicate *> children;

	/* Only a single parent may exist */
	Predicate * parent;

	/* May have multiple back edges, e.g. nested loops */
	PredSet backedges;
};

#endif /* __PREDICATE_H__ */
