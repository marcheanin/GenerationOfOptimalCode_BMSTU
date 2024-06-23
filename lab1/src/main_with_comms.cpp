#include <iostream>
#include <sstream>
#include <string>

// Заголовочные файлы GCC для работы с плагинами.
#include <gcc-plugin.h>
#include <plugin-version.h>

// Заголовочные файлы GCC для работы с типами данных.
#include <coretypes.h>

// Заголовочные файлы GCC для работы с проходами компилятора.
#include <tree-pass.h>
#include <context.h>
#include <basic-block.h>

// Заголовочные файлы GCC для работы с деревьями и GIMPLE.
#include <tree.h>
#include <tree-ssa-alias.h>
#include <gimple-expr.h>
#include <gimple.h>
#include <gimple-ssa.h>
#include <tree-phinodes.h>
#include <tree-ssa-operands.h>

// Заголовочные файлы GCC для работы с итераторами.
#include <ssa-iterators.h>
#include <gimple-iterator.h>

// Макрос, который игнорирует значение переменной, чтобы избежать предупреждений компилятора.
#define PREFIX_UNUSED(variable) ((void)variable)

// Флаг, указывающий, что плагин совместим с лицензией GPL.
int plugin_is_GPL_compatible = 1;

// Имя плагина.
#define PLUGIN_NAME    "name"

// Структура, описывающая данные для прохода GIMPLE.
static const struct pass_data lab1_pass_data = {
    .type =          GIMPLE_PASS, // Тип прохода: GIMPLE
    .name =          PLUGIN_NAME, // Имя прохода: "name"
};

// Структура, представляющая наш проход.
struct lab1_pass : gimple_opt_pass {
    // Конструктор: инициализирует базовый класс gimple_opt_pass.
    lab1_pass(gcc::context*ctx) : gimple_opt_pass(lab1_pass_data, ctx) {}

    // Виртуальный метод, вызываемый при выполнении прохода.
    virtual unsigned int execute(function*fun) override;

    // Виртуальный метод, возвращающий клон текущего объекта.
    virtual lab1_pass *clone() override { return this; }
};

// Функция, выводящая информацию о базовом блоке.
static unsigned int lab1_bb_id(basic_block bb)
{
    std::cout << "\t" << "bb: ";

    edge e;
    edge_iterator it;

    std::cout << "(";
    std::stringstream src_stream;
    // Перебираем предшественники базового блока.
    FOR_EACH_EDGE(e, it, bb->preds) {
        // Выводим индекс предшественника.
        src_stream << e->src->index << ", ";
    }
    // Формируем строку с индексами предшественников.
    std::string src = src_stream.str();
    // Удаляем лишний пробел и запятую в конце строки.
    std::cout << src.substr(0, src.size() - 2);
    std::cout << ")";

    // Выводим индекс текущего блока.
    std::cout << " -> " << "(" << bb->index << ")";

    // Выводим индекс последователей блока.
    std::cout << " -> ";
    std::cout << "(";
    std::stringstream dst_stream;
    FOR_EACH_EDGE(e, it, bb->succs) {
        dst_stream << e->dest->index << ", ";
    }
    std::string dst = dst_stream.str();
    std::cout << dst.substr(0, dst.size() - 2);
    std::cout << ")";

    return 0;
}

