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

%token INT RETURN CONST
%token <str_val> IDENT
%token <int_val> INT_CONST
%token <str_val> NOT_OP ADD_OP MUL_OP REL_OP EQ_OP AND_OP OR_OP

%type <ast_val> FuncDef FuncType Block BlockItem Stmt 
%type <ast_val> Exp Number LVal PrimaryExp UnaryExp MulExp AddExp RelExp EqExp LAndExp LOrExp ConstExp
%type <ast_val> Decl ConstDecl BType ConstDef ConstInitVal VarDecl VarDef InitVal
%type <deque_val> AnyBlockItem MoreConstDef MoreVarDef

%%

CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

FuncType
  : INT {
    auto ast = new FuncTypeAST();
    ast->type = "int";
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
  : RETURN Exp ';' {
    auto ast = new StmtAST();
    ast->type = StmtAST::RETURN;
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | LVal '=' Exp ';' {
    auto ast = new StmtAST();
    ast->type = StmtAST::LVAL;
    ast->lval = unique_ptr<BaseAST>($1);
    ast->exp = unique_ptr<BaseAST>($3);
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

Number
  : INT_CONST {
    auto ast = new NumberAST();
    ast->number = $1;
    $$ = ast;
  }
  ;

LVal
  : IDENT {
    auto ast = new LValAST();
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
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
    ast->unary_op = *unique_ptr<string>($1);
    ast->primary_or_unary_exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | ADD_OP UnaryExp {
    auto ast = new UnaryExpAST();
    ast->type = UnaryExpAST::UNARY_EXP;
    ast->unary_op = *unique_ptr<string>($1);
    ast->primary_or_unary_exp = unique_ptr<BaseAST>($2);
    $$ = ast;
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
  : CONST BType ConstDef MoreConstDef ';' {
    auto ast = new ConstDeclAST;
    ast->b_type = unique_ptr<BaseAST>($2);
    deque<unique_ptr<BaseAST>> *const_defs = $4;
    const_defs->push_front(unique_ptr<BaseAST>($3));
    ast->const_defs = move(*const_defs);
    $$ = ast;
  }

BType
  : INT {
    auto ast = new BTypeAST();
    ast->type = "int";
    $$ = ast;
  }
  ;

ConstDef
  : IDENT '=' ConstInitVal {
    auto ast = new ConstDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->const_init_val = unique_ptr<BaseAST>($3);
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
    ast->const_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

VarDecl
  : BType VarDef MoreVarDef ';' {
    auto ast = new VarDeclAST();
    ast->b_type = unique_ptr<BaseAST>($1);
    deque<unique_ptr<BaseAST>> *var_defs = $3;
    var_defs->push_front(unique_ptr<BaseAST>($2));
    ast->var_defs = move(*var_defs);
    $$ = ast;
  }
  ;

VarDef
  : IDENT {
    auto ast = new VarDefAST();
    ast->type = VarDefAST::IDENT;
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
  }
  | IDENT '=' InitVal {
    auto ast = new VarDefAST();
    ast->type = VarDefAST::INITVAL;
    ast->ident = *unique_ptr<string>($1);
    ast->init_val = unique_ptr<BaseAST>($3);
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
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

%%

void yyerror(std::unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
