/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All rights reserved.

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
#ifndef _TVG_SAVER_IMPL_H_
#define _TVG_SAVER_IMPL_H_

#include "tvgCommon.h"
#include "tvgPaint.h"
#include "tvgTvgHelper.h"
#include <fstream>

struct Saver::Impl
{
    Saver* saver;
    char* buffer = nullptr;
    uint32_t size = 0;
    uint32_t reserved = 0;
    char* bufferPosition = nullptr;


    Impl(Saver* s) : saver(s)
    {
    }


    ~Impl()
    {
        clearBuffer();
       // MGS - delete allocated memory 
    }


    void saveMemberIndicator(TvgIndicator ind)
    {
        if (size + TVG_INDICATOR_SIZE > reserved) resizeBuffer(size + TVG_INDICATOR_SIZE);

        memcpy(bufferPosition, &ind, TVG_INDICATOR_SIZE);
        bufferPosition += TVG_INDICATOR_SIZE;
        size += TVG_INDICATOR_SIZE;
    }


    void saveMemberDataSize(ByteCounter byteCnt)
    {
        if (size + BYTE_COUNTER_SIZE > reserved) resizeBuffer(size + BYTE_COUNTER_SIZE);

        memcpy(bufferPosition, &byteCnt, BYTE_COUNTER_SIZE);
        bufferPosition += BYTE_COUNTER_SIZE;
        size += BYTE_COUNTER_SIZE;
    }


    void saveMemberDataSizeAt(ByteCounter byteCnt)
    {
        memcpy(bufferPosition - byteCnt - BYTE_COUNTER_SIZE, &byteCnt, BYTE_COUNTER_SIZE);
    }


    void skipMemberDataSize()
    {
        if (size + BYTE_COUNTER_SIZE > reserved) resizeBuffer(size + BYTE_COUNTER_SIZE);
        bufferPosition += BYTE_COUNTER_SIZE;
        size += BYTE_COUNTER_SIZE;
    }


    ByteCounter saveMemberData(const void* data, ByteCounter byteCnt)
    {
        if (size + byteCnt > reserved) resizeBuffer(size + byteCnt);

        memcpy(bufferPosition, data, byteCnt);
        bufferPosition += byteCnt;
        size += byteCnt;

        return byteCnt;
    }


    ByteCounter saveMember(TvgIndicator ind, ByteCounter byteCnt, const void* data)
    {
        ByteCounter blockByteCnt = TVG_INDICATOR_SIZE + BYTE_COUNTER_SIZE + byteCnt;

        if (size + blockByteCnt > reserved) resizeBuffer(size + blockByteCnt);

        memcpy(bufferPosition, &ind, TVG_INDICATOR_SIZE);
        bufferPosition += TVG_INDICATOR_SIZE;
        memcpy(bufferPosition, &byteCnt, BYTE_COUNTER_SIZE);
        bufferPosition += BYTE_COUNTER_SIZE;
        memcpy(bufferPosition, data, byteCnt);
        bufferPosition += byteCnt;

        size += blockByteCnt;

        return blockByteCnt;
    }


    void resizeBuffer(uint32_t newSize)
    {
        // MGS - find more optimal alg ?
        reserved += 100;
        if (newSize > reserved) reserved += newSize - reserved + 100;

        auto bufferOld = buffer;

        buffer = static_cast<char*>(realloc(buffer, reserved));

        if (buffer != bufferOld)
            bufferPosition = buffer + (bufferPosition - bufferOld);
    }


    void rewindBuffer(ByteCounter bytesNum)
    {
        if (bufferPosition - bytesNum < buffer) return;

        bufferPosition -= bytesNum;
        size -= bytesNum;
    }


    ByteCounter serializePaint(const Paint* paint)
    {
        ByteCounter paintDataByteCnt = 0;

        uint8_t opacity = paint->opacity();
        if (opacity < 255) {
            paintDataByteCnt += saveMember(TVG_PAINT_OPACITY_INDICATOR, sizeof(opacity), &opacity);
        }

        const Matrix* m = nullptr;
        if (paint->transform(&m) == Result::Success) {
            paintDataByteCnt += saveMember(TVG_PAINT_TRANSFORM_MATRIX_INDICATOR, sizeof(*m), m);
        }

        return paintDataByteCnt;
    }


