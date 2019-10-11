#include "funcinst.h"
#include "predicate.h"
#include "concretepredicate.h"

Predicate::Predicate(FuncInst * func_inst, bool is_entry) :
	func_inst(func_inst),
	entry_predicate(is_entry),
	does_write(false),
	exploration_count(0),
	pred_expressions(16),
	children(),
	parent(NULL),
	backedges(16)
{}

Predicate::~Predicate()
{
	// parent and func_inst should not be deleted
	pred_expressions.reset();
	backedges.reset();
	children.clear();
}

unsigned int pred_expr_hash(struct pred_expr * expr)
{
        return (unsigned int)((uintptr_t)expr);
}

bool pred_expr_equal(struct pred_expr * p1, struct pred_expr * p2)
{
	if (p1->token != p2->token)
		return false;
	if (p1->token == EQUALITY && p1->func_inst != p2->func_inst)
		return false;
	if (p1->value != p2->value)
		return false;
	return true;
}

void Predicate::add_predicate_expr(token_t token, FuncInst * func_inst, bool value)
{
	struct pred_expr *ptr = new pred_expr(token, func_inst, value);
	pred_expressions.add(ptr);
}

void Predicate::add_child(Predicate * child)
{
	/* check duplication? */
	children.push_back(child);
}

void Predicate::copy_predicate_expr(Predicate * other)
{
	PredExprSet * other_pred_expressions = other->get_pred_expressions();
	PredExprSetIter * it = other_pred_expressions->iterator();

	while (it->hasNext()) {
		struct pred_expr * ptr = it->next();
		struct pred_expr * copy = new pred_expr(ptr->token, ptr->func_inst, ptr->value);
		pred_expressions.add(copy);
	}
}

/* Evaluate predicate expressions against the given inst_act_map */
ConcretePredicate * Predicate::evaluate(inst_act_map_t * inst_act_map, thread_id_t tid)
{
	ConcretePredicate * concrete = new ConcretePredicate(tid);
	PredExprSetIter * it = pred_expressions.iterator();

	while (it->hasNext()) {
		struct pred_expr * ptr = it->next();
		uint64_t value = 0;

		switch(ptr->token) {
			case NOPREDICATE:
				break;
			case EQUALITY:
				FuncInst * to_be_compared;
				ModelAction * last_act;

				to_be_compared = ptr->func_inst;
				last_act = inst_act_map->get(to_be_compared);
				value = last_act->get_reads_from_value();
				break;
			case NULLITY:
				break;
			default:
				break;
		}

		concrete->add_expression(ptr->token, value, ptr->value);
	}

	return concrete;
}

void Predicate::print_predicate()
{
	model_print("\"%p\" [shape=box, label=\"\n", this);
	if (entry_predicate) {
		model_print("entry node\"];\n");
		return;
	}

	func_inst->print();

	PredExprSetIter * it = pred_expressions.iterator();

	if (pred_expressions.getSize() == 0)
		model_print("predicate unset\n");

	while (it->hasNext()) {
		struct pred_expr * expr = it->next();
		FuncInst * inst = expr->func_inst;
		switch (expr->token) {
			case NOPREDICATE:
				break;
			case EQUALITY:
				model_print("predicate token: equality, position: %s, value: %d\n", inst->get_position(), expr->value);
				break;
			case NULLITY:
				model_print("predicate token: nullity, value: %d\n", expr->value);
				break;
			default:
				model_print("unknown predicate token\n");
				break;
		}
	}

	if (does_write) {
		model_print("Does write\n");
	}
	model_print("Count: %d\n", exploration_count);
	model_print("\"];\n");
}

void Predicate::print_pred_subtree()
{
	print_predicate();
	for (uint i = 0; i < children.size(); i++) {
		Predicate * child = children[i];
		child->print_pred_subtree();
		model_print("\"%p\" -> \"%p\"\n", this, child);
	}

	PredSetIter * it = backedges.iterator();
	while (it->hasNext()) {
		Predicate * backedge = it->next();
		model_print("\"%p\" -> \"%p\"[style=dashed, color=grey]\n", this, backedge);
	}
}