// Функция, выводящая информацию о дереве.
static unsigned int lab1_tree(tree t)
{   
    switch (TREE_CODE(t)) {
        // Обработка констант.
    case INTEGER_CST:
        // Выводим значение целочисленной константы.
        std::cout << TREE_INT_CST_LOW(t);
        break;
    case STRING_CST:
        // Выводим значение строковой константы.
        std::cout << "\"" << TREE_STRING_POINTER(t) << "\"";
        break;
        // Обработка объявлений.
    case LABEL_DECL: 
        // Выводим имя метки, если оно есть, иначе "label_decl".
        std::cout << (DECL_NAME(t) ? IDENTIFIER_POINTER(DECL_NAME(t)) : "label_decl") << ":";
        break;
    case VAR_DECL:
        // Выводим имя переменной, если оно есть, иначе "var_decl".
        std::cout << (DECL_NAME(t) ? IDENTIFIER_POINTER(DECL_NAME(t)) : "var_decl");
        break;
    case CONST_DECL:
        // Выводим имя константы, если оно есть, иначе "const_decl".
        std::cout << (DECL_NAME(t) ? IDENTIFIER_POINTER(DECL_NAME(t)) : "const_decl");
        break;
        // Обработка ссылок на память.
    case ARRAY_REF:
        // Выводим информацию о массиве.
        lab1_tree(TREE_OPERAND(t, 0));
        std::cout << "[";
        lab1_tree(TREE_OPERAND(t, 1));
        std::cout << "]";
        break;
    
    case MEM_REF:
        // Выводим информацию о ссылке на память.
        std::cout << "((typeof(";
        lab1_tree(TREE_OPERAND(t, 1));
        std::cout << "))";
        lab1_tree(TREE_OPERAND(t, 0));
        std::cout << ")";
        break;
        
    case SSA_NAME: {
        // Получаем оператор, где SSA_NAME был определен.
        gimple* st = SSA_NAME_DEF_STMT(t);
        // Проверяем, является ли оператор GIMPLE_PHI.
        if (gimple_code(st) == GIMPLE_PHI) {
            // Выводим имя SSA_NAME и его версию.
            std::cout << "(" << (SSA_NAME_IDENTIFIER(t) ? IDENTIFIER_POINTER(SSA_NAME_IDENTIFIER(t)) : "ssa_name") <<
            "__v" << SSA_NAME_VERSION(t);
            std::cout << " = GIMPLE_PHI(";
            // Выводим информацию о аргументах GIMPLE_PHI.
            for (unsigned int i = 0; i < gimple_phi_num_args(st); i++) {
                lab1_tree(gimple_phi_arg(st, i)->def);
                if (i != gimple_phi_num_args(st) - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "))";
        } else {
            // Выводим имя SSA_NAME и его версию.
            std::cout << (SSA_NAME_IDENTIFIER(t) ? IDENTIFIER_POINTER(SSA_NAME_IDENTIFIER(t)) : "ssa_name") <<
                "__v" << SSA_NAME_VERSION(t);
        }
        
        break;
    }
    default:
        // Выводим  код дерева, если он не известен.
        std::cout << "tree_code(" << TREE_CODE(t) << ")";
        break;
    }

    return 0;
}


// Функция, выводящая информацию об операторе дерева.
static unsigned int lab1_op(enum tree_code code)
{
    switch (code) {
    case PLUS_EXPR:
        std::cout << "+";
        break;
    case MINUS_EXPR:
        std::cout << "-";
        break;
    case MULT_EXPR:
        std::cout << "*";
        break;
    case RDIV_EXPR:
        std::cout << "/";
        break;
    case BIT_IOR_EXPR:
        std::cout << "|";
        break;
    case BIT_NOT_EXPR:
        std::cout << "!";
        break;
    case TRUTH_AND_EXPR:
        std::cout << "&&";
        break;
    case TRUTH_OR_EXPR:
        std::cout << "||";
        break;
    case TRUTH_NOT_EXPR:
        std::cout << "!";
        break;
        // Операторы отношений.
    case LT_EXPR:
        std::cout << "<";
        break;
    case LE_EXPR:
        std::cout << "<=";
        break;
    case GT_EXPR:
        std::cout << ">";
        break;
    case GE_EXPR:
        std::cout << ">=";
        break;
    case EQ_EXPR:
        std::cout << "==";
        break;
    case NE_EXPR:
        std::cout << "!=";
        break;
        // Неизвестные операторы.
    default:
        std::cout << "unknown op(" << code << ")";
        break;
    }
    return 0;
}


// Функция, выводящая информацию о GIMPLE_ASSIGN выражении.
static unsigned int lab1_on_gimple_assign(gimple* stmt)
{
    std::cout << "\t\t" << "stmt: " << "\"GIMPLE_ASSIGN\"" << " " << "(" << GIMPLE_ASSIGN << ")" << " { ";
    // Проверяем количество операндов в выражении.
    switch (gimple_num_ops(stmt)) {
    case 2:
        // Выводим информацию 
        // о левой части выражения.
        lab1_tree(gimple_assign_lhs(stmt));
        std::cout << " = ";
        //  о правой части выражения.
        lab1_tree(gimple_assign_rhs1(stmt));
        break;
    case 3:
        //  о левой части выражения.
        lab1_tree(gimple_assign_lhs(stmt));
        std::cout << " = ";
        //  о первой правой части выражения.
        lab1_tree(gimple_assign_rhs1(stmt));
        std::cout << " ";
        //  об операторе между.
        lab1_op(gimple_assign_rhs_code(stmt));
        std::cout << " ";
        //  о второй правой части выражения.
        lab1_tree(gimple_assign_rhs2(stmt));
        break;
    }
    std::cout << " }" << std::endl;
    
    return 0;
}


// Функция, выводящая информацию о GIMPLE_CALL выражении.
static unsigned int lab1_on_gimple_call(gimple* stmt)
{
    std::cout << "\t\t" << "stmt: " << "\"GIMPLE_CALL\"" << " " << "(" << GIMPLE_CALL << ")" << " { ";
    tree lhs = gimple_call_lhs (stmt);
    if (lhs) {
        lab1_tree(lhs);
        printf(" = ");
    }
    // Выводим информацию о вызываемой функции.
    std::cout << fndecl_name(gimple_call_fndecl(stmt)) << "(";
    // Выводим информацию об аргументах.
    for (unsigned int i = 0; i < gimple_call_num_args(stmt); i++) {
        lab1_tree(gimple_call_arg(stmt, i));
        if (i != gimple_call_num_args(stmt) - 1) {
            std::cout << ", ";
        }
    }
    std::cout << ")";
    std::cout << " }" << std::endl;
    
    return 0;
}


// Функция, выводящая информацию о GIMPLE_COND выражении.
static unsigned int lab1_on_gimple_cond(gimple* stmt)
{
    std::cout << "\t\t" << "stmt: " << "\"GIMPLE_COND\"" << " " << "(" << GIMPLE_COND << ")" << " { ";
    lab1_tree(gimple_cond_lhs(stmt));
    std::cout << " ";
    lab1_op(gimple_assign_rhs_code(stmt));
    std::cout << " ";
    lab1_tree(gimple_cond_rhs(stmt));
    std::cout << " }" << std::endl;
    
    return 0;
}

// Функция, выводящая информацию о GIMPLE_LABEL выражении.
static unsigned int lab1_on_gimple_label(gimple* stmt) {
    PREFIX_UNUSED(stmt);
    std::cout << "\t\t" << "stmt: " << "\"GIMPLE_LABEL\"" << " " << "(" << GIMPLE_LABEL << ")" << " {";
    std::cout << "}" << std::endl;
    
    return 0;
}

// Функция, выводящая информацию о GIMPLE_RETURN выражении.
static unsigned int lab1_on_gimple_return(gimple* stmt)
{
    PREFIX_UNUSED(stmt);
    std::cout << "\t\t" << "stmt: " << "\"GIMPLE_RETURN\"" << " " << "(" << GIMPLE_RETURN << ")" << " {";
    std::cout << "}" << std::endl;
        
    return 0;
}


// Функция, выводящая информацию о выражениях внутри базового блока.
static unsigned int lab1_statements(basic_block bb)
{
    // Итерируемся по выражениям в базовом блоке.
    for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
        gimple* stmt = gsi_stmt(gsi);

        // Определяем тип выражения и вызываем соответствующие функции.
        switch (gimple_code(stmt)) {
        case GIMPLE_ASSIGN:
            lab1_on_gimple_assign(stmt);
            break;
        case GIMPLE_CALL:
            lab1_on_gimple_call(stmt);
            break;
        case GIMPLE_COND:
            lab1_on_gimple_cond(stmt);
            break;
        case GIMPLE_LABEL:
            lab1_on_gimple_label(stmt);
            break;
        case GIMPLE_RETURN:
            lab1_on_gimple_return(stmt);
            break;
        }
    }
             
    return 0;
}

// Функция, выводящая информацию о функции и ее базовых блоках.
static unsigned int lab1_func(function* fn)
{
    std::cout << "func: " << "\"" << function_name(fn) << "\"" << " {" << std::endl;
    basic_block bb;
    
    // Перебираем все базовые блоки функции.
    FOR_EACH_BB_FN(bb, fn) {
        // Выводим информацию о базовом блоке.
        lab1_bb_id(bb);
        std::cout << " {" <<  std::endl;
        // Выводим информацию о выражениях внутри блока.
        lab1_statements(bb);
        std::cout << "\t" << "}" << std::endl;
    }
    std::cout << "}" << std::endl;
    std::cout << std::endl;
    return 0;
}

// Метод execute класса lab1_pass, который выводит информацию о функции.
unsigned int lab1_pass::execute(function*fn) { return lab1_func(fn); }



// Структура, описывающая информацию о проходе.
static struct register_pass_info lab1_pass_info = {
    .pass =                     new lab1_pass(g), // Указатель на объект прохода.
    .reference_pass_name =      "ssa", // Имя прохода, после которого будет вставлен наш проход.
    .ref_pass_instance_number = 1, // Номер экземпляра ссылочного прохода.
    .pos_op =                   PASS_POS_INSERT_AFTER, // Вставить наш проход после ссылочного прохода.
};

// Функция инициализации плагина.
int plugin_init(struct plugin_name_args *args, struct plugin_gcc_version *version)
{
    // Проверяем совместимость версий.
    if(!plugin_default_version_check(version, &gcc_version)) {
        return 1;
    }
    
    // Регистрируем проход в менеджере проходов.
    register_callback(args->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &lab1_pass_info);
    
    return 0;
}