    ByteCounter serializeScene(Node* node)
    {
        if (!node) return 0;
        auto scene = static_cast<const Scene*>(node->getPaint());
        if (!scene) return 0;

        ByteCounter sceneDataByteCnt = 0;

        saveMemberIndicator(TVG_SCENE_BEGIN_INDICATOR);
        skipMemberDataSize();

        sceneDataByteCnt += serializeNode(node);
        sceneDataByteCnt += serializePaint(scene);

        saveMemberDataSizeAt(sceneDataByteCnt);

        return TVG_INDICATOR_SIZE + BYTE_COUNTER_SIZE + sceneDataByteCnt;
    }


    ByteCounter serializeComposite(Node* node)
    {
        if (!node) return 0; // MGS: -1?  error handling
        auto paint = node->getPaint();
        if (!paint) return 0;
        auto cmpMethod = node->getCmpType();

        ByteCounter cmpDataByteCnt = 0;

        saveMemberIndicator(TVG_PAINT_CMP_TARGET_INDICATOR);
        skipMemberDataSize();

        TvgFlag cmpMethodTvgFlag;
        switch (cmpMethod) {
            case CompositeMethod::ClipPath: {
                cmpMethodTvgFlag = TVG_PAINT_CMP_METHOD_CLIPPATH_FLAG;
                break;
            }
            case CompositeMethod::AlphaMask: {
                cmpMethodTvgFlag = TVG_PAINT_CMP_METHOD_ALPHAMASK_FLAG;
                break;
            }
            case CompositeMethod::InvAlphaMask: {
                cmpMethodTvgFlag = TVG_PAINT_CMP_METHOD_INV_ALPHAMASK_FLAG;
                break;
            }
            case CompositeMethod::None: {
                break;
            }
        }
        cmpDataByteCnt += saveMember(TVG_PAINT_CMP_METHOD_INDICATOR, TVG_FLAG_SIZE, &cmpMethodTvgFlag);

        cmpDataByteCnt += serializeNode(node);
        cmpDataByteCnt += serializePaint(paint);

        saveMemberDataSizeAt(cmpDataByteCnt);

        return TVG_INDICATOR_SIZE + BYTE_COUNTER_SIZE + cmpDataByteCnt;
    }


    ByteCounter serializeShapeFill(const Fill* f, TvgIndicator fillTvgFlag)
    {
        ByteCounter fillDataByteCnt = 0;
        TvgFlag strokeTvgFlag;
        const Fill::ColorStop* stops = nullptr;
        auto stopsCnt = f->colorStops(&stops);
        if (!stops || stopsCnt == 0) return 0;

        saveMemberIndicator(fillTvgFlag);
        skipMemberDataSize();

        if (f->id() == FILL_ID_RADIAL) {
            float argRadial[3];
            auto radGrad = static_cast<const RadialGradient*>(f);
            if (radGrad->radial(argRadial, argRadial + 1,argRadial + 2) != Result::Success) {
                rewindBuffer(TVG_FLAG_SIZE + BYTE_COUNTER_SIZE);
                return 0;
            }
            fillDataByteCnt += saveMember(TVG_FILL_RADIAL_GRADIENT_INDICATOR, sizeof(argRadial), argRadial);
        }
        else {
            float argLinear[4];
            auto linGrad = static_cast<const LinearGradient*>(f);
            if (linGrad->linear(argLinear, argLinear + 1, argLinear + 2, argLinear + 3) != Result::Success) {
                rewindBuffer(TVG_FLAG_SIZE + BYTE_COUNTER_SIZE);
                return 0;
            }
            fillDataByteCnt += saveMember(TVG_FILL_LINEAR_GRADIENT_INDICATOR, sizeof(argLinear), argLinear);
        }

        switch (f->spread()) {
            case FillSpread::Pad: {
                strokeTvgFlag = TVG_FILL_FILLSPREAD_PAD_FLAG;
                break;
            }
            case FillSpread::Reflect: {
                strokeTvgFlag = TVG_FILL_FILLSPREAD_REFLECT_FLAG;
                break;
            }
            case FillSpread::Repeat: {
                strokeTvgFlag = TVG_FILL_FILLSPREAD_REPEAT_FLAG;
                break;
            }
        }
        fillDataByteCnt += saveMember(TVG_FILL_FILLSPREAD_INDICATOR, TVG_FLAG_SIZE, &strokeTvgFlag);

        fillDataByteCnt += saveMember(TVG_FILL_COLORSTOPS_INDICATOR, stopsCnt * sizeof(stops), stops);

        saveMemberDataSizeAt(fillDataByteCnt);

        return TVG_INDICATOR_SIZE + BYTE_COUNTER_SIZE + fillDataByteCnt;
    }


