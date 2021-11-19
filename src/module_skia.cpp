#include "module_skia.h"
#include "GrContextOptions.h"
#include "GrDirectContext.h"
#include "SkCanvas.h"
#include "SkColor.h"
#include "SkData.h"
#include "SkImage.h"
#include "SkPaint.h"
#include "SkPath.h"
#include "SkStream.h"
#include "SkSurface.h"
#include "gl/GrGLInterface.h"

namespace blbench
{
    uint32_t SkiaUtil::toSkiaOperator(uint32_t compOp)
    {
        switch (compOp)
        {
        case BL_COMP_OP_SRC_OVER:
            return (uint32_t)SkBlendMode::kSrcOver;
        case BL_COMP_OP_SRC_COPY:
            return (uint32_t)SkBlendMode::kSrc;
        case BL_COMP_OP_SRC_IN:
            return (uint32_t)SkBlendMode::kSrcIn;
        case BL_COMP_OP_SRC_OUT:
            return (uint32_t)SkBlendMode::kSrcOut;
        case BL_COMP_OP_SRC_ATOP:
            return (uint32_t)SkBlendMode::kSrcATop;
        case BL_COMP_OP_DST_OVER:
            return (uint32_t)SkBlendMode::kDstOver;
        case BL_COMP_OP_DST_COPY:
            return (uint32_t)SkBlendMode::kDst;
        case BL_COMP_OP_DST_IN:
            return (uint32_t)SkBlendMode::kDstIn;
        case BL_COMP_OP_DST_OUT:
            return (uint32_t)SkBlendMode::kDstOut;
        case BL_COMP_OP_DST_ATOP:
            return (uint32_t)SkBlendMode::kDstATop;
        case BL_COMP_OP_XOR:
            return (uint32_t)SkBlendMode::kXor;
        case BL_COMP_OP_CLEAR:
            return (uint32_t)SkBlendMode::kClear;
        case BL_COMP_OP_PLUS:
            return (uint32_t)SkBlendMode::kPlus;
        case BL_COMP_OP_MULTIPLY:
            return (uint32_t)SkBlendMode::kMultiply;
        case BL_COMP_OP_SCREEN:
            return (uint32_t)SkBlendMode::kScreen;
        case BL_COMP_OP_OVERLAY:
            return (uint32_t)SkBlendMode::kOverlay;
        case BL_COMP_OP_DARKEN:
            return (uint32_t)SkBlendMode::kDarken;
        case BL_COMP_OP_LIGHTEN:
            return (uint32_t)SkBlendMode::kLighten;
        case BL_COMP_OP_COLOR_DODGE:
            return (uint32_t)SkBlendMode::kColorDodge;
        case BL_COMP_OP_COLOR_BURN:
            return (uint32_t)SkBlendMode::kColorBurn;
        case BL_COMP_OP_HARD_LIGHT:
            return (uint32_t)SkBlendMode::kHardLight;
        case BL_COMP_OP_SOFT_LIGHT:
            return (uint32_t)SkBlendMode::kSoftLight;
        case BL_COMP_OP_DIFFERENCE:
            return (uint32_t)SkBlendMode::kDifference;
        case BL_COMP_OP_EXCLUSION:
            return (uint32_t)SkBlendMode::kExclusion;

        default:
            return 0xFFFFFFFFu;
        }
    }

    bool SkiaModule::supportsStyle(uint32_t style) const
    {
        return style == kBenchStyleSolid; //||
                                          //    style == kBenchStyleLinearPad ||
                                          //    style == kBenchStyleLinearRepeat ||
                                          //    style == kBenchStyleLinearReflect ||
                                          //    style == kBenchStyleRadialPad ||
                                          //    style == kBenchStyleRadialRepeat ||
                                          //    style == kBenchStyleRadialReflect ||
                                          //    style == kBenchStyleConical ||
                                          //    style == kBenchStylePatternNN ||
                                          //    style == kBenchStylePatternBI;
    }

    bool SkiaModule::supportsCompOp(uint32_t compOp) const
    {
        return SkiaUtil::toSkiaOperator(compOp) != 0xFFFFFFFFu;
    }

    SkiaModule::SkiaModule()
        : m_surface(nullptr), m_canvas(nullptr)
    {
    }

    SkiaModule::~SkiaModule()
    {
    }

