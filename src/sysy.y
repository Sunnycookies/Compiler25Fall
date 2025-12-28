%code requires {
  #include <memory>
  #include <string>
  #include <deque>
  #include "ast.hpp"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include <deque>
#include "ast.hpp"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

%parse-param { std::unique_ptr<BaseAST> &ast }

%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
  std::deque<std::unique_ptr<BaseAST>> *deque_val;
}

%token INT VOID RETURN CONST IF ELSE WHILE BREAK CONTINUE
%token <str_val> IDENT
%token <int_val> INT_CONST
%token <str_val> NOT_OP ADD_OP MUL_OP REL_OP EQ_OP AND_OP OR_OP

%type <ast_val> CompUnit
%type <ast_val> Decl ConstDecl ConstDef ConstInitVal VarDecl VarDef InitVal
%type <ast_val> FuncDef FuncFParam 
%type <ast_val> Block BlockItem Stmt MatchedStmt UnmatchedStmt
%type <ast_val> Exp LVal PrimaryExp Number UnaryExp MulExp AddExp RelExp EqExp LAndExp LOrExp ConstExp
%type <deque_val> MoreCompUnit MoreConstDef MoreVarDef
%type <deque_val> AnyBlockItem AnyConstIndex AnyIndex AnyConstInitVal AnyInitVal FuncFParams FuncRParams

%%

Program
  : CompUnit MoreCompUnit {
    auto program = make_unique<ProgramAST>();
    deque<unique_ptr<BaseAST>> *comp_units = $2;
    comp_units->push_front(unique_ptr<BaseAST>($1));
    program->comp_units = move(*comp_units);
    ast = move(program);
  }
  ;

MoreCompUnit
  : CompUnit MoreCompUnit {
    deque<unique_ptr<BaseAST>> *comp_units = $2;
    comp_units->push_front(unique_ptr<BaseAST>($1));
    $$ = comp_units;
  }
  | {
    $$ = new deque<unique_ptr<BaseAST>>;
  }
  ;

