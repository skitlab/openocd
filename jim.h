/* Jim - A small embeddable Tcl interpreter
 *
 * Copyright 2005 Salvatore Sanfilippo <antirez@invece.org>
 * Copyright 2005 Clemens Hintze <c.hintze@gmx.net>
 * Copyright 2005 patthoyts - Pat Thoyts <patthoyts@users.sf.net> 
 * Copyright 2008 oharboe - �yvind Harboe - oyvind.harboe@zylin.com
 * Copyright 2008 Andrew Lunn <andrew@lunn.ch>
 * Copyright 2008 Duane Ellis <openocd@duaneellis.com>
 * Copyright 2008 Uwe Klein <uklein@klein-messgeraete.de>
 * 
 * The FreeBSD license
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE JIM TCL PROJECT ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * JIM TCL PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the Jim Tcl Project.
 *
 *--- Inline Header File Documentation --- 
 *    [By Duane Ellis, openocd@duaneellis.com, 8/18/8]
 *
 * Belief is "Jim" would greatly benifit if Jim Internals where
 * documented in some way - form whatever, and perhaps - the package:
 * 'doxygen' is the correct approach to do that.
 *
 *   Details, see: http://www.stack.nl/~dimitri/doxygen/
 *
 * To that end please follow these guide lines:
 *
 *    (A) Document the PUBLIC api in the .H file.
 *
 *    (B) Document JIM Internals, in the .C file.
 *
 *    (C) Remember JIM is embedded in other packages, to that end do
 *    not assume that your way of documenting is the right way, Jim's
 *    public documentation should be agnostic, such that it is some
 *    what agreeable with the "package" that is embedding JIM inside
 *    of it's own doxygen documentation.
 *
 *    (D) Use minimal Doxygen tags.
 *
 * This will be an "ongoing work in progress" for some time.
 **/

#ifndef __JIM__H
#define __JIM__H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <limits.h>
#include <stdio.h>  /* for the FILE typedef definition */
#include <stdlib.h> /* In order to export the Jim_Free() macro */
#include <stdarg.h> /* In order to get type va_list */

/* -----------------------------------------------------------------------------
* Some /very/ old compiler maybe do not know how to
* handle 'const'. They even do not know, how to ignore
* it. For those compiler it may be better to compile with
* define JIM_NO_CONST activated
* ---------------------------------------------------------------------------*/

#ifdef JIM_NO_CONST
#  define const
#endif

/* -----------------------------------------------------------------------------
 * System configuration
 * For most modern systems, you can leave the default.
 * For embedded systems some change may be required.
 * ---------------------------------------------------------------------------*/

#define HAVE_LONG_LONG

/* -----------------------------------------------------------------------------
 * Compiler specific fixes.
 * ---------------------------------------------------------------------------*/

/* Long Long type and related issues */
#ifdef HAVE_LONG_LONG
#  define jim_wide long long
#  ifndef LLONG_MAX
#    define LLONG_MAX    9223372036854775807LL
#  endif
#  ifndef LLONG_MIN
#    define LLONG_MIN    (-LLONG_MAX - 1LL)
#  endif
#  define JIM_WIDE_MIN LLONG_MIN
#  define JIM_WIDE_MAX LLONG_MAX
#else
#  define jim_wide long
#  define JIM_WIDE_MIN LONG_MIN
#  define JIM_WIDE_MAX LONG_MAX
#endif

/* -----------------------------------------------------------------------------
 * LIBC specific fixes
 * ---------------------------------------------------------------------------*/

#ifdef HAVE_LONG_LONG
#  define JIM_WIDE_MODIFIER "lld"
#else
#  define JIM_WIDE_MODIFIER "ld"
#endif

/* -----------------------------------------------------------------------------
 * Exported defines
 * ---------------------------------------------------------------------------*/

/* Jim version numbering: every version of jim is marked with a
 * successive integer number. This is version 0. The first
 * stable version will be 1, then 2, 3, and so on. */
#define JIM_VERSION 60

#define JIM_OK 0
#define JIM_ERR 1
#define JIM_RETURN 2
#define JIM_BREAK 3
#define JIM_CONTINUE 4
#define JIM_EVAL 5
#define JIM_EXIT 6
#define JIM_ERR_ADDSTACK 7
#define JIM_MAX_NESTING_DEPTH 10000 /* default max nesting depth */

/* Some function get an integer argument with flags to change
 * the behaviour. */
#define JIM_NONE 0    /* no flags set */
#define JIM_ERRMSG 1    /* set an error message in the interpreter. */

/* Flags for Jim_SubstObj() */
#define JIM_SUBST_NOVAR 1 /* don't perform variables substitutions */
#define JIM_SUBST_NOCMD 2 /* don't perform command substitutions */
#define JIM_SUBST_NOESC 4 /* don't perform escapes substitutions */

