
typedef struct row_of_memory_struct {
	short unsigned int address ;
	char * label ;
	short unsigned int contents ;
	char * assembly ;
	struct row_of_memory_struct *next ;
} row_of_memory ;


int add_to_list    	      (row_of_memory** head,
			       short unsigned int address,
			       short unsigned int contents) ;

row_of_memory* search_address (row_of_memory* head,
			       short unsigned int address ) ;

row_of_memory* search_opcode  (row_of_memory* head,
			       short unsigned int opcode  ) ;

void print_list		      (row_of_memory* head ) ;

int delete_list    	      (row_of_memory** head ) ;



#endif
