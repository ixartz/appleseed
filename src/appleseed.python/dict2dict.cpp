
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2012-2013 Esteban Tovagliari, Jupiter Jazz Limited
// Copyright (c) 2014 Esteban Tovagliari, The appleseedhq Organization
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

// appleseed.python headers.
#include "dict2dict.h"

// appleseed.renderer headers.
#include "renderer/utility/paramarray.h"

// appleseed.foundation headers.
#include "foundation/math/vector.h"
#include "foundation/utility/containers/dictionary.h"
#include "foundation/utility/foreach.h"
#include "foundation/utility/iostreamop.h"
#include "foundation/utility/string.h"

namespace bpy = boost::python;
using namespace foundation;
using namespace renderer;

namespace
{
    ParamArray& push_dict(ParamArray& dict, const char* name)
    {
        return dict.push(name);
    }

    Dictionary& push_dict(Dictionary& dict, const char* name)
    {
        if (!dict.dictionaries().exist(name))
            dict.dictionaries().insert(name, Dictionary());

        return dict.dictionaries().get(name);
    }

    template <class Dict>
    Dict convert_from_bpy_dict(const bpy::dict& d)
    {
        Dict result;

        bpy::list values = d.values();
        bpy::list keys = d.keys();

        for (bpy::ssize_t i = 0, e = bpy::len(d); i < e; ++i)
        {
            bpy::object key(keys[i]);
            bpy::object value(values[i]);

            // keys
            bpy::extract<const char*> key_extractor(key);
            if (!key_extractor.check())
            {
                PyErr_SetString(PyExc_TypeError, "Incompatible key type. Only strings accepted.");
                bpy::throw_error_already_set();
            }

            // string
            {
                bpy::extract<const char*> extractor(value);
                if (extractor.check())
                {
                    result.insert(key_extractor(), extractor());
                    continue;
                }
            }

            // int
            {
                bpy::extract<int> extractor(value);
                if (extractor.check())
                {
                    result.insert(key_extractor(), extractor());
                    continue;
                }
            }

            // double
            {
                bpy::extract<double> extractor(value);
                if (extractor.check())
                {
                    result.insert(key_extractor(), extractor());
                    continue;
                }
            }

            // Vector2i
            {
                bpy::extract<Vector2i> extractor(value);
                if (extractor.check())
                {
                    result.insert(key_extractor(), extractor());
                    continue;
                }
            }

            // Vector2f
            {
                bpy::extract<Vector2f> extractor(value);
                if (extractor.check())
                {
                    result.insert(key_extractor(), extractor());
                    continue;
                }
            }

            // Vector2d
            {
                bpy::extract<Vector2d> extractor(value);
                if (extractor.check())
                {
                    result.insert(key_extractor(), extractor());
                    continue;
                }
            }

            // TODO: add conversions from bpy::tuple to Vector<T, N>.
            // ...

            // TODO: check more types here if needed... (est.)

            // dict
            {
                bpy::extract<bpy::dict> extractor(value);
                if (extractor.check())
                {
                    // recurse
                    push_dict(result, key_extractor()) = convert_from_bpy_dict<Dict>(extractor());
                    continue;
                }
            }

            PyErr_SetString(PyExc_TypeError, "Incompatible value type - must be string, int, double or dictionary.");
            bpy::throw_error_already_set();
        }

        return result;
    }

    bpy::object obj_from_string(const std::string& str)
    {
        // try to guess the type of the value represented by str.
        
        try // Vector2
        {
            Vector2d v = from_string<Vector2d>(str);
            return bpy::object(v);
        }
        catch (ExceptionStringConversionError&) {}

        try // int / double
        {
            double d = from_string<double>(str);
            return bpy::object(d);
        }
        catch (ExceptionStringConversionError&) {}
        
        try // bool
        {
            bool b = from_string<bool>(str);
            return bpy::object(b);
        }
        catch (ExceptionStringConversionError&) {}

        // TODO: check more types here if needed... (est.)

        // as a fallback, return a string
        return bpy::object(str);
    }
}

Dictionary bpy_dict_to_dictionary(const bpy::dict& d)
{
    return convert_from_bpy_dict<Dictionary>(d);
}

bpy::dict dictionary_to_bpy_dict(const Dictionary& dict)
{
    bpy::dict result;

    for (const_each<StringDictionary> it = dict.strings(); it; ++it)
        result[it->name()] = obj_from_string(it->value());

    for (const_each<DictionaryDictionary> it = dict.dictionaries(); it; ++it)
    {
        // recurse
        result[it->name()] = dictionary_to_bpy_dict(it->value());
    }

    return result;
}

ParamArray bpy_dict_to_param_array(const bpy::dict& d)
{
    return convert_from_bpy_dict<ParamArray>(d);
}

bpy::dict param_array_to_bpy_dict(const ParamArray& array)
{
    return dictionary_to_bpy_dict(array);
}