/* Unused arguments generate annoying warnings... */
#define JIM_NOTUSED(V) ((void) V)

/* Flags used by API calls getting a 'nocase' argument. */
#define JIM_CASESENS    0   /* case sensitive */
#define JIM_NOCASE      1   /* no case */

/* Filesystem related */
#define JIM_PATH_LEN 1024

/* Newline, some embedded system may need -DJIM_CRLF */
#ifdef JIM_CRLF
#define JIM_NL "\r\n"
#else
#define JIM_NL "\n"
#endif

/* -----------------------------------------------------------------------------
 * Stack
 * ---------------------------------------------------------------------------*/

typedef struct Jim_Stack {
    int len;
    int maxlen;
    void **vector;
} Jim_Stack;

/* -----------------------------------------------------------------------------
 * Hash table
 * ---------------------------------------------------------------------------*/

typedef struct Jim_HashEntry {
    const void *key;
    void *val;
    struct Jim_HashEntry *next;
} Jim_HashEntry;

typedef struct Jim_HashTableType {
    unsigned int (*hashFunction)(const void *key);
    const void *(*keyDup)(void *privdata, const void *key);
    void *(*valDup)(void *privdata, const void *obj);
    int (*keyCompare)(void *privdata, const void *key1, const void *key2);
    void (*keyDestructor)(void *privdata, const void *key);
    void (*valDestructor)(void *privdata, void *obj);
} Jim_HashTableType;

typedef struct Jim_HashTable {
    Jim_HashEntry **table;
    Jim_HashTableType *type;
    unsigned int size;
    unsigned int sizemask;
    unsigned int used;
    unsigned int collisions;
    void *privdata;
} Jim_HashTable;

typedef struct Jim_HashTableIterator {
    Jim_HashTable *ht;
    int index;
    Jim_HashEntry *entry, *nextEntry;
} Jim_HashTableIterator;

/* This is the initial size of every hash table */
#define JIM_HT_INITIAL_SIZE     16

/* ------------------------------- Macros ------------------------------------*/
#define Jim_FreeEntryVal(ht, entry) \
    if ((ht)->type->valDestructor) \
        (ht)->type->valDestructor((ht)->privdata, (entry)->val)

#define Jim_SetHashVal(ht, entry, _val_) do { \
    if ((ht)->type->valDup) \
        entry->val = (ht)->type->valDup((ht)->privdata, _val_); \
    else \
        entry->val = (_val_); \
} while(0)

#define Jim_FreeEntryKey(ht, entry) \
    if ((ht)->type->keyDestructor) \
        (ht)->type->keyDestructor((ht)->privdata, (entry)->key)

#define Jim_SetHashKey(ht, entry, _key_) do { \
    if ((ht)->type->keyDup) \
        entry->key = (ht)->type->keyDup((ht)->privdata, _key_); \
    else \
        entry->key = (_key_); \
} while(0)

#define Jim_CompareHashKeys(ht, key1, key2) \
    (((ht)->type->keyCompare) ? \
        (ht)->type->keyCompare((ht)->privdata, key1, key2) : \
        (key1) == (key2))

#define Jim_HashKey(ht, key) (ht)->type->hashFunction(key)

#define Jim_GetHashEntryKey(he) ((he)->key)
#define Jim_GetHashEntryVal(he) ((he)->val)
#define Jim_GetHashTableCollisions(ht) ((ht)->collisions)
#define Jim_GetHashTableSize(ht) ((ht)->size)
#define Jim_GetHashTableUsed(ht) ((ht)->used)

/* -----------------------------------------------------------------------------
 * Jim_Obj structure
 * ---------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------
 * Jim object. This is mostly the same as Tcl_Obj itself,
 * with the addition of the 'prev' and 'next' pointers.
 * In Jim all the objects are stored into a linked list for GC purposes,
 * so that it's possible to access every object living in a given interpreter
 * sequentially. When an object is freed, it's moved into a different
 * linked list, used as object pool.
 *
 * The refcount of a freed object is always -1.
 * ---------------------------------------------------------------------------*/
