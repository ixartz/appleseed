
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014 Francois Beaune, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// appleseed.foundation headers.
#include "foundation/image/filteredtile.h"
#include "foundation/math/filter.h"
#include "foundation/utility/test.h"

// Standard headers.
#include <cstddef>
#include <cstdio>

using namespace foundation;
using namespace std;

TEST_SUITE(Foundation_Image_FilteredTile)
{
    void dump(const char* filepath, const FilteredTile& tile)
    {
        FILE* file = fopen(filepath, "wt");

        if (file == 0)
            return;

        for (size_t y = 0; y < tile.get_height(); ++y)
        {
            for (size_t x = 0; x < tile.get_width(); ++x)
            {
                if (x > 0)
                    fprintf(file, "\t");

                const float* ptr = tile.pixel(x, y);
                const float weight = ptr[0];

                fprintf(file, "%.2f", weight);
            }

            fprintf(file, "\n");
        }

        fclose(file);
    }

    template <typename Filter>
    void test(const char* filepath, const Filter& filter)
    {
        FilteredTile tile(5, 6, 1, filter);

        const float values[1] = { 1.0f };
        tile.add(2.5, 3.0, values);

        dump(filepath, tile);
    }

    TEST_CASE(BoxFilter_Radius0Dot5)
    {
        const BoxFilter2<double> filter(0.5, 0.5);
        test("unit tests/outputs/test_filteredtile_boxfilter_radius0dot5.txt", filter);
    }

    TEST_CASE(BoxFilter_Radius1Dot0)
    {
        const BoxFilter2<double> filter(1.0, 1.0);
        test("unit tests/outputs/test_filteredtile_boxfilter_radius1dot0.txt", filter);
    }

    TEST_CASE(BoxFilter_Radius1Dot5)
    {
        const BoxFilter2<double> filter(1.5, 1.5);
        test("unit tests/outputs/test_filteredtile_boxfilter_radius1dot5.txt", filter);
    }

    TEST_CASE(BoxFilter_Radius2Dot0)
    {
        const BoxFilter2<double> filter(2.0, 2.0);
        test("unit tests/outputs/test_filteredtile_boxfilter_radius2dot0.txt", filter);
    }
}
