# Лабораторная работа №1
## Задание 

Выполнить распечатку GIMPLE/IR из gcc компилятора. Например, это можно сделать из ```./gcc-4.9.0/gcc/tree-ssa-dce.c.``` (4.9.0 или другая версия). Как вариант, можно сделать распечатку из плагина для GCC. На ваше усмотрение.

Эта распечатка должна в себя включать следующее:
* базовые блоки, блоки-предшественники, блоки-последователи
* содержимое базовых блоков (GIMPLE инструкции): - арифметические операции
- Ф-функции
- инструкции ветвления
- выражения доступа в память (ArrayRef и др)

## Как запускать 

Из папки с Makefile:

``` make lab1 ``` - создает плагин lab1.so \
``` make test ``` - запускает src/test.c под этим плагином

Че еще можно: \
Вывести граф с ssa-формой ```gcc -fdump-tree-ssa-graph src/test.c```
После этого можно нарисовать картинку ```dot -Tpng test.c.021t.ssa.dot > out.png```

## Че происходит

### GIMPLE

GIMPLE (GNAT Intermediate Programming Language) - это промежуточный язык программирования, используемый в компиляторе GCC (GNU Compiler Collection). GIMPLE представляет собой низкоуровневую, структурированную, SSA-форму (Single Static Assignment) кода, которая используется для проведения оптимизаций перед генерацией машинного кода. 

Базовые блоки - это последовательность операций GIMPLE, которые могут быть выполнены в одном, непрерывном потоке.

Существуют несколько типов базовых блоков, которые можно выделить в зависимости от того, как они используются в структуре управления потоком:

**Блоки Входа:** Это начальный блок в функции или процедуре.

**Блоки Простого Выхода:** Это блоки, которые имеют только один возможный путь перехода из них. Они часто встречаются после простых инструкций или в конце функций.

**Блоки Условного Выхода:** Эти блоки содержат инструкции условного перехода, такие как 'if' или 'switch'. Они имеют два или более выхода в зависимости от результата условия.

**Циклические Блоки:** Это блоки, которые могут быть повторно достигнуты во время выполнения программы. Они часто связаны с циклами 'for', 'while' или 'do-while'.

**Блоки Слияния:** Эти блоки могут быть достигнуты несколькими путями. Они часто следуют за блоками условного выхода.

**Блоки Исключений:** Эти блоки предназначены для обработки исключительных ситуаций и обычно имеют специальное поведение в случае возникновения ошибок.

Эти блоки связаны между собой через "края" (или связи), которые указывают, какой блок следует выполнить после текущего. Эта структура называется графом базовых блоков.

#### Ф-функции ####

Тип операции GIMPLE для слияния нескольких значений в одно.
Часто используются в if-else, циклах и других операциях управления потоком.

Пример:

```c
int foo(int x) {
    int y;
    if (x > 5) {
        y = 10;
    } else {
        y = 20;
    }
    return y;
}
```
Представление GIMPLE:

```out
foo (x: int) : int
{
  D.2153 = PHI <10(2), 20(1)>;
  return D.2153;
}
```

## По коду

```/src/main_with_comms.cpp``` к вашим услугам

Общий план проги:

Инициализация:

 - Плагин запускается GCC, и вызывается функция plugin_init.
 - В plugin_init проверяется совместимость версий GCC и плагина.
 - Проход lab1_pass регистрируется в менеджере проходов.
 - Устанавливается, что проход lab1_pass будет выполняться после прохода "ssa" (SSA-преобразование).

Выполнение прохода:

- Когда GCC доходит до этапа GIMPLE-преобразования, менеджер проходов вызывает функцию execute класса lab1_pass для каждой функции в программе.
 - Функция execute вызывает функцию lab1_func, которая отвечает за вывод информации о функции.

Вывод информации о функции:

- Функция lab1_func выводит имя функции.
- Затем она перебирает все базовые блоки функции с помощью цикла FOR_EACH_BB_FN.
- Для каждого базового блока вызывается функция lab1_bb_id, которая выводит информацию о его предшественниках и последователях.
- Далее вызывается функция lab1_statements, которая анализирует и выводит информацию о выражениях внутри блока.

Анализ выражений:

- Функция lab1_statements перебирает все выражения в базовом блоке с помощью итератора gimple_stmt_iterator.
- Для каждого выражения определяется его тип с помощью gimple_code.
- В зависимости от типа выражения вызывается одна из функций:
* lab1_on_gimple_assign: для GIMPLE_ASSIGN выражений
* lab1_on_gimple_call: для GIMPLE_CALL выражений
* lab1_on_gimple_cond: для GIMPLE_COND выражений
* lab1_on_gimple_label: для GIMPLE_LABEL выражений
* lab1_on_gimple_return: для GIMPLE_RETURN выражений

Вывод информации о выражениях:

- Каждая из функций для анализа выражений выводит информацию о них в стандартный поток вывода.
- Например, lab1_on_gimple_assign выводит левую и правую части присваивания, а также оператор, если он есть.

В целом, код плагина работает следующим образом:

Плагин регистрируется в GCC и устанавливается, чтобы он выполнялся после SSA-преобразования.
При выполнении прохода плагин получает информацию о функции и ее базовых блоках.
Плагин анализирует выражения внутри каждого блока, выводит информацию о них и о структуре базовых блоков.