typedef struct Jim_Obj {
    int refCount; /* reference count */
    char *bytes; /* string representation buffer. NULL = no string repr. */
    int length; /* number of bytes in 'bytes', not including the numterm. */
    struct Jim_ObjType *typePtr; /* object type. */
    /* Internal representation union */
    union {
        /* integer number type */
        jim_wide wideValue;
        /* hashed object type value */
        int hashValue;
        /* index type */
        int indexValue;
        /* return code type */
        int returnCode;
        /* double number type */
        double doubleValue;
        /* Generic pointer */
        void *ptr;
        /* Generic two pointers value */
        struct {
            void *ptr1;
            void *ptr2;
        } twoPtrValue;
        /* Variable object */
        struct {
            unsigned jim_wide callFrameId;
            struct Jim_Var *varPtr;
        } varValue;
        /* Command object */
        struct {
            unsigned jim_wide procEpoch;
            struct Jim_Cmd *cmdPtr;
        } cmdValue;
        /* List object */
        struct {
            struct Jim_Obj **ele;    /* Elements vector */
            int len;        /* Length */
            int maxLen;        /* Allocated 'ele' length */
        } listValue;
        /* String type */
        struct {
            int maxLength;
        } strValue;
        /* Reference type */
        struct {
            jim_wide id;
            struct Jim_Reference *refPtr;
        } refValue;
        /* Source type */
        struct {
            const char *fileName;
            int lineNumber;
        } sourceValue;
        /* Dict substitution type */
        struct {
            struct Jim_Obj *varNameObjPtr;
            struct Jim_Obj *indexObjPtr;
        } dictSubstValue;
        /* tagged binary type */
        struct {
            unsigned char *data;
            size_t         len;
        } binaryValue;
    } internalRep;
    /* This are 8 or 16 bytes more for every object
     * but this is required for efficient garbage collection
     * of Jim references. */
    struct Jim_Obj *prevObjPtr; /* pointer to the prev object. */
    struct Jim_Obj *nextObjPtr; /* pointer to the next object. */
} Jim_Obj;

/* Jim_Obj related macros */
#define Jim_IncrRefCount(objPtr) \
    ++(objPtr)->refCount
#define Jim_DecrRefCount(interp, objPtr) \
    if (--(objPtr)->refCount <= 0) Jim_FreeObj(interp, objPtr)
#define Jim_IsShared(objPtr) \
    ((objPtr)->refCount > 1)

/* This macro is used when we allocate a new object using
 * Jim_New...Obj(), but for some error we need to destroy it.
 * Instead to use Jim_IncrRefCount() + Jim_DecrRefCount() we
 * can just call Jim_FreeNewObj. To call Jim_Free directly
 * seems too raw, the object handling may change and we want
 * that Jim_FreeNewObj() can be called only against objects
 * that are belived to have refcount == 0. */
#define Jim_FreeNewObj Jim_FreeObj

/* Free the internal representation of the object. */
#define Jim_FreeIntRep(i,o) \
    if ((o)->typePtr && (o)->typePtr->freeIntRepProc) \
        (o)->typePtr->freeIntRepProc(i, o)

/* Get the internal representation pointer */
#define Jim_GetIntRepPtr(o) (o)->internalRep.ptr

/* Set the internal representation pointer */
#define Jim_SetIntRepPtr(o, p) \
    (o)->internalRep.ptr = (p)

/* The object type structure.
 * There are four methods.
 *
 * - FreeIntRep is used to free the internal representation of the object.
 *   Can be NULL if there is nothing to free.
 * - DupIntRep is used to duplicate the internal representation of the object.
 *   If NULL, when an object is duplicated, the internalRep union is
 *   directly copied from an object to another.
 *   Note that it's up to the caller to free the old internal repr of the
 *   object before to call the Dup method.
 * - UpdateString is used to create the string from the internal repr.
 * - setFromAny is used to convert the current object into one of this type.
 */

struct Jim_Interp;

typedef void (Jim_FreeInternalRepProc)(struct Jim_Interp *interp,
        struct Jim_Obj *objPtr);
typedef void (Jim_DupInternalRepProc)(struct Jim_Interp *interp,
        struct Jim_Obj *srcPtr, Jim_Obj *dupPtr);
typedef void (Jim_UpdateStringProc)(struct Jim_Obj *objPtr);
    
typedef struct Jim_ObjType {
    const char *name; /* The name of the type. */
    Jim_FreeInternalRepProc *freeIntRepProc;
    Jim_DupInternalRepProc *dupIntRepProc;
    Jim_UpdateStringProc *updateStringProc;
    int flags;
} Jim_ObjType;

/* Jim_ObjType flags */
#define JIM_TYPE_NONE 0        /* No flags */
#define JIM_TYPE_REFERENCES 1    /* The object may contain referneces. */

/* Starting from 1 << 20 flags are reserved for private uses of
 * different calls. This way the same 'flags' argument may be used
 * to pass both global flags and private flags. */
#define JIM_PRIV_FLAG_SHIFT 20

/* -----------------------------------------------------------------------------
 * Call frame, vars, commands structures
 * ---------------------------------------------------------------------------*/

