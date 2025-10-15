
SOURCES += \
    ui.cpp \
    symbol.cpp \
    drpicture.cpp

HEADERS += \
    ui.h \
    symbol.h \
    drpicture.h

#FONTS += \
#    $$PWD/fonts/entypo.ttf \
#    $$PWD/fonts/Exo2-Black.ttf \
#    $$PWD/fonts/Exo2-BlackItalic.ttf \
#    $$PWD/fonts/Exo2-Bold.ttf \
#    $$PWD/fonts/Exo2-BoldItalic.ttf \
#    $$PWD/fonts/Exo2-ExtraBold.ttf \
#    $$PWD/fonts/Exo2-ExtraBoldItalic.ttf \
#    $$PWD/fonts/Exo2-ExtraLight.ttf \
#    $$PWD/fonts/Exo2-ExtraLightItalic.ttf \
#    $$PWD/fonts/Exo2-Italic.ttf \
#    $$PWD/fonts/Exo2-Light.ttf \
#    $$PWD/fonts/Exo2-LightItalic.ttf \
#    $$PWD/fonts/Exo2-Medium.ttf \
#    $$PWD/fonts/Exo2-MediumItalic.ttf \
#    $$PWD/fonts/Exo2-Regular.ttf \
#    $$PWD/fonts/Exo2-SemiBold.ttf \
#    $$PWD/fonts/Exo2-SemiBoldItalic.ttf \
#    $$PWD/fonts/Exo2-Thin.ttf \
#    $$PWD/fonts/Exo2-ThinItalic.ttf \
#    $$PWD/fonts/FontAwesome.otf \
#    $$PWD/fonts/Roboto-Black.ttf \
#    $$PWD/fonts/Roboto-BlackItalic.ttf \
#    $$PWD/fonts/Roboto-Bold.ttf \
#    $$PWD/fonts/Roboto-BoldItalic.ttf \
#    $$PWD/fonts/Roboto-Italic.ttf \
#    $$PWD/fonts/Roboto-Light.ttf \
#    $$PWD/fonts/Roboto-LightItalic.ttf \
#    $$PWD/fonts/Roboto-Medium.ttf \
#    $$PWD/fonts/Roboto-MediumItalic.ttf \
#    $$PWD/fonts/Roboto-Regular.ttf \
#    $$PWD/fonts/Roboto-Thin.ttf \
#    $$PWD/fonts/Roboto-ThinItalic.ttf \
#    $$PWD/fonts/texgyreheros-bold.otf \
#    $$PWD/fonts/texgyreheros-bolditalic.otf \
#    $$PWD/fonts/texgyreheros-italic.otf \
#    $$PWD/fonts/texgyreheros-regular.otf \
#    $$PWD/fonts/texgyreheroscn-bold.otf \
#    $$PWD/fonts/texgyreheroscn-bolditalic.otf \
#    $$PWD/fonts/texgyreheroscn-italic.otf \
#    $$PWD/fonts/texgyreheroscn-regular.otf \
#    $$PWD/fonts/UbuntuMono-BI.ttf \
#    $$PWD/fonts/UbuntuMono-B.ttf \
#    $$PWD/fonts/UbuntuMono-RI.ttf \
#    $$PWD/fonts/UbuntuMono-R.ttf \
#    $$PWD/fonts/Ubuntu-C.ttf \
#    $$PWD/fonts/Ubuntu-RI.ttf \
#    $$PWD/fonts/Ubuntu-L.ttf \
#    $$PWD/fonts/Ubuntu-BI.ttf \
#    $$PWD/fonts/Ubuntu-LI.ttf \
#    $$PWD/fonts/Ubuntu-R.ttf \
#    $$PWD/fonts/Ubuntu-B.ttf \
#    $$PWD/fonts/Ubuntu-M.ttf \
#    $$PWD/fonts/Ubuntu-MI.ttf


DISTFILES += \
    $$PWD/qml/JSObjectEditor.qml \
    $$PWD/qml/TextFieldEditor.qml \
    $$PWD/qml/EditableText.qml \
    $$PWD/qml/TextFieldCompleter.qml \
    $$PWD/qml/Completer.qml \
    $$PWD/qml/ListWidget.qml \
    $$PWD/qml/ThinToolbar.qml \
    $$PWD/qml/ThinToolButton.qml \
    $$PWD/qml/JSObjectView.qml \
    $$PWD/qml/TinySlider.qml \
    $$PWD/qml/JSObjectViewRow.qml \
    $$PWD/qml/NavigableTableViewColumn.qml \
    $$PWD/qml/PicButton.qml \
    $$PWD/qml/CaptionLabel.qml \
    $$PWD/qml/Sidebar.qml \
    $$PWD/qml/SidebarPanel.qml \
    $$PWD/qml/VectorIconButton.qml \
    $$PWD/qml/TinyButton.qml \
    $$PWD/qml/Theme/Exo2Font.qml \
    $$PWD/qml/Theme/RobotoFont.qml \
    $$PWD/qml/Theme/TexGyreHerosFont.qml \
    $$PWD/qml/Theme/UbuntuFont.qml \
    $$PWD/qml/AppWindow.qml


QML_THEME_FILES = \
    $$PWD/qml/Theme/*.qml \


#fonts.files = $$FONTS
qml.files = $$DISTFILES

theme.files = $$QML_THEME_FILES
unix {
#    fonts.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/fonts
    qml.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/qml
    theme.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/qml/Theme
    INSTALLS += qml theme
#    INSTALLS += fonts qml theme
}

win32 {
    fonts.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/fonts
    qml.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/qml
    theme.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/qml/Theme
    INSTALLS += fonts qml theme
}

#DISTFILES += \
#    $$PWD/qml/Completer.qml \
#    $$PWD/qml/EditableText.qml \
#    $$PWD/qml/TextFieldEditor.qml \
#    $$PWD/qml/JSObjectEditor.qml