    void SkiaModule::onBeforeRun()
    {
        int w = int(_params.screenW);
        int h = int(_params.screenH);
        uint32_t style = _params.style;

        #ifdef SK_BENCH_GL
        SkImageInfo info = SkImageInfo:: MakeN32Premul(w, h);
        sk_sp<const GrGLInterface> interface = GrGLMakeNativeInterface();
        m_context = GrDirectContext::MakeGL(interface);
        m_surface = SkSurface::MakeRenderTarget(m_context.get(), SkBudgeted::kNo, info);
        if (!m_surface)
        {
            SkDebugf("SkSurface::MakeRenderTarget returned null\n");
            return;
        }
        m_canvas = m_surface->getCanvas();

        #else
        m_surface = SkSurface::MakeRasterN32Premul(w, h);
        m_canvas = m_surface->getCanvas();
        #endif // SK_BENCH_GL
    }

    void SkiaModule::onDoRectAligned(bool stroke)
    {
        BLSizeI bounds(_params.screenW, _params.screenH);
        uint32_t style = _params.style;
        int wh = _params.shapeSize;

        SkPaint paint;

        if (style == kBenchStyleSolid)
        {
            if (stroke)
            {
                paint.setStyle(SkPaint::Style::kStroke_Style);
            }
            else
            {
                paint.setStyle(SkPaint::Style::kFill_Style);
            }

            for (uint32_t i = 0, quantity = _params.quantity; i < quantity; i++)
            {
                BLRect rect(_rndCoord.nextRect(bounds, wh, wh));
                BLRgba32 color = _rndColor.nextRgba32();
                paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
                SkRect skRect = SkRect::MakeXYWH(rect.x, rect.y, rect.w, rect.h);
                m_canvas->drawRect(skRect, paint);
            }
        }
    }

    void SkiaModule::onAfterRun()
    {
        if (m_context)
        {
            m_context->abandonContext();
            m_context.reset();
        }

        if (m_surface)
        {
            m_surface.reset();
        }
    }

    void SkiaModule::onDoRectSmooth(bool stroke)
    {
        BLSizeI bounds(_params.screenW, _params.screenH);
        uint32_t style = _params.style;
        int wh = _params.shapeSize;
        SkPaint paint;

        if (style == kBenchStyleSolid)
        {
            if (stroke)
            {
                paint.setStyle(SkPaint::Style::kStroke_Style);
            }
            else
            {
                paint.setStyle(SkPaint::Style::kFill_Style);
            }
            for (uint32_t i = 0, quantity = _params.quantity; i < quantity; i++)
            {
                BLRect rect(_rndCoord.nextRect(bounds, wh, wh));
                BLRgba32 color = _rndColor.nextRgba32();
                paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
                SkRect skRect = SkRect::MakeXYWH(rect.x, rect.y, rect.w, rect.h);
                m_canvas->drawRect(skRect, paint);
            }
        }
    }

    void SkiaModule::onDoRectRotated(bool stroke)
    {
        BLSize bounds(_params.screenW, _params.screenH);
        uint32_t style = _params.style;

        double cx = double(_params.screenW) * 0.5;
        double cy = double(_params.screenH) * 0.5;
        double wh = _params.shapeSize;
        double angle = 0.0;

        SkPaint paint;
        if (style == kBenchStyleSolid)
        {
            if (stroke)
            {
                paint.setStyle(SkPaint::Style::kStroke_Style);
                paint.setStrokeWidth(SkScalar(_params.strokeWidth));
            }
            else
            {
                paint.setStyle(SkPaint::Style::kFill_Style);
            }

            // Save canvas matrices state
            m_canvas->restore();
            m_canvas->save();

            for (uint32_t i = 0, quantity = _params.quantity; i < quantity; i++, angle += 0.01)
            {
                BLRect rect(_rndCoord.nextRect(bounds, wh, wh));
                BLRgba32 color = _rndColor.nextRgba32();
                paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
                SkRect skRect = SkRect::MakeXYWH(rect.x, rect.y, rect.w, rect.h);
                m_canvas->translate(cx, cy);
                // Todo: try to rotate the canvas by radians
                m_canvas->rotate(SkRadiansToDegrees(angle));
                m_canvas->translate(-cx, -cy);
                m_canvas->drawRect(skRect, paint);
                m_canvas->restore();
            }
        }
    }

    void SkiaModule::onDoRoundSmooth(bool stroke)
    {
        BLSize bounds(_params.screenW, _params.screenH);
        uint32_t style = _params.style;
        double wh = _params.shapeSize;

        SkPaint paint;
        if (style == kBenchStyleSolid)
        {
            if (stroke)
            {
                paint.setStyle(SkPaint::Style::kStroke_Style);
                paint.setStrokeWidth(SkScalar(_params.strokeWidth));
            }
            else
            {
                paint.setStyle(SkPaint::Style::kFill_Style);
            }

            for (uint32_t i = 0, quantity = _params.quantity; i < quantity; i++)
            {

                BLRect rect(_rndCoord.nextRect(bounds, wh, wh));
                double radius = _rndExtra.nextDouble(4.0, 40.0);
                SkRect skRect = SkRect::MakeXYWH(rect.x, rect.y, rect.w, rect.h);
                m_canvas->drawRoundRect(skRect, radius, radius, paint);
            }
        }
    }