/* Call frame */
typedef struct Jim_CallFrame {
    unsigned jim_wide id; /* Call Frame ID. Used for caching. */
    struct Jim_HashTable vars; /* Where local vars are stored */
    struct Jim_HashTable *staticVars; /* pointer to procedure static vars */
    struct Jim_CallFrame *parentCallFrame;
    Jim_Obj *const *argv; /* object vector of the current procedure call. */
    int argc; /* number of args of the current procedure call. */
    Jim_Obj *procArgsObjPtr; /* arglist object of the running procedure */
    Jim_Obj *procBodyObjPtr; /* body object of the running procedure */
    struct Jim_CallFrame *nextFramePtr;
} Jim_CallFrame;

/* The var structure. It just holds the pointer of the referenced
 * object. If linkFramePtr is not NULL the variable is a link
 * to a variable of name store on objPtr living on the given callframe
 * (this happens when the [global] or [upvar] command is used).
 * The interp in order to always know how to free the Jim_Obj associated
 * with a given variable because In Jim objects memory managment is
 * bound to interpreters. */
typedef struct Jim_Var {
    Jim_Obj *objPtr;
    struct Jim_CallFrame *linkFramePtr;
} Jim_Var;
    
/* The cmd structure. */
typedef int (*Jim_CmdProc)(struct Jim_Interp *interp, int argc,
    Jim_Obj *const *argv);
typedef void (*Jim_DelCmdProc)(struct Jim_Interp *interp, void *privData);

/* A command is implemented in C if funcPtr is != NULL, otherwise
 * it's a Tcl procedure with the arglist and body represented by the
 * two objects referenced by arglistObjPtr and bodyoObjPtr. */
typedef struct Jim_Cmd {
    Jim_CmdProc cmdProc; /* Not-NULL for a C command. */
    void *privData; /* Only used for C commands. */
    Jim_DelCmdProc delProc; /* Called when the command is deleted if != NULL */
    Jim_Obj *argListObjPtr;
    Jim_Obj *bodyObjPtr;
    Jim_HashTable *staticVars; /* Static vars hash table. NULL if no statics. */
    int arityMin; /* Min number of arguments. */
    int arityMax; /* Max number of arguments. */
} Jim_Cmd;

/* Pseudo Random Number Generator State structure */
typedef struct Jim_PrngState {
    unsigned char sbox[256];
    unsigned int i, j;
} Jim_PrngState;

/* -----------------------------------------------------------------------------
 * Jim interpreter structure.
 * Fields similar to the real Tcl interpreter structure have the same names.
 * ---------------------------------------------------------------------------*/
typedef struct Jim_Interp {
    Jim_Obj *result; /* object returned by the last command called. */
    int errorLine; /* Error line where an error occurred. */
    const char *errorFileName; /* Error file where an error occurred. */
    int numLevels; /* Number of current nested calls. */
    int maxNestingDepth; /* Used for infinite loop detection. */
    int returnCode; /* Completion code to return on JIM_RETURN. */
    int exitCode; /* Code to return to the OS on JIM_EXIT. */
    Jim_CallFrame *framePtr; /* Pointer to the current call frame */
    Jim_CallFrame *topFramePtr; /* toplevel/global frame pointer. */
    struct Jim_HashTable commands; /* Commands hash table */
    unsigned jim_wide procEpoch; /* Incremented every time the result
                of procedures names lookup caching
                may no longer be valid. */
    unsigned jim_wide callFrameEpoch; /* Incremented every time a new
                callframe is created. This id is used for the
                'ID' field contained in the Jim_CallFrame
                structure. */
    Jim_Obj *liveList; /* Linked list of all the live objects. */
    Jim_Obj *freeList; /* Linked list of all the unused objects. */
    const char *scriptFileName; /* File name of the script currently in execution. */
    Jim_Obj *emptyObj; /* Shared empty string object. */
    unsigned jim_wide referenceNextId; /* Next id for reference. */
    struct Jim_HashTable references; /* References hash table. */
    jim_wide lastCollectId; /* reference max Id of the last GC
                execution. It's set to -1 while the collection
                is running as sentinel to avoid to recursive
                calls via the [collect] command inside
                finalizers. */
    time_t lastCollectTime; /* unix time of the last GC execution */
    struct Jim_HashTable sharedStrings; /* Shared Strings hash table */
    Jim_Obj *stackTrace; /* Stack trace object. */
    Jim_Obj *unknown; /* Unknown command cache */
    int unknown_called; /* The unknown command has been invoked */
    int errorFlag; /* Set if an error occurred during execution. */
    int evalRetcodeLevel; /* Level where the last return with code JIM_EVAL
                             happened. */
    void *cmdPrivData; /* Used to pass the private data pointer to
                  a command. It is set to what the user specified
                  via Jim_CreateCommand(). */

    struct Jim_HashTable stub; /* Stub hash table to export API */
    /* Jim_GetApi() function pointer, used to bootstrap the STUB table */
    int (*getApiFuncPtr)(struct Jim_Interp *, const char *, void *);
    struct Jim_CallFrame *freeFramesList; /* list of CallFrame structures. */
    struct Jim_HashTable assocData; /* per-interp storage for use by packages */
    Jim_PrngState *prngState; /* per interpreter Random Number Gen. state. */
    struct Jim_HashTable packages; /* Provided packages hash table */
} Jim_Interp;

