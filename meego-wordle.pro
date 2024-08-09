PREFIX = meego
NAME = wordle
CONFIG += meegotouch qdeclarative-boostable
QT += declarative dbus

TARGET = $${PREFIX}-$${NAME}

DEFINES += Json=QJson
DEFINES += HARMATTAN HARMATTAN_BOOSTER
LIBS += -lqjson

QMAKE_CXXFLAGS *= -Wno-unused-parameter -Wno-psabi

INSTALL_PREFIX = /opt/$${TARGET}

CONFIG(debug, debug|release) {
    DEFINES += DEBUG HARBOUR_DEBUG
}

# Directories

HARBOUR_LIB_DIR = $$_PRO_FILE_PWD_/harbour-lib

OTHER_FILES += \
    LICENSE \
    README.md \
    meego/README.md \
    meego/*.png \
    meego/*.desktop \
    meego/qml/*.js \
    meego/qml/*.qml \
    meego/qml/images/*.svg \
    translations/*.ts \
    qtc_packaging/debian_harmattan/*

# harbour-lib

HARBOUR_LIB_INCLUDE = $${HARBOUR_LIB_DIR}/include
HARBOUR_LIB_SRC = $${HARBOUR_LIB_DIR}/src
HARBOUR_LIB_QML = $${HARBOUR_LIB_DIR}/qml

INCLUDEPATH += \
    $${HARBOUR_LIB_INCLUDE}

HEADERS += \
    $${HARBOUR_LIB_INCLUDE}/HarbourDebug.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourDisplayBlanking.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourJson.h \
    $${HARBOUR_LIB_INCLUDE}/HarbourSystemState.h \
    $${HARBOUR_LIB_SRC}/HarbourMce.h

SOURCES += \
    $${HARBOUR_LIB_SRC}/HarbourJson.cpp \
    $${HARBOUR_LIB_SRC}/HarbourDisplayBlanking.cpp \
    $${HARBOUR_LIB_SRC}/HarbourMce.cpp \
    $${HARBOUR_LIB_SRC}/HarbourSystemState.cpp

# App

HEADERS += \
  src/Wordle.h \
  src/WordleBoardModel.h \
  src/WordleDefs.h \
  src/WordleGame.h \
  src/WordleHistory.h \
  src/WordleLanguage.h \
  src/WordleLanguageModel.h \
  src/WordleSettings.h \
  src/WordleTypes.h

SOURCES += \
  src/main.cpp \
  src/Wordle.cpp \
  src/WordleBoardModel.cpp \
  src/WordleGame.cpp \
  src/WordleHistory.cpp \
  src/WordleLanguage.cpp \
  src/WordleLanguageModel.cpp \
  src/WordleSettings.cpp

INCLUDEPATH += \
  meego/src

HEADERS += \
  meego/src/MeegoWordle.h

SOURCES += \
  meego/src/MeegoWordle.cpp

# Data

# Translations
TR_BASE = harbour-$${NAME}
TR_DIR = $$_PRO_FILE_PWD_/translations
TR_DEPLOY = $$INSTALL_PREFIX/translations
qtPrepareTool(LRELEASE,lrelease)

TR_EN_QM = $${TR_BASE}-en.qm
TR_EN_TS = $$TR_DIR/$${TR_BASE}.ts
lrelease_en.input = TR_EN_TS
lrelease_en.output = $$TR_DIR/$$TR_EN_QM
lrelease_en.commands = $$LRELEASE -idbased $$TR_EN_TS -qm $$TR_DIR/$$TR_EN_QM
lrelease_en.CONFIG = no_link
lrelease_en.variable_out = PRE_TARGETDEPS
translation_en.files = translations/$$TR_EN_QM
translation_en.path = $$TR_DEPLOY
QMAKE_EXTRA_COMPILERS += lrelease_en
INSTALLS += translation_en

TR_ES_QM = $${TR_BASE}-es.qm
TR_ES_TS = $$TR_DIR/$${TR_BASE}-es.ts
lrelease_es.input = TR_ES_TS
lrelease_es.output = $$TR_DIR/$$TR_ES_QM
lrelease_es.commands = $$LRELEASE -idbased $$TR_ES_TS -qm $$TR_DIR/$$TR_ES_QM
lrelease_es.CONFIG = no_link
lrelease_es.variable_out = PRE_TARGETDEPS
translation_es.files = translations/$$TR_ES_QM
translation_es.path = $$TR_DEPLOY
QMAKE_EXTRA_COMPILERS += lrelease_es
INSTALLS += translation_es

TR_FI_QM = $${TR_BASE}-fi.qm
TR_FI_TS = $$TR_DIR/$${TR_BASE}-fi.ts
lrelease_fi.input = TR_FI_TS
lrelease_fi.output = $$TR_DIR/$$TR_FI_QM
lrelease_fi.commands = $$LRELEASE -idbased $$TR_FI_TS -qm $$TR_DIR/$$TR_FI_QM
lrelease_fi.CONFIG = no_link
lrelease_fi.variable_out = PRE_TARGETDEPS
translation_fi.files = translations/$$TR_FI_QM
translation_fi.path = $$TR_DEPLOY
QMAKE_EXTRA_COMPILERS += lrelease_fi
INSTALLS += translation_fi

TR_HU_QM = $${TR_BASE}-hu.qm
TR_HU_TS = $$TR_DIR/$${TR_BASE}-hu.ts
lrelease_hu.input = TR_HU_TS
lrelease_hu.output = $$TR_DIR/$$TR_HU_QM
lrelease_hu.commands = $$LRELEASE -idbased $$TR_HU_TS -qm $$TR_DIR/$$TR_HU_QM
lrelease_hu.CONFIG = no_link
lrelease_hu.variable_out = PRE_TARGETDEPS
translation_hu.files = translations/$$TR_HU_QM
translation_hu.path = $$TR_DEPLOY
QMAKE_EXTRA_COMPILERS += lrelease_hu
INSTALLS += translation_hu

TR_PL_QM = $${TR_BASE}-pl.qm
TR_PL_TS = $$TR_DIR/$${TR_BASE}-pl.ts
lrelease_pl.input = TR_PL_TS
lrelease_pl.output = $$TR_DIR/$$TR_PL_QM
lrelease_pl.commands = $$LRELEASE -idbased $$TR_PL_TS -qm $$TR_DIR/$$TR_PL_QM
lrelease_pl.CONFIG = no_link
lrelease_pl.variable_out = PRE_TARGETDEPS
translation_pl.files = translations/$$TR_PL_QM
translation_pl.path = $$TR_DEPLOY
QMAKE_EXTRA_COMPILERS += lrelease_pl
INSTALLS += translation_pl

TR_PT_QM = $${TR_BASE}-pt.qm
TR_PT_TS = $$TR_DIR/$${TR_BASE}-pt.ts
lrelease_pt.input = TR_PT_TS
lrelease_pt.output = $$TR_DIR/$$TR_PT_QM
lrelease_pt.commands = $$LRELEASE -idbased $$TR_PT_TS -qm $$TR_DIR/$$TR_PT_QM
lrelease_pt.CONFIG = no_link
lrelease_pt.variable_out = PRE_TARGETDEPS
translation_pt.files = translations/$$TR_PT_QM
translation_pt.path = $$TR_DEPLOY
QMAKE_EXTRA_COMPILERS += lrelease_pt
INSTALLS += translation_pt

TR_RU_QM = $${TR_BASE}-ru.qm
TR_RU_TS = $$TR_DIR/$${TR_BASE}-ru.ts
lrelease_ru.input = TR_RU_TS
lrelease_ru.output = $$TR_DIR/$$TR_RU_QM
lrelease_ru.commands = $$LRELEASE -idbased $$TR_RU_TS -qm $$TR_DIR/$$TR_RU_QM
lrelease_ru.CONFIG = no_link
lrelease_ru.variable_out = PRE_TARGETDEPS
translation_ru.files = translations/$$TR_RU_QM
translation_ru.path = $$TR_DEPLOY
QMAKE_EXTRA_COMPILERS += lrelease_ru
INSTALLS += translation_ru

TR_SV_QM = $${TR_BASE}-sv.qm
TR_SV_TS = $$TR_DIR/$${TR_BASE}-sv.ts
lrelease_sv.input = TR_SV_TS
lrelease_sv.output = $$TR_DIR/$$TR_SV_QM
lrelease_sv.commands = $$LRELEASE -idbased $$TR_SV_TS -qm $$TR_DIR/$$TR_SV_QM
lrelease_sv.CONFIG = no_link
lrelease_sv.variable_out = PRE_TARGETDEPS
translation_sv.files = translations/$$TR_SV_QM
translation_sv.path = $$TR_DEPLOY
QMAKE_EXTRA_COMPILERS += lrelease_sv
INSTALLS += translation_sv

# Deployment

qml_meego.files = meego/qml
qml_meego.path = $$INSTALL_PREFIX
INSTALLS += qml_meego

data.files = $$_PRO_FILE_PWD_/data/
data.path = $$INSTALL_PREFIX
INSTALLS += data

desktopfile.files = meego/$${TARGET}.desktop
desktopfile.path = /usr/share/applications
INSTALLS += desktopfile

icon.files = meego/$${TARGET}.png
icon.path = /usr/share/icons/hicolor/80x80/apps
INSTALLS += icon

splash.files = meego/splash-*.png
splash.path = $$INSTALL_PREFIX
INSTALLS += splash

target.path = $$INSTALL_PREFIX/bin
INSTALLS += target
