#include "CSetReplace.h"
#include "Set.hpp"
#include <cstdio>

const CEventID kCEventIDInitialCondition = SetReplace::initialConditionEvent;
const CEventID kCEventIDFinalState = SetReplace::finalStateEvent;
const CGeneration kCGenerationInitial = SetReplace::initialGeneration;

using namespace SetReplace;

// MARK: - CAtomsVector

CAtomsVectorRef CAtomsVector_Create(const CAtom *const atoms, uint64_t count) {
    AtomsVector *vatoms = new AtomsVector();
    vatoms->assign(atoms, atoms+count);
    return (CAtomsVectorRef)vatoms;
}

void CAtomsVector_Destroy(CAtomsVectorRef atomsVector) {
    delete (AtomsVector *)atomsVector;
}

uint64_t CAtomsVector_Count(CAtomsVectorRef atomsVector) {
    AtomsVector *vatoms = (AtomsVector *)atomsVector;
    return vatoms->size();
}

CAtom CAtomsVector_GetAtom(CAtomsVectorRef atomsVector, uint64_t index) {
    AtomsVector *vatoms = (AtomsVector *)atomsVector;
    return (CAtom)vatoms->at(index);
}

void CAtomsVector_GetAll(CAtomsVectorRef atomsVector, CAtom *const atoms) {
    AtomsVector *vatoms = (AtomsVector *)atomsVector;
    copy(vatoms->begin(), vatoms->end()+vatoms->size(), atoms);
}

// MARK: - CAtomsVectorVector

CAtomsVectorVectorRef CAtomsVectorVector_Create(const CAtomsVectorRef *const atomVectors, uint64_t count) {
    AtomsVector *const *const _atomVectors = (AtomsVector *const *const)atomVectors;
    
    std::vector<AtomsVector> *atomsVectorVector = new std::vector<AtomsVector>();
    for (uint64_t i = 0; i < count; i++) {
        AtomsVector *atomsVector = _atomVectors[i];
        atomsVectorVector->push_back(*atomsVector);
    }
    
    return (CAtomsVectorVectorRef)atomsVectorVector;
}

void CAtomsVectorVector_Destroy(CAtomsVectorVectorRef atomsVectorVector) {
    std::vector<AtomsVector> *_avv = (std::vector<AtomsVector> *)atomsVectorVector;
    delete _avv;
}

uint64_t CAtomsVectorVector_Count(CAtomsVectorVectorRef atomsVectorVector) {
    std::vector<AtomsVector> *_avv = (std::vector<AtomsVector> *)atomsVectorVector;
    return _avv->size();
}
CAtomsVectorRef CAtomsVectorVector_GetAtomsVector(CAtomsVectorVectorRef atomsVectorVector, uint64_t index) {
    std::vector<AtomsVector> *_avv = (std::vector<AtomsVector> *)atomsVectorVector;
    AtomsVector *atomVector = new AtomsVector(_avv->at(index));
    return (CAtomsVectorRef)atomVector;
}

void CAtomsVectorVector_GetAll(CAtomsVectorVectorRef atomsVectorVector, CAtomsVector *_Nullable *const atomsVectors) {
    std::vector<AtomsVector> *_avv = (std::vector<AtomsVector> *)atomsVectorVector;
    for (uint64_t i = 0; i < _avv->size(); i++) {
        AtomsVector *vec = new AtomsVector(_avv->at(i));
        atomsVectors[i] = (CAtomsVector *)vec;
    }
}

// MARK: - CSetExpression

CSetExpressionRef CSetExpression_Create(CAtomsVectorRef atoms, CEventID creatorEvent, CEventID destroyerEvent, CGeneration generation) {
    AtomsVector *_atoms = (AtomsVector *)atoms;
    SetExpression *expr = new SetExpression();
    expr->atoms = *_atoms;
    expr->creatorEvent = creatorEvent;
    expr->destroyerEvent = destroyerEvent;
    expr->generation = generation;
    
    return (CSetExpressionRef)expr;
}

void CSetExpression_Destroy(CSetExpressionRef setExpr) {
    SetExpression *expr = (SetExpression *)setExpr;
    delete expr;
}

