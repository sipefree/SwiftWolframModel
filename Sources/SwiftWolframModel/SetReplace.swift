//
//  SetReplace.swift
//  SwiftWolframModel
//
//  Created by Simon Free on 2020-05-07.
//

import Foundation
import CSetReplace
import Combine
import Dispatch

// MARK: - SetReplace Algorithm Implementation

/// Implements the basic SetReplace algorithm API.
public final class SetReplace {
    
    // MARK: - Initialization
    
    /// Initializes a new environment with rules, initial expressions, and an update ordering spec.
    public init(
        rules: [Rule],
        initialExpressions: [[Atom]],
        orderingSpec: [Ordering],
        randomSeed: UInt32 = 0
    ) throws {
        let ruleVec = rules.to_CRulesVector()
        let exprsVec = initialExpressions.to_CAtomsVectorVector()
        let cOrdering = orderingSpec.to_COrderingSpec()
        
        var errorCode: CSetError = 0
        let set = CSet_Create(ruleVec, exprsVec, cOrdering, randomSeed) { error in
            errorCode = error
        }
        guard let _set = set else {
            throw SetReplaceError(errorCode)
        }
        
        self.set = _set
    }
    
    deinit { CSet_Destroy(self.set) }
    
    
    // MARK: - Private API
    
    /// The opaque reference to the underlying C++ `SetReplace::Set` object.
    private let set: CSetRef
    
    /// A lock that ensures only a single queue can mutate the underlying C++ object at a time.
    private let lock = DispatchSemaphore(value: 1)
    
    /// Calls the given block or function with the lock on a given queue and calls the result
    /// completion handler when the function is finished.
    private func withExecutionLock<T>(
        queue: DispatchQueue?,
        completion: @escaping (Result<T, SetReplaceError>) -> Void,
        perform: @escaping (@escaping CSetShouldAbortBlock) throws -> T
    ) -> AnyCancellable? {
        guard lock.wait(timeout: .now()) == .success else {
            completion(.failure(.locked))
            return nil
        }
        
        var isCancelled = false
        
        let cancellable = AnyCancellable {
            isCancelled = true
        }
        let shouldAbort: CSetShouldAbortBlock = {
            isCancelled ? 1 : 0
        }
        
        let queue = queue ?? DispatchQueue.global(qos: .userInitiated)
        queue.async {
            defer { self.lock.signal() }
            do {
                let result = try perform(shouldAbort)
                completion(.success(result))
            } catch let error as SetReplaceError {
                completion(.failure(error))
            } catch let error {
                fatalError(error.localizedDescription)
            }
        }
        
        return cancellable
    }
    
    /// Performs a replace-once operation synchronously.
    /// - note: Must hold lock to call.
    private func lockedReplaceOnce(
        shouldAbort: @escaping CSetShouldAbortBlock
    ) throws -> Bool {
        var errorCode: CSetError? = nil
        let result = CSet_ReplaceOnce(self.set, shouldAbort) { error in
            errorCode = error
        }
        if let code = errorCode {
            throw SetReplaceError(code)
        }
        return result == 1 ? true : false
    }
    
    /// Performs a replace operation synchronously.
    /// - note: Must hold lock to call.
    private func lockedReplace(
        step: StepSpecification,
        shouldAbort: @escaping CSetShouldAbortBlock
    ) throws -> Int {
        var errorCode: CSetError? = nil
        let stepSpec = step.to_CStepSpecification()
        let result = CSet_Replace(self.set, stepSpec, shouldAbort) { (error) in
            errorCode = error
        }
        if let code = errorCode {
            throw SetReplaceError(code)
        }
        print(TerminationReason(CSet_GetTerminationReason(self.set)).localizedDescription)
        return Int(result)
    }
    
    /// Performs a max-complete-generation operation synchronously.
    /// - note: Must hold lock to call.
    private func lockedMaxCompleteGeneration(
        shouldAbort: @escaping CSetShouldAbortBlock
    ) throws -> Generation {
        var errorCode: CSetError? = nil
        let result = CSet_MaxCompleteGeneration(self.set, shouldAbort) { (error) in
            errorCode = error
        }
        if let code = errorCode {
            throw SetReplaceError(code)
        }
        return result
    }
    
    
    // MARK: - Public API
    
    /// Returns all the current expressions in the environment.
    public var expressions: [SetExpression] {
        lock.wait()
        defer { lock.signal() }
        return [SetExpression](consuming: CSet_GetExpressions(set))
    }
   
