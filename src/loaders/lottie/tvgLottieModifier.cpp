/*
* Copyright (c) 2024 the ThorVG project. All rights capacity().

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

#include "tvgLottieModifier.h"
#include "tvgMath.h"


/************************************************************************/
/* Internal Class Implementation                                        */
/************************************************************************/

struct ContourState
{
    Line line{};
    Line firstLine{};
    bool moveto = false;
    uint32_t movetoIndex = 0;
    uint32_t movetoInIndex = 0;
};


static void _roundCorner(Array<PathCommand>& cmds, Array<Point>& pts, const Point& prev, const Point& curr, const Point& next, float roundness)
{
    auto lenPrev = length(prev - curr);
    auto rPrev = lenPrev > 0.0f ? 0.5f * std::min(lenPrev * 0.5f, roundness) / lenPrev : 0.0f;
    auto lenNext = length(next - curr);
    auto rNext = lenNext > 0.0f ? 0.5f * std::min(lenNext * 0.5f, roundness) / lenNext : 0.0f;

    auto dPrev = rPrev * (curr - prev);
    auto dNext = rNext * (curr - next);

    pts.push(curr - 2.0f * dPrev);
    pts.push(curr - dPrev);
    pts.push(curr - dNext);
    pts.push(curr - 2.0f * dNext);
    cmds.push(PathCommand::LineTo);
    cmds.push(PathCommand::CubicTo);
}


static bool _intersect(const Line& line1, const Line& line2, Point& intersection, bool& inside)
{
    if (tvg::zero(line1.pt2 - line2.pt1)) {
        intersection = line1.pt2;
        inside = true;
        return true;
    }

    constexpr float epsilon = 1e-3f;
    float denom = (line1.pt2.x - line1.pt1.x) * (line2.pt2.y - line2.pt1.y) - (line1.pt2.y - line1.pt1.y) * (line2.pt2.x - line2.pt1.x);
    if (fabsf(denom) < epsilon) return false;

    float t = ((line2.pt1.x - line1.pt1.x) * (line2.pt2.y - line2.pt1.y) - (line2.pt1.y - line1.pt1.y) * (line2.pt2.x - line2.pt1.x)) / denom;
    float u = ((line2.pt1.x - line1.pt1.x) * (line1.pt2.y - line1.pt1.y) - (line2.pt1.y - line1.pt1.y) * (line1.pt2.x - line1.pt1.x)) / denom;

    intersection.x = line1.pt1.x + t * (line1.pt2.x - line1.pt1.x);
    intersection.y = line1.pt1.y + t * (line1.pt2.y - line1.pt1.y);
    inside = t >= -epsilon && t <= 1.0f + epsilon && u >= -epsilon && u <= 1.0f + epsilon;

    return true;
}


static Line _offset(const Point& p1, const Point& p2, float offset)
{
    auto scaledNormal = normal(p1, p2) * offset;
    return {p1 - scaledNormal, p2 - scaledNormal};
}


static void _corner(const Line& line, const Line& nextLine, uint32_t movetoIndex, bool nextClose, Array<PathCommand>& cmds, Array<Point>& pts, StrokeJoin join, float miter)
{
    bool inside{};
    Point intersect{};
    if (_intersect(line, nextLine, intersect, inside)) {
        if (inside) {
            if (nextClose) pts[movetoIndex] = intersect;
            pts.push(intersect);
        } else {
            pts.push(line.pt2);
            if (join == StrokeJoin::Round) {
                cmds.push(PathCommand::CubicTo);
                pts.push((line.pt2 + intersect) * 0.5f);
                pts.push((nextLine.pt1 + intersect) * 0.5f);
                pts.push(nextLine.pt1);
            } else if (join == StrokeJoin::Miter) {
                auto norm = normal(line.pt1, line.pt2);
                auto nextNorm = normal(nextLine.pt1, nextLine.pt2);
                auto miterDirection = (norm + nextNorm) / length(norm + nextNorm);
                cmds.push(PathCommand::LineTo);
                if (1.0f <= miter * fabsf(miterDirection.x * norm.x + miterDirection.y * norm.y)) pts.push(intersect);
                else pts.push(nextLine.pt1);
            } else {
                cmds.push(PathCommand::LineTo);
                pts.push(nextLine.pt1);
            }
        }
    } else pts.push(line.pt2);

}

