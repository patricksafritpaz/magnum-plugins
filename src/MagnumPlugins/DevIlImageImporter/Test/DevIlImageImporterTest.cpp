/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Alice Margatroid <loveoverwhelming@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/AbstractImporter.h>

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct DevIlImageImporterTest: TestSuite::Tester {
    explicit DevIlImageImporterTest();

    void fileNotFound();
    void empty();
    void invalid();

    void grayPng();
    void grayJpeg();
    void rgbPng();
    void rgbJpeg();
    void rgbaPng();

    void bgrTga();
    void bgraTga();

    void icoBmp();
    void icoPng();

    void openTwice();
    void importTwice();
    void twoImporters();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

using namespace Math::Literals;

const struct {
    const char* name;
    bool openFile;
    bool succeeds;
} IcoBmpData[] {
    {"openFile", true, true},
    {"openData", false, false}
};

DevIlImageImporterTest::DevIlImageImporterTest() {
    addTests({&DevIlImageImporterTest::fileNotFound,
              &DevIlImageImporterTest::empty,
              &DevIlImageImporterTest::invalid,

              &DevIlImageImporterTest::grayPng,
              &DevIlImageImporterTest::grayJpeg,
              &DevIlImageImporterTest::rgbPng,
              &DevIlImageImporterTest::rgbJpeg,
              &DevIlImageImporterTest::rgbaPng,

              &DevIlImageImporterTest::bgrTga,
              &DevIlImageImporterTest::bgraTga});

    addInstancedTests({&DevIlImageImporterTest::icoBmp},
        Containers::arraySize(IcoBmpData));

    addTests({&DevIlImageImporterTest::icoPng,

              &DevIlImageImporterTest::openTwice,
              &DevIlImageImporterTest::importTwice,
              &DevIlImageImporterTest::twoImporters});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef DEVILIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(DEVILIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void DevIlImageImporterTest::fileNotFound() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openFile("nonexistent"));
    CORRADE_COMPARE(out.str(), "Trade::DevIlImageImporter::openFile(): cannot open the image: 0x50b\n");
}

void DevIlImageImporterTest::empty() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");

    std::ostringstream out;
    Error redirectError{&out};
    char a{};
    /* Explicitly checking non-null but empty view */
    CORRADE_VERIFY(!importer->openData({&a, 0}));
    CORRADE_COMPARE(out.str(), "Trade::DevIlImageImporter::openData(): cannot open the image: 0x509\n");
}

void DevIlImageImporterTest::invalid() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->openData("invalid"));
    CORRADE_COMPARE(out.str(), "Trade::DevIlImageImporter::openData(): cannot open the image: 0x512\n");
}

void DevIlImageImporterTest::grayPng() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(PNGIMPORTER_TEST_DIR, "gray.png")));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->size(), Vector2i(3, 2));
    CORRADE_COMPARE(image->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE_AS(image->data(), (Containers::Array<char>{Containers::InPlaceInit, {
        '\xff', '\x88', '\x00',
        '\x88', '\x00', '\xff'}}),
        TestSuite::Compare::Container<Containers::ArrayView<const char>>);
}

void DevIlImageImporterTest::grayJpeg() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(JPEGIMPORTER_TEST_DIR, "gray.jpg")));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->size(), Vector2i(3, 2));
    CORRADE_COMPARE(image->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE_AS(image->data(), (Containers::Array<char>{Containers::InPlaceInit, {
        '\xff', '\x88', '\x00',
        '\x88', '\x00', '\xff'}}),
        TestSuite::Compare::Container<Containers::ArrayView<const char>>);
}

void DevIlImageImporterTest::rgbPng() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(PNGIMPORTER_TEST_DIR, "rgb.png")));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->size(), Vector2i(3, 2));
    CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE_AS(image->data(), (Containers::Array<char>{Containers::InPlaceInit, {
        '\xca', '\xfe', '\x77',
        '\xde', '\xad', '\xb5',
        '\xca', '\xfe', '\x77',
        '\xde', '\xad', '\xb5',
        '\xca', '\xfe', '\x77',
        '\xde', '\xad', '\xb5'}}),
        TestSuite::Compare::Container<Containers::ArrayView<const char>>);
}

void DevIlImageImporterTest::rgbJpeg() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(JPEGIMPORTER_TEST_DIR, "rgb.jpg")));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->size(), Vector2i(3, 2));
    CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);
    /* Data should be similar to the PNG */
    CORRADE_COMPARE_AS(image->data(), (Containers::Array<char>{Containers::InPlaceInit, {
        '\xca', '\xfe', '\x76',
        '\xdf', '\xad', '\xb6',
        '\xca', '\xfe', '\x76',
        '\xe0', '\xad', '\xb6',
        '\xc9', '\xff', '\x76',
        '\xdf', '\xad', '\xb6'}}),
        TestSuite::Compare::Container<Containers::ArrayView<const char>>);
}

void DevIlImageImporterTest::rgbaPng() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(PNGIMPORTER_TEST_DIR, "rgba.png")));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 4);
    CORRADE_COMPARE(image->size(), Vector2i(3, 2));
    CORRADE_COMPARE(image->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE_AS(image->data(), (Containers::Array<char>{Containers::InPlaceInit, {
        '\xde', '\xad', '\xb5', '\xff',
        '\xca', '\xfe', '\x77', '\xff',
        '\x00', '\x00', '\x00', '\x00',
        '\xca', '\xfe', '\x77', '\xff',
        '\x00', '\x00', '\x00', '\x00',
        '\xde', '\xad', '\xb5', '\xff'}}),
        TestSuite::Compare::Container<Containers::ArrayView<const char>>);
}

