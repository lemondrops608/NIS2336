# 编译原理大作业 - TINY语言语法分析器

## 任务描述

实现TINY语言的语法分析器，构建抽象语法树。词法分析部分已在编程作业一中已完成，本次作业专注于语法分析的实现。

## TINY语言语法规则

```
program → stmt_seq
stmt_seq → stmt_seq ; stmt | stmt
stmt → if_stmt | repeat_stmt | assign_stmt | read_stmt | write_stmt | error
if_stmt → IF exp THEN stmt_seq END | IF exp THEN stmt_seq ELSE stmt_seq END
repeat_stmt → REPEAT stmt_seq UNTIL exp
assign_stmt → ID ASSIGN exp
read_stmt → READ ID
write_stmt → WRITE exp
exp → simple_exp comparison_op simple_exp | simple_exp
comparison_op → LT | EQ
simple_exp → simple_exp addop term | term
addop → PLUS | MINUS
term → term mulop factor | factor
mulop → TIMES | OVER
factor → LPAREN exp RPAREN | NUM | ID | error
```

## 实现要求

文件parse.c中定义了对应每个非终结符分析过程的函数。本次实验要求在读懂parse.c的基础上，参照函数stmt_sequence()补全其他非终结符对应的函数，每个函数表示将某个非终结符作为节点加入语法树的过程。将非终结符作为一个新的节点，它的子节点可以是对其进行展开后的更简单的非终结符，这时需要递归调用对应该非终结符的函数，将返回值赋给子节点；也可以是终结符，这时需要将非终结符的值赋给子节点。

基于已有的词法分析器，实现语法分析器。核心函数包括：
- `stmt_sequence()` - 解析语句序列
- `statement()` - 解析单个语句
- `if_stmt()`, `repeat_stmt()`, `assign_stmt()`, `read_stmt()`, `write_stmt()` - 解析具体语句类型
- `exp()`, `simple_exp()`, `term()`, `factor()` - 解析表达式

## 编译运行

```bash
# 进入p2_syntax_analysis\build目录下

# 输入指令：make parser，产生parser（语法分析器）
make parser

# 输入指令：make testp，输出对test/sample.tny做词法和语法分析的结果
make testp

# 每次重新生成文件之前需要通过指令: make clean 清除上一次的生成文件
```

## 测试用例及期望输出

** /test/sample.tny **
```
read x;
if 0 < x then
    fac := 1;
    repeat
        fac := fac * x;
        x := x - 1 until x = 0;
    write fac;
end
```

**期望输出:**
```
Syntax tree:
  Read: x
  If
    Op: <
      Const: 0
      Id: x
    Assign to: fac
      Const: 1
    Repeat
      Assign to: fac
        Op: *
          Id: fac
          Id: x
      Assign to: x
        Op: -
          Id: x
          Const: 1
      Op: =
        Id: x
        Const: 0
    Write
      Id: fac
```

## 提交内容

1. 完整的parser.c源代码
2. 测试结果截图
3. 简要实验报告说明实现思路

## 评分标准
本实验满分20分，评分时将使用2个测试样例（与给出的例子不同）测试输出的语法树是否正确，每个样例4分，文档报告12分。
