/*
 * Copyright (c) 2020-2021 Samsung Electronics Co., Ltd. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _TVG_NODE_H_
#define _TVG_NODE_H_

#include "tvgArray.h"

namespace tvg
{

class Node
{
    const Paint* value;
    Array<Node*> children;
    unsigned paintType;
    CompositeMethod cmpMethod;

public:
    Node(const Paint* paint = nullptr, unsigned type = 0, CompositeMethod cmp = CompositeMethod::None) : value{paint}, paintType{type}, cmpMethod{cmp}
    {
    }

    ~Node()
    {
//        TODO !!!! - check mem 
    }

    void deleteAllNodes()
    {
//        TODO !!!! - check mem 
    }

    void insert(const Paint* paint, unsigned type, CompositeMethod cmp = CompositeMethod::None)
    {
        if (!value) {
            value = paint;
            paintType = type;
            cmpMethod = cmp;
        } else {
            children.push(new Node(paint, type, cmp));
        }
    }

    Node* getLastChild()
    {
        if (children.count > 0) {
            return children.data[children.count - 1];
        }
        return this;
    }

    Array<Node*> getChildren()
    {
        return children;
    }

    const Paint* getPaint()
    {
        return value;
    }

    unsigned getPaintType()
    {
        return paintType;
    }

    CompositeMethod getCmpType()
    {
        return cmpMethod;
    }
};

}

#endif //_TVG_NODE_H_