CAtomsVectorRef /*owned*/ CSetExpression_GetAtoms(CSetExpressionRef setExpr) {
    SetExpression *expr = (SetExpression *)setExpr;
    AtomsVector *atoms = new AtomsVector(expr->atoms);
    return (CAtomsVectorRef)atoms;
}

void CSetExpression_SetAtoms(CSetExpressionRef setExpr, CAtomsVectorRef /*unowned*/ newAtoms) {
    SetExpression *expr = (SetExpression *)setExpr;
    AtomsVector *atoms = (AtomsVector *)newAtoms;
    expr->atoms = *atoms;
}

CEventID CSetExpression_GetCreatorEvent(CSetExpressionRef setExpr) {
    SetExpression *expr = (SetExpression *)setExpr;
    return expr->creatorEvent;
}

void CSetExpression_SetCreatorEvent(CSetExpressionRef setExpr, CEventID newCreatorEvent) {
    SetExpression *expr = (SetExpression *)setExpr;
    expr->creatorEvent = newCreatorEvent;
}

CEventID CSetExpression_GetDestroyerEvent(CSetExpressionRef setExpr) {
    SetExpression *expr = (SetExpression *)setExpr;
    return expr->destroyerEvent;
}

void CSetExpression_SetDestroyerEvent(CSetExpressionRef setExpr, CEventID newDestroyerEvent) {
    SetExpression *expr = (SetExpression *)setExpr;
    expr->destroyerEvent = newDestroyerEvent;
}

CGeneration CSetExpression_GetGeneration(CSetExpressionRef setExpr) {
    SetExpression *expr = (SetExpression *)setExpr;
    return expr->generation;
}

void CSetExpression_SetGeneration(CSetExpressionRef setExpr, CGeneration newGeneration) {
    SetExpression *expr = (SetExpression *)setExpr;
    expr->generation = newGeneration;
}

// MARK: CSetExpressionsVector

CSetExpressionsVectorRef CSetExpressionsVector_Create(const CSetExpressionRef *const expressions, uint64_t count) {
    std::vector<SetExpression> *vec = new std::vector<SetExpression>();
    for (uint64_t i = 0; i < count; i++) {
        SetExpression expr = *((SetExpression *)expressions[i]);
        vec->push_back(expr);
    }
    return (CSetExpressionsVectorRef)vec;
}

void CSetExpressionsVector_Destroy(CSetExpressionsVectorRef setsVector) {
    std::vector<SetExpression> *vec = (std::vector<SetExpression> *)setsVector;
    delete vec;
}

uint64_t CSetExpressionsVector_Count(CSetExpressionsVectorRef setsVector) {
    std::vector<SetExpression> *vec = (std::vector<SetExpression> *)setsVector;
    return vec->size();
}

CSetExpressionRef /*owned*/ CSetExpressionsVector_GetSetExpression(CSetExpressionsVectorRef setsVector, uint64_t index) {
    std::vector<SetExpression> *vec = (std::vector<SetExpression> *)setsVector;
    SetExpression *expr = new SetExpression(vec->at(index));
    return (CSetExpressionRef)expr;
}

void CSetExpressionsVector_GetAll(CSetExpressionsVectorRef setsVector, /*unowned*/ CSetExpression *_Nullable *const setExprs) {
    std::vector<SetExpression> *vec = (std::vector<SetExpression> *)setsVector;
    for (uint64_t i = 0; i < vec->size(); i++) {
        SetExpression *expr = new SetExpression(vec->at(i));
        setExprs[i] = (CSetExpression *)expr;
    }
}

// MARK: - CRule

CRuleRef CRule_Create(CAtomsVectorVectorRef inputs, CAtomsVectorVectorRef outputs) {
    std::vector<AtomsVector> *_inputs = (std::vector<AtomsVector> *)inputs;
    std::vector<AtomsVector> *_outputs = (std::vector<AtomsVector> *)outputs;
    Rule *_rule = new Rule(Rule{*_inputs, *_outputs});
    return (CRuleRef)_rule;
}

