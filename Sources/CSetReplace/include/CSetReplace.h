#ifndef CSetReplace_hpp
#define CSetReplace_hpp

#if __cplusplus
extern "C" {
#endif // __cplusplus

#include "inttypes.h"

_Pragma("clang assume_nonnull begin")

// MARK: - Macros

#define DeclType(_name) \
typedef struct _name _name; \
typedef const _name *_Nonnull const _name ##Ref;


// MARK: - Basic Types

typedef int64_t CAtom;

typedef int CRuleID;

typedef int64_t CExpressionID;

typedef int64_t CEventID;
extern const CEventID kCEventIDInitialCondition;
extern const CEventID kCEventIDFinalState;

typedef int64_t CGeneration;
extern const CGeneration kCGenerationInitial;


// MARK: - [Atom]

DeclType(CAtomsVector);

CAtomsVectorRef
CAtomsVector_Create(const CAtom *const atoms,
                    uint64_t count);

void
CAtomsVector_Destroy(CAtomsVectorRef atomsVector);

uint64_t
CAtomsVector_Count(CAtomsVectorRef atomsVector);

CAtom
CAtomsVector_GetAtom(CAtomsVectorRef atomsVector,
                     uint64_t index);

void
CAtomsVector_GetAll(CAtomsVectorRef atomsVector,
                    CAtom *const atoms);


// MARK: - [[Atom]]

DeclType(CAtomsVectorVector);

CAtomsVectorVectorRef
CAtomsVectorVector_Create(const CAtomsVectorRef *_Nonnull const atomVectors,
                          uint64_t count);

void
CAtomsVectorVector_Destroy(CAtomsVectorVectorRef atomsVectorVector);

uint64_t
CAtomsVectorVector_Count(CAtomsVectorVectorRef atomsVectorVector);

CAtomsVectorRef
CAtomsVectorVector_GetAtomsVector(CAtomsVectorVectorRef atomsVectorVector,
                                  uint64_t index);

void
CAtomsVectorVector_GetAll(CAtomsVectorVectorRef atomsVectorVector,
                          CAtomsVector *_Nullable *_Nonnull const atomsVectors);


// MARK: - SetExpression

DeclType(CSetExpression);

CSetExpressionRef
CSetExpression_Create(CAtomsVectorRef atoms,
                      CEventID creatorEvent,
                      CEventID destroyerEvent,
                      CGeneration generation);

void
CSetExpression_Destroy(CSetExpressionRef setExpr);

CAtomsVectorRef
CSetExpression_GetAtoms(CSetExpressionRef setExpr);

void
CSetExpression_SetAtoms(CSetExpressionRef setExpr,
                        CAtomsVectorRef newAtoms);

CEventID
CSetExpression_GetCreatorEvent(CSetExpressionRef setExpr);

void
CSetExpression_SetCreatorEvent(CSetExpressionRef setExpr,
                               CEventID newCreatorEvent);

CEventID
CSetExpression_GetDestroyerEvent(CSetExpressionRef setExpr);

void
CSetExpression_SetDestroyerEvent(CSetExpressionRef setExpr,
                                 CEventID newDestroyerEvent);

CGeneration
CSetExpression_GetGeneration(CSetExpressionRef setExpr);

void
CSetExpression_SetGeneration(CSetExpressionRef setExpr,
                             CGeneration newGeneration);


// MARK: - [SetExpression]

DeclType(CSetExpressionsVector);

CSetExpressionsVectorRef
CSetExpressionsVector_Create(const CSetExpressionRef *_Nonnull const expressions,
                             uint64_t count);

void
CSetExpressionsVector_Destroy(CSetExpressionsVectorRef setsVector);

uint64_t
CSetExpressionsVector_Count(CSetExpressionsVectorRef setsVector);

CSetExpressionRef
CSetExpressionsVector_GetSetExpression(CSetExpressionsVectorRef setsVector,
                                       uint64_t index);

void
CSetExpressionsVector_GetAll(CSetExpressionsVectorRef setsVector,
                             CSetExpression *_Nullable *_Nonnull const setExprs);


// MARK: - Rule

DeclType(CRule);

CRuleRef
CRule_Create(CAtomsVectorVectorRef inputs,
             CAtomsVectorVectorRef outputs);

void
CRule_Destroy(CRuleRef rule);

CAtomsVectorVectorRef
CRule_GetInputs(CRuleRef rule);

CAtomsVectorVectorRef
CRule_GetOutputs(CRuleRef rule);


// MARK: - [Rule]

DeclType(CRulesVector);

CRulesVectorRef
CRulesVector_Create(CRuleRef *_Nonnull rules,
                    uint64_t count);

void
CRulesVector_Destroy(CRulesVectorRef rulesVector);

uint64_t
CRulesVector_Count(CRulesVectorRef rulesVector);

CRuleRef
CRulesVector_GetRule(CRulesVectorRef rulesVector,
                     uint64_t index);

void
CRulesVector_GetAll(CRulesVectorRef rulesVector,
                    CRule *_Nullable *_Nonnull const rules);


// MARK: - Ordering

typedef uint64_t COrderingFunction;
extern const COrderingFunction kCOrderingSortedExpressionIDs;
extern const COrderingFunction kCOrderingReverseSortedExpressionIDs;
extern const COrderingFunction kCOrderingExpressionIDs;
extern const COrderingFunction kCOrderingRuleID;

typedef uint64_t COrderingDirection;
extern const COrderingDirection kCOrderingDirectionNormal;
extern const COrderingDirection kCOrderingDirectionReverse;

typedef struct COrdering {
    COrderingFunction orderingFunction;
    COrderingDirection orderingDirection;
} COrdering;


// MARK: - OrderingSpec

DeclType(COrderingSpec);

COrderingSpecRef
COrderingSpec_Create(const COrdering *const specs,
                     uint64_t count);

void
COrderingSpec_Destroy(COrderingSpecRef spec);


// MARK: - StepSpecification

typedef struct CStepSpecification {
    int64_t maxEvents;
    int64_t maxGenerationsLocal;
    int64_t maxFinalAtoms;
    int64_t maxFinalAtomDegree;
    int64_t maxFinalExpressions;
} CStepSpecification;


// MARK: - TerminationReason

typedef uint64_t CTerminationReason;
extern const CTerminationReason kCTerminationReasonNotTerminated;
extern const CTerminationReason kCTerminationReasonMaxEvents;
extern const CTerminationReason kCTerminationReasonMaxGenerationsLocal;
extern const CTerminationReason kCTerminationReasonMaxFinalAtoms;
extern const CTerminationReason kCTerminationReasonMaxFinalAtomDegree;
extern const CTerminationReason kCTerminationReasonMaxFinalExpressions;
extern const CTerminationReason kCTerminationReasonFixedPoint;
extern const CTerminationReason kCTerminationReasonAborted;


// MARK: - Error

typedef uint64_t CSetError;
extern const CSetError kCSetErrorAborted;
extern const CSetError kCSetErrorDisconnectedInputs;
extern const CSetError kCSetErrorNonPositiveAtoms;
extern const CSetError kCSetErrorAtomCountOverflow;


// MARK: - Set

typedef uint64_t(^CSetShouldAbortBlock)(void);
typedef void(^CHandleErrorBlock)(CSetError);

DeclType(CSet);

CSet *_Nullable
CSet_Create(CRulesVectorRef rules,
            CAtomsVectorVectorRef initialExpressions,
            COrderingSpecRef orderingSpec,
            unsigned int randomSeed,
            CHandleErrorBlock handleError);

void
CSet_Destroy(CSetRef set);

int64_t
CSet_ReplaceOnce(CSetRef set,
                 CSetShouldAbortBlock shouldAbort,
                 CHandleErrorBlock handleError);

int64_t
CSet_Replace(CSetRef set,
             CStepSpecification stepSpec,
             CSetShouldAbortBlock shouldAbort,
             CHandleErrorBlock handleError);

CSetExpressionsVectorRef
CSet_GetExpressions(CSetRef set);

CGeneration
CSet_MaxCompleteGeneration(CSetRef set,
                           CSetShouldAbortBlock shouldAbort,
                           CHandleErrorBlock handleError);

CTerminationReason
CSet_GetTerminationReason(CSetRef set);

_Pragma("clang assume_nonnull end")

#if __cplusplus
}
#endif // __cplusplus

#endif /* CSetReplace_hpp */