    ByteCounter serializeShapeStroke(const Shape* shape)
    {
        ByteCounter strokeDataByteCnt = 0;
        TvgFlag strokeTvgFlag;

        saveMemberIndicator(TVG_SHAPE_STROKE_INDICATOR);
        skipMemberDataSize();

        switch (shape->strokeCap()) {
            case StrokeCap::Square: {
                strokeTvgFlag = TVG_SHAPE_STROKE_CAP_SQUARE_FLAG;
                break;
            }
            case StrokeCap::Round: {
                strokeTvgFlag = TVG_SHAPE_STROKE_CAP_ROUND_FLAG;
                break;
            }
            case StrokeCap::Butt: {
                strokeTvgFlag = TVG_SHAPE_STROKE_CAP_BUTT_FLAG;
                break;
            }
        }
        strokeDataByteCnt += saveMember(TVG_SHAPE_STROKE_CAP_INDICATOR, TVG_FLAG_SIZE, &strokeTvgFlag);

        switch (shape->strokeJoin()) {
            case StrokeJoin::Bevel: {
                strokeTvgFlag = TVG_SHAPE_STROKE_JOIN_BEVEL_FLAG;
                break;
            }
            case StrokeJoin::Round: {
                strokeTvgFlag = TVG_SHAPE_STROKE_JOIN_ROUND_FLAG;
                break;
            }
            case StrokeJoin::Miter: {
                strokeTvgFlag = TVG_SHAPE_STROKE_JOIN_MITER_FLAG;
                break;
            }
        }
        strokeDataByteCnt += saveMember(TVG_SHAPE_STROKE_JOIN_INDICATOR, TVG_FLAG_SIZE, &strokeTvgFlag);

        float width = shape->strokeWidth();
        strokeDataByteCnt += saveMember(TVG_SHAPE_STROKE_WIDTH_INDICATOR, sizeof(width), &width);

        if (auto fill = shape->strokeFill()) {
            strokeDataByteCnt += serializeShapeFill(fill, TVG_SHAPE_STROKE_FILL_INDICATOR);
        }

        uint8_t color[4] = {0, 0, 0, 0};
        shape->strokeColor(color, color + 1, color + 2, color + 3);
        strokeDataByteCnt += saveMember(TVG_SHAPE_STROKE_COLOR_INDICATOR, sizeof(color), &color);

        const float* dashPattern = nullptr;
        uint32_t dashCnt = shape->strokeDash(&dashPattern);
        if (dashPattern && dashCnt > 0) {
            ByteCounter dashCntByteCnt = sizeof(dashCnt);
            ByteCounter dashPtrnByteCnt = dashCnt * sizeof(dashPattern[0]);

            saveMemberIndicator(TVG_SHAPE_STROKE_DASHPTRN_INDICATOR);
            saveMemberDataSize(dashCntByteCnt + dashPtrnByteCnt);
            strokeDataByteCnt += saveMemberData(&dashCnt, dashCntByteCnt);
            strokeDataByteCnt += saveMemberData(dashPattern, dashPtrnByteCnt);
            strokeDataByteCnt += TVG_INDICATOR_SIZE + BYTE_COUNTER_SIZE;
        }

        saveMemberDataSizeAt(strokeDataByteCnt);

        return TVG_INDICATOR_SIZE + BYTE_COUNTER_SIZE + strokeDataByteCnt;
    }