    /// Synchronously performs a single rule application on the current thread.
    /// - returns: `true` if a replacement was made, `false` if not.
    @discardableResult
    public func replaceOnce() throws -> Bool {
        lock.wait()
        defer { lock.signal() }
        return try lockedReplaceOnce(shouldAbort: { 0 })
    }
    
    /// Synchronously performs rule applications with the given specification on the current thread.
    /// - returns: The number of replacements made.
    @discardableResult
    public func replace(step: StepSpecification) throws -> Int {
        lock.wait()
        defer { lock.signal() }
        return try lockedReplace(step: step, shouldAbort: { 0 })
    }
    
    /// Synchronously calculates the largest generation that has both been reached,
    /// and has no matches that would produce expressions with that or lower generation.
    @discardableResult
    public func maxCompleteGeneration() throws -> Generation {
        lock.wait()
        defer { lock.signal() }
        return try lockedMaxCompleteGeneration(shouldAbort: { 0 })
    }
    
    /// Asynchronously performs a single rule application on the given thread or a background thread.
    /// - returns: A cancellable that can be used to cancel the execution.
    @discardableResult
    public func asyncReplaceOnce(
        queue: DispatchQueue? = nil,
        completion: @escaping (Result<Bool, SetReplaceError>) -> Void
    ) -> AnyCancellable? {
        withExecutionLock(
            queue: queue,
            completion: completion,
            perform: self.lockedReplaceOnce(shouldAbort:)
        )
    }
    
    /// Asynchronously performs rule applications with the given
    /// specification on the given thread or a background thread.
    /// - returns: A cancellable that can be used to cancel the execution.
    @discardableResult
    public func asyncReplace(
        step: StepSpecification,
        queue: DispatchQueue? = nil,
        completion: @escaping (Result<Int, SetReplaceError>) -> Void
    ) -> AnyCancellable? {
        withExecutionLock(
            queue: queue,
            completion: completion,
            perform: { try self.lockedReplace(step: step, shouldAbort: $0) }
        )
    }
    
    /// Asynchronously calculates the largest generation that has both been reached,
    /// and has no matches that would produce expressions with that or lower generation.
    @discardableResult
    public func asyncMaxCompleteGeneration(
        queue: DispatchQueue? = nil,
        completion: @escaping (Result<Generation, SetReplaceError>) -> Void
    ) -> AnyCancellable? {
        withExecutionLock(
            queue: queue,
            completion: completion,
            perform: self.lockedMaxCompleteGeneration(shouldAbort:)
        )
    }
    
    /// Yields termination reason for the previous evaluation, or `.notTerminated` if no evaluation was done yet.
    public var terminationReason: TerminationReason {
        lock.wait()
        defer { lock.signal() }
        return TerminationReason(CSet_GetTerminationReason(self.set))
    }
    
    
    // MARK: - Update Ordering Specification
    
    /// A specification for the ordering in choosing a rule application.
    public struct Ordering: Equatable {
        
        /// Initializes the ordering with its function and direction.
        public init(function: Function, direction: Direction) {
            self.function = function
            self.direction = direction
        }
        
        /// The ordering function being used.
        public let function: Function
        
        /// The direction of the ordering.
        public let direction: Direction
        
        
        // MARK: - Ordering Function
       
        /// All possible functions available to sort matches.
        /// Random is the default that is always applied last.
        public struct Function: RawRepresentable, Equatable {
            public init(_ value: UInt64) { rawValue = value }
            public init?(rawValue: UInt64) { self.init(rawValue) }
            public let rawValue: UInt64
            
            public static let sortedExpressionIDs = Function(kCOrderingSortedExpressionIDs)
            public static let reverseSortedExpressionIDs = Function(kCOrderingReverseSortedExpressionIDs)
            public static let expressionIDs = Function(kCOrderingExpressionIDs)
            public static let ruleID = Function(kCOrderingRuleID)
        }
        
        /// Whether to sort in normal or reverse order.
        public struct Direction: RawRepresentable, Equatable {
            public init(_ value: UInt64) { rawValue = value }
            public init?(rawValue: UInt64) { self.init(rawValue) }
            public let rawValue: UInt64
            
            public static let normal = Function(kCOrderingDirectionNormal)
            public static let reverse = Function(kCOrderingDirectionReverse)
        }
        
    }
    
    
    // MARK: - Step Specification
    
    /// Specification of conditions upon which to stop evaluation.
    /// - note: Each of these is UpTo, i.e., the evolution is terminated
    ///         when the first of these, fixed point, or an abort is reached.
    public struct StepSpecification: Equatable {
        