void DevIlImageImporterTest::bgrTga() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");

    /* Copy of TgaImporterTest::colorBits24() */
    constexpr char data[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        1, 2, 3, 2, 3, 4,
        3, 4, 5, 4, 5, 6,
        5, 6, 7, 6, 7, 8
    };
    CORRADE_VERIFY(importer->openData(data));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE_AS(image->data(), (Containers::Array<char>{Containers::InPlaceInit, {
        3, 2, 1, 4, 3, 2,
        5, 4, 3, 6, 5, 4,
        7, 6, 5, 8, 7, 6}}),
        TestSuite::Compare::Container<Containers::ArrayView<const char>>);
}

void DevIlImageImporterTest::bgraTga() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");

    /* Copy of TgaImporterTest::colorBits32() */
    constexpr char data[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 32, 0,
        1, 2, 3, 4, 2, 3, 4, 5,
        3, 4, 5, 6, 4, 5, 6, 7,
        5, 6, 7, 8, 6, 7, 8, 9
    };
    CORRADE_VERIFY(importer->openData(data));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 4);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE(image->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE_AS(image->data(), (Containers::Array<char>{Containers::InPlaceInit, {
        3, 2, 1, 4, 4, 3, 2, 5,
        5, 4, 3, 6, 6, 5, 4, 7,
        7, 6, 5, 8, 8, 7, 6, 9}}),
        TestSuite::Compare::Container<Containers::ArrayView<const char>>);
}

void DevIlImageImporterTest::icoBmp() {
    auto&& data = IcoBmpData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");
    std::string filename = Utility::Directory::join(ICOIMPORTER_TEST_DIR, "bmp+png.ico");
    if(data.openFile)
        CORRADE_COMPARE(importer->openFile(filename), data.succeeds);
    else
        CORRADE_COMPARE(importer->openData(Utility::Directory::read(filename)), data.succeeds);
    if(!data.succeeds) return;

    CORRADE_COMPARE(importer->image2DCount(), 1);
    {
        CORRADE_EXPECT_FAIL("DevIlImageImporter does not support image levels.");
        CORRADE_COMPARE(importer->image2DLevelCount(0), 2);
    }
    CORRADE_COMPARE(importer->image2DLevelCount(0), 1);

    {
        Containers::Optional<Trade::ImageData2D> image = importer->image2D(0, 0);
        CORRADE_VERIFY(image);
        CORRADE_COMPARE(image->format(), PixelFormat::RGBA8Unorm);
        CORRADE_COMPARE(image->size(), (Vector2i{16, 8}));
        CORRADE_COMPARE(image->pixels<Color4ub>()[0][0], 0x00ff00_rgb);
    }
}

void DevIlImageImporterTest::icoPng() {
    /* Last checked with version 1.8, May 2020 */
    CORRADE_SKIP("DevIL crashes on ICOs with embedded PNGs, skipping the test.");

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(ICOIMPORTER_TEST_DIR, "pngs.ico")));

    CORRADE_COMPARE(importer->image2DCount(), 1);
    {
        CORRADE_EXPECT_FAIL("DevIlImageImporter does not support image levels.");
        CORRADE_COMPARE(importer->image2DLevelCount(0), 2);
    }
    CORRADE_COMPARE(importer->image2DLevelCount(0), 1);

    {
        Containers::Optional<Trade::ImageData2D> image = importer->image2D(0, 0);
        CORRADE_VERIFY(image);
        CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);
        CORRADE_COMPARE(image->size(), (Vector2i{16, 8}));
        CORRADE_COMPARE(image->pixels<Color3ub>()[0][0], 0x00ff00_rgb);
    }
}

void DevIlImageImporterTest::openTwice() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(PNGIMPORTER_TEST_DIR, "gray.png")));
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(PNGIMPORTER_TEST_DIR, "gray.png")));

    /* Shouldn't crash, leak or anything */
}

void DevIlImageImporterTest::importTwice() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("DevIlImageImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(PNGIMPORTER_TEST_DIR, "gray.png")));

    /* Verify that everything is working the same way on second use */
    {
        Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
        CORRADE_VERIFY(image);
        CORRADE_COMPARE(image->size(), (Vector2i{3, 2}));
    } {
        Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
        CORRADE_VERIFY(image);
        CORRADE_COMPARE(image->size(), (Vector2i{3, 2}));
    }
}

void DevIlImageImporterTest::twoImporters() {
    Containers::Pointer<AbstractImporter> a = _manager.instantiate("DevIlImageImporter");
    Containers::Pointer<AbstractImporter> b = _manager.instantiate("DevIlImageImporter");

    CORRADE_VERIFY(a->openFile(Utility::Directory::join(JPEGIMPORTER_TEST_DIR, "rgb.jpg")));
    CORRADE_VERIFY(b->openFile(Utility::Directory::join(PNGIMPORTER_TEST_DIR, "rgba.png")));

    /* Import image A after loading file B to test that the two importers don't
       get their state mixed together */
    Containers::Optional<Trade::ImageData2D> imageA = a->image2D(0);
    Containers::Optional<Trade::ImageData2D> imageB = b->image2D(0);

    /* Colors the same as above */
    CORRADE_VERIFY(imageA);
    CORRADE_COMPARE(imageA->size(), (Vector2i{3, 2}));
    CORRADE_COMPARE(imageA->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE(imageA->pixels<Color3ub>()[0][0], 0xcafe76_rgb);

    CORRADE_VERIFY(imageB);
    CORRADE_COMPARE(imageB->size(), (Vector2i{3, 2}));
    CORRADE_COMPARE(imageB->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(imageB->pixels<Color4ub>()[0][0], 0xdeadb5ff_rgba);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::DevIlImageImporterTest)
