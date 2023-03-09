# =============================================================================
# Parts of Qt
# =============================================================================

QT += network
QT += widgets  # required to #include <QApplication>

# =============================================================================
# Overall configuration
# =============================================================================

CONFIG += mobility
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS


# =============================================================================
# Compiler and linker flags
# =============================================================================

gcc | clang {
    COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
    COMPILER_MAJOR_VERSION = $$str_member($$COMPILER_VERSION)
}

gcc {
    QMAKE_CXXFLAGS += -Werror  # warnings become errors
}

if (gcc | clang):!ios:!android!macx {
    !lessThan(COMPILER_MAJOR_VERSION, 9) {
        QMAKE_CXXFLAGS += -Wno-deprecated-copy
    }
}

if (ios | macx)  {
    QMAKE_CFLAGS_WARN_ON += -Wno-deprecated-copy
    QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-copy
}

gcc {
    QMAKE_CXXFLAGS += -fvisibility=hidden
}

# =============================================================================
# Build targets
# =============================================================================

TARGET = qt-network-test
TEMPLATE = app

# -----------------------------------------------------------------------------
# Architecture
# -----------------------------------------------------------------------------

linux : {
    CONFIG += static
}

# =============================================================================
# Source files
# =============================================================================

SOURCES += \
    main.cpp