        /// Initializes the step specification.
        public init(
            maxEvents: Int = 0,
            maxGenerationsLocal: Int = 5000,
            maxFinalAtoms: Int = .max,
            maxFinalAtomDegree: Int = .max,
            maxFinalExpressions: Int = .max
        ) {
            self.maxEvents = maxEvents
            self.maxGenerationsLocal = maxGenerationsLocal
            self.maxFinalAtoms = maxFinalAtoms
            self.maxFinalAtomDegree = maxFinalAtomDegree
            self.maxFinalExpressions = maxFinalExpressions
        }
        
        /// Total number of events to produce.
        public let maxEvents: Int
        
        /// Total number of generations. Local means the expressions of max
        /// generation will never even be matched, which means the evaluation
        /// order might be different than if the equivalent number of events is
        /// specified, and non-default evaluation order is used.
        public let maxGenerationsLocal: Int
        
        /// The evaluation will be aborted at the first attempt to apply an event,
        /// which will cause the number of atoms in the final state to go over the limit.
        public let maxFinalAtoms: Int
        
        /// Same as above, but for the maximum number of expressions a
        /// single atom is involved in.
        public let maxFinalAtomDegree: Int
        
        /// Same as for the atoms above, but for expressions.
        public let maxFinalExpressions: Int
        
        
        // MARK: Step Helpers
        
        /// A step specification to the given generation.
        public static func to(generation: Int) -> StepSpecification {
            .init(
                maxEvents: generation * 5000,
                maxGenerationsLocal: generation,
                maxFinalAtoms: .max,
                maxFinalAtomDegree: .max,
                maxFinalExpressions: .max
            )
        }
        
    }
    
    
    // MARK: - Errors
    
    /// Any of the errors that can be thrown during evaluation.
    public struct SetReplaceError: RawRepresentable, LocalizedError, Equatable {
        public init(_ value: UInt64) { rawValue = value }
        public init?(rawValue: UInt64) { self.init(rawValue) }
        public let rawValue: UInt64
        
        public static let aborted = SetReplaceError(kCSetErrorAborted)
        public static let disconnectedInputs  = SetReplaceError(kCSetErrorDisconnectedInputs)
        public static let nonPositiveAtoms  = SetReplaceError(kCSetErrorNonPositiveAtoms)
        public static let atomCountOverflow = SetReplaceError(kCSetErrorAtomCountOverflow)
        public static let locked = SetReplaceError(UInt64.max)
        
        public var errorDescription: String? {
            switch self {
            case Self.aborted: return "Aborted."
            case Self.disconnectedInputs: return "Disconnected inputs."
            case Self.nonPositiveAtoms: return "Non positive atoms."
            case Self.atomCountOverflow: return "Atom count overflow."
            case Self.locked: return "Set is busy."
            default: return nil
            }
        }
    }
    
    /// Status of evaluation / termination reason if evaluation is finished.
    public struct TerminationReason: RawRepresentable, LocalizedError, Equatable {
        public init(_ value: UInt64) { rawValue = value }
        public init?(rawValue: UInt64) { self.init(rawValue) }
        public let rawValue: UInt64
        
        public static let notTerminated = TerminationReason(kCTerminationReasonNotTerminated)
        public static let maxEvents = TerminationReason(kCTerminationReasonMaxEvents)
        public static let maxGenerationsLocal = TerminationReason(kCTerminationReasonMaxGenerationsLocal)
        public static let maxFinalAtoms = TerminationReason(kCTerminationReasonMaxFinalAtoms)
        public static let maxFinalAtomDegree = TerminationReason(kCTerminationReasonMaxFinalAtomDegree)
        public static let maxFinalExpressions = TerminationReason(kCTerminationReasonMaxFinalExpressions)
        public static let fixedPoint = TerminationReason(kCTerminationReasonFixedPoint)
        public static let aborted = TerminationReason(kCTerminationReasonAborted)
        
        public var errorDescription: String? {
            switch self {
            case .notTerminated: return "Not terminated."
            case Self.maxEvents: return "Max events."
            case Self.maxGenerationsLocal: return "Max generations local."
            case Self.maxFinalAtoms: return "Max final atoms."
            case Self.maxFinalAtomDegree: return "Max final atom degree."
            case Self.maxFinalExpressions: return "Max final expressions."
            case Self.fixedPoint: return "Fixed point."
            case Self.aborted: return "Aborted."
            default: return nil
            }
        }
    }
}
