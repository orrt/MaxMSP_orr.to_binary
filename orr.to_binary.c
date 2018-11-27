/**
 
    created by tomorr (mail@tomorr.info)
 
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

#define MAX_BIN_DIGITS 32

////////////////////////// object struct
typedef struct _to_binary
{
	t_object					ob;			// the object itself (must be first)
    t_atom                      binary_val[MAX_BIN_DIGITS]; // store the last binary value
    long                        bit_count;  // how many bits are output (8, 16, 24 or 32)
    void                        *m_outlet;  // pointer to out outlet
} t_to_binary;

///////////////////////// function prototypes
//// standard set
void *to_binary_new(t_symbol *s, long argc, t_atom *argv);

void rcv_int(t_to_binary *x, long n); // when object reveices an int
void rcv_bang(t_to_binary *x); // when object receives a bang
void to_binary(t_atom *at_array, long len, long dec_val); // convert number to binary and store

void to_binary_free(t_to_binary *x);
void to_binary_assist(t_to_binary *x, void *b, long m, long a, char *s);

//////////////////////// global class pointer variable
void *to_binary_class;


//////////////////////// INITIALISATION Routine

void ext_main(void *r)
{
	t_class *c;

	c = class_new("orr.to_binary", (method)to_binary_new, (method)to_binary_free, (long)sizeof(t_to_binary),
				  0L /* leave NULL!! */, A_GIMME, 0);
    
    ////// class_addmethod functions here
    class_addmethod(c, (method)rcv_int, "int", A_LONG, 0);
    class_addmethod(c, (method)rcv_bang, "bang", 0);
    
	/* you CAN'T call this from the patcher */
	class_addmethod(c, (method)to_binary_assist,			"assist",		A_CANT, 0);

	class_register(CLASS_BOX, c); /* CLASS_NOBOX */
	to_binary_class = c;
}



//////////////////////// ASSIST stuff

void to_binary_assist(t_to_binary *x, void *b, long m, long a, char *s)
{
//	if (m == ASSIST_INLET) { // inlet
//		sprintf(s, "I am inlet %ld", a);
//	}
//	else {	// outlet
//		sprintf(s, "I am outlet %ld", a);
//	}
}

void to_binary_free(t_to_binary *x)
{
	;
}


//////////////////////// NEW INSTANCE routine

void *to_binary_new(t_symbol *s, long argc, t_atom *argv)
{
	t_to_binary *x = NULL;
    
    // argument validation
    t_atom_long bit_cnt = -1;
    
    // if no args default is 32
    if (argc == 0) bit_cnt = 32;
    else {
        atom_arg_getlong(&bit_cnt, 0, argc, argv);
        if(bit_cnt == 8 || bit_cnt == 16 || bit_cnt == 24 || bit_cnt == 32){
            ;
        } else {
            bit_cnt = -1;
        }
    }
    
    if(bit_cnt > 0){
        x = (t_to_binary *)object_alloc(to_binary_class);
        x->bit_count = bit_cnt;
        x->m_outlet = outlet_new((t_object *)x, NULL);
        
        to_binary(x->binary_val, MAX_BIN_DIGITS, 0); // set all bits to zero
        
    } else {
        object_post(&x->ob, "invalid arguments.");
    }
    return (x);
}

//////////////////////// CUSTOM FUNCTIONS here

void rcv_int(t_to_binary *x, long val){
    
    // do conversion and write to this objects binary_val array
    to_binary(x->binary_val, x->bit_count, val);
    
    // call rcv_bang to output result
    rcv_bang(x);
    
}


void rcv_bang(t_to_binary *x){
    // send the message as a list to the outlet
    outlet_list(x->m_outlet, NIL, x->bit_count, x->binary_val);
}


void to_binary(t_atom *at_array, long len, long dec_val){
    
    for (int i = 0; i < len; i++){
        atom_setlong(at_array + (len - i - 1), (long)((dec_val >> i) & 1) );
    }

}