/* Currently provided as macro that performs the increment.
 * At some point may be a real function doing more work.
 * The proc epoch is used in order to know when a command lookup
 * cached can no longer considered valid. */
#define Jim_InterpIncrProcEpoch(i) (i)->procEpoch++
#define Jim_SetResultString(i,s,l) Jim_SetResult(i, Jim_NewStringObj(i,s,l))
#define Jim_SetResultInt(i,intval) Jim_SetResult(i, Jim_NewIntObj(i,intval))
#define Jim_SetEmptyResult(i) Jim_SetResult(i, (i)->emptyObj)
#define Jim_GetResult(i) ((i)->result)
#define Jim_CmdPrivData(i) ((i)->cmdPrivData)

/* Note that 'o' is expanded only one time inside this macro,
 * so it's safe to use side effects. */
#define Jim_SetResult(i,o) do {     \
    Jim_Obj *_resultObjPtr_ = (o);    \
    Jim_IncrRefCount(_resultObjPtr_); \
    Jim_DecrRefCount(i,(i)->result);  \
    (i)->result = _resultObjPtr_;     \
} while(0)

/* Reference structure. The interpreter pointer is held within privdata member in HashTable */
#define JIM_REFERENCE_TAGLEN 7 /* The tag is fixed-length, because the reference
                                  string representation must be fixed length. */
typedef struct Jim_Reference {
    Jim_Obj *objPtr;
    Jim_Obj *finalizerCmdNamePtr;
    char tag[JIM_REFERENCE_TAGLEN+1];
} Jim_Reference;

/* -----------------------------------------------------------------------------
 * Exported API prototypes.
 * ---------------------------------------------------------------------------*/

/* Macros that are common for extensions and core. */
#define Jim_NewEmptyStringObj(i) Jim_NewStringObj(i, "", 0)

/* The core includes real prototypes, extensions instead
 * include a global function pointer for every function exported.
 * Once the extension calls Jim_InitExtension(), the global
 * functon pointers are set to the value of the STUB table
 * contained in the Jim_Interp structure.
 *
 * This makes Jim able to load extensions even if it is statically
 * linked itself, and to load extensions compiled with different
 * versions of Jim (as long as the API is still compatible.) */

/* Macros are common for core and extensions */
#define Jim_FreeHashTableIterator(iter) Jim_Free(iter)

#define JIM_EXPORT

/* Memory allocation */
JIM_EXPORT void * Jim_Alloc (int size);
JIM_EXPORT void Jim_Free (void *ptr);
JIM_EXPORT char * Jim_StrDup (const char *s);

/* evaluation */
JIM_EXPORT int Jim_Eval(Jim_Interp *interp, const char *script);
/* in C code, you can do this and get better error messages */
/*   Jim_Eval_Named( interp, "some tcl commands", __FILE__, __LINE__ ); */
JIM_EXPORT int Jim_Eval_Named(Jim_Interp *interp, const char *script,const char *filename, int lineno);
JIM_EXPORT int Jim_EvalGlobal(Jim_Interp *interp, const char *script);
JIM_EXPORT int Jim_EvalFile(Jim_Interp *interp, const char *filename);
JIM_EXPORT int Jim_EvalObj (Jim_Interp *interp, Jim_Obj *scriptObjPtr);
JIM_EXPORT int Jim_EvalObjBackground (Jim_Interp *interp,
        Jim_Obj *scriptObjPtr);
JIM_EXPORT int Jim_EvalObjVector (Jim_Interp *interp, int objc,
        Jim_Obj *const *objv);
JIM_EXPORT int Jim_SubstObj (Jim_Interp *interp, Jim_Obj *substObjPtr,
        Jim_Obj **resObjPtrPtr, int flags);

/* stack */
JIM_EXPORT void Jim_InitStack(Jim_Stack *stack);
JIM_EXPORT void Jim_FreeStack(Jim_Stack *stack);
JIM_EXPORT int Jim_StackLen(Jim_Stack *stack);
JIM_EXPORT void Jim_StackPush(Jim_Stack *stack, void *element);
JIM_EXPORT void * Jim_StackPop(Jim_Stack *stack);
JIM_EXPORT void * Jim_StackPeek(Jim_Stack *stack);
JIM_EXPORT void Jim_FreeStackElements(Jim_Stack *stack, void (*freeFunc)(void *ptr));

