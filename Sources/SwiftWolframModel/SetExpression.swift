//
//  SetExpression.swift
//  SwiftWolframModel
//
//  Created by Simon Free on 2020-05-07.
//

import Foundation

public struct SetExpression:
    Equatable,
    Hashable,
    CustomStringConvertible
{
    
    public init(
        atoms: [Atom],
        creatorEvent: EventID,
        destroyerEvent: EventID = kFinalStateEvent,
        generation: Generation
    ) {
        self.atoms = atoms
        self.creatorEvent = creatorEvent
        self.destroyerEvent = destroyerEvent
        self.generation = generation
    }
    
    public var atoms: [Atom]
    
    public var creatorEvent: EventID
    
    public var destroyerEvent: EventID
    
    public var generation: Generation
    
    public var description: String {
        String(describing: atoms)
    }
    
}