static void _lineModifier(const Point* inPts, uint32_t& currentPt, const PathCommand* inCmds, uint32_t inCmdsCnt, uint32_t currentCmd, ContourState& state, bool degenerated, Array<PathCommand>& cmds, Array<Point>& pts, float offset, StrokeJoin join, float miter)
{
    if (tvg::zero(inPts[currentPt - 1] - inPts[currentPt])) {
        ++currentPt;
        return;
    }

    if (inCmds[currentCmd - 1] != PathCommand::LineTo) state.line = _offset(inPts[currentPt - 1], inPts[currentPt], offset);

    if (state.moveto) {
        cmds.push(PathCommand::MoveTo);
        state.movetoIndex = pts.count;
        pts.push(state.line.pt1);
        state.firstLine = state.line;
        state.moveto = false;
    }

    auto nonDegeneratedCubic = [&](uint32_t cmd, uint32_t pt) {
        return inCmds[cmd] == PathCommand::CubicTo && !tvg::zero(inPts[pt] - inPts[pt + 1]) && !tvg::zero(inPts[pt + 2] - inPts[pt + 3]);
    };

    cmds.push(PathCommand::LineTo);

    if (currentCmd + 1 == inCmdsCnt || inCmds[currentCmd + 1] == PathCommand::MoveTo || nonDegeneratedCubic(currentCmd + 1, currentPt + degenerated)) {
        pts.push(state.line.pt2);
        ++currentPt;
        return;
    }

    Line nextLine = state.firstLine;
    if (inCmds[currentCmd + 1] == PathCommand::LineTo) nextLine = _offset(inPts[currentPt + degenerated], inPts[currentPt + 1 + degenerated], offset);
    else if (inCmds[currentCmd + 1] == PathCommand::CubicTo) nextLine = _offset(inPts[currentPt + 1 + degenerated], inPts[currentPt + 2 + degenerated], offset);
    else if (inCmds[currentCmd + 1] == PathCommand::Close && fabsf(length(inPts[currentPt + degenerated] - inPts[state.movetoInIndex + degenerated])) > 1e-3)
        nextLine = _offset(inPts[currentPt + degenerated], inPts[state.movetoInIndex + degenerated], offset);

    _corner(state.line, nextLine, state.movetoIndex, inCmds[currentCmd + 1] == PathCommand::Close, cmds, pts, join, miter);

    state.line = nextLine;
    ++currentPt;
}


/************************************************************************/
/* External Class Implementation                                        */
/************************************************************************/


