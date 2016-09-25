OTHER_FILES += $$PWD/mainpage.dox

docs.commands += "QT_INSTALL_DOCS=$$[QT_INSTALL_DOCS] doxygen"
docs.CONFIG += phony
QMAKE_EXTRA_TARGETS += docs
