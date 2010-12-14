#include <portability.h>

/*************************************************************************
 * PLEASE SEE THE FILE "license.txt" (INCLUDED WITH THIS SOFTWARE PACKAGE)
 * FOR LICENSE AND COPYRIGHT INFORMATION.
 *************************************************************************/

/*************************************************************************
 *
 *  file:  soar_module.cpp
 *
 * =======================================================================
 * Description  :  Useful functions for Soar modules
 * =======================================================================
 */

#include "soar_module.h"

#include "agent.h"
#include "gdatastructs.h"
#include "instantiations.h"
#include "tempmem.h"
#include "prefmem.h"
#include "mem.h"
#include "print.h"
#include "decide.h"
#include "xml.h"
#include "wmem.h"
#include "agent.h"
#include "soar_TraceNames.h"
#include "wma.h"

wme *make_wme (agent* thisAgent, Symbol *id, Symbol *attr, Symbol *value, Bool acceptable);
typedef struct agent_struct agent;

namespace soar_module
{
	timer::timer( const char *new_name, agent *new_agent, timer_level new_level, predicate<timer_level> *new_pred ): named_object( new_name ), my_agent( new_agent ), level( new_level ), pred( new_pred )
	{
		stopwatch.set_enabled( &( new_agent->sysparams[ TIMERS_ENABLED ] ) );
		reset();
	}
	
	/////////////////////////////////////////////////////////////
	// Utility functions
	/////////////////////////////////////////////////////////////

	wme *add_module_wme( agent *my_agent, Symbol *id, Symbol *attr, Symbol *value )
	{
		slot *my_slot = make_slot( my_agent, id, attr );
		wme *w = make_wme( my_agent, id, attr, value, false );

		insert_at_head_of_dll( my_slot->wmes, w, next, prev );
		add_wme_to_wm( my_agent, w );

		return w;
	}

	void remove_module_wme( agent *my_agent, wme *w )
	{
		slot *my_slot = find_slot( w->id, w->attr );

		if ( my_slot )
		{
			remove_from_dll( my_slot->wmes, w, next, prev );

			if ( w->gds ) 
			{
				if ( w->gds->goal != NIL )
				{	             
					gds_invalid_so_remove_goal( my_agent, w );
					
					/* NOTE: the call to remove_wme_from_wm will take care of checking if GDS should be removed */
				}
			}

			remove_wme_from_wm( my_agent, w );
		}
	}

	preference *make_fake_preference( agent *my_agent, Symbol *state, Symbol *id, Symbol *attr, Symbol *value, wme_set *conditions )
	{
		// make fake preference
		preference *pref = make_preference( my_agent, ACCEPTABLE_PREFERENCE_TYPE, id, attr, value, NIL );
		pref->o_supported = true;
		symbol_add_ref( pref->id );
		symbol_add_ref( pref->attr );
		symbol_add_ref( pref->value );

		// make fake instantiation
		instantiation *inst;
		allocate_with_pool( my_agent, &( my_agent->instantiation_pool ), &inst );
		pref->inst = inst;
		pref->inst_next = pref->inst_prev = NULL;
		inst->preferences_generated = pref;
		inst->prod = NULL;
		inst->next = inst->prev = NULL;
		inst->rete_token = NULL;
		inst->rete_wme = NULL;
		inst->match_goal = state;
		inst->match_goal_level = state->id.level;
		inst->reliable = true;
		inst->backtrace_number = 0;
		inst->in_ms = FALSE;
		inst->GDS_evaluated_already = FALSE;
		
		condition *cond = NULL;
		condition *prev_cond = NULL;	
		{
			wme_set::iterator p = conditions->begin();

			while ( p != conditions->end() )
			{
				// construct the condition
				allocate_with_pool( my_agent, &( my_agent->condition_pool ), &cond );
				cond->type = POSITIVE_CONDITION;
				cond->prev = prev_cond;
				cond->next = NULL;
				if ( prev_cond != NULL )
				{
					prev_cond->next = cond;
				}
				else
				{
					inst->top_of_instantiated_conditions = cond;
					inst->bottom_of_instantiated_conditions = cond;
					inst->nots = NULL;
				}
				cond->data.tests.id_test = make_equality_test( (*p)->id );
				cond->data.tests.attr_test = make_equality_test( (*p)->attr );
				cond->data.tests.value_test = make_equality_test( (*p)->value );
				cond->test_for_acceptable_preference = (*p)->acceptable;
				cond->bt.wme_ = (*p);

				#ifndef DO_TOP_LEVEL_REF_CTS
				if ( inst->match_goal_level > TOP_GOAL_LEVEL )
				#endif
				{
					wme_add_ref( (*p) );
				}			
				
				cond->bt.level = (*p)->id->id.level;
				cond->bt.trace = (*p)->preference;
				
				if ( cond->bt.trace )
				{
					#ifndef DO_TOP_LEVEL_REF_CTS
					if ( inst->match_goal_level > TOP_GOAL_LEVEL )
					#endif
					{
						preference_add_ref( cond->bt.trace );
					}
				}				

				cond->bt.prohibits = NULL;

				prev_cond = cond;

				p++;
			}
		}

		return pref;
	}
}
