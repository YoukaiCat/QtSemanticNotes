import qbs

Product {
    type: ["application"]

    Depends {
        name: "Qt"
        submodules: ["core", "widgets", "sql"]
    }

    files: ["src/**/*", "resources/**/*"]

    version: "0.2.0"

    cpp.cxxLanguageVersion: "c++17"
    cpp.defines: [
        "QT_DEPRECATED_WARNINGS",
        "QT_DISABLE_DEPRECATED_BEFORE=0x060000",
        "VERSION=" + version
    ]
    cpp.cxxFlags: [
        "-Wpedantic",
        "-pedantic-errors",
        "-Wuninitialized",
        "-Wzero-as-null-pointer-constant"
    ]
}
