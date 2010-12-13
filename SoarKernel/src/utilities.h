/*************************************************************************
 * PLEASE SEE THE FILE "license.txt" (INCLUDED WITH THIS SOFTWARE PACKAGE)
 * FOR LICENSE AND COPYRIGHT INFORMATION. 
 *************************************************************************/

/* utilities.h */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <list>
#include "stl_support.h"

////////////////////////////////
// Returns a list of wmes that share a specified id
// The tc number of the id is checked against the tc number passed in
// If they match, then NULL is returned
// Otherwise the tc number of the id is set to the specified tc number
// To guarantee any existing wmes will be returned, use get_new_tc_num()
//  for the tc parameter
////////////////////////////////
extern SoarSTLWMEPoolList* get_augs_of_id(agent* thisAgent, Symbol * id, tc_number tc);

/*
*	This procedure parses a string to determine if it is a
*      lexeme for an identifier or context variable.
* 
*      Many interface routines take identifiers as arguments.  
*      These ids can be given as normal ids, or as special variables 
*      such as <s> for the current state, etc.  This routine reads 
*      (without consuming it) an identifier or context variable, 
*      and returns a pointer (Symbol *) to the id.  (In the case of 
*      context variables, the instantiated variable is returned.  If 
*      any error occurs (e.g., no such id, no instantiation of the 
*      variable), an error message is printed and NIL is returned.
*
* Results:
*	Pointer to a symbol for the variable or NIL.
*
* Side effects:
*	None.
*
===============================
*/
extern bool read_id_or_context_var_from_string (agent* agnt, const char * the_lexeme, Symbol * * result_id);
extern void get_lexeme_from_string (agent* agnt, const char * the_lexeme);
extern void get_context_var_info ( agent* agnt, Symbol **dest_goal, Symbol **dest_attr_of_slot, Symbol **dest_current_value);
extern Symbol *read_identifier_or_context_variable (agent* agnt);

#ifdef REAL_TIME_BEHAVIOR
/* RMJ */
extern void init_real_time (agent* thisAgent);
extern struct timeval *current_real_time;
#endif // REAL_TIME_BEHAVIOR

#ifdef ATTENTION_LAPSE
/* RMJ */
extern void wake_from_attention_lapse ();
extern void init_attention_lapse ();
extern void start_attention_lapse (int64_t duration);
#endif // ATTENTION_LAPSE

// formerly in misc.h:
//////////////////////////////////////////////////////////
// String functions
//////////////////////////////////////////////////////////

// Determine if a string represents a natural number (i.e. all numbers)
extern bool is_whole_number(const std::string &str);
extern bool is_whole_number(const char * str);

//////////////////////////////////////////////////////////
// Map functions
//////////////////////////////////////////////////////////

// get a list of all keys of a map
template <class X, class Y> std::vector<X> *map_keys( std::map<X,Y> *my_map )
{
	typename std::vector<X> *return_val = new std::vector<X>();
	typename std::map<X,Y>::iterator b, e;
	
	e = my_map->end();
	
	for ( b = my_map->begin(); b != e; b++ )
		return_val->push_back( b->first );
	
	return return_val;
}

// determine if a key is being used
template <class X, class Y> bool is_set( std::map<X,Y> *my_map, X *key )
{
	return ( my_map->find( *key ) != my_map->end() );
}

//////////////////////////////////////////////////////////
// Misc
//////////////////////////////////////////////////////////

// get a numeric value from a symbol
extern double get_number_from_symbol( Symbol *sym );

//////////////////////////////////////////////////////////
// Statistics database
//////////////////////////////////////////////////////////

class stats_statement_container: public soar_module::sqlite_statement_container
{
	public:
		soar_module::sqlite_statement *insert;

		soar_module::sqlite_statement *cache5;
		soar_module::sqlite_statement *cache20;
		soar_module::sqlite_statement *cache100;

		soar_module::sqlite_statement *sel_dc_inc;
		soar_module::sqlite_statement *sel_dc_dec;
		soar_module::sqlite_statement *sel_time_inc;
		soar_module::sqlite_statement *sel_time_dec;
		soar_module::sqlite_statement *sel_wm_changes_inc;
		soar_module::sqlite_statement *sel_wm_changes_dec;
		soar_module::sqlite_statement *sel_firing_count_inc;
		soar_module::sqlite_statement *sel_firing_count_dec;

		stats_statement_container( agent *new_agent );
};

// Store statistics in to database
extern void stats_db_store(agent* thisAgent, const uint64_t& dc_time, const uint64_t& dc_wm_changes, const uint64_t& dc_firing_counts);

extern void stats_close( agent *my_agent );

// Useful for converting enumerations to string
#define stringify( name ) # name

/* derived_kernel_time := Total of the time spent in the phases of the decision cycle, 
excluding Input Function, Output function, and pre-defined callbacks. 
This computed time should be roughly equal to total_kernel_time, 
as determined above. */
uint64_t get_derived_kernel_time_usec(agent* thisAgent);


#endif //UTILITIES_H