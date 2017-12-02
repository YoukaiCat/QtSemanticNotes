import qbs

Product {
    type: ["application"]
    consoleApplication: false

    Depends {
        name: "Qt"
        submodules: ["core", "widgets", "sql"]
    }

    files: ["src/**/*", "resources/**/*"]

    version: "0.3.0"

    cpp.cxxLanguageVersion: "c++17"
    cpp.defines: [
        "QT_DEPRECATED_WARNINGS",
        "QT_DISABLE_DEPRECATED_BEFORE=0x060000",
        "VERSION=" + version
    ]

    Properties {
        condition: qbs.toolchain == "gcc"
        cpp.cxxFlags: [
            "-Wpedantic",
            "-pedantic-errors",
            "-Wuninitialized",
            "-Wzero-as-null-pointer-constant"
        ]
    }

    Properties {
        condition: qbs.toolchain == "msvc"
        cpp.cxxFlags: "/std:c++17"
    }

    Group {
        name: "Executable"
        fileTagsFilter: "application"
        qbs.install: true
    }
}