/* hash table */
JIM_EXPORT int Jim_InitHashTable (Jim_HashTable *ht,
        Jim_HashTableType *type, void *privdata);
JIM_EXPORT int Jim_ExpandHashTable (Jim_HashTable *ht,
        unsigned int size);
JIM_EXPORT int Jim_AddHashEntry (Jim_HashTable *ht, const void *key,
        void *val);
JIM_EXPORT int Jim_ReplaceHashEntry (Jim_HashTable *ht,
        const void *key, void *val);
JIM_EXPORT int Jim_DeleteHashEntry (Jim_HashTable *ht,
        const void *key);
JIM_EXPORT int Jim_FreeHashTable (Jim_HashTable *ht);
JIM_EXPORT Jim_HashEntry * Jim_FindHashEntry (Jim_HashTable *ht,
        const void *key);
JIM_EXPORT int Jim_ResizeHashTable (Jim_HashTable *ht);
JIM_EXPORT Jim_HashTableIterator *Jim_GetHashTableIterator
        (Jim_HashTable *ht);
JIM_EXPORT Jim_HashEntry * Jim_NextHashEntry
        (Jim_HashTableIterator *iter);

/* objects */
JIM_EXPORT Jim_Obj * Jim_NewObj (Jim_Interp *interp);
JIM_EXPORT void Jim_FreeObj (Jim_Interp *interp, Jim_Obj *objPtr);
JIM_EXPORT void Jim_InvalidateStringRep (Jim_Obj *objPtr);
JIM_EXPORT void Jim_InitStringRep (Jim_Obj *objPtr, const char *bytes,
        int length);
JIM_EXPORT Jim_Obj * Jim_DuplicateObj (Jim_Interp *interp,
        Jim_Obj *objPtr);
JIM_EXPORT const char * Jim_GetString(Jim_Obj *objPtr,
        int *lenPtr);
JIM_EXPORT int Jim_Length(Jim_Obj *objPtr);

/* string object */
JIM_EXPORT Jim_Obj * Jim_NewStringObj (Jim_Interp *interp,
        const char *s, int len);
JIM_EXPORT Jim_Obj * Jim_NewStringObjNoAlloc (Jim_Interp *interp,
        char *s, int len);
JIM_EXPORT void Jim_AppendString (Jim_Interp *interp, Jim_Obj *objPtr,
        const char *str, int len);
JIM_EXPORT void Jim_AppendString_sprintf (Jim_Interp *interp, Jim_Obj *objPtr,
        const char *fmt, ... );
JIM_EXPORT void Jim_AppendObj (Jim_Interp *interp, Jim_Obj *objPtr,
        Jim_Obj *appendObjPtr);
JIM_EXPORT void Jim_AppendStrings (Jim_Interp *interp,
        Jim_Obj *objPtr, ...);
JIM_EXPORT int Jim_StringEqObj (Jim_Obj *aObjPtr,
        Jim_Obj *bObjPtr, int nocase);
JIM_EXPORT int Jim_StringMatchObj (Jim_Obj *patternObjPtr,
        Jim_Obj *objPtr, int nocase);
JIM_EXPORT Jim_Obj * Jim_StringRangeObj (Jim_Interp *interp,
        Jim_Obj *strObjPtr, Jim_Obj *firstObjPtr,
        Jim_Obj *lastObjPtr);
JIM_EXPORT Jim_Obj * Jim_FormatString (Jim_Interp *interp,
        Jim_Obj *fmtObjPtr, int objc, Jim_Obj *const *objv);
JIM_EXPORT Jim_Obj * Jim_ScanString (Jim_Interp *interp, Jim_Obj *strObjPtr,
        Jim_Obj *fmtObjPtr, int flags);
JIM_EXPORT int Jim_CompareStringImmediate (Jim_Interp *interp,
        Jim_Obj *objPtr, const char *str);

/* reference object */
JIM_EXPORT Jim_Obj * Jim_NewReference (Jim_Interp *interp,
        Jim_Obj *objPtr, Jim_Obj *tagPtr, Jim_Obj *cmdNamePtr);
JIM_EXPORT Jim_Reference * Jim_GetReference (Jim_Interp *interp,
        Jim_Obj *objPtr);
JIM_EXPORT int Jim_SetFinalizer (Jim_Interp *interp, Jim_Obj *objPtr, Jim_Obj *cmdNamePtr);
JIM_EXPORT int Jim_GetFinalizer (Jim_Interp *interp, Jim_Obj *objPtr, Jim_Obj **cmdNamePtrPtr);

