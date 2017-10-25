import qbs

Product {
    type: ["application"]

    Depends {
        name: "Qt"
        submodules: ["core", "widgets", "sql"]
    }

    files: [
        "src/*.cpp",
        "src/database/*.h",
        "src/database/*.cpp",
        "src/entities/*.h",
        "src/entities/*.cpp",
        "src/interface/*.h",
        "src/interface/*.cpp",
        "src/interface/ui/*.ui",
        "src/models/*.h",
        "src/models/*.cpp",
        "src/trees/*.h",
        "src/trees/*.cpp",
    ]

    cpp.cxxLanguageVersion: "c++17"
    cpp.defines: [
         "QT_DEPRECATED_WARNINGS",
         "QT_DISABLE_DEPRECATED_BEFORE=0x060000"
    ]
}
