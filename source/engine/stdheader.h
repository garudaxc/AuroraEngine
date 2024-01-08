#pragma once

#pragma warning(disable : 4003)

#include <Windows.h>
#include <assert.h>
#include <memory>
#include <limits>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <iterator>
#include <algorithm>
#include <io.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"

#include "AurMath.h"
#include "Color.h"
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "MathFunction.h"

#include "Types.h"


using namespace std;
using namespace rapidjson;

namespace Aurora
{
    using String = std::string;
    using StringStream = std::stringstream;

    template<class  T>
    using Array = std::vector<T>;
    
    
}