/* interpreter */
JIM_EXPORT Jim_Interp * Jim_CreateInterp (void);
JIM_EXPORT void Jim_FreeInterp (Jim_Interp *i);
JIM_EXPORT int Jim_GetExitCode (Jim_Interp *interp);
JIM_EXPORT void * Jim_SetStdin (Jim_Interp *interp, void *fp);
JIM_EXPORT void * Jim_SetStdout (Jim_Interp *interp, void *fp);
JIM_EXPORT void * Jim_SetStderr (Jim_Interp *interp, void *fp);

/* commands */
JIM_EXPORT void Jim_RegisterCoreCommands (Jim_Interp *interp);
JIM_EXPORT int Jim_CreateCommand (Jim_Interp *interp, 
        const char *cmdName, Jim_CmdProc cmdProc, void *privData,
         Jim_DelCmdProc delProc);
JIM_EXPORT int Jim_CreateProcedure (Jim_Interp *interp, 
        const char *cmdName, Jim_Obj *argListObjPtr, Jim_Obj *staticsListObjPtr,
        Jim_Obj *bodyObjPtr, int arityMin, int arityMax);
JIM_EXPORT int Jim_DeleteCommand (Jim_Interp *interp,
        const char *cmdName);
JIM_EXPORT int Jim_RenameCommand (Jim_Interp *interp, 
        const char *oldName, const char *newName);
JIM_EXPORT Jim_Cmd * Jim_GetCommand (Jim_Interp *interp,
        Jim_Obj *objPtr, int flags);
JIM_EXPORT int Jim_SetVariable (Jim_Interp *interp,
        Jim_Obj *nameObjPtr, Jim_Obj *valObjPtr);
JIM_EXPORT int Jim_SetVariableStr (Jim_Interp *interp,
        const char *name, Jim_Obj *objPtr);
JIM_EXPORT int Jim_SetGlobalVariableStr (Jim_Interp *interp,
        const char *name, Jim_Obj *objPtr);
JIM_EXPORT int Jim_SetVariableStrWithStr (Jim_Interp *interp,
        const char *name, const char *val);
JIM_EXPORT int Jim_SetVariableLink (Jim_Interp *interp,
        Jim_Obj *nameObjPtr, Jim_Obj *targetNameObjPtr,
        Jim_CallFrame *targetCallFrame);
JIM_EXPORT Jim_Obj * Jim_GetVariable (Jim_Interp *interp,
        Jim_Obj *nameObjPtr, int flags);
JIM_EXPORT Jim_Obj * Jim_GetGlobalVariable (Jim_Interp *interp,
        Jim_Obj *nameObjPtr, int flags);
JIM_EXPORT Jim_Obj * Jim_GetVariableStr (Jim_Interp *interp,
        const char *name, int flags);
JIM_EXPORT Jim_Obj * Jim_GetGlobalVariableStr (Jim_Interp *interp,
        const char *name, int flags);
JIM_EXPORT int Jim_UnsetVariable (Jim_Interp *interp,
        Jim_Obj *nameObjPtr, int flags);

/* call frame */
JIM_EXPORT int Jim_GetCallFrameByLevel (Jim_Interp *interp,
        Jim_Obj *levelObjPtr, Jim_CallFrame **framePtrPtr,
        int *newLevelPtr);

/* garbage collection */
JIM_EXPORT int Jim_Collect (Jim_Interp *interp);
JIM_EXPORT void Jim_CollectIfNeeded (Jim_Interp *interp);

/* index object */
JIM_EXPORT int Jim_GetIndex (Jim_Interp *interp, Jim_Obj *objPtr,
        int *indexPtr);

/* list object */
JIM_EXPORT Jim_Obj * Jim_NewListObj (Jim_Interp *interp,
        Jim_Obj *const *elements, int len);
JIM_EXPORT void Jim_ListInsertElements (Jim_Interp *interp,
        Jim_Obj *listPtr, int index, int objc, Jim_Obj *const *objVec);
JIM_EXPORT void Jim_ListAppendElement (Jim_Interp *interp,
        Jim_Obj *listPtr, Jim_Obj *objPtr);
JIM_EXPORT void Jim_ListAppendList (Jim_Interp *interp,
        Jim_Obj *listPtr, Jim_Obj *appendListPtr);
JIM_EXPORT void Jim_ListLength (Jim_Interp *interp, Jim_Obj *listPtr,
        int *intPtr);
JIM_EXPORT int Jim_ListIndex (Jim_Interp *interp, Jim_Obj *listPrt,
        int index, Jim_Obj **objPtrPtr, int seterr);
JIM_EXPORT int Jim_SetListIndex (Jim_Interp *interp,
        Jim_Obj *varNamePtr, Jim_Obj *const *indexv, int indexc,
        Jim_Obj *newObjPtr);
