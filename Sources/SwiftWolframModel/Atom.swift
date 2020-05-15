//
//  Atom.swift
//  SwiftWolframModel
//
//  Created by Simon Free on 2020-05-07.
//

import Foundation

// MARK: Atom

/// Identifiers for atoms, which are the elements of expressions, i.e., vertices in the graph.
public struct Atom:
    Equatable,
    Hashable,
    Comparable,
    RawRepresentable,
    ExpressibleByIntegerLiteral,
    CustomStringConvertible
{
    
    public typealias RawValue = Int64
    
    public init<IntType: BinaryInteger>(_ value: IntType) {
        self.rawValue = Int64(value)
    }
    
    public init(integerLiteral value: Int64) {
        self.rawValue = value
    }
    
    public init?(rawValue: Int64) {
        self.rawValue = rawValue
    }
    
    public var rawValue: Int64
    
    public var description: String { "\(rawValue)" }
    
    public static func < (lhs: Atom, rhs: Atom) -> Bool {
        lhs.rawValue < rhs.rawValue
    }
    
}