    ByteCounter serializeShapePath(const Shape* shape)
    {
        const PathCommand* cmds = nullptr;
        uint32_t cmdCnt = shape->pathCommands(&cmds);
        const Point* pts = nullptr;
        uint32_t ptsCnt = shape->pathCoords(&pts);

        if (!cmds || !pts || !cmdCnt || !ptsCnt) return 0;

        ByteCounter pathDataByteCnt = 0;

        saveMemberIndicator(TVG_SHAPE_PATH_INDICATOR);
        skipMemberDataSize();

        pathDataByteCnt += saveMemberData(&cmdCnt, sizeof(cmdCnt));
        pathDataByteCnt += saveMemberData(&ptsCnt, sizeof(ptsCnt));
        pathDataByteCnt += saveMemberData(cmds, cmdCnt * sizeof(cmds[0]));
        pathDataByteCnt += saveMemberData(pts, ptsCnt * sizeof(pts[0]));

        saveMemberDataSizeAt(pathDataByteCnt);

        return TVG_INDICATOR_SIZE + BYTE_COUNTER_SIZE + pathDataByteCnt;
    }


    ByteCounter serializeShape(Node* node)
    {
        if (!node) return 0;
        auto shape = static_cast<const Shape*>(node->getPaint());
        if (!shape) return 0;

        ByteCounter shapeDataByteCnt = 0;

        saveMemberIndicator(TVG_SHAPE_BEGIN_INDICATOR);
        skipMemberDataSize();

        TvgFlag ruleTvgFlag = (shape->fillRule() == FillRule::EvenOdd) ? TVG_SHAPE_FILLRULE_EVENODD_FLAG : TVG_SHAPE_FILLRULE_WINDING_FLAG;
        shapeDataByteCnt += saveMember(TVG_SHAPE_FILLRULE_INDICATOR, TVG_FLAG_SIZE, &ruleTvgFlag);

        if (shape->strokeWidth() > 0) { //MGS - 0 for now, but maybe strokeWidth could return -1 in case of an error? 
            shapeDataByteCnt += serializeShapeStroke(shape);
        }

        if (auto fill = shape->fill()) {
            shapeDataByteCnt += serializeShapeFill(fill, TVG_SHAPE_FILL_INDICATOR);
        }

        uint8_t color[4] = {0, 0, 0, 0};
        shape->fillColor(color, color + 1, color + 2, color + 3);
        shapeDataByteCnt += saveMember(TVG_SHAPE_COLOR_INDICATOR, sizeof(color), color);

        shapeDataByteCnt += serializeShapePath(shape);

        shapeDataByteCnt += serializeNode(node);
        shapeDataByteCnt += serializePaint(shape);

        saveMemberDataSizeAt(shapeDataByteCnt);

        return TVG_INDICATOR_SIZE + BYTE_COUNTER_SIZE + shapeDataByteCnt;
    }


