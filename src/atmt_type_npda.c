///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Automata a Pila No Deterministico
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <atmt.h>
#include <tape.h>
#include <set.h>
#include <stack.h>

#include <treenpda.h>


void syms_to_str(int* syms, int count, char* buf);

void stack_copy(const struct stack* stk, struct stack* dst)
{
	memcpy(dst, stk, sizeof(struct stack));
}
bool stack_equals(const struct stack* stk1, const struct stack* stk2)
{
	if(stk1->top == stk2->top)
	{
		int i;
		for(i = 0; i < stk1->top; i++)
		{
			if(stk1->buf[i] != stk2->buf[i])
				return false;
		}
		return true;
	}
	return false;
}


struct treenpda* treenpda_create_root(struct automata* autom, struct tape* tape, int initial_state)
{
	static char sz[250];
	int size;
	struct treenpda* node;

	if(!tape)
	{
		sprintf(sz, "%s, %c", autom->states[initial_state].name.buffer, CH_ZETA);
	}
	else
	{
		const char* string = &tape->buffer[tape->pos];
		sprintf(sz, "(%s, %s, %c)", autom->states[initial_state].name.buffer, string, CH_ZETA);
	}
	size = sizeof(struct treenpda) + strlen(sz) + 1;
	node = (struct treenpda*)malloc(size);
	if(node)
	{
		memset(node, 0, size);
		node->state = initial_state;
		node->symbol = 0;
		node->parent = 0;
		
		stack_init(&node->stack);
		stack_push(&node->stack, CH_ZETA);
		
		strcpy(node->label, sz);
	}
	return node;
}
struct treenpda* treenpda_create_node(struct automata* autom, struct tape* tape, struct automata_transition_pda* transition, struct treenpda* parent)
{
	static char sz[250];
	static char sz_stack[256];
	int size;
	struct treenpda* node;
	struct stack stack;
	
	stack_init(&stack);
	if(!parent)
		stack_push(&stack, CH_ZETA);
	else
		stack_copy(&parent->stack, &stack);
	//
	// Quitar elemento de la pila
	//
	if(transition->pop != CH_EPSILON)
	{
		stack_pop(&stack);
	}

	//
	// Insertar elementos en la pila
	//
	if(transition->push[0] != CH_EPSILON)
	{
		int i;
		for(i = 0; i < MAX_PUSHABLE_CHARS && transition->push[i] != CH_NULL; i++)
		{
			stack_push(&stack, transition->push[i]);
		}
	}
	syms_to_str(stack.buf, stack_size(&stack), sz_stack);
	
	if(!tape)
	{
		sprintf(sz, "%s, %s", autom->states[transition->state].name.buffer, sz_stack);
	}
	else
	{
		const char* string = &tape->buffer[tape->pos];
		if(string[0] != 0)
			sprintf(sz, "(%s, %s, %s)", autom->states[transition->state].name.buffer, string, sz_stack);
		else
			sprintf(sz, "(%s, %c, %s)", autom->states[transition->state].name.buffer, CH_EPSILON, sz_stack);
	}

	size = sizeof(struct treenpda) + strlen(sz) + 1;
	node = (struct treenpda*)malloc(size);
	if(node)
	{
		memset(node, 0, size);
		node->state = transition->state;
		node->symbol = transition->in;
		node->pop = transition->pop;
		memcpy(node->push, transition->push, sizeof(transition->push));
		node->parent = parent;
		
		if(parent)
		{
			if(!parent->first)
			{
				//
				// Si el padre no tiene hijos, 'node' es el primero y el último
				//
				parent->first = node;
				parent->last = node;
			}
			else
			{
				//
				// Agregar 'node' al inicio de la lista de hijos de 'parent'
				//
				parent->first->prev = node;
				node->next = parent->first;
				parent->first = node;
			}
		}
		stack_copy(&stack, &node->stack);
		strcpy(node->label, sz);
	}
	return node;
}


//
// Comparar si existe la pareja (estado, pila)
//
static bool contains_instance_of_state_and_stack(struct set* currents, struct treenpda* node, struct automata_transition_pda* transition)
{
	int i;
	int state = transition->state;
	struct stack stack;
	
	stack_copy(&node->stack, &stack);
	//
	// Quitar elemento de la pila
	//
	if(transition->pop != CH_EPSILON)
	{
		stack_pop(&stack);
	}
	//
	// Insertar elementos en la pila
	//
	if(transition->push[0] != CH_EPSILON)
	{
		int i;
		for(i = 0; i < MAX_PUSHABLE_CHARS && transition->push[i] != CH_NULL; i++)
		{
			stack_push(&stack, transition->push[i]);
		}
	}
	//
	// ...
	//
	for(i = 0; i < currents->size; i++)
	{
		node = (struct treenpda*)currents->elems[i];
		if(node->state == state && stack_equals(&node->stack, &stack))
			return true;
	}
	return false;
}


