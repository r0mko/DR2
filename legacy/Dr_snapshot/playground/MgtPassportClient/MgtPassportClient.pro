TEMPLATE = app

TARGET = mgt_client

QT += core gui widgets network qml quick webkitwidgets printsupport

CONFIG += c++11

HEADERS += \
    field.h \
    filterdefinition.h \
    filterobject.h \
    filterrepository.h \
    partmodel.h \
    printview.h \
    proxymodel.h \
    service.h \
    serviceworker.h \
    userdata.h \
    www.h \
    treeproxy.h

SOURCES += \
    field.cpp \
    filterdefinition.cpp \
    filterobject.cpp \
    filterrepository.cpp \
    main.cpp \
    partmodel.cpp \
    printview.cpp \
    proxymodel.cpp \
    service.cpp \
    serviceworker.cpp \
    userdata.cpp \
    www.cpp \
    treeproxy.cpp

DISTFILES += \
    qml/AuxModel.qml \
    qml/BigButton.qml \
    qml/Calculator.qml \
    qml/CalendarComponent.qml \
    qml/DatePicker.qml \
    qml/Editor.qml \
    qml/FrontPage.qml \
    qml/HToolTip.qml \
    qml/Login.qml \
    qml/main.qml \
    qml/MainUi.qml \
    qml/Map.qml \
    qml/ModelWeaver.qml \
    qml/ServerControl.qml \
    qml/SideToolTip.qml \
    qml/Tab1.qml \
    qml/Tab2.qml \
    qml/Table.qml \
#    qml/Table2.qml \
#    qml/TableC.qml \
    qml/TestTab.qml \
    qml/Tree.qml \
    qml/UserEditor.qml \
    qml/TableDelegates/TableBasicColumn.qml \
    qml/TableDelegates/TableBasicColumnDelegate.qml \
    qml/TableDelegates/TableBoolColumn.qml \
    qml/TableDelegates/TableBoolColumnDelegate.qml \
    qml/TableDelegates/TableDateColumn.qml \
    qml/TableDelegates/TableDateColumnDelegate.qml \
    qml/TableDelegates/TableIntColumn.qml \
    qml/TableDelegates/TableIntColumnDelegate.qml \
    qml/TableDelegates/TableRealColumn.qml \
    qml/TableDelegates/TableRealColumnDelegate.qml \
    qml/TableDelegates/TableTransportColumn.qml \
    qml/TableDelegates/TableTransportColumnDelegate.qml \
    qml/FormDelegates/FormBoolDelegate.qml \
    qml/FormDelegates/FormComboDelegate.qml \
    qml/FormDelegates/FormComboDelegate2.qml \
    qml/FormDelegates/FormDateDelegate.qml \
    qml/FormDelegates/FormDateDelegate2.qml \
    qml/FormDelegates/FormGenericDelegate.qml \
    qml/FormDelegates/FormIntDelegate.qml \
    qml/FormDelegates/FormLabelDelegate.qml \
    qml/FormDelegates/FormPassDelegate.qml \
    qml/FormDelegates/FormRealDelegate.qml \
    qml/FormDelegates/FormTextAreaDelegate.qml \
    qml/FormDelegates/FormTextDelegate.qml \
    qml/FormDelegates/FormTransportTypeDelegate.qml \
    images/250.gif \
    images/map.jpg \
    images/maps.jpg \
    images/logo-mosgortrans.png \
    images/NSTexturedFullScreenBackgroundColor.png \
    qml/Tree2.qml


dist.path = $$OUT_PWD
dist.files = $$DISTFILES

INSTALLS += dist

DEFINES += INSTALLPATH=\\\"$$OUT_PWD\\\"



