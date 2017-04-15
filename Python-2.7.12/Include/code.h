/* Definitions for bytecode */

#ifndef Py_CODE_H
#define Py_CODE_H
#ifdef __cplusplus
extern "C" {
#endif

/* Bytecode object */
typedef struct {
    PyObject_HEAD
    int co_argcount;		/* #arguments, except *args Code Block的位置参数, 比如说函数的位置参数的个数 */ //def func(a,b, **kwargs) a b 就是位置参数, kwargs就是条件参数
    int co_nlocals;		/* #local variables 局部变量的个数, 包括位置参数的个数*/
    int co_stacksize;		/* #entries needed for evaluation stack 执行该段Code Block所需的栈空间*/
    int co_flags;		/* CO_..., see below */
    PyObject *co_code;		/* instruction opcodes Code Block编译得到的字节码指令序列, 是PyStringObject*/ 
    PyObject *co_consts;	/* list (constants used) */	//PyTupleObject 保存Code Block中所有的常量, 包括在其中定义的类, 函数等等
    PyObject *co_names;		/* list of strings (names used) */	//PyTupleObject 保存Code Block中所有的符号
    PyObject *co_varnames;	/* tuple of strings (local variable names) Code Block的局部变量名集合*/
    PyObject *co_freevars;	/* tuple of strings (free variable names) 实现闭包所用到的*/
    PyObject *co_cellvars;      /* tuple of strings (cell variable names) */
    /* The rest doesn't count for hash/cmp */
    PyObject *co_filename;	/* string (where it was loaded from) */
    PyObject *co_name;		/* string (name, for reference) */	//代码块的名字， 通常是函数名或者类名
	int co_firstlineno;		/* first source line number */	//代码块在对应的py文件中的起始行, 即是从第几行开始的
    PyObject *co_lnotab;	/* string (encoding addr<->lineno mapping) See
				   Objects/lnotab_notes.txt for details. */
    void *co_zombieframe;     /* for optimization only (see frameobject.c) */
    PyObject *co_weakreflist;   /* to support weakrefs to code objects */
} PyCodeObject;//每个code block都会对应一个PyCodeObject

/* Masks for co_flags above */
#define CO_OPTIMIZED	0x0001
#define CO_NEWLOCALS	0x0002
#define CO_VARARGS	0x0004
#define CO_VARKEYWORDS	0x0008
#define CO_NESTED       0x0010
#define CO_GENERATOR    0x0020
/* The CO_NOFREE flag is set if there are no free or cell variables.
   This information is redundant, but it allows a single flag test
   to determine whether there is any extra work to be done when the
   call frame it setup.
*/
#define CO_NOFREE       0x0040

#if 0
/* This is no longer used.  Stopped defining in 2.5, do not re-use. */
#define CO_GENERATOR_ALLOWED    0x1000
#endif
#define CO_FUTURE_DIVISION    	0x2000
#define CO_FUTURE_ABSOLUTE_IMPORT 0x4000 /* do absolute imports by default */
#define CO_FUTURE_WITH_STATEMENT  0x8000
#define CO_FUTURE_PRINT_FUNCTION  0x10000
#define CO_FUTURE_UNICODE_LITERALS 0x20000

/* This should be defined if a future statement modifies the syntax.
   For example, when a keyword is added.
*/
#if 1
#define PY_PARSER_REQUIRES_FUTURE_KEYWORD
#endif

#define CO_MAXBLOCKS 20 /* Max static block nesting within a function */

PyAPI_DATA(PyTypeObject) PyCode_Type;

#define PyCode_Check(op) (Py_TYPE(op) == &PyCode_Type)
#define PyCode_GetNumFree(op) (PyTuple_GET_SIZE((op)->co_freevars))

/* Public interface */
PyAPI_FUNC(PyCodeObject *) PyCode_New(
	int, int, int, int, PyObject *, PyObject *, PyObject *, PyObject *,
	PyObject *, PyObject *, PyObject *, PyObject *, int, PyObject *);
        /* same as struct above */

/* Creates a new empty code object with the specified source location. */
PyAPI_FUNC(PyCodeObject *)
PyCode_NewEmpty(const char *filename, const char *funcname, int firstlineno);

/* Return the line number associated with the specified bytecode index
   in this code object.  If you just need the line number of a frame,
   use PyFrame_GetLineNumber() instead. */
PyAPI_FUNC(int) PyCode_Addr2Line(PyCodeObject *, int);

/* for internal use only */
#define _PyCode_GETCODEPTR(co, pp) \
	((*Py_TYPE((co)->co_code)->tp_as_buffer->bf_getreadbuffer) \
	 ((co)->co_code, 0, (void **)(pp)))

typedef struct _addr_pair {
        int ap_lower;
        int ap_upper;
} PyAddrPair;

/* Update *bounds to describe the first and one-past-the-last instructions in the
   same line as lasti.  Return the number of that line.
*/
PyAPI_FUNC(int) _PyCode_CheckLineNumber(PyCodeObject* co,
                                        int lasti, PyAddrPair *bounds);

/* Create a comparable key used to compare constants taking in account the
 * object type. It is used to make sure types are not coerced (e.g., float and
 * complex) _and_ to distinguish 0.0 from -0.0 e.g. on IEEE platforms
 *
 * Return (type(obj), obj, ...): a tuple with variable size (at least 2 items)
 * depending on the type and the value. The type is the first item to not
 * compare bytes and str which can raise a BytesWarning exception. */
PyAPI_FUNC(PyObject*) _PyCode_ConstantKey(PyObject *obj);

PyAPI_FUNC(PyObject*) PyCode_Optimize(PyObject *code, PyObject* consts,
                                      PyObject *names, PyObject *lineno_obj);

#ifdef __cplusplus
}
#endif
#endif /* !Py_CODE_H */
