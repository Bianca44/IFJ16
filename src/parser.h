#ifndef PARSER_H
#define PARSER_H

int parse_expression();
int parse_param_expression();
int parse_return_value();
int parse_next_param_value();
int parse_param_value ();
int parse_call_assign();
int parse_condition_list();
int parse_else();
int parse_statement();
int parse_element_list();
int parse_statement_list();
int parse_method_element();
int parse_next_param();
int parse_param_list();
int parse_method_declaration ();
int parse_value();
int parse_declaration();
int parse_param();
int parse_declaration_element();
int parse_class_element();
int parse_class_list();
int parse(FILE *source);

void add_builtin_functions();

#endif // PARSER_H
