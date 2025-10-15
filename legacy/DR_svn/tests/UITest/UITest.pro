TEMPLATE = app

QT += qml quick

SOURCES += main.cpp

RESOURCES += \
    qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.

#OTHER_FILES += \
#    main.qml \
#    Delegates/formelement/Integer.qml \
#    Delegates/formelement/NumberElement.qml

#qmlfiles.files = $$OTHER_FILES
#unix {
#    delegates.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/qml
#    INSTALLS += delegates
#}

#HEADERS +=