    ByteCounter serializePicture(Node* node)
    {
        if (!node) return 0;
        auto picture = static_cast<const Picture*>(node->getPaint());
        if (!picture) return 0;
        auto pixels = picture->data();

        ByteCounter pictureDataByteCnt = 0;

        saveMemberIndicator(TVG_PICTURE_BEGIN_INDICATOR);
        skipMemberDataSize();

        if (pixels) {
            // MGS because loader expects uints... I'd change to floats
            float vw, vh;
            picture->viewbox(nullptr, nullptr, &vw, &vh);

            uint32_t w = static_cast<uint32_t>(vw);
            uint32_t h = static_cast<uint32_t>(vh);
            ByteCounter wByteCnt = sizeof(w); // same as h size
            ByteCounter pixelsByteCnt = w * h * sizeof(pixels[0]);

            saveMemberIndicator(TVG_RAW_IMAGE_BEGIN_INDICATOR);
            saveMemberDataSize(2 * wByteCnt + pixelsByteCnt);
            pictureDataByteCnt += saveMemberData(&w, wByteCnt);
            pictureDataByteCnt += saveMemberData(&h, wByteCnt);
            pictureDataByteCnt += saveMemberData(pixels, pixelsByteCnt);
            pictureDataByteCnt += TVG_INDICATOR_SIZE + BYTE_COUNTER_SIZE;
        }

        pictureDataByteCnt += serializeNode(node);
        pictureDataByteCnt += serializePaint(picture);

        saveMemberDataSizeAt(pictureDataByteCnt);

        return TVG_INDICATOR_SIZE + BYTE_COUNTER_SIZE + pictureDataByteCnt;
    }


    bool prepareBuffer()
    {
        reserved = 8;  //MGS - define HEADER_SIZE
        buffer = static_cast<char*>(malloc(reserved));
        if (!buffer) {
            reserved = 0;
            return false;
        }
        bufferPosition = buffer;
        return true;
    }
    
    
    bool saveHeader()
    {
        const char *tvg = TVG_HEADER_TVG_SIGN_CODE;
        const char *version = TVG_HEADER_TVG_VERSION_CODE;
        // MGS - metadata not raelly used for now - type hardcoded (to be changed)
        uint16_t metadataByteCnt = 0;
        ByteCounter headerByteCnt = strlen(tvg) + strlen(version) + sizeof(metadataByteCnt);
        if (size + headerByteCnt > reserved) resizeBuffer(headerByteCnt);

        memcpy(bufferPosition, tvg, strlen(tvg));
        bufferPosition += strlen(tvg);
        memcpy(bufferPosition, version, strlen(version));
        bufferPosition += strlen(version);
        memcpy(bufferPosition, &metadataByteCnt, sizeof(metadataByteCnt));
        bufferPosition += sizeof(metadataByteCnt);

        size += headerByteCnt;
        return true;
    }


    bool saveToFile(const std::string& path)
    {
        ofstream outFile;
        outFile.open(path, ios::out | ios::trunc | ios::binary);
        if (!outFile.is_open()) return false;
        outFile.write(buffer, size);
        outFile.close();

        return true;
    }


    void clearBuffer()
    {
        if (buffer) free(buffer);
        buffer = nullptr;
        bufferPosition = nullptr;
        size = 0;
        reserved = 0;
    }


    ByteCounter serializeNode(Node* root)
    {
        if (!root) return 0;

        ByteCounter dataByteCnt = 0;

        auto children = root->getChildren();

        for (auto node = children.data; node < (children.data + children.count); ++node) {
            dataByteCnt += serialize(*node);
        }

        return dataByteCnt; // MGS: -1 error handling
    }


    ByteCounter serialize(Node* node)
    {
        if (!node) return 0;
        ByteCounter dataByteCnt = 0;

        auto type = (node)->getPaintType();
        switch (type) {
            case PAINT_ID_SHAPE: {
                dataByteCnt += serializeShape(node);
                break;
            }
            case PAINT_ID_SCENE: {
                dataByteCnt += serializeScene(node);
                break;
            }
            case PAINT_ID_PICTURE: {
                dataByteCnt += serializePicture(node);
                break;
            }
            case PAINT_ID_COMPOSITE: {
                dataByteCnt += serializeComposite(node);
                break;
            }
        }

        return dataByteCnt;
    }


    bool save(Paint* paint, const std::string& path)
    {
        if (!prepareBuffer()) return false;
        if (!saveHeader()) return false;

        auto root = (paint->serialize()).release();

        //MGS: error handling - to be improved
        if (serialize(root) == 0) return false;

        if (!saveToFile(path)) return false;
        clearBuffer();

        return true;
    }
};

#endif //_TVG_SAVER_IMPL_H_