JIM_EXPORT Jim_Obj * Jim_ConcatObj (Jim_Interp *interp, int objc,
        Jim_Obj *const *objv);

/* dict object */
JIM_EXPORT Jim_Obj * Jim_NewDictObj (Jim_Interp *interp,
        Jim_Obj *const *elements, int len);
JIM_EXPORT int Jim_DictKey (Jim_Interp *interp, Jim_Obj *dictPtr,
        Jim_Obj *keyPtr, Jim_Obj **objPtrPtr, int flags);
JIM_EXPORT int Jim_DictKeysVector (Jim_Interp *interp,
        Jim_Obj *dictPtr, Jim_Obj *const *keyv, int keyc,
        Jim_Obj **objPtrPtr, int flags);
JIM_EXPORT int Jim_SetDictKeysVector (Jim_Interp *interp,
        Jim_Obj *varNamePtr, Jim_Obj *const *keyv, int keyc,
        Jim_Obj *newObjPtr);

/* return code object */
JIM_EXPORT int Jim_GetReturnCode (Jim_Interp *interp, Jim_Obj *objPtr,
        int *intPtr);

/* expression object */
JIM_EXPORT int Jim_EvalExpression (Jim_Interp *interp,
        Jim_Obj *exprObjPtr, Jim_Obj **exprResultPtrPtr);
JIM_EXPORT int Jim_GetBoolFromExpr (Jim_Interp *interp,
        Jim_Obj *exprObjPtr, int *boolPtr);

/* integer object */
JIM_EXPORT int Jim_GetWide (Jim_Interp *interp, Jim_Obj *objPtr,
        jim_wide *widePtr);
JIM_EXPORT int Jim_GetLong (Jim_Interp *interp, Jim_Obj *objPtr,
        long *longPtr);
JIM_EXPORT void Jim_SetWide (Jim_Interp *interp, Jim_Obj *objPtr,
        jim_wide wideValue);
#define Jim_NewWideObj  Jim_NewIntObj
JIM_EXPORT Jim_Obj * Jim_NewIntObj (Jim_Interp *interp,
        jim_wide wideValue);

/* double object */
JIM_EXPORT int Jim_GetDouble(Jim_Interp *interp, Jim_Obj *objPtr,
        double *doublePtr);
JIM_EXPORT void Jim_SetDouble(Jim_Interp *interp, Jim_Obj *objPtr,
        double doubleValue);
JIM_EXPORT Jim_Obj * Jim_NewDoubleObj(Jim_Interp *interp, double doubleValue);

/* shared strings */
JIM_EXPORT const char * Jim_GetSharedString (Jim_Interp *interp, 
        const char *str);
JIM_EXPORT void Jim_ReleaseSharedString (Jim_Interp *interp,
        const char *str);

/* commands utilities */
JIM_EXPORT void Jim_WrongNumArgs (Jim_Interp *interp, int argc,
        Jim_Obj *const *argv, const char *msg);
JIM_EXPORT int Jim_GetEnum (Jim_Interp *interp, Jim_Obj *objPtr,
        const char * const *tablePtr, int *indexPtr, const char *name, int flags);
JIM_EXPORT int Jim_ScriptIsComplete (const char *s, int len,
        char *stateCharPtr);

/* package utilities */
typedef void (Jim_InterpDeleteProc)(Jim_Interp *interp, void *data);
JIM_EXPORT void * Jim_GetAssocData(Jim_Interp *interp, const char *key);
JIM_EXPORT int Jim_SetAssocData(Jim_Interp *interp, const char *key,
        Jim_InterpDeleteProc *delProc, void *data);
JIM_EXPORT int Jim_DeleteAssocData(Jim_Interp *interp, const char *key);

/* Packages C API */
/* jim-package.c */
JIM_EXPORT int Jim_PackageProvide (Jim_Interp *interp,
        const char *name, const char *ver, int flags);
JIM_EXPORT const char * Jim_PackageRequire (Jim_Interp *interp,
        const char *name, const char *ver, int flags);

/* error messages */
JIM_EXPORT void Jim_PrintErrorMessage (Jim_Interp *interp);

/* interactive mode */
JIM_EXPORT int Jim_InteractivePrompt (Jim_Interp *interp);

/* Misc */
JIM_EXPORT void Jim_Panic (Jim_Interp *interp, const char *fmt, ...);
int Jim_StringToWide(const char *str, jim_wide *widePtr, int base);

/* jim-load.c */
int Jim_LoadLibrary(Jim_Interp *interp, const char *pathName);

#ifdef __cplusplus
}
#endif

#endif /* __JIM__H */

/*
 * Local Variables: ***
 * c-basic-offset: 4 ***
 * tab-width: 4 ***
 * End: ***
 */