TEMPLATE = subdirs

SUBDIRS += \
    SharedMem \
    SharedImage \
    TestConsole \
    TestImage

SharedMem.subdir = SharedMem

SharedImage.subdir = SharedImage

TestConsole.subdir= TestConsole
TestConsole.depends = SharedMem

TestImage.subdir = TestImage
TestImage.depends = SharedImage