void CRule_Destroy(CRuleRef rule) {
    Rule *_rule = (Rule *)rule;
    delete _rule;
}

CAtomsVectorVectorRef CRule_GetInputs(CRuleRef rule) {
    Rule *_rule = (Rule *)rule;
    std::vector<AtomsVector> *inputs = new std::vector<AtomsVector>(_rule->inputs);
    return (CAtomsVectorVectorRef)inputs;
}

CAtomsVectorVectorRef CRule_GetOutputs(CRuleRef rule) {
    Rule *_rule = (Rule *)rule;
    std::vector<AtomsVector> *outputs = new std::vector<AtomsVector>(_rule->outputs);
    return (CAtomsVectorVectorRef)outputs;
}

// MARK: - CRulesVector

CRulesVectorRef CRulesVector_Create(CRuleRef  *rules, uint64_t count) {
    std::vector<Rule> *vec = new std::vector<Rule>();
    for (uint64_t i = 0; i < count; i++) {
        Rule rule = *((Rule *)rules[i]);
        vec->push_back(rule);
    }
    return (CRulesVectorRef)vec;
}

void CRulesVector_Destroy(CRulesVectorRef rulesVector) {
    std::vector<Rule> *vec = (std::vector<Rule> *)rulesVector;
    delete vec;
}

uint64_t CRulesVector_Count(CRulesVectorRef rulesVector) {
    std::vector<Rule> *vec = (std::vector<Rule> *)rulesVector;
    return vec->size();
}

CRuleRef /*owned*/ CRulesVector_GetRule(CRulesVectorRef rulesVector, uint64_t index) {
    std::vector<Rule> *vec = (std::vector<Rule> *)rulesVector;
    Rule *rule = new Rule(vec->at(index));
    return (CRuleRef)rule;
}

void CRulesVector_GetAll(CRulesVectorRef rulesVector, /*owned*/ CRule **const rules) {
    std::vector<Rule> *vec = (std::vector<Rule> *)rulesVector;
    for (uint64_t i = 0; i < vec->size(); i++) {
        Rule *rule = new Rule(vec->at(i));
        rules[i] = (CRule *)rule;
    }
}


// MARK: - COrderingFunction

template <typename T>
constexpr auto operator+(T e) noexcept
    -> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
{
    return static_cast<std::underlying_type_t<T>>(e);
}

const COrderingFunction kCOrderingSortedExpressionIDs = (+Matcher::OrderingFunction::SortedExpressionIDs);
const COrderingFunction kCOrderingReverseSortedExpressionIDs = (+Matcher::OrderingFunction::ReverseSortedExpressionIDs);
const COrderingFunction kCOrderingExpressionIDs = (+Matcher::OrderingFunction::ExpressionIDs);
const COrderingFunction kCOrderingRuleID = (+Matcher::OrderingFunction::RuleID);


const COrderingDirection kCOrderingDirectionNormal = (+Matcher::OrderingDirection::Normal);
const COrderingDirection kCOrderingDirectionReverse = (+Matcher::OrderingDirection::Reverse);

std::pair<Matcher::OrderingFunction, Matcher::OrderingDirection> ordering_to_pair(COrdering ordering) {
    Matcher::OrderingFunction function = static_cast<Matcher::OrderingFunction>(ordering.orderingFunction);
    Matcher::OrderingDirection direction = static_cast<Matcher::OrderingDirection>(ordering.orderingDirection);
    return std::make_pair(function, direction);
}

COrderingSpecRef COrderingSpec_Create(COrdering const * const specs, uint64_t count) {
    Matcher::OrderingSpec *spec = new Matcher::OrderingSpec();
    
    for (uint64_t i = 0; i < count; i++) {
        COrdering ordering = specs[i];
        auto pair = ordering_to_pair(ordering);
        spec->push_back(pair);
    }
    
    return (COrderingSpecRef)spec;
}

void COrderingSpec_Destroy(COrderingSpecRef spec) {
    Matcher::OrderingSpec *_spec = (Matcher::OrderingSpec *)spec;
    delete _spec;
}