static void npda_move_and_closure(struct automata* autom, struct tape* tape, struct set* currents, int symb, struct set* result)
{
	int i;
	struct stack stack;
	struct set multi_current_nodes_temp;
	struct stack stack_nodes;
	
	//
	// Si se calcula de cerradura con respecto a epsilon,
	// hacer una copia de los estados, sino, crear un nuevo conjunto
	//
	if(symb != CH_EPSILON)
	{
		set_init(result);
		set_init(&multi_current_nodes_temp);
	}
	else
	{
		set_copy(currents, result);
		set_copy(autom->multi_current_nodes, &multi_current_nodes_temp);
	}
	//
	// Poner los estados corrientes en la pila
	//
	stack_init(&stack);
	for(i = 0; i < currents->size; i++)
	{
		stack_push(&stack, currents->elems[i]);
	}
	//
	// Poner los nodos de los estados corrientes en la pila
	//
	stack_init(&stack_nodes);
	for(i = 0; i < autom->multi_current_nodes->size; i++)
	{
		stack_push(&stack_nodes, autom->multi_current_nodes->elems[i]);
	}
	
	//
	// ...
	//
	while(!stack_empty(&stack_nodes))
	{
		int q = stack_pop(&stack);
		struct treenpda* node = (struct treenpda*)stack_pop(&stack_nodes);
		for(i = 0; i < autom->states[node->state].transitions_count; i++)
		{
			struct automata_transition_pda* t = get_automata_transition(autom, node->state, i, automata_transition_pda);
			if(((t->in == symb) && (t->pop == stack_peek(&node->stack)))
				&& !contains_instance_of_state_and_stack(&multi_current_nodes_temp, node, t))
			{
				struct treenpda* new_node = treenpda_create_node(autom, tape, t, node);

				set_insert(result, t->state);
				set_insert(&multi_current_nodes_temp, (int)new_node);
				if(symb == CH_EPSILON)
				{
					stack_push(&stack, t->state);
					stack_push(&stack_nodes, (int)new_node);
				}
			}
		}
	}
	//
	// ...
	//
	{
		if(symb != CH_EPSILON)
		{
			for(i = 0; i < autom->multi_current_nodes->size; i++)
			{
				struct treenpda* node = (struct treenpda*)autom->multi_current_nodes->elems[i];
				if(!node->first)	// Si no tiene hijos, entonces no hay transiciones, o sea muere
				{
					node->dead = true;
				}
			}
		}
		set_copy(&multi_current_nodes_temp, autom->multi_current_nodes);
	}
}
static void npda_move(struct automata* autom, struct tape* tape, struct set* currents, int symb, struct set* result)
{
	npda_move_and_closure(autom, tape, currents, symb, result);
}
static void npda_epsilon_closure(struct automata* autom, struct tape* tape, struct set* currents, struct set* result)
{
	npda_move_and_closure(autom, tape, currents, CH_EPSILON, result);
}

static void ___npda_reset_automata(struct automata* autom, struct tape* tape)
{
	if(autom->multi_current_root)
	{
		treenpda_delete(autom->multi_current_root);
	}
	autom->multi_current_root = treenpda_create_root(autom, tape, autom->initial_state);

	set_clear(autom->multi_current_nodes);
	set_insert(autom->multi_current_nodes, (int)autom->multi_current_root);
	//
	// ...
	//
	if(autom->final_states)
	{
		struct set start;

		set_init(&start);
		set_insert(&start, autom->initial_state);	// q0 (estado inicial)
		
		set_clear(autom->multi_current_states);
		npda_epsilon_closure(autom, tape, &start, autom->multi_current_states);
	}
}
static bool ___npda_runstep_automata(struct automata* autom, struct tape* tape)
{

	int ch;
	
	ch = tape_read(tape);
	if(ch == CH_NULL)
	{
		autom->halt_code = set_intersects(autom->multi_current_states, autom->final_states) ? HALTCODE_SUCCESS : HALTCODE_ERROR;
	}
	else
	{
		struct set temp;
		tape_move(tape, +1);
		npda_move(autom, tape, autom->multi_current_states, ch, &temp);
		npda_epsilon_closure(autom, tape, &temp, autom->multi_current_states);
		
		if(!set_empty(autom->multi_current_states))
		{
			//tape_move(tape, +1);
		}
		else
		{
			autom->halt_code = set_intersects(autom->multi_current_states, autom->final_states) ? HALTCODE_SUCCESS : HALTCODE_ERROR;
		}
	}
	if(autom->halt_code != HALTCODE_NONE)
	{
		int i;

		for(i = 0; i < autom->multi_current_nodes->size; i++)
		{
			struct treenpda* node = (struct treenpda*)autom->multi_current_nodes->elems[i];
			if(!set_contains(autom->final_states, node->state))
			{
				if(!node->first)
					node->dead = true;
			}
			else
			{
				node->accept = true;
			}
		}
	}
	return (autom->halt_code == HALTCODE_NONE);
}
struct automata_operations automata_operations_for_npda = 
{
	___npda_reset_automata,
	___npda_runstep_automata,
};

