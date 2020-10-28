// swift-tools-version:5.2
// The swift-tools-version declares the minimum version of Swift required to build this package.
import PackageDescription

let package = Package(
    name: "FfeadCpp",
    products: [
        .library(name: "FfeadCpp", targets: ["FfeadCpp"]),
    ],
    targets: [
        .systemLibrary(
        	name: "FfeadCpp"
        )
    ]
)