CompUnit
  : FuncDef {
    auto ast = new CompUnitAST();
    ast->type = CompUnitAST::FUNC;
    ast->func_def_or_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | Decl {
    auto ast = new CompUnitAST();
    ast->type = CompUnitAST::DECL;
    ast->func_def_or_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

Decl
  : ConstDecl {
    auto ast = new DeclAST();
    ast->const_or_var_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | VarDecl {
    auto ast = new DeclAST();
    ast->const_or_var_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

ConstDecl
  : CONST INT ConstDef MoreConstDef ';' {
    auto ast = new ConstDeclAST;
    ast->type = BType::INT;
    deque<unique_ptr<BaseAST>> *const_defs = $4;
    const_defs->push_front(unique_ptr<BaseAST>($3));
    ast->const_defs = move(*const_defs);
    $$ = ast;
  }
  ;

ConstDef
  : IDENT AnyConstIndex '=' ConstInitVal {
    auto ast = new ConstDefAST();
    ast->ident = *unique_ptr<string>($1);
    deque<unique_ptr<BaseAST>> *array_sizes = $2;
    ast->array_sizes = move(*array_sizes);
    ast->const_init_val = unique_ptr<BaseAST>($4);
    $$ = ast;
  }
  ;

MoreConstDef
  : ',' ConstDef MoreConstDef {
    deque<unique_ptr<BaseAST>> *const_defs = $3;
    const_defs->push_front(unique_ptr<BaseAST>($2));
    $$ = const_defs;
  }
  | {
    $$ = new deque<unique_ptr<BaseAST>>;
  }
  ;

ConstInitVal
  : ConstExp {
    auto ast = new ConstInitValAST();
    ast->type = ConstInitValAST::CONST_EXP;
    ast->const_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | '{' AnyConstInitVal '}' {
    auto ast = new ConstInitValAST();
    ast->type = ConstInitValAST::INITIALIZER;
    deque<unique_ptr<BaseAST>> *const_init_vals = $2;
    ast->const_init_vals = move(*const_init_vals);
    $$ = ast;
  }
  ;

AnyConstInitVal
  : {
    $$ = new deque<unique_ptr<BaseAST>>;
  }
  | ConstInitVal {
    deque<unique_ptr<BaseAST>> *const_init_vals = new deque<unique_ptr<BaseAST>>;
    const_init_vals->push_front(unique_ptr<BaseAST>($1));
    $$ = const_init_vals;
  }
  | ConstInitVal ',' AnyConstInitVal {
    deque<unique_ptr<BaseAST>> *const_init_vals = $3;
    const_init_vals->push_front(unique_ptr<BaseAST>($1));
    $$ = const_init_vals;
  }
  ;

VarDecl
  : INT VarDef MoreVarDef ';' {
    auto ast = new VarDeclAST();
    ast->b_type = BType::INT;
    deque<unique_ptr<BaseAST>> *var_defs = $3;
    var_defs->push_front(unique_ptr<BaseAST>($2));
    ast->var_defs = move(*var_defs);
    $$ = ast;
  }
  ;

VarDef
  : IDENT AnyConstIndex {
    auto ast = new VarDefAST();
    ast->type = VarDefAST::IDENT;
    ast->ident = *unique_ptr<string>($1);
    deque<unique_ptr<BaseAST>> *array_sizes = $2;
    ast->array_sizes = move(*array_sizes);
    $$ = ast;
  }
  | IDENT AnyConstIndex '=' InitVal {
    auto ast = new VarDefAST();
    ast->type = VarDefAST::INITVAL;
    ast->ident = *unique_ptr<string>($1);
    deque<unique_ptr<BaseAST>> *array_sizes = $2;
    ast->array_sizes = move(*array_sizes);
    ast->init_val = unique_ptr<BaseAST>($4);
    $$ = ast;
  }
  ;

MoreVarDef
  : ',' VarDef MoreVarDef {
    deque<unique_ptr<BaseAST>> *var_defs = $3;
    var_defs->push_front(unique_ptr<BaseAST>($2));
    $$ = var_defs;
  }
  | {
    $$ = new deque<unique_ptr<BaseAST>>;
  }
  ;

InitVal
  : Exp {
    auto ast = new InitValAST();
    ast->type = InitValAST::EXP;
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | '{' AnyInitVal '}' {
    auto ast = new InitValAST();
    ast->type = InitValAST::INITIALIZER;
    deque<unique_ptr<BaseAST>> *init_vals = $2;
    ast->init_vals = move(*init_vals);
    $$ = ast;
  }
  ;

AnyInitVal
  : {
    $$ = new deque<unique_ptr<BaseAST>>;
  }
  | InitVal {
    deque<unique_ptr<BaseAST>> *init_vals = new deque<unique_ptr<BaseAST>>;
    init_vals->push_front(unique_ptr<BaseAST>($1));
    $$ = init_vals;
  }
  | InitVal ',' AnyInitVal {
    deque<unique_ptr<BaseAST>> *init_vals = $3;
    init_vals->push_front(unique_ptr<BaseAST>($1));
    $$ = init_vals;
  }
  ;

AnyConstIndex
  : {
    $$ = new deque<unique_ptr<BaseAST>>;
  }
  | '[' ConstExp ']' AnyConstIndex {
    deque<unique_ptr<BaseAST>> *array_sizes = $4;
    array_sizes->push_front(unique_ptr<BaseAST>($2));
    $$ = array_sizes;
  }
  ;

FuncDef
  : INT IDENT '(' FuncFParams ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = BType::INT;
    ast->ident = *unique_ptr<string>($2);
    deque<unique_ptr<BaseAST>> *params = $4;
    ast->params = move(*params);
    ast->block = unique_ptr<BaseAST>($6);
    $$ = ast;
  }
  | VOID IDENT '(' FuncFParams ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = BType::VOID;
    ast->ident = *unique_ptr<string>($2);
    deque<unique_ptr<BaseAST>> *params = $4;
    ast->params = move(*params);
    ast->block = unique_ptr<BaseAST>($6);
    $$ = ast;
  }
  ;

FuncFParams
  : FuncFParam ',' FuncFParams {
    deque<unique_ptr<BaseAST>> *params = $3;
    params->push_front(unique_ptr<BaseAST>($1));
    $$ = params;
  }
  | FuncFParam {
    deque<unique_ptr<BaseAST>> *fparam = new deque<unique_ptr<BaseAST>>;
    fparam->push_front(unique_ptr<BaseAST>($1));
    $$ = fparam;
  }
  | {
    $$ = new deque<unique_ptr<BaseAST>>;
  }
  ;

FuncFParam
  : INT IDENT {
    auto ast = new FuncFParamAST();
    ast->type = BType::INT;
    ast->ident = *unique_ptr<string>($2);
    $$ = ast;
  }
  ;

Block
  : '{' AnyBlockItem '}' {
    auto ast = new BlockAST();
    ast->block_items = move(*$2);
    $$ = ast;
  }
  ;

BlockItem
  : Decl {
    auto ast = new BlockItemAST();
    ast->decl_or_stmt = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | Stmt {
    auto ast = new BlockItemAST();
    ast->decl_or_stmt = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

AnyBlockItem
  : BlockItem AnyBlockItem {
    deque<unique_ptr<BaseAST>> *block_items = $2;
    block_items->push_front(unique_ptr<BaseAST>($1));
    $$ = block_items;
  }
  | {
    $$ = new deque<unique_ptr<BaseAST>>;
  }
  ;

Stmt
  : UnmatchedStmt {
    $$ = $1;
  }
  | MatchedStmt {
    $$ = $1;
  }
  ;

UnmatchedStmt
  : IF '(' Exp ')' Stmt {
    auto ast = new StmtAST();
    ast->type = StmtAST::IF;
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  | IF '(' Exp ')' MatchedStmt ELSE Stmt {
    auto ast = new StmtAST();
    ast->type = StmtAST::IF;
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt = unique_ptr<BaseAST>($5);
    ast->else_stmt = unique_ptr<BaseAST>($7);
    $$ = ast;
  }
  | WHILE '(' Exp ')' UnmatchedStmt {
    auto ast = new StmtAST();
    ast->type = StmtAST::WHILE;
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

MatchedStmt
  : RETURN Exp ';' {
    auto ast = new StmtAST();
    ast->type = StmtAST::RETURN;
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | LVal '=' Exp ';' {
    auto ast = new StmtAST();
    ast->type = StmtAST::LVAL;
    ast->lval_or_block = unique_ptr<BaseAST>($1);
    ast->exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RETURN ';' {
    auto ast = new StmtAST();
    ast->type = StmtAST::RETURN;
    $$ = ast;
  }
  | Exp ';' {
    auto ast = new StmtAST();
    ast->type = StmtAST::EXP;
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | ';' {
    auto ast = new StmtAST();
    ast->type = StmtAST::EXP;
    $$ = ast;
  }
  | Block {
    auto ast = new StmtAST();
    ast->type = StmtAST::BLOCK;
    ast->lval_or_block = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | WHILE '(' Exp ')' MatchedStmt {
    auto ast = new StmtAST();
    ast->type = StmtAST::WHILE;
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  | BREAK ';' {
    auto ast = new StmtAST();
    ast->type = StmtAST::BREAK;
    $$ = ast;
  }
  | CONTINUE ';' {
    auto ast = new StmtAST();
    ast->type = StmtAST::CONTINUE;
    $$ = ast;
  }
  | IF '(' Exp ')' MatchedStmt ELSE MatchedStmt {
    auto ast = new StmtAST();
    ast->type = StmtAST::IF;
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt = unique_ptr<BaseAST>($5);
    ast->else_stmt = unique_ptr<BaseAST>($7);
    $$ = ast;
  }
  ;

Exp
  : LOrExp {
    auto ast = new ExpAST();
    ast->lor_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

LVal
  : IDENT AnyIndex {
    auto ast = new LValAST();
    ast->ident = *unique_ptr<string>($1);
    deque<unique_ptr<BaseAST>> *array_indices = $2;
    ast->array_indices = move(*array_indices);
    $$ = ast;
  }
  ;

AnyIndex
  : {
    $$ = new deque<unique_ptr<BaseAST>>;
  }
  | '[' Exp ']' AnyIndex {
    deque<unique_ptr<BaseAST>> *array_indices = $4;
    array_indices->push_front(unique_ptr<BaseAST>($2));
    $$ = array_indices;
  }
  ;

PrimaryExp
  : '(' Exp ')' {
    auto ast = new PrimaryExpAST();
    ast->exp_or_lval_or_number = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | Number {
    auto ast = new PrimaryExpAST();
    ast->exp_or_lval_or_number = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LVal {
    auto ast = new PrimaryExpAST();
    ast->exp_or_lval_or_number = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

Number
  : INT_CONST {
    auto ast = new NumberAST();
    ast->number = $1;
    $$ = ast;
  }
  ;

UnaryExp
  : PrimaryExp {
    auto ast = new UnaryExpAST();
    ast->type = UnaryExpAST::PRIMARY_EXP;
    ast->primary_or_unary_exp= unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | NOT_OP UnaryExp {
    auto ast = new UnaryExpAST();
    ast->type = UnaryExpAST::UNARY_EXP;
    ast->op_or_func = *unique_ptr<string>($1);
    ast->primary_or_unary_exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | ADD_OP UnaryExp {
    auto ast = new UnaryExpAST();
    ast->type = UnaryExpAST::UNARY_EXP;
    ast->op_or_func = *unique_ptr<string>($1);
    ast->primary_or_unary_exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | IDENT '(' FuncRParams ')' {
    auto ast = new UnaryExpAST();
    ast->type = UnaryExpAST::CALL_FUNC;
    ast->op_or_func = *unique_ptr<string>($1);
    deque<unique_ptr<BaseAST>> *params = $3;
    ast->params = move(*params);
    $$ = ast;
  }
  ;

FuncRParams
  : Exp ',' FuncRParams {
    deque<unique_ptr<BaseAST>> *exps = $3;
    exps->push_front(unique_ptr<BaseAST>($1));
    $$ = exps;
  }
  | Exp {
    deque<unique_ptr<BaseAST>> *exps = new deque<unique_ptr<BaseAST>>;
    exps->push_front(unique_ptr<BaseAST>($1));
    $$ = exps;
  }
  | {
    $$ = new deque<unique_ptr<BaseAST>>;
  }
  ;

MulExp
  : UnaryExp {
    auto ast = new MulExpAST();
    ast->type = MulExpAST::UNARY_EXP;
    ast->unary_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | MulExp MUL_OP UnaryExp {
    auto ast = new MulExpAST();
    ast->type = MulExpAST::BINARY_EXP;
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->mul_op = *unique_ptr<string>($2);
    ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

AddExp
  : MulExp {
    auto ast = new AddExpAST();
    ast->type = AddExpAST::MUL_EXP;
    ast->mul_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | AddExp ADD_OP MulExp {
    auto ast = new AddExpAST();
    ast->type = AddExpAST::BINARY_EXP;
    ast->add_exp = unique_ptr<BaseAST>($1);
    ast->add_op = *unique_ptr<string>($2);
    ast->mul_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

RelExp
  : AddExp {
    auto ast = new RelExpAST();
    ast->type = RelExpAST::ADD_EXP;
    ast->add_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | RelExp REL_OP AddExp {
    auto ast = new RelExpAST();
    ast->type = RelExpAST::BINARY_EXP;
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->rel_op = *unique_ptr<string>($2);
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

EqExp
  : RelExp {
    auto ast = new EqExpAST();
    ast->type = EqExpAST::REL_EXP;
    ast->rel_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | EqExp EQ_OP RelExp {
    auto ast = new EqExpAST();
    ast->type = EqExpAST::BINARY_EXP;
    ast->eq_exp = unique_ptr<BaseAST>($1);
    ast->eq_op = *unique_ptr<string>($2);
    ast->rel_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

LAndExp
  : EqExp {
    auto ast = new LAndExpAST();
    ast->type = LAndExpAST::EQ_EXP;
    ast->eq_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LAndExp AND_OP EqExp {
    auto ast = new LAndExpAST();
    ast->type = LAndExpAST::BINARY_EXP;
    ast->land_exp = unique_ptr<BaseAST>($1);
    ast->land_op = *unique_ptr<string>($2);
    ast->eq_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

LOrExp
  : LAndExp {
    auto ast = new LOrExpAST();
    ast->type = LOrExpAST::LAND_EXP;
    ast->land_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LOrExp OR_OP LAndExp {
    auto ast = new LOrExpAST();
    ast->type = LOrExpAST::BINARY_EXP;
    ast->lor_exp = unique_ptr<BaseAST>($1);
    ast->lor_op = *unique_ptr<string>($2);
    ast->land_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

ConstExp
  : Exp {
    auto ast = new ConstExpAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

%%

void yyerror(std::unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
