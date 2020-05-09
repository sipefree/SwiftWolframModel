//
//  CInterop.swift
//  SwiftWolframModel
//
//  Created by Simon Free on 2020-05-07.
//

import Foundation
import CSetReplace

public typealias EventID = CEventID
public typealias Generation = CGeneration

public let kInitialConditionEvent: EventID = 0
public let kFinalStateEvent: EventID = -1
public let kInitialGeneration: Generation = 0

// MARK: - [Atom]

extension Array where Element == Atom {
    
    internal init(consuming vector: CAtomsVectorRef) {
        let count = Int(CAtomsVector_Count(vector))
        self.init(repeating: 0, count: count)
        for i in 0..<count {
            self[i] = Atom(CAtomsVector_GetAtom(vector, UInt64(i)))
        }
        CAtomsVector_Destroy(vector)
    }

    internal func to_CAtomsVector() -> CAtomsVectorRef {
        return withContiguousStorageIfAvailable { buffer in
            buffer.withMemoryRebound(to: CAtom.self) { atomBuffer in
                CAtomsVector_Create(atomBuffer.baseAddress!, UInt64(atomBuffer.count))
            }
        }!
    }

}

// MARK: - [[Atom]]

extension Array where Element == [Atom] {
    
    internal init(consuming vector: CAtomsVectorVectorRef) {
        let vectorCount = Int(CAtomsVectorVector_Count(vector))
        self.init(repeating: [], count: vectorCount)
        
        for vectorIndex in 0..<vectorCount {
            let vector = CAtomsVectorVector_GetAtomsVector(vector, UInt64(vectorIndex))
            self[vectorIndex] = [Atom](consuming: vector)
        }
        
        CAtomsVectorVector_Destroy(vector)
    }
    
    internal func to_CAtomsVectorVector() -> CAtomsVectorVectorRef {
        let vectors = map { $0.to_CAtomsVector() }
        let vectorVector = vectors.withContiguousStorageIfAvailable { buffer in
            CAtomsVectorVector_Create(buffer.baseAddress!, UInt64(buffer.count))
        }!
        for vector in vectors {
            CAtomsVector_Destroy(vector)
        }
        return vectorVector
    }
    
}

// MARK: - SetExpression

extension SetExpression {
    
    internal init(consuming cSetExpression: CSetExpressionRef) {
        self.init(
            atoms: [Atom](consuming: CSetExpression_GetAtoms(cSetExpression)),
            creatorEvent: CSetExpression_GetCreatorEvent(cSetExpression),
            destroyerEvent: CSetExpression_GetDestroyerEvent(cSetExpression),
            generation: CSetExpression_GetGeneration(cSetExpression)
        )
        CSetExpression_Destroy(cSetExpression)
    }
    
    internal func to_CSetExpression() -> CSetExpressionRef {
        let atomVec = atoms.to_CAtomsVector()
        let expr = CSetExpression_Create(
            atomVec,
            creatorEvent,
            destroyerEvent,
            generation
        )
        CAtomsVector_Destroy(atomVec)
        return expr
    }
    
}

// MARK: - [SetExpression]

extension Array where Element == SetExpression {
    
    internal init(consuming vector: CSetExpressionsVectorRef) {
        let count = Int(CSetExpressionsVector_Count(vector))
        self.init()
        for i in 0..<count {
            let expr = CSetExpressionsVector_GetSetExpression(vector, UInt64(i))
            self.append(SetExpression(consuming: expr))
        }
        CAtomsVector_Destroy(vector)
    }

    internal func to_CSetExpressionsVector() -> CSetExpressionsVectorRef {
        let cexprs = self.map { $0.to_CSetExpression() }
        let exprVector = cexprs.withContiguousStorageIfAvailable { buffer in
            CSetExpressionsVector_Create(buffer.baseAddress!, UInt64(buffer.count))
        }!
        for cexpr in cexprs {
            CSetExpression_Destroy(cexpr)
        }
        return exprVector
    }

}

// MARK: - Rule

extension Rule {
    
    internal init(consuming crule: CRuleRef) {
        let cinputs = CRule_GetInputs(crule)
        let coutputs = CRule_GetOutputs(crule)
        let inputs = [[Atom]](consuming: cinputs)
        let outputs = [[Atom]](consuming: coutputs)
        
        self.init(inputs: inputs, outputs: outputs)
        
        CRule_Destroy(crule)
    }
    
    internal func to_CRule() -> CRuleRef {
        let cinputs = inputs.to_CAtomsVectorVector()
        let coutputs = outputs.to_CAtomsVectorVector()
        let crule = CRule_Create(cinputs, coutputs)
        CAtomsVectorVector_Destroy(cinputs)
        CAtomsVectorVector_Destroy(coutputs)
        return crule
    }
    
}


// MARK: - [Rule]

extension Array where Element == Rule {
    
    internal init(consuming vector: CRulesVectorRef) {
        let count = Int(CRulesVector_Count(vector))
        self.init()
        for i in 0..<count {
            let crule = CRulesVector_GetRule(vector, UInt64(i))
            self.append(Rule(consuming: crule))
        }
        CAtomsVector_Destroy(vector)
    }
    
    internal func to_CRulesVector() -> CRulesVectorRef {
        let crules = self.map { $0.to_CRule() }
        let ruleVector = crules.withContiguousStorageIfAvailable { buffer in
            CRulesVector_Create(buffer.baseAddress!, UInt64(buffer.count))
        }!
        for crule in crules {
            CRule_Destroy(crule)
        }
        return ruleVector
    }
    
}

// MARK: - SetReplace.Ordering

extension SetReplace.Ordering {
    
    internal init(_ cordering: COrdering) {
        self.init(
            function: Function(cordering.orderingFunction),
            direction: Direction(cordering.orderingDirection)
        )
    }
    
    internal func to_COrdering() -> COrdering {
        COrdering(
            orderingFunction: function.rawValue,
            orderingDirection: direction.rawValue
        )
    }
    
}


// MARK: - [SetReplace.Ordering]

extension Array where Element == SetReplace.Ordering {
    
    internal func to_COrderingSpec() -> COrderingSpecRef {
        let corderings = map { $0.to_COrdering() }
        return corderings.withContiguousStorageIfAvailable { buffer in
            COrderingSpec_Create(buffer.baseAddress!, UInt64(buffer.count))
        }!
    }
    
}


// MARK: SetReplace.StepSpecification

extension SetReplace.StepSpecification {
    
    internal func to_CStepSpecification() -> CStepSpecification {
        CStepSpecification(
            maxEvents: Int64(maxEvents),
            maxGenerationsLocal: Int64(maxGenerationsLocal),
            maxFinalAtoms: Int64(maxFinalAtoms),
            maxFinalAtomDegree: Int64(maxFinalAtomDegree),
            maxFinalExpressions: Int64(maxFinalExpressions)
        )
    }
    
}
