import XCTest
@testable import SwiftWolframModel

final class SwiftWolframModelTests: XCTestCase {
    func testExample() {
        let set = try! SetReplace(
            rules: [Rule(inputs: [[-1, -2]], outputs: [[-1, -2], [-2, -3]])],
            initialExpressions: [[1, 2]],
            orderingSpec: [],
            randomSeed: 1
        )
        print(0, set.expressions)
        try! set.replace(step: .to(generation: 10))
        
        for i in 1...10 {
            print(i, set.expressions.filter { $0.generation == i })
        }
        
    }
}
