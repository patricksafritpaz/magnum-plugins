#ifndef Magnum_Trade_StbPngImageConverter_h
#define Magnum_Trade_StbPngImageConverter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>

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

/** @file
 * @brief Class @ref Magnum::Trade::StbPngImageConverter
 */

#include <Magnum/Trade/AbstractImageConverter.h>

#include "MagnumPlugins/StbPngImageConverter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_STBPNGIMAGECONVERTER_BUILD_STATIC
    #if defined(StbPngImageConverter_EXPORTS) || defined(StbPngImageConverterObjects_EXPORTS)
        #define MAGNUM_STBPNGIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_STBPNGIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_STBPNGIMAGECONVERTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_STBPNGIMAGECONVERTER_LOCAL CORRADE_VISIBILITY_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief PNG image converter plugin using stb_image_write

Creates Portable Network Graphics (`*.png`) files from images with format
@ref PixelFormat::Red, @ref PixelFormat::RG, @ref PixelFormat::RGB or
@ref PixelFormat::RGBA and type @ref PixelType::UnsignedByte. On OpenGL ES 2.0
and WebGL 1.0 accepts also @ref PixelFormat::Luminance instead of
@ref PixelFormat::Red and @ref PixelFormat::LuminanceAlpha instead of
@ref PixelFormat::RG. Does *not* support non-default
@ref PixelStorage::swapBytes() values.

This plugin is built if `WITH_STBPNGIMAGECONVERTER` is enabled when building
Magnum Plugins. To use dynamic plugin, you need to load `StbPngImageConverter`
plugin from `MAGNUM_PLUGINS_IMAGECONVERTER_DIR`. To use static plugin, you need
to request `StbPngImageConverter` component of `MagnumPlugins` package in CMake
and link to `MagnumPlugins::StbPngImageConverter` target.

This plugins provides `PngImageConverter` plugin, but note that this plugin
may generate slightly larger files and the performance might be worse than when
using plugin dedicated for given format.

See @ref building-plugins, @ref cmake-plugins and @ref plugins for more
information.
*/
class MAGNUM_STBPNGIMAGECONVERTER_EXPORT StbPngImageConverter: public AbstractImageConverter {
    public:
        /** @brief Default constructor */
        explicit StbPngImageConverter();

        /** @brief Plugin manager constructor */
        explicit StbPngImageConverter(PluginManager::AbstractManager& manager, std::string plugin);

    private:
        MAGNUM_STBPNGIMAGECONVERTER_LOCAL Features doFeatures() const override;
        MAGNUM_STBPNGIMAGECONVERTER_LOCAL Containers::Array<char> doExportToData(const ImageView2D& image) override;
};

}}

#endif