    void SkiaModule::onDoRoundRotated(bool stroke)
    {
        BLSize bounds(_params.screenW, _params.screenH);
        uint32_t style = _params.style;

        double cx = double(_params.screenW) * 0.5;
        double cy = double(_params.screenH) * 0.5;
        double wh = _params.shapeSize;
        double angle = 0.0;

        SkPaint paint;
        if (style == kBenchStyleSolid)
        {
            if (stroke)
            {
                paint.setStyle(SkPaint::Style::kStroke_Style);
                paint.setStrokeWidth(SkScalar(_params.strokeWidth));
            }
            else
            {
                paint.setStyle(SkPaint::Style::kFill_Style);
            }

            // Save canvas matrices state

            for (uint32_t i = 0, quantity = _params.quantity; i < quantity; i++, angle += 0.01)
            {
                m_canvas->save();
                double radius = _rndExtra.nextDouble(4.0, 40.0);
                BLRect rect(_rndCoord.nextRect(bounds, wh, wh));
                BLRgba32 color = _rndColor.nextRgba32();
                paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
                SkRect skRect = SkRect::MakeXYWH(rect.x, rect.y, rect.w, rect.h);
                m_canvas->translate(cx, cy);
                // Todo: try to rotate the canvas by radians
                m_canvas->rotate(SkRadiansToDegrees(angle));
                m_canvas->translate(-cx, -cy);
                m_canvas->drawRoundRect(skRect, radius, radius, paint);
                m_canvas->restore();
            }
        }
    }

    void SkiaModule::onDoPolygon(uint32_t mode, uint32_t complexity)
    {
        BLSizeI bounds(_params.screenW - _params.shapeSize,
                       _params.screenH - _params.shapeSize);
        uint32_t style = _params.style;
        double wh = double(_params.shapeSize);

        bool stroke = (mode == 2);
        SkPaint paint;
        SkPath path;

        if (stroke)
        {
            paint.setStyle(SkPaint::Style::kStroke_Style);
            paint.setStrokeWidth(SkScalar(_params.strokeWidth));
            paint.setStrokeJoin(SkPaint::Join::kMiter_Join);
        }
        else
        {
            paint.setStyle(SkPaint::Style::kFill_Style);
        }

        if (style == kBenchStyleSolid)
        {
            for (uint32_t i = 0, quantity = _params.quantity; i < quantity; i++)
            {
                BLPoint base(_rndCoord.nextPoint(bounds));
                double x = _rndCoord.nextDouble(base.x, base.x + wh);
                double y = _rndCoord.nextDouble(base.y, base.y + wh);

                path.moveTo(x, y);

                for (uint32_t p = 1; p < complexity; p++)
                {
                    x = _rndCoord.nextDouble(base.x, base.x + wh);
                    y = _rndCoord.nextDouble(base.y, base.y + wh);
                    path.lineTo(x, y);
                }

                if (style == kBenchStyleSolid)
                {
                    BLRgba32 color = _rndColor.nextRgba32();
                    paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
                    m_canvas->drawPath(path, paint);
                }

                path.reset();
            }
        }
    }

    void SkiaModule::onDoShape(bool stroke, const BLPoint *pts, size_t count)
    {
        BLSizeI bounds(_params.screenW - _params.shapeSize,
                       _params.screenH - _params.shapeSize);

        uint32_t style = _params.style;
        SkPath path;
        SkPaint paint;

        bool start = true;
        double wh = double(_params.shapeSize);

        if (stroke)
        {
            paint.setStyle(SkPaint::Style::kStroke_Style);
            paint.setStrokeWidth(SkScalar(_params.strokeWidth));
        }
        else
        {
            paint.setStyle(SkPaint::Style::kFill_Style);
        }

        if (style == kBenchStyleSolid)
        {
            for (size_t i = 0; i < count; i++)
            {
                double x = pts[i].x;
                double y = pts[i].y;

                if (x == -1.0 && y == -1.0)
                {
                    start = true;
                    continue;
                }

                if (start)
                {
                    path.moveTo(x * wh, y * wh);
                    start = false;
                }
                else
                {
                    path.lineTo(x * wh, y * wh);
                }
            }

            for (uint32_t i = 0, quantity = _params.quantity; i < quantity; i++)
            {
                m_canvas->save();

                BLPoint base(_rndCoord.nextPoint(bounds));
                m_canvas->translate(base.x, base.y);

                BLRgba32 color = _rndColor.nextRgba32();
                paint.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
                m_canvas->drawPath(path, paint);

                m_canvas->restore();
            }
        }
    }
}