const CTerminationReason kCTerminationReasonNotTerminated = (+Set::TerminationReason::NotTerminated);
const CTerminationReason kCTerminationReasonMaxEvents = (+Set::TerminationReason::MaxEvents);
const CTerminationReason kCTerminationReasonMaxGenerationsLocal = (+Set::TerminationReason::MaxGenerationsLocal);
const CTerminationReason kCTerminationReasonMaxFinalAtoms = (+Set::TerminationReason::MaxFinalAtoms);
const CTerminationReason kCTerminationReasonMaxFinalAtomDegree = (+Set::TerminationReason::MaxFinalAtomDegree);
const CTerminationReason kCTerminationReasonMaxFinalExpressions = (+Set::TerminationReason::MaxFinalExpressions);
const CTerminationReason kCTerminationReasonFixedPoint = (+Set::TerminationReason::FixedPoint);
const CTerminationReason kCTerminationReasonAborted = (+Set::TerminationReason::Aborted);

const CSetError kCSetErrorAborted = (+Set::Error::Aborted);
const CSetError kCSetErrorDisconnectedInputs = (+Set::Error::DisconnectedInputs);
const CSetError kCSetErrorNonPositiveAtoms = (+Set::Error::NonPositiveAtoms);
const CSetError kCSetErrorAtomCountOverflow = (+Set::Error::AtomCountOverflow);


CSet *_Nullable CSet_Create(CRulesVectorRef rules, CAtomsVectorVectorRef initialExpressions, COrderingSpecRef orderingSpec, unsigned int randomSeed, CHandleErrorBlock handleError) {
    
    const std::vector<Rule> _rules = *(std::vector<Rule> *)rules;
    const std::vector<AtomsVector> _initialExpressions = *(std::vector<AtomsVector> *)initialExpressions;
    const Matcher::OrderingSpec _orderingSpec = *(Matcher::OrderingSpec *)orderingSpec;
    
    try {
        Set *set = new Set(_rules, _initialExpressions, _orderingSpec, randomSeed);
        return (CSet *)set;
    } catch(Set::Error error) {
        CSetError cError = (+error);
        handleError(cError);
        return nullptr;
    }
}

void CSet_Destroy(CSetRef set) {
    Set *_set = (Set *)set;
    delete _set;
}

int64_t CSet_ReplaceOnce(CSetRef set, CSetShouldAbortBlock shouldAbort, CHandleErrorBlock handleError) {
    Set *_set = (Set *)set;
    
    try {
        return _set->replaceOnce(shouldAbort);
    } catch(Set::Error error) {
        CSetError cError = (+error);
        handleError(cError);
        return 0;
    }
}

int64_t CSet_Replace(CSetRef set, CStepSpecification stepSpec, CSetShouldAbortBlock shouldAbort, CHandleErrorBlock handleError) {
    Set *_set = (Set *)set;
    Set::StepSpecification _stepSpec = Set::StepSpecification{
        stepSpec.maxEvents,
        stepSpec.maxGenerationsLocal,
        stepSpec.maxFinalAtoms,
        stepSpec.maxFinalAtomDegree,
        stepSpec.maxFinalExpressions
    };
    
    try {
        return _set->replace(_stepSpec, shouldAbort);
    } catch(Set::Error error) {
        CSetError cError = (+error);
        handleError(cError);
        return 0;
    }
}

CSetExpressionsVectorRef CSet_GetExpressions(CSetRef set) {
    Set *_set = (Set *)set;
    
    std::vector<SetExpression> *vec = new std::vector<SetExpression>(_set->expressions());
    
    return (CSetExpressionsVectorRef)vec;
}

CGeneration CSet_MaxCompleteGeneration(CSetRef set, CSetShouldAbortBlock shouldAbort, CHandleErrorBlock handleError) {
    Set *_set = (Set *)set;
    
    try {
        return _set->maxCompleteGeneration(shouldAbort);
    } catch(Set::Error error) {
        CSetError cError = (+error);
        handleError(cError);
        return 0;
    }
}

CTerminationReason CSet_GetTerminationReason(CSetRef set) {
    Set *_set = (Set *)set;
    
    return (CTerminationReason)static_cast<int>(_set->terminationReason());
}