bool roundnessModifier(const Point* inPts, uint32_t inPtsCnt, const PathCommand* inCmds, uint32_t inCmdsCnt, Array<PathCommand>& cmds, Array<Point>& pts, Matrix* transform, float roundness)
{
    cmds.reserve(inCmdsCnt * 2);
    pts.reserve((uint32_t)(inPtsCnt * 1.5));
    auto ptsCnt = pts.count;

    uint32_t startIndex = 0;
    for (uint32_t iCmds = 0, iPts = 0; iCmds < inCmdsCnt; ++iCmds) {
        switch (inCmds[iCmds]) {
            case PathCommand::MoveTo: {
                startIndex = pts.count;
                cmds.push(PathCommand::MoveTo);
                pts.push(inPts[iPts++]);
                break;
            }
            case PathCommand::CubicTo: {
                auto& prev = inPts[iPts - 1];
                auto& curr = inPts[iPts + 2];
                if (iCmds < inCmdsCnt - 1 &&
                    tvg::zero(inPts[iPts - 1] - inPts[iPts]) &&
                    tvg::zero(inPts[iPts + 1] - inPts[iPts + 2])) {
                    if (inCmds[iCmds + 1] == PathCommand::CubicTo &&
                        tvg::zero(inPts[iPts + 2] - inPts[iPts + 3]) &&
                        tvg::zero(inPts[iPts + 4] - inPts[iPts + 5])) {
                        _roundCorner(cmds, pts, prev, curr, inPts[iPts + 5], roundness);
                        iPts += 3;
                        break;
                    } else if (inCmds[iCmds + 1] == PathCommand::Close) {
                        _roundCorner(cmds, pts, prev, curr, inPts[2], roundness);
                        pts[startIndex] = pts.last();
                        iPts += 3;
                        break;
                    }
                }
                cmds.push(PathCommand::CubicTo);
                pts.push(inPts[iPts++]);
                pts.push(inPts[iPts++]);
                pts.push(inPts[iPts++]);
                break;
            }
            case PathCommand::Close: {
                cmds.push(PathCommand::Close);
                break;
            }
            default: break;
        }
    }
    if (transform) {
        for (auto i = ptsCnt; i < pts.count; ++i) {
            pts[i] *= *transform;
        }
    }
    return true;
}


bool offsetModifier(const Point* inPts, uint32_t inPtsCnt, const PathCommand* inCmds, uint32_t inCmdsCnt, Array<PathCommand>& cmds, Array<Point>& pts, float offset, StrokeJoin join, float miter)
{
    cmds.reserve(inCmdsCnt * 2);
    pts.reserve(inPtsCnt * (join == StrokeJoin::Round ? 4 : 2));

    ContourState state;

    for (uint32_t iCmd = 0, iPt = 0; iCmd < inCmdsCnt; ++iCmd) {
        if (inCmds[iCmd] == PathCommand::MoveTo) {
            state.moveto = true;
            state.movetoInIndex = iPt++;
        } else if (inCmds[iCmd] == PathCommand::LineTo) {
            _lineModifier(inPts, iPt, inCmds, inCmdsCnt, iCmd, state, false, cmds, pts, offset, join, miter);
        } else if (inCmds[iCmd] == PathCommand::CubicTo) {
            //cubic degenerated to a line
            if (tvg::zero(inPts[iPt - 1] - inPts[iPt]) || tvg::zero(inPts[iPt + 1] - inPts[iPt + 2])) {
                ++iPt;
                _lineModifier(inPts, iPt, inCmds, inCmdsCnt, iCmd, state, true, cmds, pts, offset, join, miter);
                ++iPt;
                continue;
            }

            auto line1 = _offset(inPts[iPt - 1], inPts[iPt], offset);
            auto line2 = _offset(inPts[iPt], inPts[iPt + 1], offset);
            auto line3 = _offset(inPts[iPt + 1], inPts[iPt + 2], offset);

            if (state.moveto) {
                cmds.push(PathCommand::MoveTo);
                state.movetoIndex = pts.count;
                pts.push(line1.pt1);
                state.firstLine = line1;
                state.moveto = false;
            }

            bool inside{};
            Point intersect{};
            _intersect(line1, line2, intersect, inside);
            pts.push(intersect);
            _intersect(line2, line3, intersect, inside);
            pts.push(intersect);
            pts.push(line3.pt2);
            cmds.push(PathCommand::CubicTo);

            iPt += 3;
        }
        else {
            if (fabsf(length(inPts[iPt - 1] - inPts[state.movetoInIndex])) > 1e-3) {
                cmds.push(PathCommand::LineTo);
                _corner(state.line, state.firstLine, state.movetoIndex, true, cmds, pts, join, miter);
            }
            cmds.push(PathCommand::Close);
        }
    }

    return true;
}