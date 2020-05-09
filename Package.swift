// swift-tools-version:5.2

import PackageDescription

let package = Package(
    name: "SwiftWolframModel",
    platforms: [.iOS(.v13), .macOS(.v10_15)],
    products: [
        .library(
            name: "SwiftWolframModel",
            targets: ["SwiftWolframModel"])
    ],
    dependencies: [
    ],
    targets: [
        .target(
            name: "SwiftWolframModel",
            dependencies: ["CSetReplace"]),
        .target(
            name: "CSetReplace",
            dependencies: ["CxxSetReplace"],
            cxxSettings: [
                .headerSearchPath("../../Vendor/SetReplace/libSetReplace")
            ]),
        .target(
            name: "CxxSetReplace",
            dependencies: []),
        .testTarget(
            name: "SwiftWolframModelTests",
            dependencies: ["SwiftWolframModel"]),
    ],
    cxxLanguageStandard: .gnucxx14
)
