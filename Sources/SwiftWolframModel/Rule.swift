//
//  Rule.swift
//  SwiftWolframModel
//
//  Created by Simon Free on 2020-05-07.
//

import Foundation

public struct Rule:
    Equatable,
    Hashable
{
    
    public init(inputs: [[Atom]], outputs: [[Atom]]) {
        self.inputs = inputs
        self.outputs = outputs
    }
    
    public let inputs: [[Atom]]
    
    public let outputs: [[Atom]]
    
}
